#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
// #include <appkit-gl-engine/util/ReferenceCounter.h>
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
                std::shared_ptr<AppKit::OpenGL::GLTexture> tex;

                AppKit::GLEngine::BlendModeType blendMode;

                UnlitSetup()
                {
                    //tex = nullptr;
                    blendMode = AppKit::GLEngine::BlendModeDisabled;
                    color = MathCore::vec4f(1.0f);
                }

                void releaseTextureReferences()
                {
                    tex = nullptr;
                    // ReferenceCounter<AppKit::OpenGL::GLTexture *> *refCounter = &Engine::Instance()->textureReferenceCounter;
                    // if (tex)
                    //     refCounter->remove(tex);
                }
            };

            class PBRSetup
            {
            public:
                MathCore::vec3f albedoColor;
                MathCore::vec3f emissionColor;

                std::shared_ptr<AppKit::OpenGL::GLTexture> texAlbedo;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texNormal;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texSpecular;

                // AppKit::OpenGL::GLTexture *texCube;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texEmission;

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

                    //texAlbedo = nullptr;
                    //texNormal = nullptr;
                    //texSpecular = nullptr;

                    // texCube = nullptr;
                    //texEmission = nullptr;

                    // blendMode = AppKit::GLEngine::BlendModeDisabled;
                }

                void releaseTextureReferences()
                {
                    // ReferenceCounter<AppKit::OpenGL::GLTexture *> *refCounter = &Engine::Instance()->textureReferenceCounter;

                    // if (texAlbedo)
                    //     refCounter->remove(texAlbedo);
                    // if (texNormal)
                    //     refCounter->remove(texNormal);
                    // if (texSpecular)
                    //     refCounter->remove(texSpecular);
                    // if (texEmission)
                    //     refCounter->remove(texEmission);

                    // refCounter->remove(texCube);

                    texAlbedo = nullptr;
                    texNormal = nullptr;
                    texSpecular = nullptr;
                    texEmission = nullptr;
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

                bool always_clone;

                ComponentMaterial() : Component(ComponentMaterial::Type)
                {
                    type = MaterialNone;

                    skin_gradient_matrix = nullptr;
                    skin_shader_matrix_size_bitflag = 0;
                    skin_gradient_matrix_dirty = false;

                    always_clone = false;
                }

                ~ComponentMaterial()
                {
                    unlit.releaseTextureReferences();
                    pbr.releaseTextureReferences();
                }

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone) {
                    if (!always_clone && !force_clone)
                        return self();
                    auto result = Component::CreateShared<ComponentMaterial>();

                    result->type = this->type;

                    result->unlit = this->unlit;
                    result->pbr = this->pbr;

                    // // used for mesh skinning
                    // bool skin_gradient_matrix_dirty;
                    // std::vector<MathCore::mat4f> *skin_gradient_matrix;
                    // uint32_t skin_shader_matrix_size_bitflag; // skin hint to help to select shader

                    result->always_clone = this->always_clone;

                    return result;
                }
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap){

                }

            };
        }
    }

}