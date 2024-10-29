#include "ScenePalace.h"

//#include <appkit-gl-engine/mini-gl-engine.h>

#include "components/RotatingCross.h"
/*
#include "components/ComponentSkinnedMesh.h"
#include "components/ComponentAnimationMotion.h"
#include "components/ComponentThirdPersonPlayerController.h"
#include "components/ComponentThirdPersonCamera.h"
*/

#include "App.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

std::shared_ptr<Transform> loadSceneroot();

static bool ReplaceMaterial(std::shared_ptr<Transform> element, void* userData) {
    auto material = element->findComponent<ComponentMaterial>();
    if ( material != nullptr ){
        std::shared_ptr<ComponentMaterial> &newMaterial = *(std::shared_ptr<ComponentMaterial> *)userData;
        auto componentMaterial = element->removeComponent(material);
        element->addComponent(newMaterial);
		//little optimization
		element->makeFirstComponent(newMaterial);
    }
    return true;
}


//to load skybox, textures, cubemaps, 3DModels and setup materials
void ScenePalace::loadResources() {

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

	Texture_Guard[0] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__diffuse.jpg",true && engine->sRGBCapable);
	Texture_Guard[1] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__normal.jpg",false);
    Texture_Guard[2] = resourceHelper->createTextureFromFile("resources/castle_guard/Guard_02__specular.jpg", false);

	Model_Palace = resourceHelper->createTransformFromModel("resources/palace/colonnato.min.bams");
    Model_Cross = resourceHelper->createTransformFromModel("resources/palace/cross.bams");
    //Model_Guard = resourceHelper->createTransformFromModel("resources/castle_guard/castle_guard_01.bams");

    bool use_gpu = true;
    #if defined(GLAD_GLES2)
    use_gpu = false;
    #endif

    skinnedMesh = Component::CreateShared<AppKit::GLEngine::Components::ComponentSkinnedMesh>(resourceHelper, use_gpu);
    skinnedMesh->loadModelBase("resources/castle_guard/castle_guard_01.bams");

    skinnedMesh->loadAnimation("idle","resources/castle_guard/castle_guard_01@Idle.bams");
    skinnedMesh->loadAnimation("walk", "resources/castle_guard/castle_guard_01@Walking.bams");
    skinnedMesh->loadAnimation("run", "resources/castle_guard/castle_guard_01@Run.bams");

    skinnedMesh->done();

    // ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

    auto newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    newMaterial->pbr.metallic = 1.0f;
    newMaterial->pbr.roughness = 0.5f;
    newMaterial->pbr.texAlbedo = Texture_Guard[0];
    newMaterial->pbr.texNormal = Texture_Guard[1];
    newMaterial->pbr.texSpecular = Texture_Guard[2];

    //Model_Guard->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);

    skinnedMesh->model_base->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);

    newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 0.9814019f, 0.7490196f);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    //newMaterial->pbr.texAlbedo = texRefCount->add(Rock02Textures[0]);
    //newMaterial->pbr.texNormal = texRefCount->add(Rock02Textures[1]);
    newMaterial->pbr.albedoColor = ResourceHelper::vec3ColorGammaToLinear(newMaterial->pbr.albedoColor);
    newMaterial->pbr.emissionColor = ResourceHelper::vec3ColorGammaToLinear(MathCore::vec3f(1.0f, 1.0f, 0.5f)) * 8.0f;

    Model_Cross->findTransformByName("circle")->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);

    newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 0.8521127f, 0.7464789f);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    //newMaterial->pbr.texAlbedo = texRefCount->add(Rock03Textures[0]);
    //newMaterial->pbr.texNormal = texRefCount->add(Rock03Textures[1]);
    newMaterial->pbr.albedoColor = ResourceHelper::vec3ColorGammaToLinear(newMaterial->pbr.albedoColor);

    Model_Cross->findTransformByName("cross")->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);

}

//to load the scene graph
void ScenePalace::loadGraph() {
    root = loadSceneroot();
}

//to bind the resources to the current graph
void ScenePalace::bindResourcesToGraph() {
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    //setup ambient color
    {
        GLRenderState *renderState = GLRenderState::Instance();
        //setup renderstate
        renderState->ClearColor = vec4f(0.2784314f, 0.2784314f, 0.2784314f,1.0f);

        //renderPipeline->ambientLight.color = MathCore::vec3f(0.2641509f,0.2641509f,0.2641509f) * 2.0f;
        renderPipeline->ambientLight.color = MathCore::vec3f(0.2641509f, 0.2641509f, 0.2641509f);
        renderPipeline->ambientLight.color = ResourceHelper::vec3ColorGammaToLinear(renderPipeline->ambientLight.color);
    }

    //setup camera
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();

        ((ComponentCameraPerspective*)camera.get())->fovDegrees = 60.0f;
		((ComponentCameraPerspective*)camera.get())->nearPlane = 0.1f;
		((ComponentCameraPerspective*)camera.get())->farPlane = 30.0f;

        auto camera_look = root->findTransformByName("Camera-look");

        mainCamera->lookAtLeftHanded(camera_look);

        //ThirdPersonCamera
        {
            thirdPersonCamera = mainCamera->addNewComponent<ComponentThirdPersonCamera>();
            thirdPersonCamera->TargetRef = camera_look;
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
		std::shared_ptr<Transform> lightTransform;
        std::shared_ptr<ComponentLight> light;

        lightTransform = root->findTransformByName("Directional Light (1)");
        light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333f * 8.0f; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);


		lightTransform = root->findTransformByName("Directional Light (2)");
        light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333f * 8.0f; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);

		lightTransform = root->findTransformByName("Directional Light (3)");
        light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.3333f * 8.0f; //0.5f + 0.5f;
        light->sun.color = ResourceHelper::vec3ColorGammaToLinear(light->sun.color);
        //*/

        //if (engine->sRGBCapable)
            //light->sun.color = ResourceHelper::vec3ColorLinearToGamma(light->sun.color);
    }

    //3DModel
    {
        std::shared_ptr<Transform> node;

		node = root->findTransformByName("static_model");
        node->addChild(ResourceHelper::cloneTransformRecursive(Model_Palace));

		node = root->findTransformByName("RotatingCross");
        node->addChild(ResourceHelper::cloneTransformRecursive(Model_Cross));
        node->addComponent(Component::CreateShared<RotatingCross>());

        node = root->findTransformByName("player side");
        //node->addChild(ResourceHelper::removeEmptyTransforms(ResourceHelper::cloneTransformRecursive(Model_Guard)));
        node->addComponent(skinnedMesh);
        skinnedMesh->moveMeshToTransform();

        thirdPersonCamera->Player_ForwardRef = node;

        node = root->findTransformByName("player");
        animationMotion = node->addNewComponent<ComponentAnimationMotion>();

        animationMotion->motionInfluence.push_back( ClipMotionInfluence("idle",0,0,0) );
        animationMotion->motionInfluence.push_back( ClipMotionInfluence("walk",0,0,1) );
        animationMotion->motionInfluence.push_back( ClipMotionInfluence("run",0,0,1) );

        thirdPersonPlayerController = node->addNewComponent<ComponentThirdPersonPlayerController>();

        auto camera_look = root->findTransformByName("Camera-look");

        thirdPersonPlayerController->setCameraLook(camera_look);

        thirdPersonPlayerController->animation_str_idle = "idle";
        thirdPersonPlayerController->animation_str_walk = "walk";
        thirdPersonPlayerController->animation_str_run = "run";

        /*
        //DEBUG
        ReferenceCounter<Component*> *compRefCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;
        camera_look->addComponent(compRefCount->add(Model_Cross->findTransformByName("circle")->findComponent(Components::ComponentMaterial::Type)));
        camera_look->addComponent(Components::ComponentMesh::createSphere(0.25, 8, 8));
        */

    }

    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

//clear all loaded scene
void ScenePalace::unloadAll() {
    // ResourceHelper::releaseTransformRecursive(&root);
    // ResourceHelper::releaseTransformRecursive(&Model_Palace);
	// ResourceHelper::releaseTransformRecursive(&Model_Cross);
	//ResourceHelper::releaseTransformRecursive(&Model_Guard);
    //delete skinnedMesh);
    root = nullptr;
    camera = nullptr;

    Model_Palace = nullptr;
    Model_Cross = nullptr;
    //AppKit::GLEngine::Transform* Model_Guard;
    Texture_Guard[0] = nullptr;
    Texture_Guard[1] = nullptr;
    Texture_Guard[2] = nullptr;

    skinnedMesh = nullptr;
    animationMotion = nullptr;
    thirdPersonPlayerController = nullptr;
    thirdPersonCamera = nullptr;

}

ScenePalace::ScenePalace(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap)
{
    Model_Palace = nullptr;
    Model_Cross = nullptr;
    //Model_Guard = nullptr;
    skinnedMesh = nullptr;
    animationMotion = nullptr;
    thirdPersonPlayerController = nullptr;
    thirdPersonCamera = nullptr;
    Texture_Guard[0] = nullptr;
	Texture_Guard[1] = nullptr;
    Texture_Guard[2] = nullptr;
}

ScenePalace::~ScenePalace() {
    unload();
}

void ScenePalace::draw(){
    //Transform *node = root->findTransformByName("rock03 (test)");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * MathCore::OP<float>::deg_2_rad(30.0f), 0);

    //Transform *node = root->findTransformByName("Directional Light");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * MathCore::OP<float>::deg_2_rad(30.0f), 0);

    //App* app = (App*)AppKit::GLEngine::Engine::Instance()->app;

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

std::shared_ptr<Transform> loadSceneroot()
{
	auto _0 = Transform::CreateShared();
	_0->Name = std::string("root");
	_0->LocalPosition = MathCore::vec3f(0,0,0);
	_0->LocalRotation = quatf(0,0,0,1);
	_0->LocalScale = MathCore::vec3f(1,1,1);
	{
		auto _1 = _0->addChild(Transform::CreateShared());
		_1->Name = std::string("Main Camera");
		_1->LocalPosition = MathCore::vec3f(0,2.475,-2.268);
		_1->LocalRotation = quatf(0.2011371,0.005232482,-0.001074419,0.9795486);
		_1->LocalScale = MathCore::vec3f(1,1,1);
	}
	{
		auto _2 = _0->addChild(Transform::CreateShared());
		_2->Name = std::string("player");
		_2->LocalPosition = MathCore::vec3f(0,0,0);
		_2->LocalRotation = quatf(0,0,0,1);
		_2->LocalScale = MathCore::vec3f(1,1,1);
		{
			auto _3 = _2->addChild(Transform::CreateShared());
			_3->Name = std::string("Camera-look");
			//_3->LocalPosition = MathCore::vec3f(0.015,1.491,0.025);
            _3->LocalPosition = MathCore::vec3f(0, 1.291, 0.025);
			_3->LocalRotation = quatf(0,0,0,1);
			_3->LocalScale = MathCore::vec3f(1,1,1);
		}
        {
            auto _3 = _2->addChild(Transform::CreateShared());
            _3->Name = std::string("player side");
            _3->LocalPosition = MathCore::vec3f(-0.62, 0, 0);
            _3->LocalRotation = quatf(0, 0, 0, 1);
            _3->LocalScale = MathCore::vec3f(1, 1, 1);
        }
	}
	{
		auto _4 = _0->addChild(Transform::CreateShared());
		_4->Name = std::string("Lights");
		_4->LocalPosition = MathCore::vec3f(-1.133002,0.5275524,-4.254147);
		_4->LocalRotation = quatf(0,0,0,1);
		_4->LocalScale = MathCore::vec3f(1,1,1);
		{
			auto _5 = _4->addChild(Transform::CreateShared());
			_5->Name = std::string("Directional Light (1)");
			_5->LocalPosition = MathCore::vec3f(1.133002,2.472448,4.254147);
			_5->LocalRotation = quatf(0.4082179,-0.2345697,0.1093816,0.8754261);
			_5->LocalScale = MathCore::vec3f(1,1,1);
		}
		{
			auto _6 = _4->addChild(Transform::CreateShared());
			_6->Name = std::string("Directional Light (2)");
			_6->LocalPosition = MathCore::vec3f(2.853002,2.572448,1.264147);
			_6->LocalRotation = quatf(0.3147533,0.6047991,-0.2820223,0.6749904);
			_6->LocalScale = MathCore::vec3f(1,1,1);
		}
		{
			auto _7 = _4->addChild(Transform::CreateShared());
			_7->Name = std::string("Directional Light (3)");
			_7->LocalPosition = MathCore::vec3f(2.853002,2.572448,1.264147);
			_7->LocalRotation = quatf(-0.1208431,0.8684675,-0.404973,-0.2591489);
			_7->LocalScale = MathCore::vec3f(1,1,1);
		}
	}
	{
		auto _8 = _0->addChild(Transform::CreateShared());
		_8->Name = std::string("Ground");
		_8->LocalPosition = MathCore::vec3f(0,0,0);
		_8->LocalRotation = quatf(0,0,0,1);
		_8->LocalScale = MathCore::vec3f(27.03765,27.03765,27.03765);
		{
			auto _9 = _8->addChild(Transform::CreateShared());
			_9->Name = std::string("Cube");
			_9->LocalPosition = MathCore::vec3f(0,-0.5,0);
			_9->LocalRotation = quatf(0,0,0,1);
			_9->LocalScale = MathCore::vec3f(1,1,1);
		}
	}
	{
		auto _10 = _0->addChild(Transform::CreateShared());
		_10->Name = std::string("static");
		_10->LocalPosition = MathCore::vec3f(0,0,0);
		_10->LocalRotation = quatf(0,0,0,1);
		_10->LocalScale = MathCore::vec3f(1,1,1);
		{
			auto _11 = _10->addChild(Transform::CreateShared());
			_11->Name = std::string("static_model");
			_11->LocalPosition = MathCore::vec3f(0,3.388,-8.98);
			_11->LocalRotation = quatf(0,0,0,1);
			_11->LocalScale = MathCore::vec3f(1,1,1);
		}
	}
	{
		auto _12 = _0->addChild(Transform::CreateShared());
		_12->Name = std::string("RotatingCross");
		_12->LocalPosition = MathCore::vec3f(0,0,5.1);
		_12->LocalRotation = quatf(0,1,0,0);
		_12->LocalScale = MathCore::vec3f(0.7,0.7,0.7);
	}
	return _0;
}
