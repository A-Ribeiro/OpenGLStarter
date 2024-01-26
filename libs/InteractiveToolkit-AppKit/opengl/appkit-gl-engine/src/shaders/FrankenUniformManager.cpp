#include <appkit-gl-engine/shaders/FrankenUniformManager.h>
#include <appkit-gl-engine/shaders/FrankenShader.h>
#include <appkit-gl-engine/util/Generation.h>

namespace AppKit
{
    namespace GLEngine
    {

        const AppKit::OpenGL::VirtualTexture *FrankenUniformManager::last_tex_unit[FrankenUniformTextureSlot_Count] =
            {
                NULL, NULL, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL // shadowSunLight
        };

        FrankenUniformManager::FrankenUniformManager(FrankenShader *_frankenShader)
        {
            frankenShader = _frankenShader;
            clear();
        }

        void FrankenUniformManager::clear()
        {
            memset(tex_unit, 0, sizeof(const AppKit::OpenGL::GLTexture *) * FrankenUniformTextureSlot_Count);
            different_shader = false;
        }

        void FrankenUniformManager::setMVP(const MathCore::mat4f *_mvp)
        {
            if (mvp != *_mvp)
            {
                mvp = *_mvp;
                frankenShader->setUniform(frankenShader->u_mvp, mvp);
            }
        }

        void FrankenUniformManager::setNormalTransform(Transform *node)
        {
            if (frankenShader->normalMap.u_localToWorld >= 0)
            {
                MathCore::mat4f &aux = node->getMatrix(true);
                if (aux != normal_LocalToWorld)
                {
                    normal_LocalToWorld = aux;
                    frankenShader->setUniform(frankenShader->normalMap.u_localToWorld, normal_LocalToWorld);
                }
            }
            if (frankenShader->normalMap.u_localToWorld_it >= 0)
            {
                MathCore::mat4f &aux = node->getMatrixInverseTranspose(true);
                if (aux != normal_LocalToWorld_it)
                {
                    normal_LocalToWorld_it = aux;
                    frankenShader->setUniform(frankenShader->normalMap.u_localToWorld_it, normal_LocalToWorld_it);
                }
            }
        }

        void FrankenUniformManager::setCameraReference(Components::ComponentCamera *camera)
        {
            if (frankenShader->u_cameraPosWorld >= 0)
            {
                MathCore::vec3f aux = camera->transform[0]->getPosition(true);
                if (aux != camera_PosWorld)
                {
                    camera_PosWorld = aux;
                    frankenShader->setUniform(frankenShader->u_cameraPosWorld, camera_PosWorld);
                }
            }
        }

        void FrankenUniformManager::setPBRMaterial(const Components::ComponentMaterial *material)
        {

            if (material->type != Components::MaterialPBR)
                return;

            bool diff = false;

            if (frankenShader->PBR.u_materialAlbedoColor >= 0)
                if (material->pbr.albedoColor != material_PBRSetup.albedoColor)
                {
                    frankenShader->setUniform(frankenShader->PBR.u_materialAlbedoColor, material->pbr.albedoColor);
                    diff = true;
                }

            if (frankenShader->PBR.u_materialEmissionColor >= 0)
                if (material->pbr.emissionColor != material_PBRSetup.emissionColor)
                {
                    frankenShader->setUniform(frankenShader->PBR.u_materialEmissionColor, material->pbr.emissionColor);
                    diff = true;
                }

            if (frankenShader->PBR.u_materialRoughness >= 0)
                if (material->pbr.roughness != material_PBRSetup.roughness)
                {
                    frankenShader->setUniform(frankenShader->PBR.u_materialRoughness, material->pbr.roughness);
                    diff = true;
                }

            if (frankenShader->PBR.u_materialMetallic >= 0)
                if (material->pbr.metallic != material_PBRSetup.metallic)
                {
                    frankenShader->setUniform(frankenShader->PBR.u_materialMetallic, material->pbr.metallic);
                    diff = true;
                }

            // tex_unit[FrankenUniformTextureSlot_Normal] = material->pbr.texNormal;
            // tex_unit[FrankenUniformTextureSlot_Albedo] = material->pbr.texAlbedo;
            // tex_unit[FrankenUniformTextureSlot_Specular] = material->pbr.texSpecular;
            // tex_unit[FrankenUniformTextureSlot_Emission] = material->pbr.texEmission;
            setNormalTexture(material->pbr.texNormal);
            setAlbedoTexture(material->pbr.texAlbedo);
            setSpecularTexture(material->pbr.texSpecular);
            setEmissionTexture(material->pbr.texEmission);

            if (material->pbr.texAlbedo != material_PBRSetup.texAlbedo ||
                material->pbr.texNormal != material_PBRSetup.texNormal ||
                material->pbr.texSpecular != material_PBRSetup.texSpecular ||
                material->pbr.texEmission != material_PBRSetup.texEmission)
                diff = true;

            if (diff)
                material_PBRSetup = material->pbr;
        }

        void FrankenUniformManager::setAmbientLightColor(const MathCore::vec3f &ambientLightColor)
        {
            if (frankenShader->ambientLight.u_ambientColor >= 0)
            {
                if (ambientLightColor != ambientColor)
                {
                    ambientColor = ambientLightColor;
                    frankenShader->setUniform(frankenShader->ambientLight.u_ambientColor, ambientColor);
                }
            }
        }

        void FrankenUniformManager::setNormalTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->normalMap.u_textureNormal >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_Normal] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_Normal] = frankenShader->normalMap.u_textureNormal;
            }
        }

        void FrankenUniformManager::setAlbedoTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->PBR.u_textureAlbedo >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_Albedo] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_Albedo] = frankenShader->PBR.u_textureAlbedo;
            }
        }

        void FrankenUniformManager::setSpecularTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->PBR.u_textureSpecular >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_Specular] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_Specular] = frankenShader->PBR.u_textureSpecular;
            }
        }

        void FrankenUniformManager::setEmissionTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->PBR.u_textureEmission >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_Emission] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_Emission] = frankenShader->PBR.u_textureEmission;
            }
        }

        void FrankenUniformManager::setEnvironmentCubeTexture(const AppKit::OpenGL::GLCubeMap *tex)
        {
            if (frankenShader->ambientLight.u_textureCubeAmbient >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = frankenShader->ambientLight.u_textureCubeAmbient;
            }
        }

        void FrankenUniformManager::setEnvironmentSphereTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->ambientLight.u_textureSphereAmbient >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = frankenShader->ambientLight.u_textureSphereAmbient;
            }
        }

        void FrankenUniformManager::setPBROptimizationTexture(const AppKit::OpenGL::GLTexture *tex)
        {
            if (frankenShader->PBR.u_TexturePBR_F_NDF_G_GHigh >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_PBROptimization] = tex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_PBROptimization] = frankenShader->PBR.u_TexturePBR_F_NDF_G_GHigh;
            }
        }

        void FrankenUniformManager::setSunLightConfig(int index, const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection)
        {
            if (index < 0 || index >= 4)
                return;

            if (frankenShader->sunLight[index].u_lightRadiance >= 0)
            {
                if (sunLight_Radiance[index] != radiance)
                {
                    sunLight_Radiance[index] = radiance;
                    frankenShader->setUniform(frankenShader->sunLight[index].u_lightRadiance, sunLight_Radiance[index]);
                }
            }
            if (frankenShader->sunLight[index].u_lightDir >= 0)
            {
                if (sunLight_WorldDirection[index] != worldDirection)
                {
                    sunLight_WorldDirection[index] = worldDirection;
                    frankenShader->setUniform(frankenShader->sunLight[index].u_lightDir, sunLight_WorldDirection[index]);
                }
            }
        }

        void FrankenUniformManager::setShadowSunLightConfig(int index,
                                                            const MathCore::vec3f &radiance, const MathCore::vec3f &worldDirection,
                                                            const MathCore::vec3f &dimension, const MathCore::vec3f &cone_cos_sin_tan,
                                                            const MathCore::mat4f &shadowProjMatrix,
                                                            const AppKit::OpenGL::GLTexture *shadowTex)
        {
            if (index < 0 || index >= 4)
                return;

            if (frankenShader->shadowSunLight[index].u_lightRadiance >= 0)
            {
                if (shadowSunLight_Radiance[index] != radiance)
                {
                    shadowSunLight_Radiance[index] = radiance;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_lightRadiance, shadowSunLight_Radiance[index]);
                }
            }
            if (frankenShader->shadowSunLight[index].u_lightDir >= 0)
            {
                if (shadowSunLight_WorldDirection[index] != worldDirection)
                {
                    shadowSunLight_WorldDirection[index] = worldDirection;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_lightDir, shadowSunLight_WorldDirection[index]);
                }
            }
            if (frankenShader->shadowSunLight[index].u_projMatrix >= 0)
            {
                if (shadowSunLight_ProjMatrix[index] != shadowProjMatrix)
                {
                    shadowSunLight_ProjMatrix[index] = shadowProjMatrix;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_projMatrix, shadowSunLight_ProjMatrix[index]);
                }
            }

            if (frankenShader->shadowSunLight[index].u_shadowDimension >= 0)
            {
                if (shadowSunLight_Dimension[index] != dimension)
                {
                    shadowSunLight_Dimension[index] = dimension;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_shadowDimension, shadowSunLight_Dimension[index]);
                }
            }

            if (frankenShader->shadowSunLight[index].u_shadowDimension_inv >= 0)
            {
                MathCore::vec3f dimension_inv = 1.0f / dimension;
                if (shadowSunLight_Dimension_inv[index] != dimension_inv)
                {
                    shadowSunLight_Dimension_inv[index] = dimension_inv;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_shadowDimension_inv, shadowSunLight_Dimension_inv[index]);
                }
            }

            if (frankenShader->shadowSunLight[index].u_shadowCone >= 0)
            {
                if (shadowSunLight_Cone[index] != cone_cos_sin_tan)
                {
                    shadowSunLight_Cone[index] = cone_cos_sin_tan;
                    frankenShader->setUniform(frankenShader->shadowSunLight[index].u_shadowCone, shadowSunLight_Cone[index]);
                }
            }

            if (frankenShader->shadowSunLight[index].u_textureDepth >= 0)
            {
                tex_unit[FrankenUniformTextureSlot_ShadowSunLight0 + index] = shadowTex;
                // tex_unit_uniform_location[FrankenUniformTextureSlot_PBROptimization] = frankenShader->PBR.u_TexturePBR_F_NDF_G_GHigh;
            }
        }

        void FrankenUniformManager::activateShader()
        {
            GLRenderState *state = GLRenderState::Instance();

            AppKit::OpenGL::GLShader *oldShader = state->CurrentShader;

            different_shader = oldShader != frankenShader;

            if (different_shader)
            {
                state->CurrentShader = frankenShader;
                if (strcmp(oldShader->class_name, "FrankenShader") == 0)
                    old_frankenShader = (FrankenShader *)oldShader;
                else
                    old_frankenShader = NULL;
            }
            else
                old_frankenShader = NULL;
        }

        void FrankenUniformManager::activeTexUnit()
        {
            int count = 0;
            int count_old = 0;
            for (int i = 0; i < FrankenUniformTextureSlot_Count; i++)
            {

                if ( // old_frankenShader->frankenUniformManager.tex_unit[i] == tex_unit[i] &&
                    last_tex_unit[i] == tex_unit[i])
                {
                    if (tex_unit[i] != NULL)
                    {
                        count++;
                        count_old++;
                    }
                    continue;
                }

                if (last_tex_unit[i] != NULL)
                {
                    last_tex_unit[i]->deactive(count_old);
                    count_old++;
                }

                last_tex_unit[i] = tex_unit[i];
                if (tex_unit[i] != NULL)
                {
                    tex_unit[i]->active(count);
                    // frankenShader->setUniform(tex_unit_uniform_location[i], count);
                    count++;
                }
            }
        }

        void FrankenUniformManager::deactiveTexUnit()
        {
            int count = 0;
            for (int i = 0; i < FrankenUniformTextureSlot_Count; i++)
            {
                if (last_tex_unit[i] != NULL)
                    last_tex_unit[i]->deactive(count++);
                last_tex_unit[i] = NULL;
            }
        }

        void FrankenUniformManager::readUniformsFromShaderAndInitStatic()
        {

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = frankenShader;

            if (frankenShader->u_mvp >= 0)
                mvp = frankenShader->getUniformMat4(frankenShader->u_mvp);

            if (frankenShader->normalMap.u_localToWorld >= 0)
                normal_LocalToWorld = frankenShader->getUniformMat4(frankenShader->normalMap.u_localToWorld);
            if (frankenShader->normalMap.u_localToWorld_it >= 0)
                normal_LocalToWorld_it = frankenShader->getUniformMat4(frankenShader->normalMap.u_localToWorld_it);

            if (frankenShader->u_cameraPosWorld >= 0)
                camera_PosWorld = frankenShader->getUniformVec3(frankenShader->u_cameraPosWorld);

            if (frankenShader->PBR.u_materialAlbedoColor >= 0)
                material_PBRSetup.albedoColor = frankenShader->getUniformVec3(frankenShader->PBR.u_materialAlbedoColor);
            if (frankenShader->PBR.u_materialEmissionColor >= 0)
                material_PBRSetup.emissionColor = frankenShader->getUniformVec3(frankenShader->PBR.u_materialEmissionColor);
            if (frankenShader->PBR.u_materialRoughness >= 0)
                material_PBRSetup.roughness = frankenShader->getUniformFloat(frankenShader->PBR.u_materialRoughness);
            if (frankenShader->PBR.u_materialMetallic >= 0)
                material_PBRSetup.metallic = frankenShader->getUniformFloat(frankenShader->PBR.u_materialMetallic);

            if (frankenShader->ambientLight.u_ambientColor >= 0)
                ambientColor = frankenShader->getUniformVec3(frankenShader->ambientLight.u_ambientColor);

            for (int i = 0; i < 4; i++)
            {

                // sunLight
                if (frankenShader->sunLight[i].u_lightRadiance >= 0)
                    sunLight_Radiance[i] = frankenShader->getUniformVec3(frankenShader->sunLight[i].u_lightRadiance);
                if (frankenShader->sunLight[i].u_lightDir >= 0)
                    sunLight_WorldDirection[i] = frankenShader->getUniformVec3(frankenShader->sunLight[i].u_lightDir);

                // shadow sunLight

                if (frankenShader->shadowSunLight[i].u_lightRadiance >= 0)
                    shadowSunLight_Radiance[i] = frankenShader->getUniformVec3(frankenShader->shadowSunLight[i].u_lightRadiance);
                if (frankenShader->shadowSunLight[i].u_lightDir >= 0)
                    shadowSunLight_WorldDirection[i] = frankenShader->getUniformVec3(frankenShader->shadowSunLight[i].u_lightDir);
                if (frankenShader->shadowSunLight[i].u_projMatrix >= 0)
                    shadowSunLight_ProjMatrix[i] = frankenShader->getUniformMat4(frankenShader->shadowSunLight[i].u_projMatrix);

                if (frankenShader->shadowSunLight[i].u_shadowDimension >= 0)
                    shadowSunLight_Dimension[i] = frankenShader->getUniformVec3(frankenShader->shadowSunLight[i].u_shadowDimension);
                if (frankenShader->shadowSunLight[i].u_shadowDimension_inv >= 0)
                    shadowSunLight_Dimension_inv[i] = frankenShader->getUniformVec3(frankenShader->shadowSunLight[i].u_shadowDimension_inv);
                if (frankenShader->shadowSunLight[i].u_shadowCone >= 0)
                    shadowSunLight_Cone[i] = frankenShader->getUniformVec3(frankenShader->shadowSunLight[i].u_shadowCone);
            }

            // upload static data
            if (frankenShader->u_shadow_spread_coefs >= 0)
            {
                const int SHADOW_SAMPLES = 32;
                float shadowSpreadCoefs[SHADOW_SAMPLES][2];
                std::vector<MathCore::vec3f> points = PatternPoints::sunFlower(SHADOW_SAMPLES - 1, 2, false);
                points.push_back(MathCore::vec3f(0, 0, 0));
                PatternPoints::optimize2DAccess(&points, 8, true);
                // insert in the first position...
                // points.insert(points.begin(), MathCore::vec3f(0,0,0));

                for (int i = 0; i < (int)points.size(); i++)
                {
                    shadowSpreadCoefs[i][0] = points[i].x;
                    shadowSpreadCoefs[i][1] = points[i].y;
                }
                OPENGL_CMD(glUniform2fv(frankenShader->u_shadow_spread_coefs, (int)points.size(), &shadowSpreadCoefs[0][0]));
            }

            GLint tex_unit_uniform_location[FrankenUniformTextureSlot_Count];

            for (int i = 0; i < FrankenUniformTextureSlot_Count; i++)
                tex_unit_uniform_location[i] = -1;

            tex_unit_uniform_location[FrankenUniformTextureSlot_Normal] = frankenShader->normalMap.u_textureNormal;
            tex_unit_uniform_location[FrankenUniformTextureSlot_Albedo] = frankenShader->PBR.u_textureAlbedo;
            tex_unit_uniform_location[FrankenUniformTextureSlot_Specular] = frankenShader->PBR.u_textureSpecular;
            tex_unit_uniform_location[FrankenUniformTextureSlot_Emission] = frankenShader->PBR.u_textureEmission;
            if (frankenShader->ambientLight.u_textureCubeAmbient >= 0)
                tex_unit_uniform_location[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = frankenShader->ambientLight.u_textureCubeAmbient;
            else if (frankenShader->ambientLight.u_textureSphereAmbient >= 0)
                tex_unit_uniform_location[FrankenUniformTextureSlot_EnvironmentCubeOrSphere] = frankenShader->ambientLight.u_textureSphereAmbient;
            tex_unit_uniform_location[FrankenUniformTextureSlot_PBROptimization] = frankenShader->PBR.u_TexturePBR_F_NDF_G_GHigh;
            for (int i = 0; i < 4; i++)
                tex_unit_uniform_location[FrankenUniformTextureSlot_ShadowSunLight0 + i] = frankenShader->shadowSunLight[i].u_textureDepth;

            int count = 0;
            for (int i = 0; i < FrankenUniformTextureSlot_Count; i++)
            {
                if (tex_unit_uniform_location[i] >= 0)
                    frankenShader->setUniform(tex_unit_uniform_location[i], count++);
            }
        }

    }

}