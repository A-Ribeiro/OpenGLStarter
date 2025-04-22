#include "MainScene.h"
#include "App.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>
#include <InteractiveToolkit/EaseCore/EaseCore.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources()
{
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    fontBuilder.load("resources/SansMono-80.basof2");
}
// to load the scene graph
void MainScene::loadGraph()
{
    root = Transform::CreateShared();
    root->addChild(Transform::CreateShared())->Name = "Main Camera";

    scaleNode = root->addChild(Transform::CreateShared());

    // text transform
    {
        auto textNode = scaleNode->addChild(Transform::CreateShared());
        textNode->Name = "text1";
        textNode->setLocalPosition(MathCore::vec3f(0, 0, -0.1f));

        font_line1 = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
    }

    {
        auto textNode = scaleNode->addChild(Transform::CreateShared());
        textNode->Name = "text2";
        textNode->setLocalPosition(MathCore::vec3f(0, 0, -0.1f));

        font_line2 = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
    }

    // //plane with texture
    // {
    //     imageNode = scaleNode->addChild(Transform::CreateShared());
    //     imageNode->Name = "yuv420 image";
    //     imageNode->setLocalRotation(MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(90.0f), 0));
    // }
}

void MainScene::setText(
    const std::string &text,
    float _size,
    float window_width,
    std::shared_ptr<AppKit::GLEngine::Components::ComponentFontToMesh> toMesh)
{

    if (toMesh == nullptr)
        return;

    // this->text = text;
    // this->text_size = _size;
    // this->text_margin = horiz_margin;

    float text_scale_factor = _size / fontBuilder.glFont2.size;
    // float text_margin_scale_factor = horiz_margin / fontBuilder.glFont2.size;

    fontBuilder.faceColor = MathCore::vec4f(0.758f, 0.754f, 0.752f, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.75f, 0.75f, 0.75f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
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
        to_center = (mesh_wrapper->aabb.max_box - mesh_wrapper->aabb.min_box) * -0.5f;

        to_center.y = -to_center.y;
        to_center.z = 0;

        font_line1_transform->setLocalPosition(to_center);

        max_text_width = MathCore::OP<float>::maximum(max_text_width, to_center.x * -2.0f + MathCore::EPSILON<float>::low_precision);
    }

    {
        auto font_line2_transform = this->font_line2->getTransform();
        auto mesh_wrapper = font_line2_transform->findComponent<ComponentMeshWrapper>();

        if (mesh_wrapper == nullptr)
            return;

        mesh_wrapper->computeFinalPositions(false);

        MathCore::vec3f to_center;
        to_center = (mesh_wrapper->aabb.max_box - mesh_wrapper->aabb.min_box) * -0.5f;

        to_center.y = to_center.y;
        to_center.z = 0;

        font_line2_transform->setLocalPosition(to_center);

        max_text_width = MathCore::OP<float>::maximum(max_text_width, to_center.x * -2.0f + MathCore::EPSILON<float>::low_precision);
    }
}

void MainScene::setTextWithWidth(float width)
{

    setText(
        u8" ███████                                                                               ██\n"
        u8"░██░░░░██                  █████                                                      ░██\n"
        u8"░██   ░██ ██████  ██████  ██░░░██ ██████  ██████   ██████████   ██████   ███████      ░██  ██████\n"
        u8"░███████ ░░██░░█ ██░░░░██░██  ░██░░██░░█ ░░░░░░██ ░░██░░██░░██ ░░░░░░██ ░░██░░░██  ██████ ██░░░░██\n"
        u8"░██░░░░   ░██ ░ ░██   ░██░░██████ ░██ ░   ███████  ░██ ░██ ░██  ███████  ░██  ░██ ██░░░██░██   ░██\n"
        u8"░██       ░██   ░██   ░██ ░░░░░██ ░██    ██░░░░██  ░██ ░██ ░██ ██░░░░██  ░██  ░██░██  ░██░██   ░██\n"
        u8"░██      ░███   ░░██████   █████ ░███   ░░████████ ███ ░██ ░██░░████████ ███  ░██░░██████░░██████\n"
        u8"░░       ░░░     ░░░░░░   ░░░░░  ░░░     ░░░░░░░░ ░░░  ░░  ░░  ░░░░░░░░ ░░░   ░░  ░░░░░░  ░░░░░░",
        16.0f, // px
        width, // width
        this->font_line1);

    setText(
        u8" ████████                                  ██             ███████              ██   ██\n"
        u8"░██░░░░░                                  ░██            ░██░░░░██            ░██  ░░\n"
        u8"░██        ██████  ██████  ██████         ░██  ██████    ░██   ░██   ██████  ██████ ██ ███████   ██████\n"
        u8"░███████  ██░░░░██░░██░░█ ░░░░░░██     ██████ ░░░░░░██   ░███████   ██░░░░██░░░██░ ░██░░██░░░██ ░░░░░░██\n"
        u8"░██░░░░  ░██   ░██ ░██ ░   ███████    ██░░░██  ███████   ░██░░░██  ░██   ░██  ░██  ░██ ░██  ░██  ███████\n"
        u8"░██      ░██   ░██ ░██    ██░░░░██   ░██  ░██ ██░░░░██   ░██  ░░██ ░██   ░██  ░██  ░██ ░██  ░██ ██░░░░██\n"
        u8"░██      ░░██████ ░███   ░░████████  ░░██████░░████████  ░██   ░░██░░██████   ░░██ ░██ ███  ░██░░████████\n"
        u8"░░        ░░░░░░  ░░░     ░░░░░░░░    ░░░░░░  ░░░░░░░░   ░░     ░░  ░░░░░░     ░░  ░░ ░░░   ░░  ░░░░░░░░",
        16.0f, // px
        width, // width
        this->font_line2);

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

    // ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    // {
    //     auto imageNode = root->findTransformByName("yuv420 image");

    //     auto imageMaterial = imageNode->addNewComponent<ComponentMaterial>();
    //     imageNode->addComponent(ComponentMesh::createPlaneXY(1920.0f, 1080.0f));

    //     imageMaterial->type = MaterialUnlitTexture;
    //     imageMaterial->unlit.blendMode = BlendModeAlpha;

    //     texture = std::make_shared<AppKit::OpenGL::GLTexture>(1920, 1080, GL_RGBA);

    //     std::vector<uint8_t> data(1920 * 1080 * 4, 255);
    //     texture->uploadBufferRGBA_8888(&data[0], 1920, 1080, false);

    //     imageMaterial->unlit.tex = texture;
    // }

    // texRefCount->add(&fontBuilder.glFont2.texture);
    // texRefCount->add(texture);

    // call resize
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    resize(engine->app->window->getSize());

    // Add AABB for all meshs...
    {
        // root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }

    auto screenRenderWindow = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;

    screenRenderWindow->inputManager.onKeyboardEvent.add([&](const AppKit::Window::KeyboardEvent &evt)
                                                         {
        if (evt.type == AppKit::Window::KeyboardEventType::KeyPressed &&
            evt.code == AppKit::Window::Devices::KeyCode::Space){
            printf("space pressed...\n");

            if (this->font_line1->material->unlit.color.a != 0.f){
                this->font_line1->material->unlit.blendMode = AppKit::GLEngine::BlendModeType::BlendModeAlpha;
                this->font_line1->material->unlit.color.a = 0.f;
    
                this->font_line2->material->unlit.blendMode = AppKit::GLEngine::BlendModeType::BlendModeAlpha;
                this->font_line2->material->unlit.color.a = 0.f;

                auto screenRenderWindow = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;

            } else {
                this->recording = true;
                this->time_sec = 0;
                this->video_image_count = 0;
            }
        } });

    colorTexture.setSize(1920, 1080, GL_RGB);
    fbo.setSize(1920, 1080);

    fbo.enable();
    fbo.setColorAttachment(&colorTexture, 0);
    // fbo.setDrawBufferCount(1);
    fbo.checkAttachment();
    fbo.disable();

    screenRenderWindow->OnUpdate.add(&MainScene::update, this);

}

// clear all loaded scene
void MainScene::unloadAll()
{

    // ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;
    font_line1 = nullptr;
    font_line2 = nullptr;
    scaleNode = nullptr;

    texBuffer.dispose();
}

void MainScene::update(Platform::Time *elapsed)
{

    if (recording)
    {
        auto screenRenderWindow = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;
        // float screen_width = (float)screenRenderWindow->CameraViewport.c_val().w;

        float lrp = MathCore::OP<float>::clamp(this->time_sec / this->complete_time_sec, 0.0f, 1.0f);
        lrp = MathCore::OP<float>::pow(lrp, 1.3f);
        float aux = max_text_width * lrp * 1.025f;

        printf("%f\n", aux);

        setTextWithWidth(aux);

        float lrp_fade = MathCore::OP<float>::clamp(this->time_sec / this->fade_time_sec, 0.0f, 1.0f);

        this->font_line1->material->unlit.blendMode = AppKit::GLEngine::BlendModeType::BlendModeAlpha;
        this->font_line1->material->unlit.color.a = lrp_fade;

        this->font_line2->material->unlit.blendMode = AppKit::GLEngine::BlendModeType::BlendModeAlpha;
        this->font_line2->material->unlit.color.a = lrp_fade;
    }
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

    if (recording)
    {
        // write png

        auto out_filename = ITKCommon::PrintfToStdString("video_%04i.png", this->video_image_count);

        printf("%s\n", out_filename.c_str());

        auto screenRenderWindow = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;
        fbo.setSize(screenRenderWindow->CameraViewport.c_val().w, screenRenderWindow->CameraViewport.c_val().h);
        fbo.blitFromBackBuffer(0, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        this->colorTexture.download(&texBuffer);

        ITKExtension::Image::PNG::writePNG(
            out_filename.c_str(),
            texBuffer.width, texBuffer.height,
            texBuffer.input_component_count,
            (char *)texBuffer.data
        );

        this->video_image_count++;

        if (this->time_sec > this->total_time_sec)
        {
            recording = false;

            //system("rm output.mp4");
            printf("%s",
                "now you can run:\n\n"
                "ffmpeg -framerate 30 -i video_%04d.png -i Vinheta.wav -vf 'vflip' -c:v h264_nvenc -preset slow -rc vbr -cq 19 -b:v 5M -c:a aac -b:a 192k -ar 48000 -shortest output.mp4"
                "\n\n");
            //system("ffmpeg -framerate 30 -i video_%04d.png -i Vinheta.wav -vf 'vflip' -c:v h264_nvenc -preset slow -rc vbr -cq 19 -b:v 5M -c:a aac -b:a 192k -ar 48000 -shortest output.mp4");
            //system("rm *.png");
        }
        else
        {
            // this->time_sec += this->fps_interval_sec;
            // static int count = 0;
            this->time_sec += this->fps_interval_sec;

            // if (AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::A )){
            //     count++;
            //     printf("count: %i\n", count);
            //     Platform::Sleep::millis(500);
            // }
        }
    }
}

void MainScene::resize(const MathCore::vec2i &size)
{
    // // fixed height of 1080 pixels
    // float new_scale = size.height / 1080.0f;

    // float aspect_window = (float)size.width / (float)size.height;
    // float aspect_image = (float)texture->width / (float)texture->height;

    // bool fit_width = (aspect_window < aspect_image);
    // if (fit_width) {
    //     new_scale /= aspect_image / aspect_window;
    // }

    // scaleNode->setLocalScale(new_scale);

    setTextWithWidth(size.width);
}

MainScene::MainScene(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap)
{
    font_line1 = nullptr;
    font_line2 = nullptr;
    scaleNode = nullptr;

    recording = false;
}

MainScene::~MainScene()
{
    unload();
}
