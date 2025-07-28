#include "SceneJesus.h"

//#include <appkit-gl-engine/mini-gl-engine.h>

#include "App.h"

using namespace AppKit::GLEngine;
using namespace AppKit::GLEngine::Components;
using namespace AppKit::OpenGL;
using namespace AppKit::Window::Devices;
using namespace MathCore;

std::shared_ptr<Transform> loadSceneroot_jesus();

static bool ReplaceMaterial(std::shared_ptr<Transform> element, void* userData) {
    
    auto material = element->findComponent<ComponentMaterial>();
    
    if ( material != nullptr ){
        std::shared_ptr<ComponentMaterial> &newMaterial = *(std::shared_ptr<ComponentMaterial> *)userData;
        
        //ReferenceCounter<Component*> *compRefCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;
        
        auto componentMaterial = element->removeComponent(material);
        //compRefCount->remove(componentMaterial);

        element->addComponent(newMaterial);

		//little optimization
		element->makeFirstComponent(newMaterial);
    }
    
    return true;
}


//to load skybox, textures, cubemaps, 3DModels and setup materials
void SceneJesus::loadResources() {
	
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();


    //diffuse + normal
    JesusTextures[0] = nullptr;
    JesusTextures[1] = resourceHelper->createTextureFromFile("resources/Jesus/NormalMap_jesus.jpg",false);
    
    Jesus3DModel = resourceHelper->createTransformFromModel("resources/Jesus/Jesus.bams", resourceMap->defaultPBRMaterial);


    // ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    
    std::shared_ptr<ComponentMaterial> newMaterial;
    
    newMaterial =  Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = resourceMap->defaultAlbedoTexture;
    newMaterial->pbr.texNormal = JesusTextures[1];
    
    Jesus3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);
    
}

//to load the scene graph
void SceneJesus::loadGraph() {
    root = loadSceneroot_jesus();
}

//to bind the resources to the current graph
void SceneJesus::bindResourcesToGraph() {
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    //setup cubemap
    {
        if (renderPipeline->cubeSkyBox != nullptr)
            renderPipeline->cubeSkyBox = nullptr;
        renderPipeline->cubeSkyBox = resourceHelper->createSkybox("SantaMariaDeiMiracoli",
            true && engine->sRGBCapable,
            1024
        );

        if (renderPipeline->cubeAmbientLight_1x1 != nullptr)
            renderPipeline->cubeAmbientLight_1x1 = nullptr;

#if ITK_RPI
        renderPipeline->cubeAmbientLight_1x1 = resourceHelper->createCubeMap("SantaMariaDeiMiracoli",
            true && engine->sRGBCapable,
            1
        );
#else
        if (engine->sRGBCapable)
            renderPipeline->cubeAmbientLight_1x1 = std::make_shared<GLCubeMap>(1, 1, GL_SRGB, 1);
        else
            renderPipeline->cubeAmbientLight_1x1 = std::make_shared<GLCubeMap>(1, 1, GL_RGB, 1);

        resourceHelper->copyCubeMapEnhanced(
            &renderPipeline->cubeSkyBox->cubeMap,
            renderPipeline->cubeSkyBox->cubeMap.max_mip_level,
            renderPipeline->cubeAmbientLight_1x1.get(),0
        );
        
        /*
        resourceHelper->copyCubeMap(&renderPipeline->cubeSkyBox->cubeMap,
            renderPipeline->cubeAmbientLight_1x1,
            engine->sRGBCapable,
            1
        );
        */
#endif

        renderPipeline->cubeAmbientLight_1x1->active(0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        renderPipeline->cubeAmbientLight_1x1->deactive(0);

    }
   
    //setup camera
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();
        
        ((ComponentCameraPerspective*)camera.get())->fovDegrees = 60.0f;
		//((ComponentCameraPerspective*)camera)->nearPlane = 5.0f;
		((ComponentCameraPerspective*)camera.get())->farPlane = 50.0f;

        auto  toLookNode = root->findTransformByName("RotationPivot");
        
        mainCamera->lookAtLeftHanded(toLookNode);

		//componentCameraRotateOnTarget
		{
			componentCameraRotateOnTarget = mainCamera->addNewComponent<ComponentCameraRotateOnTarget>();
			componentCameraRotateOnTarget->Target = toLookNode;
		}
    }

    //cube mesh
    {
        auto cube = root->findTransformByName("SacredHeartBase");
        if (cube) {
            //ReferenceCounter<GLTexture*> *refCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

            auto material = cube->addNewComponent<ComponentMaterial>();
            cube->addComponent(ComponentMesh::createBox(MathCore::vec3f(1, 1, 1)));
            //material->type = MaterialUnlit;
            //material->unlit.color = vec4(0.5f,0.5f,0.5f,1.0f);

            material->type = MaterialPBR;
            material->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
            material->pbr.metallic = 0.0f;
            material->pbr.roughness = 1.0f;
            material->pbr.texAlbedo = resourceMap->defaultAlbedoTexture;
            material->pbr.texNormal = nullptr;//refCount->add( resourceHelper->defaultNormalTexture );

        }
    }
    
    //light
    {
        auto lightTransform = root->findTransformByName("DirectionalLight0");
        auto light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.6f;
    }
    
    //Mary 3DModel
    {
        auto node = root->findTransformByName("SacredHeart");
        node->addChild(ResourceHelper::cloneTransformRecursive(Jesus3DModel));
    }
    
    //Add AABB for all meshs...
    {
        //root->traversePreOrder_DepthFirst( AddAABBMesh );
        resourceHelper->addAABBMesh(root);
    }
}

//clear all loaded scene
void SceneJesus::unloadAll() {
    // ResourceHelper::releaseTransformRecursive(&root);
    // ResourceHelper::releaseTransformRecursive(&Jesus3DModel);

    root = nullptr;
    camera = nullptr;
    Jesus3DModel = nullptr;
}

SceneJesus::SceneJesus(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow) 
{
    Jesus3DModel = nullptr;
}

SceneJesus::~SceneJesus() {

    Jesus3DModel = nullptr;
    JesusTextures[0] = nullptr;
    JesusTextures[1] = nullptr;
    componentCameraRotateOnTarget = nullptr;

    unload();
}

void SceneJesus::draw() {

    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    auto t = root->findTransformByName("Light");
    if (t != nullptr &&
        ITKCommon::StringUtil::endsWith(((App*)engine->app)->sceneGUI->button_SunLightRotate->rendered_text, "ON")
        ) {
        t->LocalRotation = t->LocalRotation * GEN<quatf>::fromEuler(0, time->deltaTime * MathCore::OP<float>::deg_2_rad(20.0f), 0);
    }

    AppKit::GLEngine::SceneBase::draw();
}

std::shared_ptr<Transform> loadSceneroot_jesus()
{
    auto _0 = Transform::CreateShared();
    _0->affectComponentStart = true;
    _0->Name = std::string("root");
    _0->LocalPosition = MathCore::vec3f(0, 0, 0);
    _0->LocalRotation = MathCore::quatf(0, 0, 0, 1);
    _0->LocalScale = MathCore::vec3f(1, 1, 1);
    {
        auto _1 = _0->addChild(Transform::CreateShared());
        _1->Name = std::string("Main Camera");
        //_1->LocalPosition = MathCore::vec3f(7.9, 6.5, 0);
        _1->LocalPosition = MathCore::vec3f(5.0, 6.5, 0);
        _1->LocalRotation = MathCore::quatf(0.1481994, -0.6914021, 0.1481994, 0.6914022);
        _1->LocalScale = MathCore::vec3f(1, 1, 1);
    }
    {
        auto _2 = _0->addChild(Transform::CreateShared());
        _2->Name = std::string("shadow_plane");
        _2->LocalPosition = MathCore::vec3f(0.006, -0.81, 0.04770672);
        _2->LocalRotation = MathCore::quatf(0, 0, 0, 1);
        _2->LocalScale = MathCore::vec3f(30, 0.6394801, 30);
    }
    {
        auto _3 = _0->addChild(Transform::CreateShared());
        _3->Name = std::string("Light");
        _3->LocalPosition = MathCore::vec3f(0, 0, 0);
        _3->LocalRotation = MathCore::quatf(0, 0.4520463, 0, 0.8919945);
        _3->LocalScale = MathCore::vec3f(1, 1, 1);
        {
            auto _4 = _3->addChild(Transform::CreateShared());
            _4->Name = std::string("DirectionalLight0");
            _4->LocalPosition = MathCore::vec3f(0, 4.92, 2.24);
            _4->LocalRotation = MathCore::quatf(0.7429565, 0.5067579, -0.3551862, 0.2550589);
            _4->LocalScale = MathCore::vec3f(1, 1, 1);
        }
        {
            auto _5 = _3->addChild(Transform::CreateShared());
            _5->Name = std::string("DirectionalLight1");
            _5->LocalPosition = MathCore::vec3f(0, 4.92, 2.24);
            _5->LocalRotation = MathCore::quatf(0.2382822, 0.1166905, 0.597909, 0.7563793);
            _5->LocalScale = MathCore::vec3f(1, 1, 1);
        }
    }
    {
        auto _6 = _0->addChild(Transform::CreateShared());
        _6->Name = std::string("VirginMary");
        _6->LocalPosition = MathCore::vec3f(0, 0, 0);
        _6->LocalRotation = MathCore::quatf(0, 0, 0, 1);
        _6->LocalScale = MathCore::vec3f(1, 1, 1);
        {
            auto _7 = _6->addChild(Transform::CreateShared());
            _7->Name = std::string("Model");
            _7->LocalPosition = MathCore::vec3f(0, 0, 0);
            _7->LocalRotation = MathCore::quatf(-0.7071068, 0, 0, 0.7071067);
            _7->LocalScale = MathCore::vec3f(100, 100, 100);
        }
        {
            auto _8 = _6->addChild(Transform::CreateShared());
            _8->Name = std::string("VirginMaryBase");
            _8->LocalPosition = MathCore::vec3f(0.006, -5.969997, 0.0477066);
            _8->LocalRotation = MathCore::quatf(0, 0, 0, 1);
            _8->LocalScale = MathCore::vec3f(1.11952, 12.00112, 1.39);
        }
    }
    {
        auto _9 = _0->addChild(Transform::CreateShared());
        _9->Name = std::string("SacredHeart");
        _9->LocalPosition = MathCore::vec3f(0.084, 0, 0.046);
        _9->LocalRotation = MathCore::quatf(0, 0.7071068, 0, 0.7071068);
        _9->LocalScale = MathCore::vec3f(0.4760795, 0.4760795, 0.4760795);
        {
            auto _10 = _9->addChild(Transform::CreateShared());
            _10->Name = std::string("Jesus");
            _10->LocalPosition = MathCore::vec3f(0, 0, 0);
            _10->LocalRotation = MathCore::quatf(-0.7071068, 0, 0, 0.7071067);
            _10->LocalScale = MathCore::vec3f(100, 100, 100);
        }
        {
            auto _11 = _9->addChild(Transform::CreateShared());
            _11->Name = std::string("SacredHeartBase");
            _11->LocalPosition = MathCore::vec3f(0.0966225, -12.53992, -0.1764411);
            _11->LocalRotation = MathCore::quatf(0, -0.7071068, 0, 0.7071068);
            _11->LocalScale = MathCore::vec3f(3.374974, 25.20823, 3.292908);
        }
    }
    {
        auto _12 = _0->addChild(Transform::CreateShared());
        _12->Name = std::string("RotationPivot");
        _12->LocalPosition = MathCore::vec3f(0, 2.739999, 0);
        _12->LocalRotation = MathCore::quatf(0, 0, 0, 1);
        _12->LocalScale = MathCore::vec3f(1, 1, 1);
    }
    return _0;
}

