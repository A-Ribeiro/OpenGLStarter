#include "SceneJesus.h"

#include <mini-gl-engine/mini-gl-engine.h>

#include "App.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

Transform* loadSceneroot_jesus();

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
void SceneJesus::loadResources() {
	
    GLEngine::Engine *engine = GLEngine::Engine::Instance();


    //diffuse + normal
    JesusTextures[0] = NULL;
    JesusTextures[1] = resourceHelper->createTextureFromFile("resources/Jesus/NormalMap_jesus.jpg",false);
    
    Jesus3DModel = resourceHelper->createTransformFromModel("resources/Jesus/Jesus.bams");


    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;
    
    ComponentMaterial * newMaterial;
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = texRefCount->add(resourceHelper->defaultAlbedoTexture);
    newMaterial->pbr.texNormal = texRefCount->add(JesusTextures[1]);
    
    Jesus3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
}

//to load the scene graph
void SceneJesus::loadGraph() {
    root = loadSceneroot_jesus();
}

//to bind the resources to the current graph
void SceneJesus::bindResourcesToGraph() {
    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    //setup cubemap
    {
        aRibeiro::setNullAndDelete(renderPipeline->cubeSkyBox);
        renderPipeline->cubeSkyBox = resourceHelper->createSkybox("SantaMariaDeiMiracoli",
            true && engine->sRGBCapable,
            1024
        );

        aRibeiro::setNullAndDelete(renderPipeline->cubeAmbientLight_1x1);

#if ARIBEIRO_RPI
        renderPipeline->cubeAmbientLight_1x1 = resourceHelper->createCubeMap("SantaMariaDeiMiracoli",
            true && engine->sRGBCapable,
            1
        );
#else
        renderPipeline->cubeAmbientLight_1x1 = new GLCubeMap(engine->sRGBCapable, 1);

        resourceHelper->copyCubeMap(&renderPipeline->cubeSkyBox->cubeMap,
            renderPipeline->cubeAmbientLight_1x1,
            engine->sRGBCapable,
            1
        );
#endif

        renderPipeline->cubeAmbientLight_1x1->active(0);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        renderPipeline->cubeAmbientLight_1x1->deactive(0);

    }
   
    //setup camera
    {
        Transform *mainCamera = root->findTransformByName("Main Camera");
        mainCamera->addComponent(camera = new ComponentCameraPerspective());
        
        ((ComponentCameraPerspective*)camera)->fovDegrees = 60.0f;
		//((ComponentCameraPerspective*)camera)->nearPlane = 5.0f;
		((ComponentCameraPerspective*)camera)->farPlane = 50.0f;

        Transform *toLookNode = root->findTransformByName("RotationPivot");
        
        mainCamera->lookAtLeftHanded(toLookNode);

		//componentCameraRotateOnTarget
		{
			mainCamera->addComponent(componentCameraRotateOnTarget = new ComponentCameraRotateOnTarget());
			componentCameraRotateOnTarget->Target = toLookNode;
		}
    }

    //cube mesh
    {
        Transform *cube = root->findTransformByName("SacredHeartBase");
        if (cube) {
            ReferenceCounter<GLTexture*> *refCount = &GLEngine::Engine::Instance()->textureReferenceCounter;

            ComponentMaterial *material = (ComponentMaterial*)cube->addComponent(new ComponentMaterial());
            cube->addComponent(ComponentMesh::createBox(vec3(1, 1, 1)));
            //material->type = MaterialUnlit;
            //material->unlit.color = vec4(0.5f,0.5f,0.5f,1.0f);

            material->type = MaterialPBR;
            material->pbr.albedoColor = vec4(1, 1, 1, 1);
            material->pbr.metallic = 0.0f;
            material->pbr.roughness = 1.0f;
            material->pbr.texAlbedo = refCount->add( resourceHelper->defaultAlbedoTexture );
            material->pbr.texNormal = NULL;//refCount->add( resourceHelper->defaultNormalTexture );

        }
    }
    
    //light
    {
        Transform *lightTransform = root->findTransformByName("DirectionalLight0");
        ComponentLight *light = (ComponentLight *)lightTransform->addComponent(new ComponentLight());
        light->type = LightSun;
        light->sun.color = aRibeiro::vec3(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.6f;
    }
    
    //Mary 3DModel
    {
        Transform *node = root->findTransformByName("SacredHeart");
        node->addChild(ResourceHelper::cloneTransformRecursive(Jesus3DModel));
    }
    
}

//clear all loaded scene
void SceneJesus::unloadAll() {
    ResourceHelper::releaseTransformRecursive(&root);
    ResourceHelper::releaseTransformRecursive(&Jesus3DModel);
}

SceneJesus::SceneJesus(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) 
{
    Jesus3DModel = NULL;
}

SceneJesus::~SceneJesus() {
    unload();
}

void SceneJesus::draw() {


    GLEngine::Engine *engine = GLEngine::Engine::Instance();


    Transform *t = root->findTransformByName("Light");
    if (t != NULL &&
        aRibeiro::StringUtil::endsWith(((App*)engine->app)->sceneGUI->button_SunLightRotate->rendered_text, "ON")
        ) {
        t->LocalRotation = t->LocalRotation * quatFromEuler(0, time->deltaTime * DEG2RAD(20.0f), 0);
    }

    GLEngine::SceneBase::draw();
}

Transform* loadSceneroot_jesus()
{
    Transform* _0 = new Transform();
    _0->Name = std::string("root");
    _0->LocalPosition = vec3(0, 0, 0);
    _0->LocalRotation = quat(0, 0, 0, 1);
    _0->LocalScale = vec3(1, 1, 1);
    {
        Transform* _1 = _0->addChild(new Transform());
        _1->Name = std::string("Main Camera");
        //_1->LocalPosition = vec3(7.9, 6.5, 0);
        _1->LocalPosition = vec3(5.0, 6.5, 0);
        _1->LocalRotation = quat(0.1481994, -0.6914021, 0.1481994, 0.6914022);
        _1->LocalScale = vec3(1, 1, 1);
    }
    {
        Transform* _2 = _0->addChild(new Transform());
        _2->Name = std::string("shadow_plane");
        _2->LocalPosition = vec3(0.006, -0.81, 0.04770672);
        _2->LocalRotation = quat(0, 0, 0, 1);
        _2->LocalScale = vec3(30, 0.6394801, 30);
    }
    {
        Transform* _3 = _0->addChild(new Transform());
        _3->Name = std::string("Light");
        _3->LocalPosition = vec3(0, 0, 0);
        _3->LocalRotation = quat(0, 0.4520463, 0, 0.8919945);
        _3->LocalScale = vec3(1, 1, 1);
        {
            Transform* _4 = _3->addChild(new Transform());
            _4->Name = std::string("DirectionalLight0");
            _4->LocalPosition = vec3(0, 4.92, 2.24);
            _4->LocalRotation = quat(0.7429565, 0.5067579, -0.3551862, 0.2550589);
            _4->LocalScale = vec3(1, 1, 1);
        }
        {
            Transform* _5 = _3->addChild(new Transform());
            _5->Name = std::string("DirectionalLight1");
            _5->LocalPosition = vec3(0, 4.92, 2.24);
            _5->LocalRotation = quat(0.2382822, 0.1166905, 0.597909, 0.7563793);
            _5->LocalScale = vec3(1, 1, 1);
        }
    }
    {
        Transform* _6 = _0->addChild(new Transform());
        _6->Name = std::string("VirginMary");
        _6->LocalPosition = vec3(0, 0, 0);
        _6->LocalRotation = quat(0, 0, 0, 1);
        _6->LocalScale = vec3(1, 1, 1);
        {
            Transform* _7 = _6->addChild(new Transform());
            _7->Name = std::string("Model");
            _7->LocalPosition = vec3(0, 0, 0);
            _7->LocalRotation = quat(-0.7071068, 0, 0, 0.7071067);
            _7->LocalScale = vec3(100, 100, 100);
        }
        {
            Transform* _8 = _6->addChild(new Transform());
            _8->Name = std::string("VirginMaryBase");
            _8->LocalPosition = vec3(0.006, -5.969997, 0.0477066);
            _8->LocalRotation = quat(0, 0, 0, 1);
            _8->LocalScale = vec3(1.11952, 12.00112, 1.39);
        }
    }
    {
        Transform* _9 = _0->addChild(new Transform());
        _9->Name = std::string("SacredHeart");
        _9->LocalPosition = vec3(0.084, 0, 0.046);
        _9->LocalRotation = quat(0, 0.7071068, 0, 0.7071068);
        _9->LocalScale = vec3(0.4760795, 0.4760795, 0.4760795);
        {
            Transform* _10 = _9->addChild(new Transform());
            _10->Name = std::string("Jesus");
            _10->LocalPosition = vec3(0, 0, 0);
            _10->LocalRotation = quat(-0.7071068, 0, 0, 0.7071067);
            _10->LocalScale = vec3(100, 100, 100);
        }
        {
            Transform* _11 = _9->addChild(new Transform());
            _11->Name = std::string("SacredHeartBase");
            _11->LocalPosition = vec3(0.0966225, -12.53992, -0.1764411);
            _11->LocalRotation = quat(0, -0.7071068, 0, 0.7071068);
            _11->LocalScale = vec3(3.374974, 25.20823, 3.292908);
        }
    }
    {
        Transform* _12 = _0->addChild(new Transform());
        _12->Name = std::string("RotationPivot");
        _12->LocalPosition = vec3(0, 2.739999, 0);
        _12->LocalRotation = quat(0, 0, 0, 1);
        _12->LocalScale = vec3(1, 1, 1);
    }
    return _0;
}

