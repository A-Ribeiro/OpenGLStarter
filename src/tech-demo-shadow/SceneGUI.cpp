#include "SceneGUI.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources(){
    AppKit::GLEngine::Engine* engine = AppKit::GLEngine::Engine::Instance();

    cursorTexture = resourceHelper->createTextureFromFile("resources/cursor.png", true && engine->sRGBCapable);

    fontBuilder.load("resources/Roboto-Regular-32.basof2");

    /*
    button_SoftParticles = new Button( 
        0,// _position, 
        true,// _left, 
        "button_SoftParticles",//_id, 
        "Soft Particles ON",//_text, 
        &fontBuilder//_fontBuilder 
    ); */

    button_SoftParticles = nullptr;

}
//to load the scene graph
void SceneGUI::loadGraph(){
    root = Transform::CreateShared();

    auto t = root->addChild( Transform::CreateShared() );
    t->Name = "Main Camera";

    //t = root->addChild( button_SoftParticles->getTransform() );

    {
        cursorTransform = root->addChild(Transform::CreateShared());
    }
    
}
//to bind the resources to the current graph
void SceneGUI::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate

    auto mainCamera = root->findTransformByName("Main Camera");
    std::shared_ptr<ComponentCameraOrthographic> componentCameraOrthographic;
    camera = componentCameraOrthographic = mainCamera->addNewComponent<ComponentCameraOrthographic>();

    //ReferenceCounter<AppKit::OpenGL::GLTexture*>* texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    {
        auto cursorMaterial = cursorTransform->addNewComponent<ComponentMaterial>();
        cursorTransform->addComponent(ComponentMesh::createPlaneXY(cursorTexture->width, cursorTexture->height));

        cursorMaterial->type = MaterialUnlitTexture;
        cursorMaterial->unlit.blendMode = BlendModeAlpha;
        cursorMaterial->unlit.tex = cursorTexture;
    }

    //texRefCount->add(&fontBuilder.glFont2.texture);
    
    //texRefCount->add(cursorTexture);


    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

//clear all loaded scene
void SceneGUI::unloadAll(){

    //ResourceHelper::releaseTransformRecursive(&root);
    root = nullptr;
    camera = nullptr;

    if (button_SoftParticles != nullptr){
        delete button_SoftParticles;
        button_SoftParticles = nullptr;
    }

    //ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    //texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);
    //texRefCount->remove(cursorTexture);
    cursorTexture = nullptr;
    cursorTransform = nullptr;

}

void SceneGUI::draw(){
    AppKit::GLEngine::Engine* engine = AppKit::GLEngine::Engine::Instance();

    MathCore::vec3f pos3D = MathCore::vec3f(
        engine->app->screenRenderWindow->MousePosRelatedToCenter, 
        0.0f
    );

    if (cursorTransform != nullptr)
        cursorTransform->setLocalPosition(pos3D);

    if (button_SoftParticles != nullptr)
        button_SoftParticles->update(pos3D);
        //button_SoftParticles->update(Button::App2MousePosition());

    
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, false);
        
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::resize(const MathCore::vec2i&size) {
    if (button_SoftParticles != nullptr)
        button_SoftParticles->resize(size);
}

SceneGUI::SceneGUI(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow)
{
    
    button_SoftParticles = nullptr;

    cursorTexture = nullptr;
    cursorTransform = nullptr;
}

SceneGUI::~SceneGUI() {
    unload();
}
