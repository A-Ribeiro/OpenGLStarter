#include "SceneGUI.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

#include "App.h"

//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources(){
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    cursorTexture = resourceHelper->createTextureFromFile("resources/cursor.png", true && engine->sRGBCapable);

    fontBuilder.load("resources/Roboto-Regular-32.basof2");
}
//to load the scene graph
void SceneGUI::loadGraph(){
    root = new Transform();
    root->setRenderWindowRegion(this->renderWindow);

    Transform *t = root->addChild( new Transform() );
    t->Name = "Main Camera";

    //text transform
    {
        Transform *textNode = root->addChild(new Transform());
        textNode->Name = "fps";
        fps = (AppKit::GLEngine::Components::ComponentFontToMesh*)textNode->addComponent(new Components::ComponentFontToMesh());
        f_fps = 0.0f;
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

    ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

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

    // call resize
    //AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    //resize( MathCore::vec2i::Create(this->renderWindow->Viewport.value.w,this->renderWindow->Viewport.value.h) );
    OnViewportChange(this->renderWindow->Viewport, this->renderWindow->Viewport);

    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }

    this->renderWindow->Viewport.OnChange.add(&SceneGUI::OnViewportChange, this);
}

//clear all loaded scene
void SceneGUI::unloadAll(){
    this->renderWindow->Viewport.OnChange.remove(&SceneGUI::OnViewportChange, this);

    ResourceHelper::releaseTransformRecursive(&root);

    ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    
    texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);
    if (cursorTexture != NULL) {
        texRefCount->remove(cursorTexture);
        cursorTexture = NULL;
    }
}

void SceneGUI::draw() {

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(root, camera, false);
        
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::OnViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue) {

    //printf("OnViewportChange %i %i\n", rect->value.w,rect->value.h);

    MathCore::vec2i size = MathCore::vec2i(value.w,value.h);

    int center_x = size.width >> 1;
    int center_y = size.height >> 1;
    int margin = 32 - 9;


    Transform *textNode = fps->transform[0];
    textNode->setLocalPosition(MathCore::vec3f(-center_x + margin, -center_y + margin, 0));

}

SceneGUI::SceneGUI(App *app, RenderWindowRegion *renderWindow): 
AppKit::GLEngine::SceneBase(&app->time, &app->renderPipeline, &app->resourceHelper) {
    
    this->app = app;
    this->renderWindow = renderWindow;

    cursorTexture = NULL;
    cursorTransform = NULL;

    this->renderWindow->OnUpdate.add(&SceneGUI::OnUpdate, this);
}

SceneGUI::~SceneGUI() {
    this->renderWindow->OnUpdate.remove(&SceneGUI::OnUpdate, this);
    unload();
}

void SceneGUI::OnUpdate(Platform::Time* time) {

    if (time->unscaledDeltaTime > 1.0 / 10000.0f)
        f_fps = MathCore::OP<float>::move(f_fps, 1.0f / time->unscaledDeltaTime, time->unscaledDeltaTime * 100.0f);

    fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.0f, 0.0f, 0.0f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = false;

    char txt[64];
    sprintf(txt, "%i fps", (int)(f_fps + 0.5f));
    fontBuilder.build(txt);
    fps->toMesh(fontBuilder, true);
    fps->transform[0]->setLocalScale(MathCore::vec3f(28.0f / fontBuilder.glFont2.size));


    MathCore::vec3f pos3D = MathCore::vec3f(this->renderWindow->MousePosRelatedToCenter, 0.0f);

    if (cursorTransform != NULL)
        cursorTransform->setLocalPosition(pos3D);

}