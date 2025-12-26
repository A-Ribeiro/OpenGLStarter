#include "App.h"

#include <appkit-gl-engine/StartEventManager.h>

using namespace AppKit::Window::Devices;
using namespace MathCore;

App::App()
{
    // forward app reference that could be used by newly created components
    Engine::Instance()->app = this;

    resourceHelper.initialize();

#if (!defined(__APPLE__))
    Engine::Instance()->window->setMouseCursorVisible(true);
#endif

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate
    // renderState->ClearColor = vec4f(0.129f, 0.129f, 0.129f, 1.0f);
    renderState->ClearColor = vec4f(1, 1, 1, 1);
    renderState->FrontFace = FrontFaceCCW;
#ifndef ITK_RPI
    // renderState->Wireframe = WireframeBack;
    // renderState->CullFace = CullFaceNone;

    renderState->Wireframe = WireframeDisabled;
    renderState->CullFace = CullFaceBack;
#else
    renderState->CullFace = CullFaceBack;
#endif

    AppBase::OnGainFocus.add(&App::onGainFocus, this);
    AppBase::screenRenderWindow->CameraViewport.OnChange.add(&App::onViewportChange, this);
    AppBase::screenRenderWindow->inputManager.onWindowEvent.add(&App::onWindowEvent, this);

    // fade = STL_Tools::make_unique<Fade>(&time, this->eventHandlerSet);

    // fade->fadeOut(5.0f, nullptr);
    // time.update();
    gain_focus = true;

    mainScene = nullptr;
    gameScene = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    // screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    fps = 0;
    below_min_hz_count = 0;
    draw_stats_enabled = false;

    applySettingsChanges();
}

void App::load()
{
    mainScene = GameScene::CreateShared<MainScene>(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    mainScene->load();

    fade = STL_Tools::make_unique<Fade>(&time, mainScene);
}

App::~App()
{
    gameScene.reset();
    mainScene.reset();
    fade.reset();

    resourceMap.clear();
    resourceHelper.finalize();
}

void App::draw()
{
    while (executeOnMainThread.size() > 0)
    {
        auto callback = executeOnMainThread.dequeue(nullptr, true);
        callback();
    }

    fps_timer.update();
    time.update();
    // 1000 hz protection code
    if (time.unscaledDeltaTime <= 1.0f / 1000.0f)
        time.rollback_and_set_zero();
    // 2 hz protection code - for fallback from window move
    if (time.unscaledDeltaTime > 1.0f / 2.0f && below_min_hz_count < 2)
    {
        below_min_hz_count++;
        time.reset();
        fps_timer.reset();
    }
    else
        below_min_hz_count = 0;

    if (gain_focus)
    {
        gain_focus = false;
        time.reset();
        fps_timer.reset();
    }

    if (fps_timer.unscaledDeltaTime > EPSILON<float>::high_precision)
        fps = MathCore::OP<float>::lerp(fps, 1.0f / fps_timer.unscaledDeltaTime, 0.05f);

    // set min delta time (the passed time or the time to render at 24fps)
    // time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    SceneBase *scenes[] = {
        (SceneBase *)gameScene.get(),
        (SceneBase *)mainScene.get()};

    for (auto scene : scenes)
        if (scene != nullptr)
            threadPool.postTask([this, scene]()
                                {
                // component start event
                scene->startEventManager.processAllComponentsWithTransform();
                
                scene->OnPreUpdate(&time);
                scene->OnUpdate(&time);
                scene->OnLateUpdate(&time);

                // pre process scene graph
                scene->precomputeSceneGraphAndCamera();

                scene->OnAfterGraphPrecompute(&time);

                semaphore_aux.release(); });

    // global event for fade FX
    // eventHandlerSet->OnUpdate(&time);
    // parallel tasks on Main Loop

    // blocking until all scenes are precomputed
    for (auto scene : scenes)
        if (scene != nullptr)
            semaphore_aux.blockingAcquire();

    // smart clear stage
    {
        GLRenderState *renderState = GLRenderState::Instance();

        // MathCore::vec4f clearColor = renderState->ClearColor;
        AppKit::GLEngine::iRect viewport = renderState->Viewport;

        // renderState->ClearColor = MathCore::vec4f(0, 0, 0, 1);

        // glDisable(GL_SCISSOR_TEST);
        // glClear(GL_COLOR_BUFFER_BIT);

        // renderState->ClearColor = clearColor;

        glEnable(GL_SCISSOR_TEST);
        glScissor(viewport.x,
                  viewport.y,
                  viewport.w,
                  viewport.h);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    if (gameScene != nullptr)
        gameScene->draw();
    if (mainScene != nullptr)
        mainScene->draw();

    if (fade != nullptr)
        fade->draw();

    if (draw_stats_enabled)
        drawStats();

    // draw black bars
    if (fade != nullptr)
    {

        glDisable(GL_SCISSOR_TEST);

        GLRenderState *renderState = GLRenderState::Instance();

        AppKit::GLEngine::DepthTestType oldDepthTest = renderState->DepthTest;
        bool oldDepthTestEnabled = renderState->DepthWrite;
        AppKit::GLEngine::BlendModeType oldBlendMode = renderState->BlendMode;
        AppKit::GLEngine::iRect viewport = renderState->Viewport;
        MathCore::vec2i windowSize = window->getSize();
        renderState->Viewport = AppKit::GLEngine::iRect(windowSize.x, windowSize.y);

        MathCore::vec3f vertex[8] = {
            MathCore::vec3f(0, 0, 0),
            MathCore::vec3f(windowSize.x, 0, 0),
            MathCore::vec3f(windowSize.x, windowSize.y, 0),
            MathCore::vec3f(0, windowSize.y, 0),

            MathCore::vec3f(viewport.x, viewport.y, 0),
            MathCore::vec3f(viewport.x + viewport.w, viewport.y, 0),
            MathCore::vec3f(viewport.x + viewport.w, viewport.y + viewport.h, 0),
            MathCore::vec3f(viewport.x, viewport.y + viewport.h, 0)};

        /*
        3     2
          7 6
          4 5
        0     1
        */
        uint32_t indices[24] = {
            0, 1, 5,
            0, 5, 4,

            1, 2, 6,
            1, 6, 5,

            2, 3, 7,
            2, 7, 6,

            3, 0, 4,
            3, 4, 7};

        AppKit::OpenGL::GLShaderColor *shaderColor = fade->getShaderColor();

        renderState->CurrentShader = shaderColor;
        renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;
        renderState->DepthWrite = false;
        renderState->BlendMode = AppKit::GLEngine::BlendModeDisabled;

        shaderColor->setMatrix(
            MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
                0.0f,                // Left
                (float)windowSize.x, // Right
                0.0f,                // Bottom
                (float)windowSize.y, // Top
                -1.0f,               // ZNear
                1.0f                 // ZFar
                ));

        shaderColor->setColor(MathCore::vec4f(0, 0, 0, 1));

        OPENGL_CMD(glEnableVertexAttribArray(shaderColor->vPosition));
        OPENGL_CMD(glVertexAttribPointer(shaderColor->vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));
        // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 3));
        OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

        OPENGL_CMD(glDisableVertexAttribArray(shaderColor->vPosition));

        // renderstate->CurrentShader = oldShader;
        renderState->DepthTest = oldDepthTest;
        renderState->DepthWrite = oldDepthTestEnabled;
        renderState->BlendMode = oldBlendMode;
        renderState->Viewport = viewport;
    }

    // if (Keyboard::isPressed(KeyCode::Escape))
    //     exitApp();

    if (fade != nullptr && fade->isFading)
        return;
}

void App::drawStats()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> fontResource =
        resourceMap.getTextureFont("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

    if (!fontResource->material->shader->compareType(AppKit::GLEngine::ShaderUnlitTextureVertexColorAlpha::Type))
        return;

    auto renderState = GLRenderState::Instance();
    // AppKit::GLEngine::iRect viewport = renderState->Viewport;
    AppKit::GLEngine::iRect viewport = screenRenderWindow->CameraViewport;

    auto builder = fontResource->fontBuilder.get();

    builder->size = 64.0f;
    builder->faceColor = ui::colorFromHex("#FFFF00FF");
    builder->strokeColor = ui::colorFromHex("#000000ff");
    builder->strokeOffset = MathCore::vec3f(0, 0, 0.001f);
    builder->horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
    builder->verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    builder->lineHeight = 1.0f;
    builder->wrapMode = AppKit::OpenGL::GLFont2WrapMode_NoWrap;
    builder->firstLineHeightMode = AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight;
    builder->wordSeparatorChar = U' ';
    builder->drawFace = true;
    builder->drawStroke = true;

    builder->richBuild(
        ITKCommon::PrintfToStdString("%i FPS", (int)(fps + 0.5f)).c_str(),
        engine->sRGBCapable,
        -1,
        fontResource->polygonFontCache);

    if (builder->vertexAttrib.size() == 0)
        return;

    auto shader = (AppKit::GLEngine::ShaderUnlitTextureVertexColorAlpha *)fontResource->material->shader.get();

    shader->ActiveShader_And_SetUniformsFromMaterial(
        renderState, &resourceMap,
        &renderPipeline,
        fontResource->material.get());

    shader->setMVP(
        MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
            0.0f,              // Left
            (float)viewport.w, // Right
            0.0f,              // Bottom
            (float)viewport.h, // Top
            -1.0f,             // ZNear
            1.0f               // ZFar
            ) *
        MathCore::GEN<MathCore::mat4f>::translateHomogeneous(
            16.0f,
            16.0f));

    AppKit::GLEngine::DepthTestType oldDepthTest = renderState->DepthTest;
    bool oldDepthTestEnabled = renderState->DepthWrite;

    renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;
    renderState->DepthWrite = false;

    int aPos = shader->queryAttribLocation("aPosition");
    int aUV = shader->queryAttribLocation("aUV0");
    int aColor = shader->queryAttribLocation("aColor0");

    OPENGL_CMD(glEnableVertexAttribArray(aPos));
    OPENGL_CMD(glVertexAttribPointer(aPos, 3, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].pos.array));
    OPENGL_CMD(glEnableVertexAttribArray(aUV));
    OPENGL_CMD(glVertexAttribPointer(aUV, 2, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].uv.array));
    OPENGL_CMD(glEnableVertexAttribArray(aColor));
    OPENGL_CMD(glVertexAttribPointer(aColor, 4, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].color.array));

    OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)builder->vertexAttrib.size()));
    // OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

    OPENGL_CMD(glDisableVertexAttribArray(aPos));
    OPENGL_CMD(glDisableVertexAttribArray(aUV));
    OPENGL_CMD(glDisableVertexAttribArray(aColor));

    renderState->DepthTest = oldDepthTest;
    renderState->DepthWrite = oldDepthTestEnabled;

    renderState->clearTextureUnitActivationArray();
    renderState->CurrentShader = nullptr;

    // for (size_t i = 0; i < builder->vertexAttrib.size(); i++)
    // {
    //     mesh->pos.push_back(builder->vertexAttrib[i].pos);
    //     mesh->uv[0].push_back(MathCore::vec3f(builder->vertexAttrib[i].uv, 0.0f));
    //     mesh->color[0].push_back(builder->vertexAttrib[i].color);

    //     // keep CCW orientation
    //     if ((i % 3) == 0)
    //     {
    //         mesh->indices.push_back((uint16_t)i);
    //         mesh->indices.push_back((uint16_t)(i + 1));
    //         mesh->indices.push_back((uint16_t)(i + 2));
    //     }
    // }
}

void App::onGainFocus()
{
    // time.update();
    gain_focus = true;
}

void App::onViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue)
{
    // GLRenderState *renderState = GLRenderState::Instance();
    // renderState->Viewport = AppKit::GLEngine::iRect(value.w, value.h);
    // if (mainScene != nullptr)
    //     mainScene->resize(value, oldValue);
    if (gameScene != nullptr)
        gameScene->onCameraViewportUpdate(MathCore::vec2i(value.w, value.h));
}

void App::onWindowEvent(const AppKit::Window::WindowEvent &evt)
{
    if (evt.type == AppKit::Window::WindowEventType::Resized)
    {
        if (mainScene != nullptr)
            mainScene->onWindowResized(MathCore::vec2i(evt.resized.width, evt.resized.height));
    }
}

void App::applySettingsChanges()
{

    auto engine = AppKit::GLEngine::Engine::Instance();
    auto options = AppOptions::OptionsManager::Instance();

    {
        const char *vSync = options->getGroupValueSelectedForKey("Video", "VSync");
        engine->window->glSetVSync(strcmp(vSync, "ON") == 0);
    }

    {
        const char *meshCrusher = options->getGroupValueSelectedForKey("Extra", "MeshCrusher");
        if (strcmp(meshCrusher, "ON") == 0)
            renderPipeline.agregateMesh_ConcatenateLowerThanTriangleCount = 1024;
        else
            renderPipeline.agregateMesh_ConcatenateLowerThanTriangleCount = 0;
    }

    {
        const char *onGameStats = options->getGroupValueSelectedForKey("Extra", "OnGameStats");
        draw_stats_enabled = strcmp(onGameStats, "FPS") == 0;
    }

    if (mainScene != nullptr)
        mainScene->applySettingsChanges();
}

void App::createGameScene()
{
    executeOnMainThread.enqueue([this]()
                                {
        if (gameScene == nullptr) {
            gameScene = GameScene::CreateShared<GameScene>(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
            gameScene->load();
        } });
}