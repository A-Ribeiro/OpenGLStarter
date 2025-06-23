#include <appkit-gl-engine/util/ResourceHelper.h>

// #include <appkit-gl-engine/mini-gl-engine.h>
// #include <aRibeiroCore/aRibeiroCore.h>
#include <appkit-gl-engine/util/Basof2ToResource.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

namespace AppKit
{
    namespace GLEngine
    {

        ResourceHelper::ResourceHelper()
        {
            cubeMapHelper = nullptr;
            // defaultAlbedoTexture = nullptr;
            // defaultNormalTexture = nullptr;
            // defaultPBRMaterial = nullptr;
        }

        void ResourceHelper::initialize()
        {

            AppKit::GLEngine::Engine *engine = AppKit::GLEngine::Engine::Instance();

            cubeMapHelper = new CubeMapHelper();

            // defaultAlbedoTexture = createTextureFromFile("DEFAULT_ALBEDO", true && engine->sRGBCapable);
            // defaultNormalTexture = createTextureFromFile("DEFAULT_NORMAL", false);

            // defaultPBRMaterial = Component::CreateShared<Components::ComponentMaterial>();

            // defaultPBRMaterial->type = Components::MaterialPBR;
            // defaultPBRMaterial->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
            // defaultPBRMaterial->pbr.metallic = 0.0f;
            // defaultPBRMaterial->pbr.roughness = 1.0f;

            // // ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

            // // texRefCount->add(defaultAlbedoTexture);
            // // texRefCount->add(defaultNormalTexture);

            // defaultPBRMaterial->pbr.texAlbedo = defaultAlbedoTexture;
            // defaultPBRMaterial->pbr.texNormal = defaultNormalTexture;

            // ReferenceCounter<Component *> *compRefCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;
            // compRefCount->add(defaultPBRMaterial);
        }

        void ResourceHelper::finalize()
        {
            // ReferenceCounter<Component *> *compRefCount = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;

            // compRefCount->removeNoDelete(defaultPBRMaterial);

            // if (defaultPBRMaterial != nullptr)
            // {
            //     delete defaultPBRMaterial;
            //     defaultPBRMaterial = nullptr;
            // }
            // defaultPBRMaterial = nullptr;

            // ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

            // texRefCount->removeNoDelete(defaultAlbedoTexture);
            // texRefCount->removeNoDelete(defaultNormalTexture);

            // if (defaultAlbedoTexture != nullptr)
            // {
            //     delete defaultAlbedoTexture;
            //     defaultAlbedoTexture = nullptr;
            // }
            // if (defaultNormalTexture != nullptr)
            // {
            //     delete defaultNormalTexture;
            //     defaultNormalTexture = nullptr;
            // }

            // defaultAlbedoTexture = nullptr;
            // defaultNormalTexture = nullptr;

            if (cubeMapHelper != nullptr)
            {
                delete cubeMapHelper;
                cubeMapHelper = nullptr;
            }
        }

        /*
            bool ResourceHelper::copyCubeMap(
                AppKit::OpenGL::GLCubeMap *src,
                AppKit::OpenGL::GLCubeMap *target,
                bool sRGB,
                int targetResolution) {
                // the cubemap copy uses the gamma correction to copy the skybox
                if (sRGB)
                    glEnable(GL_FRAMEBUFFER_SRGB);

                //1x1 resolution cubemap copy
                cubeMapHelper->copyCubeMap(targetResolution,
                    src,
                    target
                );

                if (sRGB)
                    glDisable(GL_FRAMEBUFFER_SRGB);

                return true;
            }
            */

        void ResourceHelper::copyCubeMapEnhanced(AppKit::OpenGL::GLCubeMap *inputcubemap, int inputMip, AppKit::OpenGL::GLCubeMap *targetcubemap, int outputMip)
        {
            cubeMapHelper->copyCubeMapEnhanced(inputcubemap, inputMip, targetcubemap, outputMip);
        }

        void ResourceHelper::render1x1CubeIntoSphereTexture(AppKit::OpenGL::GLCubeMap *inputcubemap, AppKit::OpenGL::GLTexture *targetTexture, int width, int height)
        {
            cubeMapHelper->render1x1CubeIntoSphereTexture(inputcubemap, targetTexture, width, height);
        }

        std::shared_ptr<AppKit::GLEngine::GLCubeMapSkyBox> ResourceHelper::createSkybox(const std::string &name, bool sRGB, int maxResolution)
        {
            std::string basePath = std::string("resources/Skyboxes/");
            std::string separator = std::string("/");
            return std::make_shared<GLCubeMapSkyBox>( sRGB,
                                       basePath + name + separator + std::string("negz.jpg"),
                                       basePath + name + separator + std::string("posz.jpg"),
                                       basePath + name + separator + std::string("negx.jpg"),
                                       basePath + name + separator + std::string("posx.jpg"),
                                       basePath + name + separator + std::string("negy.jpg"),
                                       basePath + name + separator + std::string("posy.jpg"),
                                       10.0f,        // distance
                                       maxResolution // cubeResolution
            );
        }

        std::shared_ptr<AppKit::OpenGL::GLCubeMap> ResourceHelper::createCubeMap(const std::string &name, bool sRGB, int maxResolution)
        {
            GLint maxCubeMapSize;
            glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapSize);

            if (maxResolution > maxCubeMapSize)
                maxResolution = maxCubeMapSize; 

            auto cubeMap = std::make_shared<AppKit::OpenGL::GLCubeMap>(0, 0, 0xffffffff, maxResolution);

            std::string basePath = std::string("resources/Skyboxes/");
            std::string separator = std::string("/");

            if (name.length() > 0)
                cubeMap->loadFromFile(
                    basePath + name + separator + std::string("negz.jpg"),
                    basePath + name + separator + std::string("posz.jpg"),
                    basePath + name + separator + std::string("negx.jpg"),
                    basePath + name + separator + std::string("posx.jpg"),
                    basePath + name + separator + std::string("negy.jpg"),
                    basePath + name + separator + std::string("posy.jpg"),
                    sRGB, true);

            cubeMap->active(0);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            cubeMap->deactive(0);

            return cubeMap;
        }

        std::shared_ptr<AppKit::OpenGL::GLTexture> ResourceHelper::createTextureFromFile(const std::string &path, bool sRGB)
        {
            if (path.compare("DEFAULT_ALBEDO") == 0)
            {
                auto result =  std::make_shared<AppKit::OpenGL::GLTexture>();
                {
                    uint32_t tex[16 * 16];
                    for (int i = 0; i < 16 * 16; i++)
                        tex[i] = 0xffffffff;
                    result->uploadBufferRGBA_8888(tex, 16, 16, sRGB);
                    result->active(0);
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                    result->deactive(0);
                }
                return result;
            }
            else if (path.compare("DEFAULT_NORMAL") == 0)
            {
                auto result =  std::make_shared<AppKit::OpenGL::GLTexture>();
                {
                    uint32_t tex[16 * 16];
                    for (int i = 0; i < 16 * 16; i++)
                        tex[i] = 0xffff7f7f;
                    result->uploadBufferRGBA_8888(tex, 16, 16, false);
                    result->active(0);
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                    OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                    result->deactive(0);
                }
                return result;
            }
            else
            {
                AppKit::OpenGL::GLTexture *result1 = AppKit::OpenGL::GLTexture::loadFromFile(path.c_str(), false, sRGB);
                auto result = std::shared_ptr<AppKit::OpenGL::GLTexture>(result1, std::default_delete<AppKit::OpenGL::GLTexture>());
                result->generateMipMap();
                result->setAnisioLevel(16.0f);
                return result;
            }
            return nullptr;
        }

        std::shared_ptr<Transform> ResourceHelper::createTransformFromModel(const std::string &path, std::shared_ptr<Components::ComponentMaterial> defaultPBRMaterial, uint32_t model_dynamic_upload, uint32_t model_static_upload)
        {
            return Basof2ToResource::loadAndConvert(path.c_str(), defaultPBRMaterial, nullptr, model_dynamic_upload, model_static_upload);
        }

        // bool ResourceHelper::traverse_delete(Transform *element, void *userData)
        // {
        //     Transform *rootNode = (Transform *)userData;

        //     ReferenceCounter<AppKit::GLEngine::Component *> *refCounter = &Engine::Instance()->componentReferenceCounter;

        //     Transform *parent = element->Parent;
        //     if (parent != nullptr)
        //     {
        //         ITK_ABORT(parent->getChildAt(parent->getChildCount() - 1) != element, "Wrong traverse setup...");
        //         parent->removeChild(parent->getChildCount() - 1);
        //     }

        //     for (int i = element->getComponentCount() - 1; i >= 0; i--)
        //     {
        //         Component *component = element->removeComponentAt(i);
        //         refCounter->remove(component);
        //     }

        //     // avoid delete the caller object (the root)...
        //     if (rootNode != element)
        //         delete element;

        //     return true;
        // }

        // void ResourceHelper::releaseTransformRecursive(Transform **root)
        // {
        //     if (root == nullptr || *root == nullptr)
        //         return;

        //     Transform *node = *root;
        //     *root = nullptr;

        //     // traverse will delete all children, but the root remains allocated...
        //     node->traversePostOrder_DepthFirst(&ResourceHelper::traverse_delete, node);

        //     delete node;
        // }

        std::shared_ptr<Transform> ResourceHelper::traverse_copy(std::shared_ptr<Transform> element)
        {
            auto result = Transform::CreateShared();

            result->setName(element->getName());
            result->setLocalPosition(element->getLocalPosition());
            result->setLocalRotation(element->getLocalRotation());
            result->setLocalScale(element->getLocalScale());

            // copy components by reference (just material and mesh)
            //   all external refs need to be done after clone manually...
            //ReferenceCounter<Component *> *refComponent = &AppKit::GLEngine::Engine::Instance()->componentReferenceCounter;
            for (int i = 0; i < element->getComponentCount(); i++)
            {
                if (element->getComponentAt(i)->compareType(Components::ComponentMaterial::Type) ||
                    element->getComponentAt(i)->compareType(Components::ComponentMesh::Type))
                    result->addComponent(element->getComponentAt(i));
            }

            for (int i = 0; i < element->getChildCount(); i++)
            {
                result->addChild(traverse_copy(element->getChildAt(i)));
            }

            return result;
        }

        std::shared_ptr<Transform> ResourceHelper::cloneTransformRecursive(std::shared_ptr<Transform>root)
        {
            return traverse_copy(root);
        }

        bool ResourceHelper::traverse_remove_empty(std::shared_ptr<Transform> element)
        {

            if (element->getComponentCount() > 0)
                return false;

            for (int i = element->getChildCount() - 1; i >= 0; i--)
            {
                if (traverse_remove_empty(element->getChildAt(i)))
                {
                    printf("Removing children : %s\n", element->getChildAt(i)->getName().c_str());
                    //delete element->removeChild(i);
                    element->removeChild(i);
                }
            }

            return element->getComponentCount() == 0 && element->getChildCount() == 0;
        }

        std::shared_ptr<Transform> ResourceHelper::removeEmptyTransforms(std::shared_ptr<Transform> root)
        {
            traverse_remove_empty(root);
            return root;
        }

        MathCore::vec4f ResourceHelper::vec4ColorGammaToLinear(const MathCore::vec4f &v)
        {
            MathCore::vec4f result;
            result.r = (float)pow(v.r, 2.2);
            result.g = (float)pow(v.g, 2.2);
            result.b = (float)pow(v.b, 2.2);
            result.a = v.a;
            return result;
        }

        MathCore::vec4f ResourceHelper::vec4ColorLinearToGamma(const MathCore::vec4f &v)
        {
            MathCore::vec4f result;
            result.r = (float)pow(v.r, 1.0 / 2.2);
            result.g = (float)pow(v.g, 1.0 / 2.2);
            result.b = (float)pow(v.b, 1.0 / 2.2);
            result.a = v.a;
            return result;
        }

        MathCore::vec3f ResourceHelper::vec3ColorGammaToLinear(const MathCore::vec3f &v)
        {
            MathCore::vec3f result;
            result.r = (float)pow(v.r, 2.2);
            result.g = (float)pow(v.g, 2.2);
            result.b = (float)pow(v.b, 2.2);
            return result;
        }

        MathCore::vec3f ResourceHelper::vec3ColorLinearToGamma(const MathCore::vec3f &v)
        {
            MathCore::vec3f result;
            result.r = (float)pow(v.r, 1.0 / 2.2);
            result.g = (float)pow(v.g, 1.0 / 2.2);
            result.b = (float)pow(v.b, 1.0 / 2.2);
            return result;
        }

        // void ResourceHelper::setTexture(AppKit::OpenGL::GLTexture **dst, AppKit::OpenGL::GLTexture *src)
        // {
        //     ReferenceCounter<AppKit::OpenGL::GLTexture *> *texRefCount = &AppKit::GLEngine::Engine::Instance()->textureReferenceCounter;

        //     texRefCount->remove(*dst);
        //     *dst = texRefCount->add(src);
        // }

        bool ResourceHelper::addAABBMesh_traverser(std::shared_ptr<Transform> element, void *userData)
        {
            // skip if the element has a mesh wrapper already
            auto meshWrapper = element->findComponent<Components::ComponentMeshWrapper>();
            if (meshWrapper != nullptr)
                return true;

            auto material = element->findComponent<Components::ComponentMaterial>();
            auto mesh = element->findComponent<Components::ComponentMesh>();

            if (material != nullptr && mesh != nullptr)
            {

                meshWrapper = element->addNewComponent<Components::ComponentMeshWrapper>();
                // meshWrapper->updateMeshOBB();
                meshWrapper->updateMeshAABB();
                // meshWrapper->updateMeshSphere();
                // meshWrapper->computeFinalPositions(false);
                // meshWrapper->debugCollisionShapes = true;

                element->makeFirstComponent(meshWrapper);
            }

            return true;
        }

        void ResourceHelper::addAABBMesh(std::shared_ptr<Transform> element, bool recursive)
        {
            if (recursive)
                element->traversePreOrder_DepthFirst(
                    EventCore::CallbackWrapper(&ResourceHelper::addAABBMesh_traverser, this)
                );
            else
                ResourceHelper::addAABBMesh_traverser(element, nullptr);
        }

    }

}
