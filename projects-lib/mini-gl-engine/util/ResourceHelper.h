#ifndef resource_manager_h_
#define resource_manager_h_

#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/CubeMapHelper.h>
#include <mini-gl-engine/ComponentMaterial.h>
#include <mini-gl-engine/GLCubeMapSkyBox.h>

#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLCubeMap.h>

#include <aribeiro/aribeiro.h>

//#include "Basof2ToResource.h"


namespace GLEngine {

    class ResourceHelper {

    private:
        
        CubeMapHelper *cubeMapHelper;

    public:

        openglWrapper::GLTexture *defaultAlbedoTexture;
        openglWrapper::GLTexture *defaultNormalTexture;

        Components::ComponentMaterial *defaultPBRMaterial;

        ResourceHelper();

        void initialize() ;

        void finalize() ;


        bool copyCubeMap(  
            openglWrapper::GLCubeMap *src, 
            openglWrapper::GLCubeMap *target, 
            bool sRGB,
            int targetResolution) ;


        // Load skybox from the folder: resources/Skyboxes/
        GLEngine::GLCubeMapSkyBox * createSkybox(const std::string &name, bool sRGB, int maxResolution = 1024);

        // Load cubemap from the folder: resources/Skyboxes/
        openglWrapper::GLCubeMap * createCubeMap (const std::string &name, bool sRGB, int maxResolution = 1024);

        openglWrapper::GLTexture * createTextureFromFile(const std::string &path, bool sRGB);


        Transform *createTransformFromModel(const std::string &path, uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);


    private:
        static bool traverse_delete(Transform *element, void* userData);
    public:
        static void releaseTransformRecursive(Transform **root);

    private:

        static Transform * traverse_copy(Transform *element);
        static bool traverse_remove_empty(Transform *root);

    public:
        static Transform* cloneTransformRecursive(Transform *root);
        static Transform* removeEmptyTransforms(Transform *root);
        
        
        
    public:
        
        static aRibeiro::vec4 vec4ColorGammaToLinear(const aRibeiro::vec4 &v);
        static aRibeiro::vec4 vec4ColorLinearToGamma(const aRibeiro::vec4 &v);
        
        static aRibeiro::vec3 vec3ColorGammaToLinear(const aRibeiro::vec3 &v);
        static aRibeiro::vec3 vec3ColorLinearToGamma(const aRibeiro::vec3 &v);
        
        
        static void setTexture( openglWrapper::GLTexture **dst, openglWrapper::GLTexture *src );
        
    };

}


#endif
