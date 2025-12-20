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

    fade = new Fade(&time);

    // fade->fadeOut(5.0f, nullptr);
    // time.update();
    gain_focus = true;

    mainScene = nullptr;

    renderPipeline.ambientLight.lightMode = AmbientLightMode_None;

    screenRenderWindow->setHandleWindowCloseButtonEnabled(true);
    // screenRenderWindow->setViewportFromRealWindowSizeEnabled(true);
    // screenRenderWindow.setEventForwardingEnabled(true);

    fps = 0;
    below_min_hz_count = 0;
}

void App::load()
{
    mainScene = new MainScene(this, &time, &renderPipeline, &resourceHelper, &resourceMap, this->screenRenderWindow);
    mainScene->load();
}

App::~App()
{
    if (mainScene != nullptr)
    {
        mainScene->unload();
        delete mainScene;
        mainScene = nullptr;
    }
    if (fade != nullptr)
    {
        delete fade;
        fade = nullptr;
    }
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

    time.update();
    // 1000 hz protection code
    if (time.unscaledDeltaTime <= 1.0f / 1000.0f)
        time.rollback_and_set_zero();
    // 2 hz protection code - for fallback from window move
    if (time.unscaledDeltaTime > 1.0f / 2.0f && below_min_hz_count < 2)
    {
        below_min_hz_count++;
        time.reset();
    }
    else
        below_min_hz_count = 0;

    fps_timer.update();
    if (fps_timer.unscaledDeltaTime > 1.0f / 5.0f)
        fps_timer.reset();

    if (gain_focus)
    {
        gain_focus = false;
        time.reset();
        fps_timer.reset();
    }

    if (fps_timer.unscaledDeltaTime > EPSILON<float>::high_precision)
        fps = MathCore::OP<float>::lerp(fps, 1.0f / fps_timer.unscaledDeltaTime, 0.1f);

    this->fps_accumulator -= fps_timer.unscaledDeltaTime;
    if (this->fps_accumulator < 0)
    {
        this->fps_accumulator = App::fps_time_sec;
        printf("%.2f FPS\n", fps);
    }

    // set min delta time (the passed time or the time to render at 24fps)
    // time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    StartEventManager::Instance()->processAllComponentsWithTransform();

    screenRenderWindow->OnPreUpdate(&time);
    screenRenderWindow->OnUpdate(&time);
    screenRenderWindow->OnLateUpdate(&time);

    // pre process all scene graphs
    if (mainScene != nullptr)
        mainScene->precomputeSceneGraphAndCamera();

    screenRenderWindow->OnAfterGraphPrecompute(&time);

    // smart clear stage
    {
        GLRenderState *renderState = GLRenderState::Instance();

        MathCore::vec4f clearColor = renderState->ClearColor;
        AppKit::GLEngine::iRect viewport = renderState->Viewport;

        renderState->ClearColor = MathCore::vec4f(0, 0, 0, 1);
        
        // glDisable(GL_SCISSOR_TEST);
        // glClear(GL_COLOR_BUFFER_BIT);

        renderState->ClearColor = clearColor;

        glEnable(GL_SCISSOR_TEST);
        glScissor(viewport.x,
                  viewport.y,
                  viewport.w,
                  viewport.h);

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    if (mainScene != nullptr)
        mainScene->draw();

    fade->draw();

    // draw black bars
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
            MathCore::vec3f(viewport.x, viewport.y + viewport.h, 0)
        };
        
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
            3, 4, 7
        };

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

    if (fade->isFading)
        return;
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

    mainScene->applySettingsChanges();
}