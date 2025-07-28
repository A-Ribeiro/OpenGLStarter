#include "MainScene.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

//to load skybox, textures, cubemaps, 3DModels and setup materials
void MainScene::loadResources(){
    auto engine = AppKit::GLEngine::Engine::Instance();
#if defined(ARIBEIRO_LINUX_LOADER_USE_USR_LOCAL_PATH)
    fontBuilder.load("/usr/local/etc/OpenMultimedia/Roboto-Regular-100.basof2", engine->sRGBCapable);
#else
    fontBuilder.load("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);
#endif
}
//to load the scene graph
void MainScene::loadGraph(){
    root = Transform::CreateShared();
    root->affectComponentStart = true;
    root->addChild( Transform::CreateShared() )->Name = "Main Camera";

    scaleNode = root->addChild(Transform::CreateShared());

    //text transform
    {
        auto textNode = scaleNode->addChild(Transform::CreateShared());
        textNode->Name = "center Text";
        textNode->setLocalPosition(MathCore::vec3f(0,0,-0.1f));

        componentFontToMesh = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
    }

    //plane with texture
    {
        imageNode = scaleNode->addChild(Transform::CreateShared());
        imageNode->Name = "yuv420 image";
        imageNode->setLocalRotation(MathCore::GEN<MathCore::quatf>::fromEuler(0, MathCore::OP<float>::deg_2_rad(90.0f), 0));
    }
}

void MainScene::setText(const std::string &text, float _size, float horiz_margin) {
    
    if (componentFontToMesh == nullptr)
        return;

    this->text = text;
    this->text_size = _size;
    this->text_margin = horiz_margin;
    float text_scale_factor = text_size / fontBuilder.glFont2.size;
    float text_margin_scale_factor = text_margin / fontBuilder.glFont2.size;
    
    fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.75f, 0.75f, 0.75f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_center;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_middle;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = false;
    fontBuilder.lineHeight = 1.3f;
    fontBuilder.firstLineHeightMode = AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight;

    //split by lines
    std::vector<std::string> lines = ITKCommon::StringUtil::tokenizer(text, "\n");
    std::vector<std::string> result;

    float screen_scale = scaleNode->getLocalScale().x;
    float window_width = (float)AppKit::GLEngine::Engine::Instance()->window->getSize().width;
    float valid_width = window_width / screen_scale - text_margin * 2.0f;
    valid_width /= text_scale_factor;

    for (auto line : lines) {
        std::vector<std::string> words = ITKCommon::StringUtil::tokenizer(line, " ");
        std::string aux = "";
        bool first = true;
        for (auto word : words) {
            std::string aux_with_word = aux;
            if (first)
                aux_with_word += word;
            else 
                aux_with_word += " " + word;
            float xmin, xmax, ymin, ymax;
            
            auto aabb = fontBuilder.richComputeBox(aux_with_word.c_str());
            xmin = aabb.min_box.x;
            xmax = aabb.max_box.x;
            ymin = aabb.min_box.y;
            ymax = aabb.max_box.y;

            float width = xmax - xmin;
            if (!first && width > valid_width) {
                result.push_back(aux);
                aux = word;
            }
            else {
                aux = aux_with_word;
            }
            first = false;
        }
        result.push_back(aux);
    }

    std::string breakLinedTest = "";
    bool first = true;
    for (auto line : result) {
        if (first) {
            breakLinedTest += line;
            first = false;
        }
        else
            breakLinedTest += "\n" + line;
    }

    fontBuilder.richBuild(breakLinedTest.c_str(), false);
    componentFontToMesh->toMesh(fontBuilder, true);
    auto componentFontToMesh_transform = componentFontToMesh->getTransform();
    componentFontToMesh_transform->setLocalScale(text_scale_factor);
}

//to bind the resources to the current graph
void MainScene::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

    //ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    {
        auto imageNode = root->findTransformByName("yuv420 image");
        
        auto imageMaterial = imageNode->addNewComponent<ComponentMaterial>();
        imageNode->addComponent(ComponentMesh::createPlaneXY(1920.0f, 1080.0f));

        imageMaterial->type = MaterialUnlitTexture;
        imageMaterial->unlit.blendMode = BlendModeAlpha;

        texture = std::make_shared<AppKit::OpenGL::GLTexture>(1920, 1080, GL_RGBA);

        std::vector<uint8_t> data(1920 * 1080 * 4, 255);
        texture->uploadBufferRGBA_8888(&data[0], 1920, 1080, false);

        imageMaterial->unlit.tex = texture;
    }

    // texRefCount->add(&fontBuilder.glFont2.texture);
    // texRefCount->add(texture);

    // call resize
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    resize( engine->app->window->getSize() );

    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

//clear all loaded scene
void MainScene::unloadAll(){

    //ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;

    // ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    // texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);
    // if (texture != nullptr){
    //     texRefCount->remove(texture);
    //     texture = nullptr;
    // }

    texture = nullptr;
    imageNode = nullptr;

    componentFontToMesh = nullptr;
    scaleNode = nullptr;
}

void MainScene::draw() {

    if (yuy2_queue.queue_header_ptr->size > 0 && yuy2_queue.read(&data_buffer)) {
        //new texture
        int64_t size_check = 1920 * 1080 * 2;
        if (data_buffer.size == size_check) {
            if (aux_rgb_buffer.size != 1920 * 1080 * 4) {
                aux_rgb_buffer.setSize(1920 * 1080 * 4);
                memset(aux_rgb_buffer.data, 255, 1920 * 1080 * 4);

                componentFontToMesh = nullptr;
                
                auto textNode = scaleNode->findTransformByName("center Text");
                textNode->setParent(nullptr);
                textNode = nullptr;
                //ResourceHelper::releaseTransformRecursive(&textNode);

                imageNode->setLocalRotation(MathCore::quatf());
            }
            
            uint8_t* in_buffer = (uint8_t*)data_buffer.data;
            uint8_t* out_buffer = (uint8_t*)aux_rgb_buffer.data;
            
            //memset(aux_rgb_buffer.data, 128, 1920 * 1080 * 4);

            m_YUV2RGB_Multithread.yuy2_to_rgba(in_buffer, out_buffer, 1920, 1080);
            /*
            for (int y = 0; y < 1080; y++) {
                for (int x = 0; x < 1920; x++) {
                    int index = x + y * 1920;

                    int y = in_buffer[index * 2 + 0];
                    
                    int u, v;
                    if (x % 2 == 0) {
                        u = in_buffer[index * 2 + 1];
                        v = in_buffer[(index + 1) * 2 + 1];
                    }
                    else {
                        u = in_buffer[(index - 1) * 2 + 1];
                        v = in_buffer[index * 2 + 1];
                    }

                    y = (y - 16) * 298;
                    u = u - 128;
                    v = v - 128;

                    out_buffer[index * 4 + 0] = YUV2RO(y, u, v);
                    out_buffer[index * 4 + 1] = YUV2GO(y, u, v);
                    out_buffer[index * 4 + 2] = YUV2BO(y, u, v);
                }
            }
            */

            texture->uploadBufferRGBA_8888(out_buffer, 1920, 1080, false);
        }
    }

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, true);
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void MainScene::resize(const MathCore::vec2i&size) {
    // fixed height of 1080 pixels
    float new_scale = size.height / 1080.0f;
    
    float aspect_window = (float)size.width / (float)size.height;
    float aspect_image = (float)texture->width / (float)texture->height;
    
    bool fit_width = (aspect_window < aspect_image);
    if (fit_width) {
        new_scale /= aspect_image / aspect_window;
    }

    scaleNode->setLocalScale(new_scale);
    setText(text, text_size, text_margin);
}

MainScene::MainScene(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : 
        AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow),
        text_size(0), 
        text_margin(0),
        yuy2_queue( "aRibeiro Cam 01", Platform::IPC::QueueIPC_READ, 8, 1920 * 1080 * 2, false),
        m_YUV2RGB_Multithread( Platform::Thread::QueryNumberOfSystemThreads(), Platform::Thread::QueryNumberOfSystemThreads()*4 )
{
    texture = nullptr;
    componentFontToMesh = nullptr;
    scaleNode = nullptr;
    imageNode = nullptr;
}

MainScene::~MainScene() {
    unload();
}
