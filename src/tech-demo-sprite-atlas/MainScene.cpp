#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
// #include <InteractiveToolkit/EaseCore/EaseCore.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    auto engine = AppKit::GLEngine::Engine::Instance();
    fontBuilder.load("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

    // polygonFontCache = fontBuilder.createPolygonCache(
    //     60.0f,  // size
    //     600.0f   // max_distance_tolerance
    // );
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    root->addChild(scaleNode = Transform::CreateShared());

    // box background
    {
        bgNode = scaleNode->addChild(Transform::CreateShared());
        auto materialBackground = bgNode->addNewComponent<Components::ComponentMaterial>();
        bgNode->addComponent(Components::ComponentMesh::createPlaneXY(1, 1));
        // bgNode->LocalPosition = MathCore::vec3f((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f, 0);

        materialBackground->type = Components::MaterialUnlit;
        materialBackground->unlit.blendMode = BlendModeAlpha;
        materialBackground->unlit.color = MathCore::vec4f(1, 1, 1, 0.2);
        auto engine = AppKit::GLEngine::Engine::Instance();
        if (engine->sRGBCapable)
            materialBackground->unlit.color = CVT<vec4f>::sRGBToLinear(materialBackground->unlit.color);
    }

    // text transform
    {
        auto textNode = scaleNode->addChild(Transform::CreateShared());
        textNode->Name = "text1";
        textNode->setLocalPosition(MathCore::vec3f(0, 0, -0.1f));

        font_line1 = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
    }
}

void MainScene::setTextWithWidth(float width)
{

    fontBuilder.lineHeight = 1.5f;
    fontBuilder.size = 600.0f*2;
    fontBuilder.horizontalAlign = GLFont2HorizontalAlign_center;
    fontBuilder.verticalAlign = GLFont2VerticalAlign_middle;
    fontBuilder.wrapMode = GLFont2WrapMode_Word;
    fontBuilder.firstLineHeightMode = GLFont2FirstLineHeightMode_UseCharacterMaxHeight;
    fontBuilder.wordSeparatorChar = U' ';

    // setText(
    //     ,
    //     60.0f, // px
    //     width, // width
    //     this->font_line1);

    std::string txt =
        u8"Use " Font_L_stick u8" para {push;lineHeight:0.8;faceColor:ff0000ff;size:80.0;}andar{pop;} e " Font_xbox_a " para pular.\n"
        u8"Use " Font_R_stick u8" para andar e " Font_ps_square_white " para pular.\n"
        u8"Use " Font_Key_arrows u8" para andar e " Font_Key_z " para pular.\n"
        u8"\n"

        u8"botões Xbox:" Font_xbox_a Font_xbox_b Font_xbox_x Font_xbox_y u8"\n"
        u8"botões PS(color):" Font_ps_circle_color Font_ps_cross_color Font_ps_square_color Font_ps_triangle_color u8"\n"
        u8"botões PS(white):" Font_ps_circle_white Font_ps_cross_white Font_ps_square_white Font_ps_triangle_white u8"\n"
        u8"teclado:" Font_Key_z Font_Key_x Font_Key_c;

    txt = "aba"; // for testing

    fontBuilder.richBuild(txt.c_str(), false, width, polygonFontCache);
    font_line1->toMesh(fontBuilder, true);
    font_line1->getTransform()->setLocalScale(1.0f);

    //printf("[starting]\n");
    auto txt_aabb = fontBuilder.richComputeBox(txt.c_str(), width);
    auto aabb_size = txt_aabb.max_box - txt_aabb.min_box;
    auto aabb_center = (txt_aabb.max_box + txt_aabb.min_box) * 0.5f;

    bgNode->setLocalScale(MathCore::vec3f(aabb_size.x, aabb_size.y, 1));
    bgNode->setLocalPosition(MathCore::vec3f(aabb_center.x, aabb_center.y, 0.0f));

    // centerAllMesh();
}

// to bind the resources to the current graph
void MainScene::bindResourcesToGraph()
{

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

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
            if (polygonFontCache != nullptr)
                polygonFontCache = nullptr;
            else {
                Platform::Time timer;
                timer.update();
                polygonFontCache = fontBuilder.createPolygonCache(
                    600.0f*2,  // size
                    1.0f,   // max_distance_tolerance
                    &app->threadPool
                );
                timer.update();
                printf("Font cache created in %.3f ms\n", timer.deltaTime * 1000.0f);
            }
            auto rect = renderWindow->CameraViewport.c_ptr();
            resize(MathCore::vec2i(rect->w, rect->h));
        } });

    renderWindow->OnUpdate.add(&MainScene::update, this);
}

// clear all loaded scene
void MainScene::unloadAll()
{
    renderWindow->OnUpdate.remove(&MainScene::update, this);

    root = nullptr;
    camera = nullptr;
    font_line1 = nullptr;
    scaleNode = nullptr;
    bgNode = nullptr;
}

void MainScene::update(Platform::Time *elapsed)
{
}

void MainScene::draw()
{
    auto engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, true);
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void MainScene::resize(const MathCore::vec2i &size)
{
    // fixed height of 1080 pixels
    float new_scale = (float)size.height / 1080.0f;

    // float aspect_window = (float)size.width / (float)size.height;
    // float aspect_image = (float)texture->width / (float)texture->height;

    // bool fit_width = (aspect_window < aspect_image);
    // if (fit_width) {
    //     new_scale /= aspect_image / aspect_window;
    // }

    scaleNode->setLocalScale(new_scale);

    setTextWithWidth((float)size.width / new_scale);
}

MainScene::MainScene(
    App *app,
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
{
    this->app = app;

    font_line1 = nullptr;
    scaleNode = nullptr;
}

MainScene::~MainScene()
{
    unload();
}
