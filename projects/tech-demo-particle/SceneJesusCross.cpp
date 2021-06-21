#include "SceneJesusCross.h"

#include <mini-gl-engine/mini-gl-engine.h>

#include "App.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

Transform* loadSceneroot();

bool ReplaceMaterial(Transform *element, void* userData) {
    
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
void SceneJesusCross::loadResources() {
	
    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    particleTexture = resourceHelper->createTextureFromFile("resources/smoke.png",true && engine->sRGBCapable);
    
    //diffuse + normal
    JesusTextures[0] = resourceHelper->createTextureFromFile("resources/Jesus/UVJesus.jpg",true && engine->sRGBCapable);
    JesusTextures[1] = NULL;//resourceHelper->defaultNormalTexture;
    
    //diffuse + normal
    Rock02Textures[0] = resourceHelper->createTextureFromFile("resources/Rocks/rock02_diffuse.jpg",true && engine->sRGBCapable);
    Rock02Textures[1] = resourceHelper->createTextureFromFile("resources/Rocks/rock02_normal.jpg",false);
    
    //diffuse + normal
    Rock03Textures[0] = resourceHelper->createTextureFromFile("resources/Rocks/rock03_diffuse.jpg",true && engine->sRGBCapable);
    Rock03Textures[1] = resourceHelper->createTextureFromFile("resources/Rocks/rock03_normal.jpg",false);
    
    Jesus3DModel = resourceHelper->createTransformFromModel("resources/Jesus/JesusOnCross.bams");
    
    Rocks02_3DModel = resourceHelper->createTransformFromModel("resources/Rocks/Rocks02.bams");
    Rocks03_3DModel = resourceHelper->createTransformFromModel("resources/Rocks/Rocks03.bams");
    
    ReferenceCounter<openglWrapper::GLTexture*> *texRefCount = &GLEngine::Engine::Instance()->textureReferenceCounter;
    
    ComponentMaterial * newMaterial;
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = texRefCount->add(JesusTextures[0]);
    newMaterial->pbr.texNormal = NULL;//texRefCount->add(JesusTextures[1]);
    
    Jesus3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = texRefCount->add(Rock02Textures[0]);
    newMaterial->pbr.texNormal = texRefCount->add(Rock02Textures[1]);
    
    Rocks02_3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
    newMaterial = new ComponentMaterial();
    newMaterial->type = Components::MaterialPBR;
    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 1, 1, 1);
    newMaterial->pbr.metallic = 0.0f;
    newMaterial->pbr.roughness = 1.0f;
    newMaterial->pbr.texAlbedo = texRefCount->add(Rock03Textures[0]);
    newMaterial->pbr.texNormal = texRefCount->add(Rock03Textures[1]);
    
    Rocks03_3DModel->traversePreOrder_DepthFirst(ReplaceMaterial, newMaterial);
    
}

//to load the scene graph
void SceneJesusCross::loadGraph() {
    root = loadSceneroot();
}

//to bind the resources to the current graph
void SceneJesusCross::bindResourcesToGraph() {
    GLEngine::Engine *engine = GLEngine::Engine::Instance();
    
    //setup ambient color
    {
        GLRenderState *renderState = GLRenderState::Instance();

        //setup renderstate
        renderState->ClearColor = vec4(0.1607843f, 0.1568628f, 0.1568628f,1.0f);
        
        renderPipeline->ambientLight.color = vec3(0.2641509f,0.2641509f,0.2641509f);

		/*
        ResourceHelper::vec3ColorLinearToGamma(
            //global ambient light
            ResourceHelper::vec3ColorGammaToLinear(vec3(0.2641509f,0.2641509f,0.2641509f))
            +
            //light0 global light
            ResourceHelper::vec3ColorGammaToLinear(vec3(0.2705882f,0.2901961f,0.3098039f))
        );
		*/
        
        if (engine->sRGBCapable) {
            //renderState->ClearColor = ResourceHelper::vec4ColorGammaToLinear(renderState->ClearColor);
            //renderPipeline->ambientLight.color = ResourceHelper::vec3ColorLinearToGamma(renderPipeline->ambientLight.color);
        }
        
    }
    
    //setup camera
    {
        Transform *mainCamera = root->findTransformByName("Main Camera");
        mainCamera->addComponent(camera = new ComponentCameraPerspective());
        
        ((ComponentCameraPerspective*)camera)->fovDegrees = 60.0f;
		//((ComponentCameraPerspective*)camera)->nearPlane = 5.0f;
		((ComponentCameraPerspective*)camera)->farPlane = 50.0f;

        Transform *toLookNode = root->findTransformByName("ToLookNode");
        
        mainCamera->lookAtLeftHanded(toLookNode);

		//componentCameraRotateOnTarget
		{
			mainCamera->addComponent(componentCameraRotateOnTarget = new ComponentCameraRotateOnTarget());
			componentCameraRotateOnTarget->Target = toLookNode;
		}

/*
		//setup rotation
		{
				distanceRotation = ::distance(mainCamera->Position, toLookNode->Position);


                //convert the transform camera to camera move euler angles...
                vec3 forward = toVec3(mainCamera->getMatrix()[2]);
                vec3 proj_y = vec3(forward.x, 0, forward.z);
                float length_proj_y = length(proj_y);
                proj_y = normalize(proj_y);
                vec3 cone_proj_x = normalize(vec3(length_proj_y, forward.y, 0));

                euler.x = -atan2(cone_proj_x.y, cone_proj_x.x);
                euler.y = atan2(proj_y.x, proj_y.z);
                euler.z = 0;

                while (euler.x < -DEG2RAD(90.0f))
                    euler.x += DEG2RAD(360.0f);
                while (euler.x > DEG2RAD(90.0f))
                    euler.x -= DEG2RAD(360.0f);
				

		}*/
    }
    
    //light
    {
        Transform *lightTransform = root->findTransformByName("Directional Light");
        ComponentLight *light = (ComponentLight *)lightTransform->addComponent(new ComponentLight());
        light->type = LightSun;
        light->sun.color = aRibeiro::vec3(1, 0.9568627f, 0.8392157f);
        light->sun.intensity = 0.5f + 0.5f;
        
        //if (engine->sRGBCapable)
            //light->sun.color = ResourceHelper::vec3ColorLinearToGamma(light->sun.color);
    }
    
    //Jesus 3DModel
    {
        Transform *node = root->findTransformByName("JesusCross1")->findTransformByName("ToInsertModel");
        node->addChild(ResourceHelper::cloneTransformRecursive(Jesus3DModel));
    }
    
    //rocks
    {
        std::vector<Transform*> nodes;
        nodes = root->findTransformsByName("rock02");
        
        for(size_t i=0;i<nodes.size();i++)
            nodes[i]->addChild( ResourceHelper::cloneTransformRecursive(Rocks02_3DModel) );

        nodes = root->findTransformsByName("rock03");
        
        for(size_t i=0;i<nodes.size();i++)
            nodes[i]->addChild( ResourceHelper::cloneTransformRecursive(Rocks03_3DModel) );
    }


	//Particle System
	{
		ComponentParticleSystem *particleSystem = new ComponentParticleSystem();
		Transform *node = root->findTransformByName("Particle System");
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
		particleSystem->Color.addKey(Key<vec3>(0.0f, vec3(1,1,1)));

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

		ResourceHelper::setTexture(&particleSystem->texture,particleTexture);
		particleSystem->textureColor = aRibeiro::vec4(0.3584906f, 0.3584906f, 0.3584906f, 0.6039216f);
		
		particleSystem->prewarmStart();
		//particleSystem->emmitStart();
	}
    
    /*
    {
        Transform *node = root->findTransformByName("rock03 (test)");
        node->addChild(ResourceHelper::cloneTransformRecursive(Rocks03_3DModel));
    }
    */
    
}

//clear all loaded scene
void SceneJesusCross::unloadAll() {
    ResourceHelper::releaseTransformRecursive(&root);
    ResourceHelper::releaseTransformRecursive(&Jesus3DModel);
    
    ResourceHelper::releaseTransformRecursive(&Rocks02_3DModel);
    ResourceHelper::releaseTransformRecursive(&Rocks03_3DModel);
}

SceneJesusCross::SceneJesusCross(
    aRibeiro::PlatformTime *_time,
    GLEngine::RenderPipeline *_renderPipeline,
    GLEngine::ResourceHelper *_resourceHelper) : GLEngine::SceneBase(_time, _renderPipeline, _resourceHelper) 
{
    Jesus3DModel = NULL;
	Rocks02_3DModel = NULL;
	Rocks03_3DModel = NULL;

//    mouseMoving = false;
}

SceneJesusCross::~SceneJesusCross() {
    unload();
}

void SceneJesusCross::draw(){
    //Transform *node = root->findTransformByName("rock03 (test)");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * DEG2RAD(30.0f), 0);
    
    //Transform *node = root->findTransformByName("Directional Light");
    //node->LocalRotation = (quat)node->LocalRotation * quatFromEuler(0, time->deltaTime * DEG2RAD(30.0f), 0);

    App* app = (App*)GLEngine::Engine::Instance()->app;

	keyP.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::P));

    if (keyP.down)
        time->timeScale = 1.0f - time->timeScale;

	mouseBtn1.setState(app->mousePressed);

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

    SceneBase::draw();
}

Transform* loadSceneroot()
{
	Transform* _0 = new Transform();
	_0->Name = std::string("root");
	_0->LocalPosition = vec3(0,0,0);
	_0->LocalRotation = quat(0,0,0,1);
	_0->LocalScale = vec3(1,1,1);
	{
		Transform* _1 = _0->addChild(new Transform());
		_1->Name = std::string("Main Camera");
		//_1->LocalPosition = vec3(0,3.45,-7.37);
		//_1->LocalPosition = vec3(0,3.45,-17.37);
        _1->LocalPosition = vec3(3.736165, 6.254054, -5.700460);
		_1->LocalRotation = quat(0,0,0,1);
		_1->LocalScale = vec3(1,1,1);
	}
	{
		Transform* _2 = _0->addChild(new Transform());
		_2->Name = std::string("Directional Light");
		_2->LocalPosition = vec3(0,3,0);
		_2->LocalRotation = quat(-0.4886241,0.4844012,0.06177928,-0.723039);
		_2->LocalScale = vec3(1,1,1);
	}
	{
		Transform* _3 = _0->addChild(new Transform());
		_3->Name = std::string("JesusCross1");
		_3->LocalPosition = vec3(0,0,0);
		_3->LocalRotation = quat(0,1,0,0);
		_3->LocalScale = vec3(0.4457346,0.4457346,0.4457346);
		{
			Transform* _4 = _3->addChild(new Transform());
			_4->Name = std::string("ToInsertModel");
			_4->LocalPosition = vec3(-11.63,6.67,0.12);
			_4->LocalRotation = quat(0,0,0,1);
			_4->LocalScale = vec3(1,1,1);
		}
	}
	{
		Transform* _5 = _0->addChild(new Transform());
		_5->Name = std::string("props");
		_5->LocalPosition = vec3(0,0,0);
		_5->LocalRotation = quat(0,0,0,1);
		_5->LocalScale = vec3(1,1,1);
		{
			Transform* _6 = _5->addChild(new Transform());
			_6->Name = std::string("Base");
			_6->LocalPosition = vec3(0,-0.75,0);
			_6->LocalRotation = quat(0,0,0,1);
			_6->LocalScale = vec3(1,1,1);
			{
				Transform* _7 = _6->addChild(new Transform());
				_7->Name = std::string("rock03");
				_7->LocalPosition = vec3(1,0,0);
				_7->LocalRotation = quat(0,0,0,1);
				_7->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _8 = _6->addChild(new Transform());
				_8->Name = std::string("rock03");
				_8->LocalPosition = vec3(-1,0,0);
				_8->LocalRotation = quat(0,0,0,1);
				_8->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _9 = _6->addChild(new Transform());
				_9->Name = std::string("rock03");
				_9->LocalPosition = vec3(0.03594612,-1.080334E-07,1.001629);
				_9->LocalRotation = quat(3.723534E-18,0.7121994,8.490078E-08,0.7019773);
				_9->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _10 = _6->addChild(new Transform());
				_10->Name = std::string("rock03");
				_10->LocalPosition = vec3(0.00703415,1.071021E-07,-0.9981615);
				_10->LocalRotation = quat(3.723534E-18,0.7121994,8.490078E-08,0.7019773);
				_10->LocalScale = vec3(1,1,1);
			}
		}
		{
			Transform* _11 = _5->addChild(new Transform());
			_11->Name = std::string("Top");
			_11->LocalPosition = vec3(0,-0.337,0);
			_11->LocalRotation = quat(0,0,0,1);
			_11->LocalScale = vec3(1,1,1);
			{
				Transform* _12 = _11->addChild(new Transform());
				_12->Name = std::string("rock02");
				_12->LocalPosition = vec3(-0.276,0,-0.192);
				_12->LocalRotation = quat(0,0,0,1);
				_12->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _13 = _11->addChild(new Transform());
				_13->Name = std::string("rock02");
				_13->LocalPosition = vec3(0.527,0,-0.192);
				_13->LocalRotation = quat(0,0,0,1);
				_13->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _14 = _11->addChild(new Transform());
				_14->Name = std::string("rock02");
				_14->LocalPosition = vec3(0.527,0,0.571);
				_14->LocalRotation = quat(0,0,0,1);
				_14->LocalScale = vec3(1,1,1);
			}
			{
				Transform* _15 = _11->addChild(new Transform());
				_15->Name = std::string("rock02");
				_15->LocalPosition = vec3(-0.276,0,0.571);
				_15->LocalRotation = quat(0,0,0,1);
				_15->LocalScale = vec3(1,1,1);
			}
		}
		{
			Transform* _16 = _5->addChild(new Transform());
			_16->Name = std::string("Ground");
			_16->LocalPosition = vec3(0,-1.98,-8);
			_16->LocalRotation = quat(0,0,0,1);
			_16->LocalScale = vec3(1.3,1.3,1.3);
			{
				Transform* _17 = _16->addChild(new Transform());
				_17->Name = std::string("rock03");
				_17->LocalPosition = vec3(0.98,-0.02,0.013366);
				_17->LocalRotation = quat(-0.03860917,-0.03860917,0.7060519,0.7060519);
				_17->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _18 = _16->addChild(new Transform());
				_18->Name = std::string("rock03");
				_18->LocalPosition = vec3(-0.98,-0.02,0.013366);
				_18->LocalRotation = quat(0.03287782,0.03287782,0.706342,0.706342);
				_18->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _19 = _16->addChild(new Transform());
				_19->Name = std::string("rock03");
				_19->LocalPosition = vec3(3.23,-0.02,0.013366);
				_19->LocalRotation = quat(0.05547897,0.05547897,0.704927,0.704927);
				_19->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _20 = _16->addChild(new Transform());
				_20->Name = std::string("rock03");
				_20->LocalPosition = vec3(-3.23,-0.02,0.013366);
				_20->LocalRotation = quat(-0.06138256,-0.06138256,0.7044375,0.7044375);
				_20->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _21 = _16->addChild(new Transform());
				_21->Name = std::string("rock03");
				_21->LocalPosition = vec3(5.73,-0.02,0.013366);
				_21->LocalRotation = quat(0.05394091,0.05394091,0.7050464,0.7050464);
				_21->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _22 = _16->addChild(new Transform());
				_22->Name = std::string("rock03");
				_22->LocalPosition = vec3(-5.73,-0.02,0.013366);
				_22->LocalRotation = quat(-0.04556966,-0.04556966,0.7056369,0.7056369);
				_22->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _23 = _16->addChild(new Transform());
				_23->Name = std::string("rock03");
				_23->LocalPosition = vec3(7.98,-0.02,0.013366);
				_23->LocalRotation = quat(0.04452279,0.04452279,0.7057037,0.7057037);
				_23->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _24 = _16->addChild(new Transform());
				_24->Name = std::string("rock03");
				_24->LocalPosition = vec3(-7.98,-0.02,0.013366);
				_24->LocalRotation = quat(-0.03669898,-0.03669898,0.7061538,0.7061538);
				_24->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
		}
		{
			Transform* _25 = _5->addChild(new Transform());
			_25->Name = std::string("Ground (1)");
			_25->LocalPosition = vec3(0,-1.98,8);
			_25->LocalRotation = quat(0,0,0,1);
			_25->LocalScale = vec3(1.3,1.3,1.3);
			{
				Transform* _26 = _25->addChild(new Transform());
				_26->Name = std::string("rock03");
				_26->LocalPosition = vec3(0.98,-0.02,0.013366);
				_26->LocalRotation = quat(0.1083799,0.1083799,0.6987516,0.6987516);
				_26->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _27 = _25->addChild(new Transform());
				_27->Name = std::string("rock03");
				_27->LocalPosition = vec3(-0.98,-0.02,0.013366);
				_27->LocalRotation = quat(-0.07996473,-0.07996473,0.7025707,0.7025707);
				_27->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _28 = _25->addChild(new Transform());
				_28->Name = std::string("rock03");
				_28->LocalPosition = vec3(3.23,-0.02,0.013366);
				_28->LocalRotation = quat(0.1067489,0.1067489,0.6990026,0.6990026);
				_28->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _29 = _25->addChild(new Transform());
				_29->Name = std::string("rock03");
				_29->LocalPosition = vec3(-3.23,-0.02,0.013366);
				_29->LocalRotation = quat(-0.111026,-0.111026,0.6983361,0.6983361);
				_29->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _30 = _25->addChild(new Transform());
				_30->Name = std::string("rock03");
				_30->LocalPosition = vec3(5.73,-0.02,0.013366);
				_30->LocalRotation = quat(-0.093238,-0.093238,0.7009327,0.7009327);
				_30->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _31 = _25->addChild(new Transform());
				_31->Name = std::string("rock03");
				_31->LocalPosition = vec3(-5.73,-0.02,0.013366);
				_31->LocalRotation = quat(0.09847496,0.09847496,0.7002162,0.7002162);
				_31->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _32 = _25->addChild(new Transform());
				_32->Name = std::string("rock03");
				_32->LocalPosition = vec3(7.98,-0.02,0.013366);
				_32->LocalRotation = quat(-0.06915998,-0.06915998,0.7037165,0.7037165);
				_32->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _33 = _25->addChild(new Transform());
				_33->Name = std::string("rock03");
				_33->LocalPosition = vec3(-7.98,-0.02,0.013366);
				_33->LocalRotation = quat(-0.07008335,-0.07008335,0.7036251,0.7036251);
				_33->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
		}
		{
			Transform* _34 = _5->addChild(new Transform());
			_34->Name = std::string("Ground (2)");
			_34->LocalPosition = vec3(0,-1.98,2.5);
			_34->LocalRotation = quat(0,0,0,1);
			_34->LocalScale = vec3(1.3,1.3,1.3);
			{
				Transform* _35 = _34->addChild(new Transform());
				_35->Name = std::string("rock03");
				_35->LocalPosition = vec3(0.98,-0.02,0.013366);
				_35->LocalRotation = quat(-0.0393486,-0.0393486,0.7060111,0.7060111);
				_35->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _36 = _34->addChild(new Transform());
				_36->Name = std::string("rock03");
				_36->LocalPosition = vec3(-0.98,-0.02,0.013366);
				_36->LocalRotation = quat(0.0432294,0.0432294,0.7057841,0.7057841);
				_36->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _37 = _34->addChild(new Transform());
				_37->Name = std::string("rock03");
				_37->LocalPosition = vec3(3.23,-0.02,0.013366);
				_37->LocalRotation = quat(-0.04107346,-0.04107346,0.7059129,0.7059129);
				_37->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _38 = _34->addChild(new Transform());
				_38->Name = std::string("rock03");
				_38->LocalPosition = vec3(-3.23,-0.02,0.013366);
				_38->LocalRotation = quat(-0.02251909,-0.02251909,0.7067481,0.7067481);
				_38->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _39 = _34->addChild(new Transform());
				_39->Name = std::string("rock03");
				_39->LocalPosition = vec3(5.73,-0.02,0.013366);
				_39->LocalRotation = quat(-0.03170656,-0.03170656,0.7063956,0.7063956);
				_39->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _40 = _34->addChild(new Transform());
				_40->Name = std::string("rock03");
				_40->LocalPosition = vec3(-5.73,-0.02,0.013366);
				_40->LocalRotation = quat(-0.04107346,-0.04107346,0.7059129,0.7059129);
				_40->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _41 = _34->addChild(new Transform());
				_41->Name = std::string("rock03");
				_41->LocalPosition = vec3(7.98,-0.02,0.013366);
				_41->LocalRotation = quat(0.0432294,0.0432294,0.7057841,0.7057841);
				_41->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _42 = _34->addChild(new Transform());
				_42->Name = std::string("rock03");
				_42->LocalPosition = vec3(-7.98,-0.02,0.013366);
				_42->LocalRotation = quat(-0.0393486,-0.0393486,0.7060111,0.7060111);
				_42->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
		}
		{
			Transform* _43 = _5->addChild(new Transform());
			_43->Name = std::string("Ground (3)");
			_43->LocalPosition = vec3(0,-1.98,-2.5);
			_43->LocalRotation = quat(0,0,0,1);
			_43->LocalScale = vec3(1.3,1.3,1.3);
			{
				Transform* _44 = _43->addChild(new Transform());
				_44->Name = std::string("rock03");
				_44->LocalPosition = vec3(0.98,-0.02,0.013366);
				_44->LocalRotation = quat(0.0447691,0.0447691,0.7056881,0.7056881);
				_44->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _45 = _43->addChild(new Transform());
				_45->Name = std::string("rock03");
				_45->LocalPosition = vec3(-0.98,-0.02,0.013366);
				_45->LocalRotation = quat(0.01850988,0.01850988,0.7068645,0.7068645);
				_45->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _46 = _43->addChild(new Transform());
				_46->Name = std::string("rock03");
				_46->LocalPosition = vec3(3.23,-0.02,0.013366);
				_46->LocalRotation = quat(-0.03873239,-0.03873239,0.7060452,0.7060452);
				_46->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _47 = _43->addChild(new Transform());
				_47->Name = std::string("rock03");
				_47->LocalPosition = vec3(-3.23,-0.02,0.013366);
				_47->LocalRotation = quat(-0.01918836,-0.01918836,0.7068464,0.7068464);
				_47->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _48 = _43->addChild(new Transform());
				_48->Name = std::string("rock03");
				_48->LocalPosition = vec3(5.73,-0.02,0.013366);
				_48->LocalRotation = quat(-0.01918836,-0.01918836,0.7068464,0.7068464);
				_48->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _49 = _43->addChild(new Transform());
				_49->Name = std::string("rock03");
				_49->LocalPosition = vec3(-5.73,-0.02,0.013366);
				_49->LocalRotation = quat(-0.03873239,-0.03873239,0.7060452,0.7060452);
				_49->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _50 = _43->addChild(new Transform());
				_50->Name = std::string("rock03");
				_50->LocalPosition = vec3(7.98,-0.02,0.013366);
				_50->LocalRotation = quat(0.01850988,0.01850988,0.7068645,0.7068645);
				_50->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
			{
				Transform* _51 = _43->addChild(new Transform());
				_51->Name = std::string("rock03");
				_51->LocalPosition = vec3(-7.98,-0.02,0.013366);
				_51->LocalRotation = quat(0.0447691,0.0447691,0.7056881,0.7056881);
				_51->LocalScale = vec3(2.374482,2.374482,2.374482);
			}
		}
	}
	{
		Transform* _52 = _0->addChild(new Transform());
		_52->Name = std::string("ToLookNode");
		_52->LocalPosition = vec3(0,3.45,0);
		_52->LocalRotation = quat(0,0,0,1);
		_52->LocalScale = vec3(1,1,1);
	}
	{
		Transform* _53 = _0->addChild(new Transform());
		_53->Name = std::string("Particle System");
		_53->LocalPosition = vec3(0,3.5,-12.97);
		_53->LocalRotation = quat(0,0,0,1);
		_53->LocalScale = vec3(1,1,1);
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
			Transform* ParticleBox = _53->addChild(new Transform());
			ParticleBox->Name = std::string("ParticleBox");
			ParticleBox->LocalPosition = vec3(0,0,0);
			ParticleBox->LocalRotation = quat(0,0,0,1);
			ParticleBox->LocalScale = vec3(25.5,10.05,1);
		}
	}
    
    {
        Transform* _54 = _0->addChild(new Transform());
        _54->Name = std::string("rock03 (test)");
        _54->LocalPosition = vec3(0,3.45,-4.37);
        _54->LocalRotation = quat(0,0,0,1);
        _54->LocalScale = vec3(1,1,1);
    }
    
	return _0;
}
