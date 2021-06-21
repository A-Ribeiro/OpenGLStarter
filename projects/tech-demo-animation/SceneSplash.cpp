#include "SceneSplash.h"
#include <mini-gl-engine/mini-gl-engine.h>

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;


//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneSplash::loadResources(){
    
    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    
    Milky_512_512 = resourceHelper->createTextureFromFile("resources/milkyway_logo_white.png",true && engine->sRGBCapable);

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    texRefCount->add(Milky_512_512);

}
//to load the scene graph
void SceneSplash::loadGraph(){
    root = new Transform();

    Transform *t = root->addChild( new Transform() );
    t->Name = "Main Camera";
//    t->LocalPosition = vec3(0,0,0);

    t = root->addChild( new Transform() );
    t->Name = "Sprite";
}
//to bind the resources to the current graph
void SceneSplash::bindResourcesToGraph(){

    GLRenderState *renderState = GLRenderState::Instance();

    //setup renderstate
    renderState->ClearColor = vec4(0.0f, 0.0f, 0.0f,1.0f);

    Transform *mainCamera = root->findTransformByName("Main Camera");
    ComponentCameraOrthographic* componentCameraOrthographic;
    mainCamera->addComponent(camera = componentCameraOrthographic = new ComponentCameraOrthographic());
    
    componentCameraOrthographic->useSizeY = true;
    componentCameraOrthographic->sizeY = 512.0f * 2.5f;
    
    Transform *spriteTransform = root->findTransformByName("Sprite");
    //spriteTransform->LocalRotation = quatFromEuler(DEG2RAD(-90.0f), 0, 0);

    ComponentMaterial *material;
    spriteTransform->addComponent(material = new ComponentMaterial());
    spriteTransform->addComponent(ComponentMesh::createPlaneXY(512.0f,512.0f) );

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    material->type = MaterialUnlitTexture;
    material->unlit.color = vec4(1.0f);
    material->unlit.blendMode = BlendModeAlpha;
    material->unlit.tex = texRefCount->add(Milky_512_512);
}

//clear all loaded scene
void SceneSplash::unloadAll(){

    ResourceHelper::releaseTransformRecursive(&root);
    

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

    texRefCount->removeNoDelete(Milky_512_512);
    aRibeiro::setNullAndDelete(Milky_512_512);
}

//openglWrapper::GLTexture *Milky_512_512;

SceneSplash::SceneSplash(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) {
    
    Milky_512_512 = NULL;

}

SceneSplash::~SceneSplash() {
    unload();
}
