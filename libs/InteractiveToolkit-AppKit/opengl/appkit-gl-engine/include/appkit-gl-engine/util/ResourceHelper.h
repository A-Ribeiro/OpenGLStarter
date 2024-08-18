#pragma once

#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/util/CubeMapHelper.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>
#include <appkit-gl-engine/GL/GLCubeMapSkyBox.h>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLCubeMap.h>

#include <InteractiveToolkit/MathCore/MathCore.h>

// #include "Basof2ToResource.h"

namespace AppKit
{
    namespace GLEngine
    {

        class ResourceHelper : public EventCore::HandleCallback
        {

        private:
            CubeMapHelper *cubeMapHelper;

        public:
            std::shared_ptr<AppKit::OpenGL::GLTexture> defaultAlbedoTexture;
            std::shared_ptr<AppKit::OpenGL::GLTexture> defaultNormalTexture;

            std::shared_ptr<Components::ComponentMaterial> defaultPBRMaterial;

            ResourceHelper();

            void initialize();

            void finalize();

            /*
            bool copyCubeMap(
                AppKit::OpenGL::GLCubeMap *src,
                AppKit::OpenGL::GLCubeMap *target,
                bool sRGB,
                int targetResolution) ;*/

            void copyCubeMapEnhanced(
                AppKit::OpenGL::GLCubeMap *inputcubemap,
                int inputMip,
                AppKit::OpenGL::GLCubeMap *targetcubemap,
                int outputMip);
            void render1x1CubeIntoSphereTexture(
                AppKit::OpenGL::GLCubeMap *inputcubemap,
                AppKit::OpenGL::GLTexture *targetTexture,
                int width, int height);

            // Load skybox from the folder: resources/Skyboxes/
            std::shared_ptr<AppKit::GLEngine::GLCubeMapSkyBox> createSkybox(const std::string &name, bool sRGB, int maxResolution = 1024);

            // Load cubemap from the folder: resources/Skyboxes/
            std::shared_ptr<AppKit::OpenGL::GLCubeMap> createCubeMap(const std::string &name, bool sRGB, int maxResolution = 1024);

            std::shared_ptr<AppKit::OpenGL::GLTexture> createTextureFromFile(const std::string &path, bool sRGB);

            std::shared_ptr<Transform> createTransformFromModel(const std::string &path, uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);

        private:
            // static bool traverse_delete(Transform *element, void *userData);

        public:
            // static void releaseTransformRecursive(Transform **root);

        private:
            static std::shared_ptr<Transform> traverse_copy(std::shared_ptr<Transform> element);
            static bool traverse_remove_empty(std::shared_ptr<Transform> root);

        public:
            static std::shared_ptr<Transform> cloneTransformRecursive(std::shared_ptr<Transform> root);
            static std::shared_ptr<Transform> removeEmptyTransforms(std::shared_ptr<Transform> root);

        public:
            static MathCore::vec4f vec4ColorGammaToLinear(const MathCore::vec4f &v);
            static MathCore::vec4f vec4ColorLinearToGamma(const MathCore::vec4f &v);

            static MathCore::vec3f vec3ColorGammaToLinear(const MathCore::vec3f &v);
            static MathCore::vec3f vec3ColorLinearToGamma(const MathCore::vec3f &v);

            // static void setTexture(AppKit::OpenGL::GLTexture **dst, AppKit::OpenGL::GLTexture *src);

        private:
            bool addAABBMesh_traverser(std::shared_ptr<Transform> element, void *userData);

        public:
            void addAABBMesh(std::shared_ptr<Transform> element, bool recursive = true);
        };

    }
}