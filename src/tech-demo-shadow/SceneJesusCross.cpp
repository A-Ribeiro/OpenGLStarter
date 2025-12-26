#include "SceneJesusCross.h"

//#include <appkit-gl-engine/mini-gl-engine.h>

#include "App.h"

#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

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
void SceneJesusCross::loadResources() {
	
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    //diffuse + normal
    JesusTextures[0] = resourceHelper->createTextureFromFile("resources/Jesus/UVJesus.jpg",true && engine->sRGBCapable);
    JesusTextures[1] = nullptr;//resourceHelper->defaultNormalTexture;
        
    Jesus3DModel = resourceHelper->createTransformFromModel("resources/Jesus/JesusOnCross.bams", resourceMap, resourceMap->defaultPBRMaterial);
    
    //ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    
    auto newMaterial = Component::CreateShared<ComponentMaterial>();
    // newMaterial->type = Components::MaterialPBR;
    // newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    // newMaterial->pbr.metallic = 0.0f;
    // newMaterial->pbr.roughness = 1.0f;
    // newMaterial->pbr.texAlbedo = JesusTextures[0];
    // newMaterial->pbr.texNormal = nullptr;//texRefCount->add(JesusTextures[1]);

    newMaterial->setShader(resourceMap->pbrShaderSelector);
    newMaterial->property_bag.getProperty("texAlbedo").set<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>(JesusTextures[0]);
    
    Jesus3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);

}

//to load the scene graph
void SceneJesusCross::loadGraph() {
    root = loadSceneroot();
    root->setRootPropertiesFromDefaultScene(this->self());
}

//to bind the resources to the current graph
void SceneJesusCross::bindResourcesToGraph() {
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();
    
    //setup ambient color
    {
        GLRenderState *renderState = GLRenderState::Instance();

        //setup renderstate
        renderState->ClearColor = vec4f(0.1607843f, 0.1568628f, 0.1568628f,1.0f);
        
        renderPipeline->ambientLight.color = MathCore::vec3f(0.2641509f,0.2641509f,0.2641509f);
        renderPipeline->ambientLight.color = ResourceHelper::vec3ColorGammaToLinear(renderPipeline->ambientLight.color);        
    }
    
    //setup camera
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();
        
        ((ComponentCameraPerspective*)camera.get())->fovDegrees = 60.0f;
		((ComponentCameraPerspective*)camera.get())->nearPlane = 1.0f;
		((ComponentCameraPerspective*)camera.get())->farPlane = 50.0f;

        auto toLookNode = root->findTransformByName("ToLookNode");
        mainCamera->lookAtLeftHanded(toLookNode);

		//componentCameraRotateOnTarget
		{
			componentCameraRotateOnTarget = mainCamera->addNewComponent<ComponentCameraRotateOnTarget>();
            componentCameraRotateOnTarget->rotation_x_deg_min = -90.0f;
            componentCameraRotateOnTarget->rotation_x_deg_max = 90.0f;
			componentCameraRotateOnTarget->Target = toLookNode;
            
		}
    }
    
    //light
    {
        auto lightTransform = root->findTransformByName("Directional Light");
        light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.5f + 0.5f;
        light->cast_shadow = true;
        light->createDebugLines();

        light->sun.render_after_skybox = true;
        //solar info:
        //  radius: 695.700.000 m = 695.700 km
        //  distance: 150.000.000.000 m = 150.000.000 km
        // earth Sun information
        //light->sun.smartSunLightCalculation(695.7*10.0, 150000.0,  500.0f);
        light->sun.smartSunLightCalculation(695.7 * 10.0, 150000.0, 500.0f);

        renderPipeline->shaderShadowAlgorithm = ShaderShadowAlgorithm_PCSS_PCF;

    }
    
    //Jesus 3DModel
    {
        auto node = root->findTransformByName("JesusCross1")->findTransformByName("ToInsertModel");
        node->addChild(ResourceHelper::cloneTransformRecursive(Jesus3DModel));
    }

	//Ground
	{
		auto node = root->findTransformByName("Ground_aux");
		if (node) {
            //ReferenceCounter<GLTexture*> *refCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

            auto material = node->addNewComponent<ComponentMaterial>();
            auto mesh = ComponentMesh::createBox(MathCore::vec3f(50, 1, 50));
            //mesh->syncVBOStatic();
            node->addComponent(mesh);
            //material->type = MaterialUnlit;
            //material->unlit.color = vec4(0.5f,0.5f,0.5f,1.0f);

            material->setShader(resourceMap->pbrShaderSelector);
            material->property_bag.getProperty("texAlbedo").set<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>(resourceMap->defaultAlbedoTexture);

            // material->type = MaterialPBR;
            // material->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
            // material->pbr.metallic = 0.0f;
            // material->pbr.roughness = 1.0f;
            // material->pbr.texAlbedo = resourceMap->defaultAlbedoTexture;
            // material->pbr.texNormal = nullptr;//refCount->add( resourceHelper->defaultNormalTexture );

        }
	}



    //Sphere
    {
        auto node = root->addChild(Transform::CreateShared());
        if (node) {
            // ReferenceCounter<GLTexture*>* refCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

            auto material = node->addNewComponent<ComponentMaterial>();
            auto mesh = ComponentMesh::createSphere(1.0f,16,16);
            //mesh->syncVBOStatic();
            node->addComponent(mesh);
            node->LocalPosition = MathCore::vec3f(2,3,0);
            node->LocalScale = MathCore::vec3f(0.2f,0.4f,0.2f);
            //node->LocalRotation = quatFromEuler(0,MathCore::OP<float>::deg_2_rad(30.0f), MathCore::OP<float>::deg_2_rad(30.0f));
            //material->type = MaterialUnlit;
            //material->unlit.color = vec4(0.5f,0.5f,0.5f,1.0f);

            material->setShader(resourceMap->pbrShaderSelector);
            material->property_bag.getProperty("texAlbedo").set<std::shared_ptr<AppKit::OpenGL::VirtualTexture>>(resourceMap->defaultAlbedoTexture);
            material->property_bag.getProperty("albedoColor").set<MathCore::vec3f>(MathCore::vec3f(1, 1, 0));


            // material->type = MaterialPBR;
            // material->pbr.albedoColor = MathCore::vec3f(1, 1, 0);
            // material->pbr.metallic = 0.0f;
            // material->pbr.roughness = 1.0f;
            // material->pbr.texAlbedo = resourceMap->defaultAlbedoTexture;
            // material->pbr.texNormal = nullptr;//refCount->add( resourceHelper->defaultNormalTexture );

        }
    }


    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }


}

//clear all loaded scene
void SceneJesusCross::unloadAll() {
    // ResourceHelper::releaseTransformRecursive(&root);
    // ResourceHelper::releaseTransformRecursive(&Jesus3DModel);

    root = nullptr;
    camera = nullptr;
    Jesus3DModel = nullptr;

    JesusTextures[0] = nullptr;
    JesusTextures[1] = nullptr;

    componentCameraRotateOnTarget = nullptr;
    light = nullptr;

}

SceneJesusCross::SceneJesusCross(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow) 
{
    Jesus3DModel = nullptr;
}

SceneJesusCross::~SceneJesusCross() {
    unload();
}

void SceneJesusCross::draw(){
    //Transform *node = root->findTransformByName("rock03 (test)");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * MathCore::OP<float>::deg_2_rad(30.0f), 0);
    
    //Transform *node = root->findTransformByName("Directional Light");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * MathCore::OP<float>::deg_2_rad(30.0f), 0);

    App* app = (App*)AppKit::GLEngine::Engine::Instance()->app;

    static EventCore::PressReleaseDetector num0;
    num0.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num1 ));
    static EventCore::PressReleaseDetector num1;
    num1.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num2 ));
    static EventCore::PressReleaseDetector num2;
    num2.setState(AppKit::Window::Devices::Keyboard::isPressed( AppKit::Window::Devices::KeyCode::Num3 ));

    if (num0.down)
        light->sun.smartSunLightCalculation(695.7 * 10.0, 150000.0, 500.0f);
    if (num1.down)
        light->sun.smartSunLightCalculation(695.7 * 15.0, 150000.0,  500.0f);
    if (num2.down)
        light->sun.smartSunLightCalculation(695.7 * 20.0, 150000.0,  500.0f);

    SceneBase::draw();
}

std::shared_ptr<Transform> loadSceneroot()
{
	auto _0 = Transform::CreateShared();
    // _0->affectComponentStart = true;
	_0->Name = std::string("root");
	_0->LocalPosition = MathCore::vec3f(0,0,0);
	_0->LocalRotation = quatf(0,0,0,1);
	_0->LocalScale = MathCore::vec3f(1,1,1);
	{
		auto _1 = _0->addChild(Transform::CreateShared());
		_1->Name = std::string("Main Camera");
		//_1->LocalPosition = MathCore::vec3f(0,3.45,-7.37);
		//_1->LocalPosition = MathCore::vec3f(0,3.45,-17.37);
        _1->LocalPosition = MathCore::vec3f(3.736165, 6.254054, -5.700460);
		_1->LocalRotation = quatf(0,0,0,1);
		_1->LocalScale = MathCore::vec3f(1,1,1);
	}
	{
		auto _2 = _0->addChild(Transform::CreateShared());
		_2->Name = std::string("Directional Light");
		_2->LocalPosition = MathCore::vec3f(0,3,0);
		_2->LocalRotation = quatf(-0.4886241,0.4844012,0.06177928,-0.723039);
		_2->LocalScale = MathCore::vec3f(1,1,1);

        _2->LocalPosition = _2->LocalPosition - _2->LocalRotation * MathCore::vec3f(0, 0, 3);
	}
	{
		auto _3 = _0->addChild(Transform::CreateShared());
		_3->Name = std::string("JesusCross1");
		_3->LocalPosition = MathCore::vec3f(0,0,0);
		//_3->LocalRotation = quatf(0,1,0,0);
        _3->LocalRotation = GEN<quatf>::fromEuler(0,MathCore::OP<float>::deg_2_rad(45.0f),0);
		_3->LocalScale = MathCore::vec3f(0.4457346,0.4457346,0.4457346);
		{
			auto _4 = _3->addChild(Transform::CreateShared());
			_4->Name = std::string("ToInsertModel");
			_4->LocalPosition = MathCore::vec3f(-11.63,6.67,0.12);
			_4->LocalRotation = quatf(0,0,0,1);
			_4->LocalScale = MathCore::vec3f(1,1,1);
		}
	}
	{
		auto _5 = _0->addChild(Transform::CreateShared());
		_5->Name = std::string("ground");
		_5->LocalPosition = MathCore::vec3f(0,0,0);
		_5->LocalRotation = quatf(0,0,0,1);
		_5->LocalScale = MathCore::vec3f(1,1,1);
		{
			auto _16 = _5->addChild(Transform::CreateShared());
			_16->Name = std::string("Ground_aux");
			//_16->LocalPosition = MathCore::vec3f(0,-1.98,0);
			_16->LocalPosition = MathCore::vec3f(0,-0.5,0);
			_16->LocalRotation = quatf(0,0,0,1);
			_16->LocalScale = MathCore::vec3f(1,1,1);
		}
	}
	{
		auto _52 = _0->addChild(Transform::CreateShared());
		_52->Name = std::string("ToLookNode");
		_52->LocalPosition = MathCore::vec3f(0,3.45,0);
		_52->LocalRotation = quatf(0,0,0,1);
		_52->LocalScale = MathCore::vec3f(1,1,1);
	}
	{
		auto _53 = _0->addChild(Transform::CreateShared());
		_53->Name = std::string("Particle System");
		_53->LocalPosition = MathCore::vec3f(0,3.5,-12.97);
		_53->LocalRotation = quatf(0,0,0,1);
		_53->LocalScale = MathCore::vec3f(1,1,1);
	}
	return _0;
}
