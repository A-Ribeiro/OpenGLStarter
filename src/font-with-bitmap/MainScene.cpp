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
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    root->addChild(scaleNode = Transform::CreateShared());

    // text transform
    {
        auto textNode = scaleNode->addChild(Transform::CreateShared());
        textNode->Name = "text1";
        textNode->setLocalPosition(MathCore::vec3f(0, 0, -0.1f));

        font_line1 = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
    }
}

void MainScene::setText(
    const std::string &text,
    float _size,
    float window_width,
    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> toMesh)
{

    if (toMesh == nullptr)
        return;

    float text_scale_factor = _size / fontBuilder.glFont2.size;

    //fontBuilder.faceColor = MathCore::vec4f(0.758f, 0.754f, 0.752f, 1);
    fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.75f, 0.75f, 0.75f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_center;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_middle;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = false;

    // split by lines
    std::vector<std::string> lines = ITKCommon::StringUtil::tokenizer(text, "\n");
    std::vector<std::string> result;

    float screen_scale = scaleNode->getLocalScale().x;
    // float window_width = (float)AppKit::GLEngine::Engine::Instance()->window->getSize().width;
    float valid_width = window_width / screen_scale; // - text_margin * 2.0f;
    valid_width /= text_scale_factor;

    for (auto line : lines)
    {
        std::vector<std::string> words = ITKCommon::StringUtil::tokenizer(line, " ");
        std::string aux = "";
        bool first = true;
        for (auto word : words)
        {
            std::string aux_with_word = aux;
            if (first)
                aux_with_word += word;
            else
                aux_with_word += " " + word;
            float xmin, xmax, ymin, ymax;
            fontBuilder.computeBox(aux_with_word.c_str(), &xmin, &xmax, &ymin, &ymax);
            float width = xmax - xmin;
            if (!first && width > valid_width)
            {
                result.push_back(aux);
                aux = word;
            }
            else
            {
                aux = aux_with_word;
            }
            first = false;
        }
        result.push_back(aux);
    }

    std::string breakLinedTest = "";
    bool first = true;
    for (auto line : result)
    {
        if (first)
        {
            breakLinedTest += line;
            first = false;
        }
        else
            breakLinedTest += "\n" + line;
    }

    fontBuilder.build(breakLinedTest.c_str());
    toMesh->toMesh(fontBuilder, true);
    auto componentFontToMesh_transform = toMesh->getTransform();
    componentFontToMesh_transform->setLocalScale(text_scale_factor);
}

void MainScene::centerAllMesh()
{
    

    {
        auto font_line1_transform = this->font_line1->getTransform();
        auto mesh_wrapper = font_line1_transform->findComponent<ComponentMeshWrapper>();

        if (mesh_wrapper == nullptr)
            return;

        mesh_wrapper->computeFinalPositions(false);

        MathCore::vec3f to_center;
        to_center = (mesh_wrapper->aabb.max_box + mesh_wrapper->aabb.min_box) * -0.5f;

        //to_center.y = -to_center.y;
        to_center.z = 0;

        font_line1_transform->setPosition(to_center);

        // max_text_width = MathCore::OP<float>::maximum(max_text_width, to_center.x * -2.0f + MathCore::EPSILON<float>::low_precision);
    }
}

void MainScene::setTextWithWidth(float width)
{

    fontBuilder.lineHeight = 1.5f;
    
    setText( 
        

        u8"Use " Font_L_stick u8" para andar e " Font_xbox_a " para pular.\n"
        u8"Use " Font_R_stick u8" para andar e " Font_ps_square_white " para pular.\n"         
        u8"Use " Font_Key_arrows u8" para andar e " Font_Key_z " para pular.\n"
        u8"\n"
        
        u8"botões Xbox:" Font_xbox_a Font_xbox_b Font_xbox_x Font_xbox_y u8"\n"
        u8"botões PS(color):" Font_ps_circle_color Font_ps_cross_color Font_ps_square_color Font_ps_triangle_color u8"\n"
        u8"botões PS(white):" Font_ps_circle_white Font_ps_cross_white Font_ps_square_white Font_ps_triangle_white u8"\n"
        u8"teclado:" Font_Key_z Font_Key_x Font_Key_c
        
        ,
        60.0f, // px
        width, // width
        this->font_line1);

    //char32_t aux[2] = { 0,0 };
    //aux[0] = 65536;

    //auto utf_8_str = ITKCommon::StringUtil::utf32_to_utf8(aux);
    //for (auto v : utf_8_str) {
    //    printf("%.0x ", v);
    //}
    //printf("\n");

    ////utf_8_str = "\xF0\x90\x80\x80";
    //utf_8_str = Font_xbox_a;

    //for (auto v : utf_8_str) {
    //    printf("%.0x ", v);
    //}
    //printf("\n");
    //
    //fontBuilder.build(utf_8_str.c_str());

    //font_line1->toMesh(fontBuilder, true);

    centerAllMesh();
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
}

void MainScene::update(Platform::Time *elapsed)
{
}

void MainScene::draw()
{
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
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
    float new_scale = size.height / 1080.0f;

    // float aspect_window = (float)size.width / (float)size.height;
    // float aspect_image = (float)texture->width / (float)texture->height;

    // bool fit_width = (aspect_window < aspect_image);
    // if (fit_width) {
    //     new_scale /= aspect_image / aspect_window;
    // }

    scaleNode->setLocalScale(new_scale);

    setTextWithWidth(size.width);
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
