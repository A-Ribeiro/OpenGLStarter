#include "SceneJesusCross.h"

//#include <appkit-gl-engine/mini-gl-engine.h>

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
void SceneJesusCross::loadResources() {
	
    AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

    particleTexture = resourceHelper->createTextureFromFile("resources/smoke.png",true && engine->sRGBCapable);
    
    //diffuse + normal
    JesusTextures[0] = resourceHelper->createTextureFromFile("resources/Jesus/UVJesus.jpg",true && engine->sRGBCapable);
    JesusTextures[1] = nullptr;//resourceHelper->defaultNormalTexture;
    
    //diffuse + normal
    Rock02Textures[0] = resourceHelper->createTextureFromFile("resources/Rocks/rock02_diffuse.jpg",true && engine->sRGBCapable);
    Rock02Textures[1] = resourceHelper->createTextureFromFile("resources/Rocks/rock02_normal.jpg",false);
    
    //diffuse + normal
    Rock03Textures[0] = resourceHelper->createTextureFromFile("resources/Rocks/rock03_diffuse.jpg",true && engine->sRGBCapable);
    Rock03Textures[1] = resourceHelper->createTextureFromFile("resources/Rocks/rock03_normal.jpg",false);
    
    Jesus3DModel = resourceHelper->createTransformFromModel("resources/Jesus/JesusOnCross.bams", resourceMap->defaultPBRMaterial);
    
    Rocks02_3DModel = resourceHelper->createTransformFromModel("resources/Rocks/Rocks02.bams", resourceMap->defaultPBRMaterial);
    Rocks03_3DModel = resourceHelper->createTransformFromModel("resources/Rocks/Rocks03.bams", resourceMap->defaultPBRMaterial);
    
    //ReferenceCounter<AppKit::OpenGL::GLTexture*> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;
    
    auto newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = JesusTextures[0];
    newMaterial->pbr.texNormal = nullptr;//texRefCount->add(JesusTextures[1]);
    
    Jesus3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);
    
    newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = Rock02Textures[0];
    newMaterial->pbr.texNormal = Rock02Textures[1];
    
    Rocks02_3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);
    
    newMaterial = Component::CreateShared<ComponentMaterial>();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = Rock03Textures[0];
    newMaterial->pbr.texNormal = Rock03Textures[1];
    
    Rocks03_3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, &newMaterial);
    
}

//to load the scene graph
void SceneJesusCross::loadGraph() {
    root = loadSceneroot();
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

		/*
        ResourceHelper::vec3ColorLinearToGamma(
            //global ambient light
            ResourceHelper::vec3ColorGammaToLinear(MathCore::vec3f(0.2641509f,0.2641509f,0.2641509f))
            +
            //light0 global light
            ResourceHelper::vec3ColorGammaToLinear(MathCore::vec3f(0.2705882f,0.2901961f,0.3098039f))
        );
		*/
        
        if (engine->sRGBCapable) {
            //renderState->ClearColor = ResourceHelper::vec4ColorGammaToLinear(renderState->ClearColor);
            //renderPipeline->ambientLight.color = ResourceHelper::vec3ColorLinearToGamma(renderPipeline->ambientLight.color);
        }
        
    }
    
    //setup camera
    {
        auto mainCamera = root->findTransformByName("Main Camera");
        camera = mainCamera->addNewComponent<ComponentCameraPerspective>();
        
        ((ComponentCameraPerspective*)camera.get())->fovDegrees = 60.0f;
		//((ComponentCameraPerspective*)camera)->nearPlane = 5.0f;
		((ComponentCameraPerspective*)camera.get())->farPlane = 50.0f;

        auto toLookNode = root->findTransformByName("ToLookNode");
        
        mainCamera->lookAtLeftHanded(toLookNode);

		//componentCameraRotateOnTarget
		{
			componentCameraRotateOnTarget = mainCamera->addNewComponent<ComponentCameraRotateOnTarget>();
			componentCameraRotateOnTarget->Target = toLookNode;
		}

/*
		//setup rotation
		{
				distanceRotation = ::distance(mainCamera->Position, toLookNode->Position);


                //convert the transform camera to camera move euler angles...
                vec3 forward = MathCore::CVT<MathCore::vec4f>::toVec3(mainCamera->getMatrix()[2]);
                vec3 proj_y = MathCore::vec3f(forward.x, 0, forward.z);
                float length_proj_y = length(proj_y);
                proj_y = normalize(proj_y);
                vec3 cone_proj_x = normalize(MathCore::vec3f(length_proj_y, forward.y, 0));

                euler.x = -atan2(cone_proj_x.y, cone_proj_x.x);
                euler.y = atan2(proj_y.x, proj_y.z);
                euler.z = 0;

                while (euler.x < -MathCore::OP<float>::deg_2_rad(90.0f))
                    euler.x += MathCore::OP<float>::deg_2_rad(360.0f);
                while (euler.x > MathCore::OP<float>::deg_2_rad(90.0f))
                    euler.x -= MathCore::OP<float>::deg_2_rad(360.0f);
				

		}*/
    }
    
    //light
    {
        auto lightTransform = root->findTransformByName("Directional Light");
        auto light = lightTransform->addNewComponent<ComponentLight>();
        light->type = LightSun;
        light->sun.color = MathCore::vec3f(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.5f + 0.5f;
        
        //if (engine->sRGBCapable)
            //light->sun.color = ResourceHelper::vec3ColorLinearToGamma(light->sun.color);
    }
    
    //Jesus 3DModel
    {
        auto node = root->findTransformByName("JesusCross1")->findTransformByName("ToInsertModel");
        node->addChild(ResourceHelper::cloneTransformRecursive(Jesus3DModel));
    }
    
    //rocks
    {
        auto nodes = root->findTransformsByName("rock02");
        
        for(size_t i=0;i<nodes.size();i++)
            nodes[i]->addChild( ResourceHelper::cloneTransformRecursive(Rocks02_3DModel) );

        nodes = root->findTransformsByName("rock03");
        
        for(size_t i=0;i<nodes.size();i++)
            nodes[i]->addChild( ResourceHelper::cloneTransformRecursive(Rocks03_3DModel) );
    }


	//Particle System
	{
		auto particleSystem = Component::CreateShared<ComponentParticleSystem>();
		auto node = root->findTransformByName("Particle System");
        node->addComponent(particleSystem);


		// duration: 25
		// prewarm: True
		// startLifeTime: 25
		// startSpeed: 1
		// startSize: 8
	
			// rateOverTime: 1.5
	
			// time -> Color
			// 0 -> 1, 1, 1
			// 1 -> 1, 1, 1
			// time -> Alpha
			// 0 -> 0
			// 0.05000382 -> 1
			// 0.9499962 -> 1
			// 1 -> 0

		particleSystem->duration_sec = 25.0f;
		particleSystem->loop = true;
		particleSystem->setLifetime(25.0f);
		particleSystem->Speed.addKey(Key<float>(0.0f, 1.0f));
		particleSystem->Size.addKey(Key<float>(0.0f, 8.0f));
		particleSystem->setRateOverTime(1.5f);
		particleSystem->Color.addKey(Key<MathCore::vec3f>(0.0f, MathCore::vec3f(1,1,1)));

		particleSystem->Alpha.addKey(Key<float>(0.0f, 0.0f));
		particleSystem->Alpha.addKey(Key<float>(0.05f, 1.0f));
		particleSystem->Alpha.addKey(Key<float>(0.95f, 1.0f));
		particleSystem->Alpha.addKey(Key<float>(1.0f, 0.0f));

		//sample some values
        /*
        printf("-1.0f = %f\n", particleSystem->Alpha.getValue(-1.0f));
		printf("0.0f = %f\n",particleSystem->Alpha.getValue(0.0f));
		printf("0.025f = %f\n",particleSystem->Alpha.getValue(0.025f));
		printf("0.05f = %f\n",particleSystem->Alpha.getValue(0.05f));
		printf("0.5f = %f\n",particleSystem->Alpha.getValue(0.5f));
		printf("0.95f = %f\n",particleSystem->Alpha.getValue(0.95f));
		printf("0.975f = %f\n",particleSystem->Alpha.getValue(0.975f));
		printf("1.0f = %f\n",particleSystem->Alpha.getValue(1.0f));
        printf("2.0f = %f\n", particleSystem->Alpha.getValue(2.0f));
        */

		particleSystem->boxEmmiter = node->findTransformByName("ParticleBox");

		// ResourceHelper::setTexture(&particleSystem->texture,particleTexture);
        particleSystem->texture = particleTexture;
		particleSystem->textureColor = MathCore::vec4f(0.3584906f, 0.3584906f, 0.3584906f, 0.6039216f);
		
		particleSystem->prewarmStart();
		//particleSystem->emmitStart();
	}
    
    /*
    {
        Transform *node = root->findTransformByName("rock03 (test)");
        node->addChild(ResourceHelper::cloneTransformRecursive(Rocks03_3DModel));
    }
    */
    
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
    
    // ResourceHelper::releaseTransformRecursive(&Rocks02_3DModel);
    // ResourceHelper::releaseTransformRecursive(&Rocks03_3DModel);
    root = nullptr;
    camera = nullptr;

    Jesus3DModel = nullptr;
    JesusTextures[0] = nullptr;
    JesusTextures[1] = nullptr;
    
    Rocks02_3DModel = nullptr;
    Rock02Textures[0] = nullptr;
    Rock02Textures[1] = nullptr;
    
    Rocks03_3DModel = nullptr;
    Rock03Textures[0] = nullptr;
    Rock03Textures[1] = nullptr;

    particleTexture = nullptr;
    
    componentCameraRotateOnTarget = nullptr;
}

SceneJesusCross::SceneJesusCross(
    Platform::Time *_time,
    AppKit::GLEngine::RenderPipeline *_renderPipeline,
    AppKit::GLEngine::ResourceHelper *_resourceHelper,
    AppKit::GLEngine::ResourceMap *_resourceMap,
    std::shared_ptr<AppKit::GLEngine::RenderWindowRegion> renderWindow) : AppKit::GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper, _resourceMap, renderWindow) 
{
    Jesus3DModel = nullptr;
	Rocks02_3DModel = nullptr;
	Rocks03_3DModel = nullptr;

//    mouseMoving = false;
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

	keyP.setState(Keyboard::isPressed(KeyCode::P));

    if (keyP.down)
        time->timeScale = 1.0f - time->timeScale;

	mouseBtn1.setState(app->mousePressed);

    if (mouseBtn1.down) {
        if (app->sceneGUI->button_SoftParticles->selected) {

			componentCameraRotateOnTarget->enabled = false;

			auto node = root->findTransformByName("Particle System");
			auto particleSystem = node->findComponent<ComponentParticleSystem>();
			
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
	}
	{
		auto _3 = _0->addChild(Transform::CreateShared());
		_3->Name = std::string("JesusCross1");
		_3->LocalPosition = MathCore::vec3f(0,0,0);
		_3->LocalRotation = quatf(0,1,0,0);
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
		_5->Name = std::string("props");
		_5->LocalPosition = MathCore::vec3f(0,0,0);
		_5->LocalRotation = quatf(0,0,0,1);
		_5->LocalScale = MathCore::vec3f(1,1,1);
		{
			auto _6 = _5->addChild(Transform::CreateShared());
			_6->Name = std::string("Base");
			_6->LocalPosition = MathCore::vec3f(0,-0.75,0);
			_6->LocalRotation = quatf(0,0,0,1);
			_6->LocalScale = MathCore::vec3f(1,1,1);
			{
				auto _7 = _6->addChild(Transform::CreateShared());
				_7->Name = std::string("rock03");
				_7->LocalPosition = MathCore::vec3f(1,0,0);
				_7->LocalRotation = quatf(0,0,0,1);
				_7->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _8 = _6->addChild(Transform::CreateShared());
				_8->Name = std::string("rock03");
				_8->LocalPosition = MathCore::vec3f(-1,0,0);
				_8->LocalRotation = quatf(0,0,0,1);
				_8->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _9 = _6->addChild(Transform::CreateShared());
				_9->Name = std::string("rock03");
				_9->LocalPosition = MathCore::vec3f(0.03594612,-1.080334E-07,1.001629);
				_9->LocalRotation = quatf(3.723534E-18,0.7121994,8.490078E-08,0.7019773);
				_9->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _10 = _6->addChild(Transform::CreateShared());
				_10->Name = std::string("rock03");
				_10->LocalPosition = MathCore::vec3f(0.00703415,1.071021E-07,-0.9981615);
				_10->LocalRotation = quatf(3.723534E-18,0.7121994,8.490078E-08,0.7019773);
				_10->LocalScale = MathCore::vec3f(1,1,1);
			}
		}
		{
			auto _11 = _5->addChild(Transform::CreateShared());
			_11->Name = std::string("Top");
			_11->LocalPosition = MathCore::vec3f(0,-0.337,0);
			_11->LocalRotation = quatf(0,0,0,1);
			_11->LocalScale = MathCore::vec3f(1,1,1);
			{
				auto _12 = _11->addChild(Transform::CreateShared());
				_12->Name = std::string("rock02");
				_12->LocalPosition = MathCore::vec3f(-0.276,0,-0.192);
				_12->LocalRotation = quatf(0,0,0,1);
				_12->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _13 = _11->addChild(Transform::CreateShared());
				_13->Name = std::string("rock02");
				_13->LocalPosition = MathCore::vec3f(0.527,0,-0.192);
				_13->LocalRotation = quatf(0,0,0,1);
				_13->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _14 = _11->addChild(Transform::CreateShared());
				_14->Name = std::string("rock02");
				_14->LocalPosition = MathCore::vec3f(0.527,0,0.571);
				_14->LocalRotation = quatf(0,0,0,1);
				_14->LocalScale = MathCore::vec3f(1,1,1);
			}
			{
				auto _15 = _11->addChild(Transform::CreateShared());
				_15->Name = std::string("rock02");
				_15->LocalPosition = MathCore::vec3f(-0.276,0,0.571);
				_15->LocalRotation = quatf(0,0,0,1);
				_15->LocalScale = MathCore::vec3f(1,1,1);
			}
		}
		{
			auto _16 = _5->addChild(Transform::CreateShared());
			_16->Name = std::string("Ground");
			_16->LocalPosition = MathCore::vec3f(0,-1.98,-8);
			_16->LocalRotation = quatf(0,0,0,1);
			_16->LocalScale = MathCore::vec3f(1.3,1.3,1.3);
			{
				auto _17 = _16->addChild(Transform::CreateShared());
				_17->Name = std::string("rock03");
				_17->LocalPosition = MathCore::vec3f(0.98,-0.02,0.013366);
				_17->LocalRotation = quatf(-0.03860917,-0.03860917,0.7060519,0.7060519);
				_17->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _18 = _16->addChild(Transform::CreateShared());
				_18->Name = std::string("rock03");
				_18->LocalPosition = MathCore::vec3f(-0.98,-0.02,0.013366);
				_18->LocalRotation = quatf(0.03287782,0.03287782,0.706342,0.706342);
				_18->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _19 = _16->addChild(Transform::CreateShared());
				_19->Name = std::string("rock03");
				_19->LocalPosition = MathCore::vec3f(3.23,-0.02,0.013366);
				_19->LocalRotation = quatf(0.05547897,0.05547897,0.704927,0.704927);
				_19->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _20 = _16->addChild(Transform::CreateShared());
				_20->Name = std::string("rock03");
				_20->LocalPosition = MathCore::vec3f(-3.23,-0.02,0.013366);
				_20->LocalRotation = quatf(-0.06138256,-0.06138256,0.7044375,0.7044375);
				_20->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _21 = _16->addChild(Transform::CreateShared());
				_21->Name = std::string("rock03");
				_21->LocalPosition = MathCore::vec3f(5.73,-0.02,0.013366);
				_21->LocalRotation = quatf(0.05394091,0.05394091,0.7050464,0.7050464);
				_21->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _22 = _16->addChild(Transform::CreateShared());
				_22->Name = std::string("rock03");
				_22->LocalPosition = MathCore::vec3f(-5.73,-0.02,0.013366);
				_22->LocalRotation = quatf(-0.04556966,-0.04556966,0.7056369,0.7056369);
				_22->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _23 = _16->addChild(Transform::CreateShared());
				_23->Name = std::string("rock03");
				_23->LocalPosition = MathCore::vec3f(7.98,-0.02,0.013366);
				_23->LocalRotation = quatf(0.04452279,0.04452279,0.7057037,0.7057037);
				_23->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _24 = _16->addChild(Transform::CreateShared());
				_24->Name = std::string("rock03");
				_24->LocalPosition = MathCore::vec3f(-7.98,-0.02,0.013366);
				_24->LocalRotation = quatf(-0.03669898,-0.03669898,0.7061538,0.7061538);
				_24->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
		}
		{
			auto _25 = _5->addChild(Transform::CreateShared());
			_25->Name = std::string("Ground (1)");
			_25->LocalPosition = MathCore::vec3f(0,-1.98,8);
			_25->LocalRotation = quatf(0,0,0,1);
			_25->LocalScale = MathCore::vec3f(1.3,1.3,1.3);
			{
				auto _26 = _25->addChild(Transform::CreateShared());
				_26->Name = std::string("rock03");
				_26->LocalPosition = MathCore::vec3f(0.98,-0.02,0.013366);
				_26->LocalRotation = quatf(0.1083799,0.1083799,0.6987516,0.6987516);
				_26->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _27 = _25->addChild(Transform::CreateShared());
				_27->Name = std::string("rock03");
				_27->LocalPosition = MathCore::vec3f(-0.98,-0.02,0.013366);
				_27->LocalRotation = quatf(-0.07996473,-0.07996473,0.7025707,0.7025707);
				_27->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _28 = _25->addChild(Transform::CreateShared());
				_28->Name = std::string("rock03");
				_28->LocalPosition = MathCore::vec3f(3.23,-0.02,0.013366);
				_28->LocalRotation = quatf(0.1067489,0.1067489,0.6990026,0.6990026);
				_28->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _29 = _25->addChild(Transform::CreateShared());
				_29->Name = std::string("rock03");
				_29->LocalPosition = MathCore::vec3f(-3.23,-0.02,0.013366);
				_29->LocalRotation = quatf(-0.111026,-0.111026,0.6983361,0.6983361);
				_29->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _30 = _25->addChild(Transform::CreateShared());
				_30->Name = std::string("rock03");
				_30->LocalPosition = MathCore::vec3f(5.73,-0.02,0.013366);
				_30->LocalRotation = quatf(-0.093238,-0.093238,0.7009327,0.7009327);
				_30->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _31 = _25->addChild(Transform::CreateShared());
				_31->Name = std::string("rock03");
				_31->LocalPosition = MathCore::vec3f(-5.73,-0.02,0.013366);
				_31->LocalRotation = quatf(0.09847496,0.09847496,0.7002162,0.7002162);
				_31->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _32 = _25->addChild(Transform::CreateShared());
				_32->Name = std::string("rock03");
				_32->LocalPosition = MathCore::vec3f(7.98,-0.02,0.013366);
				_32->LocalRotation = quatf(-0.06915998,-0.06915998,0.7037165,0.7037165);
				_32->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _33 = _25->addChild(Transform::CreateShared());
				_33->Name = std::string("rock03");
				_33->LocalPosition = MathCore::vec3f(-7.98,-0.02,0.013366);
				_33->LocalRotation = quatf(-0.07008335,-0.07008335,0.7036251,0.7036251);
				_33->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
		}
		{
			auto _34 = _5->addChild(Transform::CreateShared());
			_34->Name = std::string("Ground (2)");
			_34->LocalPosition = MathCore::vec3f(0,-1.98,2.5);
			_34->LocalRotation = quatf(0,0,0,1);
			_34->LocalScale = MathCore::vec3f(1.3,1.3,1.3);
			{
				auto _35 = _34->addChild(Transform::CreateShared());
				_35->Name = std::string("rock03");
				_35->LocalPosition = MathCore::vec3f(0.98,-0.02,0.013366);
				_35->LocalRotation = quatf(-0.0393486,-0.0393486,0.7060111,0.7060111);
				_35->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _36 = _34->addChild(Transform::CreateShared());
				_36->Name = std::string("rock03");
				_36->LocalPosition = MathCore::vec3f(-0.98,-0.02,0.013366);
				_36->LocalRotation = quatf(0.0432294,0.0432294,0.7057841,0.7057841);
				_36->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _37 = _34->addChild(Transform::CreateShared());
				_37->Name = std::string("rock03");
				_37->LocalPosition = MathCore::vec3f(3.23,-0.02,0.013366);
				_37->LocalRotation = quatf(-0.04107346,-0.04107346,0.7059129,0.7059129);
				_37->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _38 = _34->addChild(Transform::CreateShared());
				_38->Name = std::string("rock03");
				_38->LocalPosition = MathCore::vec3f(-3.23,-0.02,0.013366);
				_38->LocalRotation = quatf(-0.02251909,-0.02251909,0.7067481,0.7067481);
				_38->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _39 = _34->addChild(Transform::CreateShared());
				_39->Name = std::string("rock03");
				_39->LocalPosition = MathCore::vec3f(5.73,-0.02,0.013366);
				_39->LocalRotation = quatf(-0.03170656,-0.03170656,0.7063956,0.7063956);
				_39->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _40 = _34->addChild(Transform::CreateShared());
				_40->Name = std::string("rock03");
				_40->LocalPosition = MathCore::vec3f(-5.73,-0.02,0.013366);
				_40->LocalRotation = quatf(-0.04107346,-0.04107346,0.7059129,0.7059129);
				_40->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _41 = _34->addChild(Transform::CreateShared());
				_41->Name = std::string("rock03");
				_41->LocalPosition = MathCore::vec3f(7.98,-0.02,0.013366);
				_41->LocalRotation = quatf(0.0432294,0.0432294,0.7057841,0.7057841);
				_41->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _42 = _34->addChild(Transform::CreateShared());
				_42->Name = std::string("rock03");
				_42->LocalPosition = MathCore::vec3f(-7.98,-0.02,0.013366);
				_42->LocalRotation = quatf(-0.0393486,-0.0393486,0.7060111,0.7060111);
				_42->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
		}
		{
			auto _43 = _5->addChild(Transform::CreateShared());
			_43->Name = std::string("Ground (3)");
			_43->LocalPosition = MathCore::vec3f(0,-1.98,-2.5);
			_43->LocalRotation = quatf(0,0,0,1);
			_43->LocalScale = MathCore::vec3f(1.3,1.3,1.3);
			{
				auto _44 = _43->addChild(Transform::CreateShared());
				_44->Name = std::string("rock03");
				_44->LocalPosition = MathCore::vec3f(0.98,-0.02,0.013366);
				_44->LocalRotation = quatf(0.0447691,0.0447691,0.7056881,0.7056881);
				_44->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _45 = _43->addChild(Transform::CreateShared());
				_45->Name = std::string("rock03");
				_45->LocalPosition = MathCore::vec3f(-0.98,-0.02,0.013366);
				_45->LocalRotation = quatf(0.01850988,0.01850988,0.7068645,0.7068645);
				_45->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _46 = _43->addChild(Transform::CreateShared());
				_46->Name = std::string("rock03");
				_46->LocalPosition = MathCore::vec3f(3.23,-0.02,0.013366);
				_46->LocalRotation = quatf(-0.03873239,-0.03873239,0.7060452,0.7060452);
				_46->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _47 = _43->addChild(Transform::CreateShared());
				_47->Name = std::string("rock03");
				_47->LocalPosition = MathCore::vec3f(-3.23,-0.02,0.013366);
				_47->LocalRotation = quatf(-0.01918836,-0.01918836,0.7068464,0.7068464);
				_47->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _48 = _43->addChild(Transform::CreateShared());
				_48->Name = std::string("rock03");
				_48->LocalPosition = MathCore::vec3f(5.73,-0.02,0.013366);
				_48->LocalRotation = quatf(-0.01918836,-0.01918836,0.7068464,0.7068464);
				_48->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _49 = _43->addChild(Transform::CreateShared());
				_49->Name = std::string("rock03");
				_49->LocalPosition = MathCore::vec3f(-5.73,-0.02,0.013366);
				_49->LocalRotation = quatf(-0.03873239,-0.03873239,0.7060452,0.7060452);
				_49->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _50 = _43->addChild(Transform::CreateShared());
				_50->Name = std::string("rock03");
				_50->LocalPosition = MathCore::vec3f(7.98,-0.02,0.013366);
				_50->LocalRotation = quatf(0.01850988,0.01850988,0.7068645,0.7068645);
				_50->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
			{
				auto _51 = _43->addChild(Transform::CreateShared());
				_51->Name = std::string("rock03");
				_51->LocalPosition = MathCore::vec3f(-7.98,-0.02,0.013366);
				_51->LocalRotation = quatf(0.0447691,0.0447691,0.7056881,0.7056881);
				_51->LocalScale = MathCore::vec3f(2.374482,2.374482,2.374482);
			}
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
		// duration: 25
		// prewarm: True
		// startLifeTime: 25
		// startSpeed: 1
		// startSize: 8
	
			// rateOverTime: 1.5
	
			// time -> Color
			// 0 -> 1, 1, 1
			// 1 -> 1, 1, 1
			// time -> Alpha
			// 0 -> 0
			// 0.05000382 -> 1
			// 0.9499962 -> 1
			// 1 -> 0
		{
			auto ParticleBox = _53->addChild(Transform::CreateShared());
			ParticleBox->Name = std::string("ParticleBox");
			ParticleBox->LocalPosition = MathCore::vec3f(0,0,0);
			ParticleBox->LocalRotation = quatf(0,0,0,1);
			ParticleBox->LocalScale = MathCore::vec3f(25.5,10.05,1);
		}
	}
    
    {
        auto _54 = _0->addChild(Transform::CreateShared());
        _54->Name = std::string("rock03 (test)");
        _54->LocalPosition = MathCore::vec3f(0,3.45,-4.37);
        _54->LocalRotation = quatf(0,0,0,1);
        _54->LocalScale = MathCore::vec3f(1,1,1);
    }
    
	return _0;
}
