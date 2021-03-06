#include "ScenePalace.h"

#include <mini-gl-engine/mini-gl-engine.h>

#include "components/SkinnedMesh.h"

#include "components/RotatingCross.h"
#include "components/AnimationMotion.h"
#include "components/ThirdPersonPlayerController.h"
#include "components/ThirdPersonCamera.h"


#include "App.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

static Transform* loadSceneroot();

static bool ReplaceMaterial(Transform *element, void* userData) {
    
    ComponentMaterial *material = (ComponentMaterial *)element->findComponent(Components::ComponentMaterial::Type);
    
    if ( material != NULL ){
        ComponentMaterial *newMaterial = (ComponentMaterial *)userData;
        
        ReferenceCounter<Component*> *compRefCount = &GLEngine::Engine::Instance()->componentReferenceCounter;
        
        Component * componentMaterial = element->removeComponent(material);
        compRefCount->remove(componentMaterial);
        
        element->addComponent(compRefCount->add(newMaterial));

		//little optimization
		element->makeFirstComponent(newMaterial);
    }
    
    return true;
}


//to load skybox, textures, cubemaps, 3DModels and setup materials
void ScenePalace::loadResources() {
	
    GLEngine::Engine *engine = GLEngine::Engine::Instance();

	Texture_Guard[0] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__diffuse.jpg",true && engine->sRGBCapable);
	Texture_Guard[1] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__normal.jpg",false);
    Texture_Guard[2] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__specular.jpg", false);

	Model_Palace = resourceHelper->createTransformFromModel("resources/palace/colonnato.min.bams");
    Model_Cross = resourceHelper->createTransformFromModel("resources/palace/cross.bams");
    //Model_Guard = resourceHelper->createTransformFromModel("resources/castle_guard/castle_guard_01.bams");

    skinnedMesh = new GLEngine::Components::SkinnedMesh( resourceHelper, true );
    skinnedMesh->loadModelBase("resources/castle_guard/castle_guard_01.bams");

    skinnedMesh->loadAnimation("idle","resources/castle_guard/castle_guard_01@Idle.bams");
    skinnedMesh->loadAnimation("walk", "resources/castle_guard/castle_guard_01@Walking.bams");
    skinnedMesh->loadAnimation("run", "resources/castle_guard/castle_guard_01@Run.bams");
    
    skinnedMesh->done();

    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;
    
    ComponentMaterial * newMaterial;
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 1, 1, 1);
    newMaterial->pbr.metallic = 1.0f;
    newMaterial->pbr.roughness = 0.5f;
    newMaterial->pbr.texAlbedo = texRefCount->add(Texture_Guard[0]);
    newMaterial->pbr.texNormal = texRefCount->add(Texture_Guard[1]);
    newMaterial->pbr.texSpecular = texRefCount->add(Texture_Guard[2]);
    
    //Model_Guard->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);

    skinnedMesh->model_base->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 0.9814019f, 0.7490196f, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    //newMaterial->pbr.texAlbedo = texRefCount->add(Rock02Textures[0]);
    //newMaterial->pbr.texNormal = texRefCount->add(Rock02Textures[1]);
    newMaterial->pbr.albedoColor = ResourceHelper::vec4ColorGammaToLinear(newMaterial->pbr.albedoColor);
    
    Model_Cross->findTransformByName("circle")->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 0.8521127f, 0.7464789f, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    //newMaterial->pbr.texAlbedo = texRefCount->add(Rock03Textures[0]);
    //newMaterial->pbr.texNormal = texRefCount->add(Rock03Textures[1]);
    newMaterial->pbr.albedoColor = ResourceHelper::vec4ColorGammaToLinear(newMaterial->pbr.albedoColor);
    
    Model_Cross->findTransformByName("cross")->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
}

//to load the scene graph
void ScenePalace::loadGraph() {
    root = loadSceneroot();
}

//to bind the resources to the current graph
void ScenePalace::bindResourcesToGraph() {
    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    
    //setup ambient color
    {
        GLRenderState *renderState = GLRenderState::Instance();
        //setup renderstate
        renderState->ClearColor = vec4(0.2784314f, 0.2784314f, 0.2784314f,1.0f);

        //renderPipeline->ambientLight.color = vec3(0.2641509f,0.2641509f,0.2641509f) * 2.0f;
        renderPipeline->ambientLight.color = vec3(0.2641509f, 0.2641509f, 0.2641509f);
    }
    
    //setup camera
    {
        Transform *mainCamera = root->findTransformByName("Main Camera");
        mainCamera->addComponent(camera = new ComponentCameraPerspective());
        
        ((ComponentCameraPerspective*)camera)->fovDegrees = 60.0f;
		((ComponentCameraPerspective*)camera)->nearPlane = 0.1f;
		((ComponentCameraPerspective*)camera)->farPlane = 30.0f;

        Transform *camera_look = root->findTransformByName("Camera-look");
        
        mainCamera->lookAtLeftHanded(camera_look);

        //ThirdPersonCamera
        {
            mainCamera->addComponent(thirdPersonCamera = new ThirdPersonCamera());
            thirdPersonCamera->Target = camera_look;
        }

        /*
		//componentCameraRotateOnTarget
		{
            ComponentCameraRotateOnTarget *componentCameraRotateOnTarget;
			mainCamera->addComponent(componentCameraRotateOnTarget = new ComponentCameraRotateOnTarget());
			componentCameraRotateOnTarget->Target = camera_look;
		}
        */

    }
    
    //light
    {
		Transform *lightTransform;
        ComponentLight *light;

        lightTransform = root->findTransformByName("Directional Light (1)");
        light = (ComponentLight *)lightTransform->addComponent(new ComponentLight());
        light->type = LightSun;
        light->sun.color = aRibeiro::vec3(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);
        
        
		lightTransform = root->findTransformByName("Directional Light (2)");
        light = (ComponentLight *)lightTransform->addComponent(new ComponentLight());
        light->type = LightSun;
        light->sun.color = aRibeiro::vec3(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);

		lightTransform = root->findTransformByName("Directional Light (3)");
        light = (ComponentLight *)lightTransform->addComponent(new ComponentLight());
        light->type = LightSun;
        light->sun.color = aRibeiro::vec3(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);
        //*/

        //if (engine->sRGBCapable)
            //light->sun.color = ResourceHelper::vec3ColorLinearToGamma(light->sun.color);
    }
    
    //3DModel
    {
        Transform *node;

		node = root->findTransformByName("static_model");
        node->addChild(ResourceHelper::cloneTransformRecursive(Model_Palace));

		node = root->findTransformByName("RotatingCross");
        node->addChild(ResourceHelper::cloneTransformRecursive(Model_Cross));
        node->addComponent(new RotatingCross());

        node = root->findTransformByName("player");
        //node->addChild(ResourceHelper::removeEmptyTransforms(ResourceHelper::cloneTransformRecursive(Model_Guard)));
        node->addComponent(skinnedMesh);
        skinnedMesh->moveMeshToTransform();

        node->addComponent(animationMotion = new AnimationMotion());

        animationMotion->motionInfluence.push_back( ClipMotionInfluence("idle",0,0,0) );
        animationMotion->motionInfluence.push_back( ClipMotionInfluence("walk",0,0,1) );
        animationMotion->motionInfluence.push_back( ClipMotionInfluence("run",0,0,1) );

        node->addComponent(thirdPersonPlayerController = new ThirdPersonPlayerController());

    }
}

//clear all loaded scene
void ScenePalace::unloadAll() {
    ResourceHelper::releaseTransformRecursive(&root);
    ResourceHelper::releaseTransformRecursive(&Model_Palace);
	ResourceHelper::releaseTransformRecursive(&Model_Cross);
	//ResourceHelper::releaseTransformRecursive(&Model_Guard);
    //aRibeiro::setNullAndDelete(skinnedMesh);
}

ScenePalace::ScenePalace(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) 
{
    Model_Palace = NULL;
    Model_Cross = NULL;
    //Model_Guard = NULL;
    skinnedMesh = NULL;
    animationMotion = NULL;
    thirdPersonPlayerController = NULL;
    thirdPersonCamera = NULL;
    Texture_Guard[0] = NULL;
	Texture_Guard[1] = NULL;
    Texture_Guard[2] = NULL;
}

ScenePalace::~ScenePalace() {
    unload();
}

void ScenePalace::draw(){
    //Transform *node = root->findTransformByName("rock03 (test)");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * DEG2RAD(30.0f), 0);
    
    //Transform *node = root->findTransformByName("Directional Light");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * DEG2RAD(30.0f), 0);

    //App* app = (App*)GLEngine::Engine::Instance()->app;

	/*
	keyP.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::P));

    if (keyP.down)
        time->timeScale = 1.0f - time->timeScale;

	mouseBtn1.setState(app->mousePressed);
	*/

    /*
    if (mouseBtn1.down) {
        if (app->sceneGUI->button_SoftParticles->selected) {

			componentCameraRotateOnTarget->enabled = false;

			Transform *node = root->findTransformByName("Particle System");
			ComponentParticleSystem *particleSystem = (ComponentParticleSystem*)node->findComponent(ComponentParticleSystem::Type);
			
			particleSystem->soft = !particleSystem->soft;
            if (particleSystem->soft)
                app->sceneGUI->button_SoftParticles->updateText("Soft Particles ON");
            else
                app->sceneGUI->button_SoftParticles->updateText("Soft Particles OFF");
        }
        else {
			componentCameraRotateOnTarget->enabled = true;
        }
    }
    */

    /*
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        animationMotion->TriggerClip("idle");
        //skinnedMesh->mixer.play("idle");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        animationMotion->TriggerClip("walk");
        //skinnedMesh->mixer.play("walk");
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        animationMotion->TriggerClip("run");
        //skinnedMesh->mixer.play("run");
    }
    */

    SceneBase::draw();
}

static Transform* loadSceneroot()
{
	Transform* _0 = new Transform();
	_0->Name = std::string("root");
	_0->LocalPosition = vec3(0,0,0);
	_0->LocalRotation = quat(0,0,0,1);
	_0->LocalScale = vec3(1,1,1);
	{
		Transform* _1 = _0->addChild(new Transform());
		_1->Name = std::string("Main Camera");
		_1->LocalPosition = vec3(0,2.475,-2.268);
		_1->LocalRotation = quat(0.2011371,0.005232482,-0.001074419,0.9795486);
		_1->LocalScale = vec3(1,1,1);
	}
	{
		Transform* _2 = _0->addChild(new Transform());
		_2->Name = std::string("player");
		_2->LocalPosition = vec3(0,0,0);
		_2->LocalRotation = quat(0,0,0,1);
		_2->LocalScale = vec3(1,1,1);
		{
			Transform* _3 = _2->addChild(new Transform());
			_3->Name = std::string("Camera-look");
			//_3->LocalPosition = vec3(0.015,1.491,0.025);
            _3->LocalPosition = vec3(0, 1.291, 0.025);
			_3->LocalRotation = quat(0,0,0,1);
			_3->LocalScale = vec3(1,1,1);
		}
	}
	{
		Transform* _4 = _0->addChild(new Transform());
		_4->Name = std::string("Lights");
		_4->LocalPosition = vec3(-1.133002,0.5275524,-4.254147);
		_4->LocalRotation = quat(0,0,0,1);
		_4->LocalScale = vec3(1,1,1);
		{
			Transform* _5 = _4->addChild(new Transform());
			_5->Name = std::string("Directional Light (1)");
			_5->LocalPosition = vec3(1.133002,2.472448,4.254147);
			_5->LocalRotation = quat(0.4082179,-0.2345697,0.1093816,0.8754261);
			_5->LocalScale = vec3(1,1,1);
		}
		{
			Transform* _6 = _4->addChild(new Transform());
			_6->Name = std::string("Directional Light (2)");
			_6->LocalPosition = vec3(2.853002,2.572448,1.264147);
			_6->LocalRotation = quat(0.3147533,0.6047991,-0.2820223,0.6749904);
			_6->LocalScale = vec3(1,1,1);
		}
		{
			Transform* _7 = _4->addChild(new Transform());
			_7->Name = std::string("Directional Light (3)");
			_7->LocalPosition = vec3(2.853002,2.572448,1.264147);
			_7->LocalRotation = quat(-0.1208431,0.8684675,-0.404973,-0.2591489);
			_7->LocalScale = vec3(1,1,1);
		}
	}
	{
		Transform* _8 = _0->addChild(new Transform());
		_8->Name = std::string("Ground");
		_8->LocalPosition = vec3(0,0,0);
		_8->LocalRotation = quat(0,0,0,1);
		_8->LocalScale = vec3(27.03765,27.03765,27.03765);
		{
			Transform* _9 = _8->addChild(new Transform());
			_9->Name = std::string("Cube");
			_9->LocalPosition = vec3(0,-0.5,0);
			_9->LocalRotation = quat(0,0,0,1);
			_9->LocalScale = vec3(1,1,1);
		}
	}
	{
		Transform* _10 = _0->addChild(new Transform());
		_10->Name = std::string("static");
		_10->LocalPosition = vec3(0,0,0);
		_10->LocalRotation = quat(0,0,0,1);
		_10->LocalScale = vec3(1,1,1);
		{
			Transform* _11 = _10->addChild(new Transform());
			_11->Name = std::string("static_model");
			_11->LocalPosition = vec3(0,3.388,-8.98);
			_11->LocalRotation = quat(0,0,0,1);
			_11->LocalScale = vec3(1,1,1);
		}
	}
	{
		Transform* _12 = _0->addChild(new Transform());
		_12->Name = std::string("RotatingCross");
		_12->LocalPosition = vec3(0,0,5.1);
		_12->LocalRotation = quat(0,1,0,0);
		_12->LocalScale = vec3(0.7,0.7,0.7);
	}
	return _0;
}