#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>
#include "components/ComponentGrow.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    // auto engine = AppKit::GLEngine::Engine::Instance();

    SpriteAtlasGenerator gen;

    gen.addEntry("resources/smoke.png");
    gen.addEntry("resources/opengl_logo_white.png");

    auto engine = AppKit::GLEngine::Engine::Instance();
    spriteAtlas = gen.generateAtlas(*resourceMap, engine->sRGBCapable, true, 10);
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->affectComponentStart = true;

    auto main_camera = root->addChild(Transform::CreateShared());
    main_camera->Name = "Main Camera";

    // root->addChild(Transform::CreateShared())->Name = "Smoke";

    root->addChild(Transform::CreateShared())->Name = "bg";

    main_camera->addChild(Transform::CreateShared())->Name = "ui";
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    GLRenderState *renderState = GLRenderState::Instance();

    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();
    }

    {
        spriteNode = Transform::CreateShared("smoke");
        spriteNode->setLocalScale(MathCore::vec3f(0));
        auto componentSprite = spriteNode->addNewComponent<ComponentSprite>();
        componentSprite->setTextureFromAtlas(
            resourceMap,
            spriteAtlas, "resources/smoke.png",
            MathCore::vec2f(0.5f, 0.5f),             // pivot
            MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
            MathCore::vec2f(-1, 256),                // size constraint
            false,                                   // x_invert
            false,                                   // y_invert
            MeshUploadMode_Direct                    // direct draw, causes better performance on old hardware
        );
        auto componentGrow = spriteNode->addNewComponent<ComponentGrow>();
        componentGrow->app = app;
        componentGrow->transformPool = &transformPool;
        // componentSprite->mesh->always_clone = true;
    }

    {
        logoNode = Transform::CreateShared("logo");
        logoNode->setLocalScale(MathCore::vec3f(0));
        auto componentSprite = logoNode->addNewComponent<ComponentSprite>();
        componentSprite->setTextureFromAtlas(
            resourceMap,
            spriteAtlas, "resources/opengl_logo_white.png",
            MathCore::vec2f(0.5f, 0.5f),             // pivot
            MathCore::vec4f(1.0f, 1.0f, 1.0f, 0.4f), // color
            MathCore::vec2f(-1, 256),                // size constraint
            false,                                   // x_invert
            false,                                   // y_invert
            MeshUploadMode_Direct                    // direct draw, causes better performance on old hardware
        );
        auto componentGrow = logoNode->addNewComponent<ComponentGrow>();
        componentGrow->app = app;
        componentGrow->transformPool = &transformPool;
        // componentSprite->mesh->always_clone = true;
    }

    // componentSprite->meshWrapper->debugCollisionShapes = true;
    // spriteNode->skip_traversing = true;
    // componentSprite->material->custom_shader_property_bag.getProperty("UseDiscard").set(true);

    bgNode = root->findTransformByName("bg");
    bgNode->setLocalPosition(MathCore::vec3f(0, 0, 10));
    bgComponentSprite = bgNode->addNewComponent<ComponentSprite>();
    bgComponentSprite->setTexture(
        resourceMap,
        resourceMap->getTexture("resources/Skyboxes/SanFrancisco4/posy.jpg", engine->sRGBCapable),
        MathCore::vec2f(0.5f, 0.5f),             // pivot
        MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f), // color
        MathCore::vec2f(-1, 1024),               // size constraint
        false,                                   // x_invert
        false,                                   // y_invert
        MeshUploadMode_Static                    // static mesh
    );

    uiNode = root->findTransformByName("ui");
    // uiNode->setLocalPosition(MathCore::vec3f(0, 0, camera));

    uiComponent = uiNode->addNewComponent<ComponentUI>();
    uiComponent->Initialize(resourceMap);

    auto base_mask = uiComponent->addRectangle(
                                    vec2f(-150, 0),                           // pos
                                    vec2f(256, 512),                          // size
                                    MathCore::vec4f(0.0f, 0.25f, 0.5f, 0.8f), // color
                                    MathCore::vec4f(64, 0, 16, -1),           // radius
                                    StrokeModeGrowInside,                     // stroke mode
                                    15.0f,                                    // stroke thickness
                                    MathCore::vec4f(0.0f, 0.0f, 0.8f, 0.6f),  // stroke color
                                    40.0f,                                    // drop shadow thickness
                                    MathCore::vec4f(0.4f, 0.4f, 0.4f, 0.5f),  // drop shadow color
                                    0                                         // z
                                    ,
                                    "rect_mask")
                         .get<ComponentRectangle>();

    base_mask->getTransform()->setLocalRotation(MathCore::GEN<MathCore::quatf>::fromAxisAngle(MathCore::vec3f(0, 0, 1), MathCore::OP<float>::deg_2_rad(15.0f)));

    base_mask->getTransform()->setLocalScale(MathCore::vec3f(2.0f, 1.0f, 1.0f));

    auto front_screen = uiComponent->addRectangle(
                                       vec2f(0, 0),                                                        // pos
                                       vec2f(app->window->getSize().width, app->window->getSize().height), // size
                                       MathCore::vec4f(1.0f, 0.0f, 0.0f, 0.2f),                            // color
                                       MathCore::vec4f(-1, -1, -1, -1),                                    // radius
                                       StrokeModeGrowInside,                                               // stroke mode
                                       0.0f,                                                               // stroke thickness
                                       MathCore::vec4f(0),                                                 // stroke color
                                       0,                                                                  // drop shadow thickness
                                       MathCore::vec4f(0),                                                 // drop shadow color
                                       -10                                                                 // z
                                       )
                            .get<ComponentRectangle>();
    front_screen->setMask(resourceMap, camera, base_mask);

    auto sprite = uiComponent->addSpriteFromAtlas(
                                 vec2f(0, 0),                             // pos
                                 spriteAtlas,                             // atlas
                                 "resources/opengl_logo_white.png",       // texture
                                 vec2f(0.5f),                             // pivot
                                 MathCore::vec4f(1.0f, 1.0f, 1.0f, 1.0f), // color
                                 MathCore::vec2f(256, -1),                // size constraint
                                 -1,                                       // z
                                 false,
                                 false
                                 )
                      .get<ComponentSprite>();
    sprite->setMask(resourceMap, camera, base_mask);

    auto textComponent = uiComponent->addTextureText(
                                        "resources/Roboto-Regular-100.basof2",                                            // font_path
                                        MathCore::vec2f(0, 128),                                                          // pos
                                        -1,                                                                               // z
                                        "Hello, {push;lineHeight:0.8;faceColor:ff0000ff;size:80.0;}World{pop;} (Text) !", // text
                                        64.0f,                                                                            // size
                                        -1.0f,                                                                            // max_width
                                        MathCore::vec4f(1.0f, 1.0f, 0.0f, 1.0f),                                          // faceColor
                                        MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f),                                          // strokeColor
                                        MathCore::vec3f(0.0f, 0.0f, -0.02f),                                              // strokeOffset
                                        AppKit::OpenGL::GLFont2HorizontalAlign_center,                                    // horizontalAlign
                                        AppKit::OpenGL::GLFont2VerticalAlign_bottom,                                      // verticalAlign
                                        1.0f,                                                                             // lineHeight
                                        AppKit::OpenGL::GLFont2WrapMode_Word,                                             // wrapMode
                                        AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,                 // firstLineHeightMode
                                        U' ',                                                                             // wordSeparatorChar
                                        "top_text")
                             .get<ComponentFont>();
    textComponent->setMask(resourceMap, camera, base_mask);

    textComponent = uiComponent->addPolygonText(
                                   "resources/Roboto-Regular-100.basof2",                                            // font_path
                                   64.0f,                                                                            // polygon_size
                                   5.0f,                                                                             // polygon_distance_tolerance
                                   &app->threadPool,                                                                 // polygon_threadPool
                                   MathCore::vec2f(0, -128),                                                         // pos
                                   -1,                                                                               // z
                                   "Hello, {push;lineHeight:0.8;faceColor:ff0000ff;size:80.0;}World{pop;} (Text) !", // text
                                   64.0f,                                                                            // size
                                   -1.0f,                                                                            // max_width
                                   MathCore::vec4f(1.0f, 1.0f, 0.0f, 1.0f),                                          // faceColor
                                   MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f),                                          // strokeColor
                                   MathCore::vec3f(0.0f, 0.0f, -0.02f),                                              // strokeOffset
                                   AppKit::OpenGL::GLFont2HorizontalAlign_center,                                    // horizontalAlign
                                   AppKit::OpenGL::GLFont2VerticalAlign_top,                                         // verticalAlign
                                   1.0f,                                                                             // lineHeight
                                   AppKit::OpenGL::GLFont2WrapMode_Word,                                             // wrapMode
                                   AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,                 // firstLineHeightMode
                                   U' ',                                                                             // wordSeparatorChar
                                   "bottom_text")
                        .get<ComponentFont>();
    textComponent->setMask(resourceMap, camera, base_mask);

    // setup renderstate
    uiNode->setLocalPosition(MathCore::vec3f(0, 0, componentCameraOrthographic->nearPlane + 100.0f));

    {
        auto uiNodeParent = uiNode->getParent();
        auto new_node = uiNodeParent->addChild(uiNode->clone(resourceMap, false));
        new_node->setLocalPosition(MathCore::vec3f(550.0f, 0, componentCameraOrthographic->nearPlane + 100.0f));

        auto new_ui = new_node->findComponent<ComponentUI>();

        auto top_text_font = new_ui->getItemByName("top_text").get<ComponentFont>();
        top_text_font->setText(
            this->resourceMap,
            "resources/Roboto-Regular-100.basof2", // font_path
            0,                                     // polygon_size
            0,                                     // polygon_distance_tolerance
            nullptr,                               // polygon_threadPool
            engine->sRGBCapable,
            "Top Text!",                                                      // text
            64.0f,                                                            // size
            -1.0f,                                                            // max_width
            MathCore::vec4f(1.0f, 1.0f, 0.0f, 1.0f),                          // faceColor
            MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f),                          // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_bottom,                      // verticalAlign
            1.0f,                                                             // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
            U' '                                                              // wordSeparatorChar
        );

        top_text_font = new_ui->getItemByName("bottom_text").get<ComponentFont>();

        top_text_font->setText(
            this->resourceMap,
            "resources/Roboto-Regular-100.basof2", // font_path
            64.0f,                                 // polygon_size
            5.0f,                                  // polygon_distance_tolerance
            &app->threadPool,                      // polygon_threadPool
            engine->sRGBCapable,
            "Text Bottom! With Many Letters",                                 // text
            64.0f,                                                            // size
            -1.0f,                                                            // max_width
            MathCore::vec4f(1.0f, 1.0f, 0.0f, 1.0f),                          // faceColor
            MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f),                          // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                              // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                    // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_top,                         // verticalAlign
            1.0f,                                                             // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                             // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight, // firstLineHeightMode
            U' '                                                              // wordSeparatorChar
        );
    }

    auto rect = renderWindow->CameraViewport.c_ptr();
    resize(MathCore::vec2i(rect->w, rect->h));

    // Add AABB for all meshs...
    {
        resourceHelper->addAABBMesh(root);
    }

    renderWindow->inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &evt)
                                                   {
        if (evt.type == AppKit::Window::KeyboardEventType::KeyPressed &&
            evt.code == AppKit::Window::Devices::KeyCode::Space){
            auto new_element = root->addChild(spriteNode->clone(resourceMap, false));
            // new_element->addNewComponent<ComponentGrow>();
        } });

    renderWindow->OnUpdate.add(&MainScene::update, this);

    // initialize pool
    for (int i = 0; i < 75; i++)
    {
        transformPool.enqueue(spriteNode->clone(resourceMap, false));
        transformPool.enqueue(logoNode->clone(resourceMap, false));
    }
}

// clear all loaded scene
void MainScene::unloadAll()
{
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;

    // componentSprite = nullptr;
    spriteNode = nullptr;
    logoNode = nullptr;

    bgComponentSprite = nullptr;
    bgNode = nullptr;

    while (transformPool.size() > 0)
        transformPool.dequeue(nullptr, true);

    uiComponent = nullptr;
    uiNode = nullptr;

    spriteAtlas = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
    static float s_angle = 0;
    s_angle = MathCore::OP<float>::fmod(s_angle + elapsed->deltaTime * 0.5f, MathCore::CONSTANT<float>::PI * 2.0f);

    camera->getTransform()->setLocalRotation(MathCore::GEN<MathCore::quatf>::fromAxisAngle(MathCore::vec3f(0, 0, 1), s_angle));

    randomNext -= elapsed->deltaTime;

    if (randomNext <= 0.0f)
    {
        randomNext = mathRandom.nextRange(0.1f, 0.3f);

        for (int i = 0; i < random32.getRange<int>(10, 50); i++)
        {
            if (transformPool.size() <= 0)
                break;
            auto new_element = root->addChild(transformPool.dequeue(nullptr, true));
            new_element->setLocalPosition(MathCore::vec3f(
                mathRandom.nextRange(-512.0f, 512.0f),
                mathRandom.nextRange(-512.0f, 512.0f),
                0));
            // new_element->addNewComponent<ComponentGrow>();
        }
    }

    if (elapsed->unscaledDeltaTime > 0)
    {
        auto engine = AppKit::GLEngine::Engine::Instance();
        auto &top_text_font = uiComponent->getItemByName("top_text").get<ComponentFont>();
        top_text_font->setText(
            this->resourceMap,
            "resources/Roboto-Regular-100.basof2", // font_path
            0,                                     // polygon_size
            0,                                     // polygon_distance_tolerance
            nullptr,                               // polygon_threadPool
            engine->sRGBCapable,
            ITKCommon::PrintfToStdString("%i fps", (int)(MathCore::OP<float>::round(app->fps) + 0.5f)), // text
            64.0f,                                                                                      // size
            -1.0f,                                                                                      // max_width
            MathCore::vec4f(1.0f, 1.0f, 0.0f, 1.0f),                                                    // faceColor
            MathCore::vec4f(0.0f, 0.0f, 0.0f, 1.0f),                                                    // strokeColor
            MathCore::vec3f(0.0f, 0.0f, -0.02f),                                                        // strokeOffset
            AppKit::OpenGL::GLFont2HorizontalAlign_center,                                              // horizontalAlign
            AppKit::OpenGL::GLFont2VerticalAlign_bottom,                                                // verticalAlign
            1.0f,                                                                                       // lineHeight
            AppKit::OpenGL::GLFont2WrapMode_Word,                                                       // wrapMode
            AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight,                           // firstLineHeightMode
            U' '                                                                                        // wordSeparatorChar
        );
    }
}

void MainScene::draw()
{
    auto engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(resourceMap, root, camera, true, OrthographicFilter_UsingAABB, &app->threadPool);
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void MainScene::resize(const MathCore::vec2i &size)
{
    // fixed height of 1080 pixels
    // float new_scale = (float)size.height / 1080.0f;
}

MainScene::MainScene(
    App *app,
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow),
                                                                          random32(ITKCommon::RandomDefinition<uint32_t>::randomSeed()),
                                                                          mathRandom(&random32),
                                                                          transformPool(false)
{
    this->app = app;

    // spriteShader = nullptr;

    // //componentSprite = nullptr;
    // spriteNode = nullptr;
    // logoNode = nullptr;

    // bgComponentSprite = nullptr;
    // bgNode = nullptr;

    // uiComponent = nullptr;
    // uiNode = nullptr;

    randomNext = 0.0f;
}

MainScene::~MainScene()
{
    unload();
}
