#include "SceneGUI.h"
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;


//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources(){
    
    fontBuilder.load("resources/Roboto-Regular-32.basof2");

    button_SoftParticles = new Button( 
        0,// _position, 
        true,// _left, 
        "button_SoftParticles",//_id, 
        "Soft Particles ON",//_text, 
        &fontBuilder//_fontBuilder 
    );

}
//to load the scene graph
void SceneGUI::loadGraph(){
    root = new Transform();

    Transform *t = root->addChild( new Transform() );
    t->Name = "Main Camera";

    t = root->addChild( button_SoftParticles->getTransform() );
    
}
//to bind the resources to the current graph
void SceneGUI::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate

    Transform *mainCamera = root->findTransformByName("Main Camera");
    ComponentCameraOrthographic* componentCameraOrthographic;
    mainCamera->addComponent(camera = componentCameraOrthographic = new ComponentCameraOrthographic());

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    texRefCount->add(&fontBuilder.glFont2.texture);

}

//clear all loaded scene
void SceneGUI::unloadAll(){

    ResourceHelper::releaseTransformRecursive(&root);

    aRibeiro::setNullAndDelete(button_SoftParticles);

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;
    
    texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);

}

void SceneGUI::draw(){

    if (button_SoftParticles != NULL)
        button_SoftParticles->update(Button::App2MousePosition());

    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
    
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, false);
        
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::resize(const sf::Vector2i &size) {
    if (button_SoftParticles != NULL)
        button_SoftParticles->resize(size);
}

SceneGUI::SceneGUI(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) {
    
    button_SoftParticles = NULL;

}

SceneGUI::~SceneGUI() {
    unload();
}
