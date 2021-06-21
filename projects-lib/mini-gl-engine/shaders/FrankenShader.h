#ifndef _FrankenShader_h_
#define _FrankenShader_h_

#include <mini-gl-engine/DefaultEngineShader.h>

#include <mini-gl-engine/ComponentCamera.h>
#include <mini-gl-engine/ComponentMaterial.h>

namespace GLEngine {
    
    
    static void findAndReplaceAll(std::string *data, const std::string &toSearch, const std::string &replaceStr)
    {
        // Get the first occurrence
        size_t pos = data->find(toSearch);
        
        // Repeat till end is reached
        while( pos != std::string::npos)
        {
            // Replace this occurrence of Sub String
            data->replace(pos, toSearch.size(), replaceStr);
            // Get the next occurrence from the current position
            pos = data->find(toSearch, pos + replaceStr.size());
        }
    }
    
    enum ShaderAlgorithmsEnum {
        
        //generate texture albedo access
        ShaderAlgorithms_TextureAlbedo = (1 << 0),
        
        //generate texture specular access
        ShaderAlgorithms_TextureSpecular = (1 << 1),

        //generate normal
        ShaderAlgorithms_NormalMap = (1 << 2),//need aTangent
        
        //generate output color
        ShaderAlgorithms_AmbientLightColor = (1 << 3),//needs Albedo
        ShaderAlgorithms_AmbientLightSkybox = (1 << 4),//needs textureCube could use normalMap
        
        //generate output color
        ShaderAlgorithms_SunLight0 = (1 << 5),//needs camera pos world
        ShaderAlgorithms_SunLight1 = (1 << 6),//needs camera pos world
        ShaderAlgorithms_SunLight2 = (1 << 7),//needs camera pos world
        ShaderAlgorithms_SunLight3 = (1 << 8),//needs camera pos world
        


        //mesh skinning
        ShaderAlgorithms_skinGradientMatrix_16 = (1 << 9),//can upload max of 16 matrix
        ShaderAlgorithms_skinGradientMatrix_32 = (1 << 10),//can upload max of 32 matrix
        ShaderAlgorithms_skinGradientMatrix_64 = (1 << 11),//can upload max of 64 matrix
        ShaderAlgorithms_skinGradientMatrix_96 = (1 << 12),//can upload max of 96 matrix
        ShaderAlgorithms_skinGradientMatrix_128 = (1 << 13),//can upload max of 128 matrix


        //used to render on non-capable sRGB devices
        ShaderAlgorithms_sRGB = (1 << 31)
    };
    
    class FrankenShader : public DefaultEngineShader {
        
        int u_mvp;
        int u_cameraPosWorld;
        
        //material setup
        struct PBRUniform {
            int u_textureAlbedo;
            int u_textureSpecular;
            int u_materialAlbedoColor;
            int u_materialRoughness;
            int u_materialMetallic;
        } PBR;
        
        struct normalMapUniform {
            int u_localToWorld;
            int u_localToWorld_it;
            int u_textureNormal;
        } normalMap;
        
        struct AmbientLight {
            int u_textureCubeAmbient;
            int u_ambientColor;
        } ambientLight;
        
        struct SunLightUniform {
            int u_lightRadiance;
            int u_lightDir;
        } sunLight[4];

        // Mesh Skinning uniform
        int u_gradient_matrix_array;
        
        void compileShader();
        void queryShaderUniforms();
    public:
        uint32_t frankenFormat;
        
        FrankenShader( uint32_t shaderAlgorithms ){
            frankenFormat = shaderAlgorithms;
            compileShader();
        }

        void sendSkinMatrix(aRibeiro::aligned_vector<aRibeiro::mat4> *gradient_matrix);
        
        void sendParamaters(const aRibeiro::mat4 *mvp, Transform *node, Components::ComponentCamera *camera,
            const Components::ComponentMaterial* material, const aRibeiro::vec3 &ambientLightColor,
                            int albedoTexUnit, int specularTexUnit, int normalTexUnit, int ambientCubeTexUnit);
        void sendSunLightConfig(int index, const aRibeiro::vec3 &radiance, const aRibeiro::vec3 &worldDirection);
        
        
        static uint32_t MountFormat(uint32_t flags, int sunCount ){
            
            ARIBEIRO_ABORT(sunCount > 4,"FrankenShader support max of 4 sun light.\n");
            
            for(int i=0;i<sunCount;i++)
                flags |= ShaderAlgorithms_SunLight0 << i;
            
            //check incompatible flags
            if (!(flags & (ShaderAlgorithms_AmbientLightSkybox |
                                   ShaderAlgorithms_SunLight0 |
                                   ShaderAlgorithms_SunLight1 |
                                   ShaderAlgorithms_SunLight2 |
                                   ShaderAlgorithms_SunLight3
                                   )))
                flags &= ~ShaderAlgorithms_NormalMap;
            if (flags & ShaderAlgorithms_AmbientLightSkybox)
                flags &= ~ShaderAlgorithms_AmbientLightColor;
            
            return flags;
        }
        
    };
    
    
}

#endif
