#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
#include "App.h"
#include "CustomComponents/ComponentFrustumVisibleSetColor.h"



Transform* loadSceneTestCollision()
{
    Transform* _0 = new Transform();
    _0->Name = std::string("TestCollision");
    _0->LocalPosition = vec3(0,0,0);
    _0->LocalRotation = quat(0,0,0,1);
    _0->LocalScale = vec3(1,1,1);
    {
        Transform* _1 = _0->addChild(new Transform());
        _1->Name = std::string("Main Camera");
        _1->LocalPosition = vec3(0,1.84,-2.12);
        _1->LocalRotation = quat(0.3571975,0,0,0.9340289);
        _1->LocalScale = vec3(1,1,1);
    }
    {
        Transform* _2 = _0->addChild(new Transform());
        _2->Name = std::string("Camera1");
        _2->LocalPosition = vec3(0,0,0);
        _2->LocalRotation = quat(0,0,0,1);
        _2->LocalScale = vec3(1,1,1);
        {
            Transform* _3 = _2->addChild(new Transform());
            _3->Name = std::string("CubeCollision");
            _3->LocalPosition = vec3(-1.5,0,0);
            _3->LocalRotation = quat(0.2044709,-0.6768985,0.204471,0.6768985);
            _3->LocalScale = vec3(0.32,0.32,1);
        }
        {
            Transform* _4 = _2->addChild(new Transform());
            _4->Name = std::string("SphereCollision");
            _4->LocalPosition = vec3(1.5,0,0);
            _4->LocalRotation = quat(0,-0.7071068,0,0.7071068);
            _4->LocalScale = vec3(1,1,1);
        }
    }
    {
        Transform* _5 = _0->addChild(new Transform());
        _5->Name = std::string("Cube");
        _5->LocalPosition = vec3(0,0,1.5);
        _5->LocalRotation = quat(0,0,0,1);
        _5->LocalScale = vec3(1,1,1);
    }
    {
        Transform* _6 = _0->addChild(new Transform());
        _6->Name = std::string("RedSphere");
        _6->LocalPosition = vec3(0,0,-1.5);
        _6->LocalRotation = quat(-0.4548343,0.327839,0.1847353,0.8071681);
        _6->LocalScale = vec3(1,1,1);
    }
    {
        Transform* _6 = _0->addChild(new Transform());
        _6->Name = std::string("RedSphere");
        _6->LocalPosition = vec3(1.5,0,1.5);
        _6->LocalRotation = quat(-0.4548343,0.327839,0.1847353,0.8071681);
        _6->LocalScale = vec3(1,1,1);
    }
    return _0;
}



App::App()
{
    //forward app reference that could be used by newly created components
    Engine::Instance()->app = this;
    
    renderState = GLRenderState::Instance();
    shaderColor = new GLShaderColor();

    //setup renderstate
    renderState->ClearColor = vec4(1.0f,1.0f,250.0f/255.0f,1.0f);
    renderState->FrontFace = FrontFaceCW;
    #ifndef ARIBEIRO_RPI
    renderState->Wireframe = WireframeBack;
    renderState->CullFace = CullFaceNone;
    #else
    renderState->CullFace = CullFaceBack;
    #endif
    
    // Construct scene graph
    root = loadSceneTestCollision();

    Transform *mainCameraTransform = root->findTransformByName("Main Camera");
    mainCameraTransform->addComponent(cameraPerspective = new ComponentCameraPerspective());
    mainCameraTransform->addComponent(new ComponentCameraMove());

    //cameraPerspective->rightHanded = true;

    ComponentFrustumCulling *frustumAux;

    cube = root->findTransformByName("Cube");
    ComponentColorMesh *colorMeshAux;
    cube->addComponent(colorMeshAux =
                       ComponentColorMesh::createBox(vec4(0.0f,0.5f,0.0f,1.0f), vec3(1.0f,1.0f,1.0f)));
    colorMeshAux->syncVBOStatic();
    cube->addComponent(frustumAux = ComponentFrustumCulling::createShapeAABB(collision::AABB(vec3(-0.5f), vec3(0.5f))));
    frustumAux->debugCollisionShapes = true;
    cube->addComponent(new ComponentFrustumVisibleSetColor());


    std::vector<Transform*> spheres = root->findTransformsByName("RedSphere");
    if (spheres.size() > 0){
        for (int i = 0; i < spheres.size(); i++) {
            
            ComponentColorMesh* sphereModel = ComponentColorMesh::createSphere(vec4(0.5f,0.0f,0.0f,1.0f), 0.5f,16,16);
            sphereModel->syncVBOStatic();
            
            
            comps.add(spheres[i]->addComponent(sphereModel));
            spheres[i]->addComponent(frustumAux = ComponentFrustumCulling::createShapeSphere(vec3(0),0.5f));
            frustumAux->debugCollisionShapes = true;
            spheres[i]->addComponent(new ComponentFrustumVisibleSetColor());

            spheres[i]->LocalScale = vec3(1, 2, 1);
        }
    }

    spheres = root->findTransformsByName("MagentaSphere");
    if (spheres.size() > 0){
        ComponentColorMesh* sphereModel = ComponentColorMesh::createSphere(vec4(0.5f,0.0f,0.5f,1.0f), 0.5f,16,16);
        sphereModel->syncVBOStatic();
        for (int i=0;i<spheres.size();i++)
            comps.add(spheres[i]->addComponent(sphereModel));
    }

    root->findTransformByName("CubeCollision")->addComponent(ComponentColorMesh::createBox(vec4(0.0f,0.5f,0.0f,1.0f), vec3(1.0f,1.0f,1.0f)));

    root->findTransformByName("SphereCollision")->addComponent(ComponentColorMesh::createSphere(vec4(0.0f,0.0f,0.5f,1.0f), 0.5f,16,16));


    ComponentCameraPerspective* cameraTest;
    camera1 = root->findTransformByName("Camera1");
    camera1->addComponent(cameraTest = new ComponentCameraPerspective());

    //cameraTest->rightHanded = true;
    cameraTest->nearPlane = 0.5f;
    cameraTest->farPlane = 2.5f;
    cameraTest->addLinesComponent();

    //camera1->lookAtLeftHanded(root->findTransformByName("RedSphere"));


    //
    // Testing bounding sphere over frustum
    //

    Transform *newNode = camera1->addChild(new Transform());

    collision::Frustum frustum(cameraTest->projection);
    collision::Sphere sphere = collision::Sphere::fromFrustum(frustum);

    newNode->addComponent(frustumAux = ComponentFrustumCulling::createShapeSphere(
        sphere.center, sphere.radius
    ));
    frustumAux->debugCollisionShapes = true;

    ComponentColorLine *lines = (ComponentColorLine *)newNode->addComponent(new ComponentColorLine());
    lines->color = vec4(0, 1, 1, 1);

    AppBase::OnGainFocus.add(this, &App::onGainFocus);
    AppBase::WindowSize.OnChange.add(this, &App::onWindowSizeChange);

    time.update();
}

App::~App(){
    setNullAndDelete(shaderColor);
    
    root->traversePostOrder_DepthFirst(TransformTraverseMethod(this, &App::traverse_delete));
    root = NULL;
}

void App::draw() {
    time.update();

    //set min delta time (the passed time or the time to render at 24fps)
    time.deltaTime = minimum(time.deltaTime,1.0f/24.0f);

    //printf("%f\n",1.0f/time.deltaTime);
    OnPreUpdate(&time);
    OnUpdate(&time);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Update Nodes
    vec3 euler = camera1->LocalEuler;
    euler.y += time.deltaTime * DEG2RAD(10.0f);
    camera1->LocalEuler = euler;

    static float f = 0;
    f = fmod(f + time.deltaTime*DEG2RAD(30.0f), DEG2RAD(360));

    vec3 p = camera1->LocalRotation * vec3(0,0,1) * cos(f) * 4.0f;

    camera1->LocalPosition = p;

    cube->LocalRotation = quatFromEuler(f*2.0f, f,0);

    OnLateUpdate(&time);

    // Draw
    render_pipeline();
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        exitApp();
    
}

void App::onGainFocus() {
    //printf("GainFocus...\n");

    MousePos = MousePosCenter;//set app state do cursor center
    moveMouseToScreenCenter();//queue update to screen center

    time.update();
}

void App::onWindowSizeChange(Property<sf::Vector2i> *prop) {
    renderState->Viewport = GLEngine::iRect(prop->value.x, prop->value.y);
}

struct FrustumParameter{
    ComponentCameraPerspective *camera;
    collision::Frustum *frustum;
};

void App::render_pipeline() {
    // call start for new components
    StartEventManager::Instance()->processAllComponentsWithTransform();

    renderState->CurrentShader = shaderColor;

    // scene update transforms
    root->resetVisited();
    root->preComputeTransforms();

    // precompute visible camera matrixes
    cameraPerspective->precomputeViewProjection(true);

    //traverse to render
    root->traversePreOrder_DepthFirst(TransformTraverseMethod(this,&App::traverse_render));

    // Frustum processing
    ComponentCameraPerspective *cameraComponent = (ComponentCameraPerspective *)camera1->findComponent(ComponentCameraPerspective::Type);
    cameraComponent->precomputeViewProjection(true);
    collision::Frustum frustum(cameraComponent->projection,cameraComponent->view);
    
    FrustumParameter parameter = {cameraComponent,&frustum};
    
    root->traversePreOrder_DepthFirst(TransformTraverseMethod(this,&App::traverse_frustum), &parameter);

}

bool App::traverse_render(Transform *element, void* userData) {
    
    bool alreadySetupMatrix = false;
    
    for(int i=0;i<element->getComponentCount();i++){
        Component* component = element->getComponentAt(i);
        
        // Setup Transform Matrix
        if ((
             component->compareType( ComponentColorMesh::Type ) ||
             component->compareType( ComponentColorLine::Type )
             )
            &&
            !alreadySetupMatrix){
            alreadySetupMatrix = true;
            mat4 *mvp;
            mat4 *mv;
            mat4 *mvIT;
            mat4 *mvInv;
            element->computeRenderMatrix(cameraPerspective->viewProjection,
                                         cameraPerspective->view,
                                         cameraPerspective->viewIT,
                                         cameraPerspective->viewInv,
                                         &mvp,
                                         &mv,
                                         &mvIT,
                                         &mvInv);
            
            shaderColor->setMatrix(*mvp);
        }
        
        // render object
        if (component->compareType( ComponentColorMesh::Type )){
            ComponentColorMesh* colorMesh = (ComponentColorMesh*) component;
            shaderColor->setColor( colorMesh->color );
            colorMesh->setLayoutPointers(GLShaderColor::vPosition);
            colorMesh->draw();
            colorMesh->unsetLayoutPointers(GLShaderColor::vPosition);
        } else if (component->compareType( ComponentColorLine::Type )){
            ComponentColorLine* lines = (ComponentColorLine*)component;
            renderState->LineWidth = lines->width;
            shaderColor->setColor( lines->color );
            lines->setLayoutPointers(GLShaderColor::vPosition);
            lines->draw();
            lines->unsetLayoutPointers(GLShaderColor::vPosition);
        }
    }
    
    return true;
}

bool App::traverse_frustum(Transform *element, void *userData) {

    ComponentFrustumCulling *frustumCulling = (ComponentFrustumCulling *)element->findComponent(ComponentFrustumCulling::Type);

    if (frustumCulling != NULL) {
        FrustumParameter *parameter = (FrustumParameter *)userData;
        if (frustumCulling->cullingShape == CullingShapeSphere) {
            frustumCulling->setVisibilityFromCamera(parameter->camera, collision::Frustum::sphereOverlapsFrustum(frustumCulling->sphere, *parameter->frustum));
        }
        else if (frustumCulling->cullingShape == CullingShapeAABB) {
            frustumCulling->setVisibilityFromCamera(parameter->camera, collision::Frustum::aabbOverlapsFrustum(frustumCulling->aabb, *parameter->frustum));
        }
    }
    
    return true;
}

bool App::traverse_delete(Transform *element, void* userData) {
    
    Transform *parent = element->Parent;
    if (parent != NULL) {
        ARIBEIRO_ABORT( parent->getChildAt( parent->getChildCount() - 1 ) != element, "Wrong traverse setup..." );
        parent->removeChild( parent->getChildCount() - 1 );
    }
    
    for(int i = element->getComponentCount() - 1 ; i>=0 ; i-- ){
        Component* component = element->removeComponentAt(i);
        comps.remove(component);
    }
    
    delete element;
    return true;
}
