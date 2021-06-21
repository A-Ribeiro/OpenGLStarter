#ifndef _ComponentMaterial_h_
#define _ComponentMaterial_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentColorMesh.h>
#include <mini-gl-engine/GLRenderState.h>
#include <mini-gl-engine/ReferenceCounter.h>
#include <data-model/data-model.h>

//#include <mini-gl-engine/DefaultEngineShader.h>

namespace GLEngine {

    namespace Components {

        enum MaterialType {
            MaterialNone,

            MaterialUnlit,//implemented
            MaterialUnlitTexture,//implemented
            MaterialUnlitTextureVertexColor,//implemented
            MaterialUnlitTextureVertexColorFont,//implemented

            MaterialPBR
        };

        class _SSE2_ALIGN_PRE UnlitSetup {
        public:
            aRibeiro::vec4 color;
            openglWrapper::GLTexture *tex;

            GLEngine::BlendModeType blendMode;

            UnlitSetup() {
                tex = NULL;
                blendMode = GLEngine::BlendModeDisabled;
                color = aRibeiro::vec4(1.0f);
            }

            virtual ~UnlitSetup() {
                ReferenceCounter<openglWrapper::GLTexture*> *refCounter = &Engine::Instance()->textureReferenceCounter;
                refCounter->remove(tex);
            }

            SSE2_CLASS_NEW_OPERATOR
        } _SSE2_ALIGN_POS;

        class _SSE2_ALIGN_PRE PBRSetup {
        public:
            aRibeiro::vec4 albedoColor;

            openglWrapper::GLTexture *texAlbedo;
            openglWrapper::GLTexture *texNormal;
            openglWrapper::GLTexture *texSpecular;
            
            //openglWrapper::GLTexture *texCube;
            //openglWrapper::GLTexture *texEmission;

            float roughness;
            float metallic;
            //float emission;

            //GLEngine::BlendModeType blendMode;

            PBRSetup() {
                //emission = 0;
                
                albedoColor = aRibeiro::vec4(1.0f);
                roughness = 1.0f;
                metallic = 0.0f;

                texAlbedo = NULL;
                texNormal = NULL;
                texSpecular = NULL;
                
                //texCube = NULL;
                //texEmission = NULL;

                //blendMode = GLEngine::BlendModeDisabled;
            }

            virtual ~PBRSetup() {
                ReferenceCounter<openglWrapper::GLTexture*> *refCounter = &Engine::Instance()->textureReferenceCounter;

                refCounter->remove(texAlbedo);
                refCounter->remove(texNormal);
                refCounter->remove(texSpecular);
                //refCounter->remove(texCube);
            }

            SSE2_CLASS_NEW_OPERATOR
        } _SSE2_ALIGN_POS;

        class ComponentMaterial : public Component {
        public:

            static const ComponentType Type;

            //material type
            MaterialType type;

            //parameters
            UnlitSetup unlit;
            PBRSetup pbr;

            // used for mesh skinning
            bool skin_gradient_matrix_dirty;
            aRibeiro::aligned_vector<aRibeiro::mat4> *skin_gradient_matrix;
            uint32_t skin_shader_matrix_size_bitflag;//skin hint to help to select shader

            ComponentMaterial() :Component(ComponentMaterial::Type) {
                type = MaterialNone;

                skin_gradient_matrix = NULL;
                skin_shader_matrix_size_bitflag = 0;
                skin_gradient_matrix_dirty = false;
            }

        };
    }
}

#endif
