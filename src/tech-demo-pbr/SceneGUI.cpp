#include "SceneGUI.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

// to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources()
{
    auto engine = AppKit::GLEngine::Engine::Instance();

    cursorTexture = resourceHelper->createTextureFromFile("resources/cursor.png", true && engine->sRGBCapable);

    fontBuilder.load("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

    button_NormalMap = new Button(
        0,                  // _position,
        true,               // _left,
        "button_NormalMap", //_id,
        "Normal Map ON",    //_text,
        &fontBuilder        //_fontBuilder
    );

    button_AmbientLight = new Button(
        1,                         // _position,
        true,                      // _left,
        "button_AmbientLight",     //_id,
        "Ambient Light SphereMap", //_text,
        &fontBuilder               //_fontBuilder
    );

    button_SunLight = new Button(
        2,                 // _position,
        true,              // _left,
        "button_SunLight", //_id,
        "Sun Light ON",    //_text,
        &fontBuilder       //_fontBuilder
    );

    button_SunLightRotate = new Button(
        3,                       // _position,
        true,                    // _left,
        "button_SunLightRotate", //_id,
        "Sun Light Rotate OFF",  //_text,
        &fontBuilder             //_fontBuilder
    );

    // right
    button_NextScene = new Button(
        0,                  // _position,
        false,              // _left,
        "button_NextScene", //_id,
        "Next Scene",       //_text,
        &fontBuilder        //_fontBuilder
    );

    allButtons.push_back(button_NormalMap);
    allButtons.push_back(button_AmbientLight);
    allButtons.push_back(button_SunLight);
    allButtons.push_back(button_SunLightRotate);
    allButtons.push_back(button_NextScene);
}
// to load the scene graph
void SceneGUI::loadGraph()
{
    root = Transform::CreateShared();

    auto t = root->addChild(Transform::CreateShared());
    t->Name = "Main Camera";

    for (size_t i = 0; i < allButtons.size(); i++)
    {
        t = root->addChild(allButtons[i]->getTransform());
    }

    // text transform
    {
        auto textNode = root->addChild(Transform::CreateShared());
        textNode->Name = "bottom Text";

        componentFontToMesh = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();

        textNode = root->addChild(Transform::CreateShared());
        textNode->Name = "fps";
        fps = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
        f_fps = 0.0f;
    }

    {
        cursorTransform = root->addChild(Transform::CreateShared());
    }
}

void SceneGUI::setText(const std::string &text)
{
    fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.0f, 0.0f, 0.0f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_right;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = true;

    fontBuilder.richBuild(text.c_str(), false);
    componentFontToMesh->toMesh(fontBuilder, true);
}

// to bind the resources to the current graph
void SceneGUI::bindResourcesToGraph()
{

    GLRenderState *renderState = GLRenderState::Instance();

    // setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

    //ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    {
        auto cursorMaterial = cursorTransform->addNewComponent<ComponentMaterial>();
        cursorTransform->addComponent(ComponentMesh::createPlaneXY(cursorTexture->width, cursorTexture->height));

        cursorMaterial->type = MaterialUnlitTexture;
        cursorMaterial->unlit.blendMode = BlendModeAlpha;
        cursorMaterial->unlit.tex = cursorTexture;
    }

    // texRefCount->add(&fontBuilder.glFont2.texture);

    // texRefCount->add(cursorTexture);

    // call resize
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    resize(engine->app->window->getSize());

    // Add AABB for all meshs...
    {
        // root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

// clear all loaded scene
void SceneGUI::unloadAll()
{

    //ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;

    allButtons.clear();

    if (button_NormalMap != nullptr)
    {
        delete button_NormalMap;
        button_NormalMap = nullptr;
    }
    if (button_AmbientLight != nullptr)
    {
        delete button_AmbientLight;
        button_AmbientLight = nullptr;
    }
    if (button_SunLight != nullptr)
    {
        delete button_SunLight;
        button_SunLight = nullptr;
    }
    if (button_SunLightRotate != nullptr)
    {
        delete button_SunLightRotate;
        button_SunLightRotate = nullptr;
    }
    if (button_NextScene != nullptr)
    {
        delete button_NextScene;
        button_NextScene = nullptr;
    }

    // ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    // texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);

    // if (cursorTexture != nullptr){
    //     // texRefCount->remove(cursorTexture);
    //     cursorTexture = nullptr;
    // }

    cursorTexture = nullptr;
    componentFontToMesh = nullptr;
    fps = nullptr;
    cursorTransform = nullptr;
}

void SceneGUI::draw()
{

    if (time->deltaTime > 1.0 / 10000.0f)
        f_fps = OP<float>::move(f_fps, 1.0f / time->deltaTime, time->deltaTime * 500.0f);

    fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.0f, 0.0f, 0.0f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = true;

    char txt[64];
    sprintf(txt, "%i fps", (int)(f_fps + 0.5f));
    fontBuilder.richBuild(txt, false);
    fps->toMesh(fontBuilder, true);

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    MathCore::vec3f pos3D = MathCore::vec3f(
        engine->app->screenRenderWindow->MousePosRelatedToCenter, 
        0.0f
    );

    if (cursorTransform != nullptr)
        cursorTransform->setLocalPosition(pos3D);

    for (size_t i = 0; i < allButtons.size(); i++)
    {
        allButtons[i]->update(pos3D);
    }

    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);

    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, false);

    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::resize(const MathCore::vec2i &size)
{
    for (size_t i = 0; i < allButtons.size(); i++)
    {
        allButtons[i]->resize(size);
    }

    int center_x = size.width >> 1;
    int center_y = size.height >> 1;
    int margin = 32 - 9;

    float font_size = 32.0f / fontBuilder.glFont2.size;

    auto textNode = componentFontToMesh->getTransform();
    textNode->setLocalPosition(MathCore::vec3f(center_x - margin, -center_y + margin, 0));
    textNode->setLocalScale(MathCore::vec3f(font_size, font_size, 1));

    textNode = fps->getTransform();
    textNode->setLocalPosition(MathCore::vec3f(-center_x + margin, -center_y + margin, 0));
    textNode->setLocalScale(MathCore::vec3f(font_size, font_size, 1));
}

bool SceneGUI::anyButtonSelected()
{
    for (size_t i = 0; i < allButtons.size(); i++)
    {
        if (allButtons[i]->selected)
            return true;
    }
    return false;
}

SceneGUI::SceneGUI(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
{

    button_NormalMap = nullptr;
    button_AmbientLight = nullptr;
    button_SunLight = nullptr;
    button_SunLightRotate = nullptr;

    // right
    button_NextScene = nullptr;

    cursorTexture = nullptr;
    cursorTransform = nullptr;
}

SceneGUI::~SceneGUI()
{
    unload();
}
