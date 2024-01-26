#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ReferenceCounter.h>
//#include <aRibeiroData/aRibeiroData.h>

// #include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            enum MaterialType
            {
                MaterialNone,

                MaterialUnlit,                       // implemented
                MaterialUnlitTexture,                // implemented
                MaterialUnlitTextureVertexColor,     // implemented
                MaterialUnlitTextureVertexColorFont, // implemented

                MaterialPBR
            };

            class UnlitSetup
            {
            public:
                MathCore::vec4f color;
                AppKit::OpenGL::GLTexture *tex;

                AppKit::GLEngine::BlendModeType blendMode;

                UnlitSetup()
                {
                    tex = NULL;
                    blendMode = AppKit::GLEngine::BlendModeDisabled;
                    color = MathCore::vec4f(1.0f);
                }

                void releaseTextureReferences()
                {
                    ReferenceCounter<AppKit::OpenGL::GLTexture *> *refCounter = &Engine::Instance()->textureReferenceCounter;
                    if (tex)
                        refCounter->remove(tex);
                }
            };

            class PBRSetup
            {
            public:
                MathCore::vec3f albedoColor;
                MathCore::vec3f emissionColor;

                AppKit::OpenGL::GLTexture *texAlbedo;
                AppKit::OpenGL::GLTexture *texNormal;
                AppKit::OpenGL::GLTexture *texSpecular;

                // AppKit::OpenGL::GLTexture *texCube;
                AppKit::OpenGL::GLTexture *texEmission;

                float roughness;
                float metallic;
                // float emission;

                // AppKit::GLEngine::BlendModeType blendMode;

                PBRSetup()
                {
                    // emission = 0;

                    albedoColor = MathCore::vec3f(1.0f);
                    emissionColor = MathCore::vec3f(0.0f); // [0.0f .. 8.0f]
                    roughness = 1.0f;
                    metallic = 0.0f;

                    texAlbedo = NULL;
                    texNormal = NULL;
                    texSpecular = NULL;

                    // texCube = NULL;
                    texEmission = NULL;

                    // blendMode = AppKit::GLEngine::BlendModeDisabled;
                }

                void releaseTextureReferences()
                {
                    ReferenceCounter<AppKit::OpenGL::GLTexture *> *refCounter = &Engine::Instance()->textureReferenceCounter;

                    if (texAlbedo)
                        refCounter->remove(texAlbedo);
                    if (texNormal)
                        refCounter->remove(texNormal);
                    if (texSpecular)
                        refCounter->remove(texSpecular);
                    if (texEmission)
                        refCounter->remove(texEmission);

                    // refCounter->remove(texCube);
                }
            };

            class ComponentMaterial : public Component
            {
            public:
                static const ComponentType Type;

                // material type
                MaterialType type;

                // parameters
                UnlitSetup unlit;
                PBRSetup pbr;

                // used for mesh skinning
                bool skin_gradient_matrix_dirty;
                std::vector<MathCore::mat4f> *skin_gradient_matrix;
                uint32_t skin_shader_matrix_size_bitflag; // skin hint to help to select shader

                ComponentMaterial() : Component(ComponentMaterial::Type)
                {
                    type = MaterialNone;

                    skin_gradient_matrix = NULL;
                    skin_shader_matrix_size_bitflag = 0;
                    skin_gradient_matrix_dirty = false;
                }

                ~ComponentMaterial()
                {
                    unlit.releaseTextureReferences();
                    pbr.releaseTextureReferences();
                }
            };
        }
    }

}