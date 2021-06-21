#include "SceneGUI.h"
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;


//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources(){
    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    cursorTexture = resourceHelper->createTextureFromFile("resources/cursor.png", true && engine->sRGBCapable);

    fontBuilder.load("resources/Roboto-Regular-32.basof2");


    button_NormalMap = new Button( 
        0,// _position, 
        true,// _left, 
        "button_NormalMap",//_id, 
        "Normal Map ON",//_text, 
        &fontBuilder//_fontBuilder 
    );

    button_AmbientLight = new Button( 
        1,// _position, 
        true,// _left, 
        "button_AmbientLight",//_id, 
        "Ambient Light Skybox",//_text, 
        &fontBuilder//_fontBuilder 
    );

    button_SunLight = new Button( 
        2,// _position, 
        true,// _left, 
        "button_SunLight",//_id, 
        "Sun Light ON",//_text, 
        &fontBuilder//_fontBuilder 
    );

    button_SunLightRotate = new Button( 
        3,// _position, 
        true,// _left, 
        "button_SunLightRotate",//_id, 
        "Sun Light Rotate OFF",//_text, 
        &fontBuilder//_fontBuilder 
    );

    //right
    button_NextScene = new Button( 
        0,// _position, 
        false,// _left, 
        "button_NextScene",//_id, 
        "Next Scene",//_text, 
        &fontBuilder//_fontBuilder 
    );


    allButtons.push_back(button_NormalMap);
    allButtons.push_back(button_AmbientLight);
    allButtons.push_back(button_SunLight);
    allButtons.push_back(button_SunLightRotate);
    allButtons.push_back(button_NextScene);

}
//to load the scene graph
void SceneGUI::loadGraph(){
    root = new Transform();

    Transform *t = root->addChild( new Transform() );
    t->Name = "Main Camera";

    for (size_t i = 0; i < allButtons.size(); i++) {
        t = root->addChild(allButtons[i]->getTransform());
    }

    {
        cursorTransform = root->addChild(new Transform());
    }
    
}
//to bind the resources to the current graph
void SceneGUI::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate

    Transform *mainCamera = root->findTransformByName("Main Camera");
    ComponentCameraOrthographic* componentCameraOrthographic;
    mainCamera->addComponent(camera = componentCameraOrthographic = new ComponentCameraOrthographic());

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    {
        ComponentMaterial *cursorMaterial;
        cursorTransform->addComponent(cursorMaterial = new ComponentMaterial());
        cursorTransform->addComponent(ComponentMesh::createPlaneXY(cursorTexture->width, cursorTexture->height));

        cursorMaterial->type = MaterialUnlitTexture;
        cursorMaterial->unlit.blendMode = BlendModeAlpha;
        cursorMaterial->unlit.tex = texRefCount->add(cursorTexture);
    }

    texRefCount->add(&fontBuilder.glFont2.texture);

    texRefCount->add(cursorTexture);

}

//clear all loaded scene
void SceneGUI::unloadAll(){

    ResourceHelper::releaseTransformRecursive(&root);

    allButtons.clear();

    aRibeiro::setNullAndDelete(button_NormalMap);
    aRibeiro::setNullAndDelete(button_AmbientLight);
    aRibeiro::setNullAndDelete(button_SunLight);
    aRibeiro::setNullAndDelete(button_SunLightRotate);
    aRibeiro::setNullAndDelete(button_NextScene);

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;
    
    texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);

    texRefCount->remove(cursorTexture);


}

void SceneGUI::draw() {

    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    aRibeiro::vec3 pos3D = aRibeiro::vec3(engine->app->MousePosRelatedToCenter, 0.0f);

    if (cursorTransform != NULL)
        cursorTransform->setLocalPosition(pos3D);

    for(size_t i =0;i< allButtons.size();i++) {
        allButtons[i]->update(pos3D);
    }

    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
    
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, false);
        
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::resize(const sf::Vector2i &size) {
    for(size_t i =0;i< allButtons.size();i++) {
        allButtons[i]->resize(size);
    }
}

bool SceneGUI::anyButtonSelected() {
    for(size_t i =0;i< allButtons.size();i++) {
        if (allButtons[i]->selected)
            return true;
    }
    return false;
}

SceneGUI::SceneGUI(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) {
    
    button_NormalMap = NULL;
    button_AmbientLight = NULL;
    button_SunLight = NULL;
    button_SunLightRotate = NULL;

    //right
    button_NextScene = NULL;

    cursorTexture = NULL;
    cursorTransform = NULL;

}

SceneGUI::~SceneGUI() {
    unload();
}
