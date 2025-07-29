#include "SceneGUI.h"

#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

#include "App.h"

#include "./ImGui/ImGuiManager.h"
#include "./InnerViewport.h"
#include "./Scene3D.h"

//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneGUI::loadResources(){
    auto engine = AppKit::GLEngine::Engine::Instance();

    cursorTexture = resourceHelper->createTextureFromFile("resources/cursor.png", true && engine->sRGBCapable);

    fontBuilder.load("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);
}
//to load the scene graph
void SceneGUI::loadGraph(){
    root = Transform::CreateShared();
    root->affectComponentStart = true;
    root->setRenderWindowRegion(this->renderWindow);

    auto t = root->addChild( Transform::CreateShared() );
    t->Name = "Main Camera";

    //text transform
    {
        auto textNode = root->addChild(Transform::CreateShared());
        textNode->Name = "fps";
        fps = textNode->addNewComponent<AppKit::GLEngine::Components::ComponentFontToMesh>();
        f_fps = 0.0f;
    }

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

    // ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    {
        auto cursorMaterial = cursorTransform->addNewComponent<ComponentMaterial>();
        cursorTransform->addComponent(ComponentMesh::createPlaneXY(cursorTexture->width, cursorTexture->height));

        cursorMaterial->setShader(resourceMap->shaderUnlitTextureAlpha);
        //cursorMaterial->property_bag.getProperty("BlendMode").set((int)AppKit::GLEngine::BlendModeAlpha);
        cursorMaterial->property_bag.getProperty("uTexture").set<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>(cursorTexture);

        // cursorMaterial->type = MaterialUnlitTexture;
        // cursorMaterial->unlit.blendMode = BlendModeAlpha;
        // cursorMaterial->unlit.tex = cursorTexture;
    }

    //texRefCount->add(&fontBuilder.glFont2.texture);

    //texRefCount->add(cursorTexture);

    // call resize
    //AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    //resize( MathCore::vec2i::Create(this->renderWindow->Viewport.value.w,this->renderWindow->Viewport.value.h) );
    OnViewportChange(this->renderWindow->CameraViewport, this->renderWindow->CameraViewport);

    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }

    this->renderWindow->CameraViewport.OnChange.add(&SceneGUI::OnViewportChange, this);
}

//clear all loaded scene
void SceneGUI::unloadAll(){
    this->renderWindow->CameraViewport.OnChange.remove(&SceneGUI::OnViewportChange, this);

    root = nullptr;
    camera = nullptr;
    //ResourceHelper::releaseTransformRecursive(&root);

    //ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    
    //texRefCount->removeNoDelete(&fontBuilder.glFont2.texture);

    // if (cursorTexture != nullptr) {
    //     //texRefCount->remove(cursorTexture);
    //     cursorTexture = nullptr;
    // }

    cursorTexture = nullptr;
    cursorTransform = nullptr;

}

void SceneGUI::draw() {

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    if (engine->sRGBCapable)
        glDisable(GL_FRAMEBUFFER_SRGB);
    
    GLRenderState *state = GLRenderState::Instance();
    state->DepthTest = DepthTestDisabled;
    renderPipeline->runSinglePassPipeline(resourceMap, root, camera, false);
        
    if (engine->sRGBCapable)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

void SceneGUI::OnViewportChange(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue) {

    //printf("OnViewportChange %i %i\n", rect->value.w,rect->value.h);

    MathCore::vec2i size = MathCore::vec2i(value.w,value.h);

    int center_x = size.width >> 1;
    int center_y = size.height >> 1;
    int margin = 32 - 9;


    auto textNode = fps->getTransform();
    textNode->setLocalPosition(MathCore::vec3f(-center_x + margin, -center_y + margin, 0));

}

SceneGUI::SceneGUI(App *app, std::shared_ptr<RenderWindowRegion> renderWindow): 
AppKit::GLEngine::SceneBase(&app->time, &app->renderPipeline, &app->resourceHelper, &app->resourceMap, renderWindow) {
    
    this->app = app;
    this->renderWindow = renderWindow;

    cursorTexture = nullptr;
    cursorTransform = nullptr;

    this->renderWindow->OnUpdate.add(&SceneGUI::OnUpdate, this);
}

SceneGUI::~SceneGUI() {
    this->renderWindow->OnUpdate.remove(&SceneGUI::OnUpdate, this);
    unload();
}

void SceneGUI::OnUpdate(Platform::Time* time) {

    if (time->unscaledDeltaTime > 1.0 / 10000.0f)
        f_fps = MathCore::OP<float>::move(f_fps, 1.0f / time->unscaledDeltaTime, time->unscaledDeltaTime * 100.0f);

    //fontBuilder.faceColor = MathCore::vec4f(1, 1, 1, 1);
    fontBuilder.faceColor = MathCore::vec4f(0.f, 0.f, 0.f, 1);
    //fontBuilder.strokeColor = MathCore::vec4f(0.3f, 0.3f, 0.3f, 1);
    fontBuilder.strokeColor = MathCore::vec4f(0.2f, 0.2f, 0.2f, 1);
    fontBuilder.horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
    fontBuilder.verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    fontBuilder.strokeOffset = MathCore::vec3f(0, 0, -0.001f);
    fontBuilder.drawFace = true;
    fontBuilder.drawStroke = true;

    char txt[64];
    sprintf(txt, "%i fps", (int)(f_fps + 0.5f));
    fontBuilder.richBuild(txt, false);
    fps->toMesh(resourceMap, fontBuilder, true);
    
    auto fps_transform = fps->getTransform();
    fps_transform->setLocalScale(MathCore::vec3f(40.0f / fontBuilder.glFont2.size));


    //MathCore::vec3f pos3Dnorm = MathCore::vec3f(this->renderWindow->MousePosRelatedToCenterNormalized, 0.0f);
    //printf("%f %f %f\n", pos3Dnorm.x, pos3Dnorm.y, pos3Dnorm.z);

    MathCore::vec3f pos3D = MathCore::vec3f(this->renderWindow->MousePosRelatedToCenter * this->renderWindow->windowToCameraScale, 0.0f);
    if (cursorTransform != nullptr)
        cursorTransform->setLocalPosition(pos3D);

    // auto window_rect = this->renderWindow->WindowViewport.c_val();
    // auto mouse_relative_to_window = this->renderWindow->MousePos.c_val();
    // if (window_rect.contains(mouse_relative_to_window)){
    //     printf("inside...\n");
    // } else {
    //     printf("not inside...\n");
    // }


    cursorTransform->skip_traversing = ImGuiManager::Instance()->innerViewport->scene3D->is_to_hide_mouse.pressed;


}
