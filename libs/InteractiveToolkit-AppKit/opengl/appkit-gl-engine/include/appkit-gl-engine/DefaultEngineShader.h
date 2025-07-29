#pragma once

#include <appkit-gl-base/GLShader.h>

// namespace ITKExtension { namespace Model { typedef uint32_t BitMask; } }
#include <InteractiveToolkit-Extension/model/Geometry.h>

// Forward declarations
namespace AppKit
{
    namespace GLEngine
    {
        class Transform;
        class GLRenderState;
        class RenderPipeline;
        class ResourceMap;
    }
}
namespace AppKit
{
    namespace GLEngine
    {
        namespace Utils
        {
            class ShaderPropertyBag;
        }
    }
}
namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            class ComponentCamera;
            class ComponentMaterial;
        }
    }
}

#include <appkit-gl-base/VirtualTexture.h>

namespace AppKit
{
    namespace GLEngine
    {

#if !defined(GLAD_GLES2)
#define SHADER_HEADER_120 "#version 120\n"
#else
#define SHADER_HEADER_120 ""
#endif

        class DefaultEngineShader : public AppKit::OpenGL::GLShader
        {
        public:
            ITKExtension::Model::BitMask format;

            DefaultEngineShader();

            virtual int queryAttribLocation(const char *aname);

            int getUserAttribLocationStart();

            void setupAttribLocation();

            // virtual void activateShaderAndSetPropertiesFromBag(
            //     Components::ComponentCamera *camera,
            //     const MathCore::mat4f *mvp,
            //     const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal,
            //     GLRenderState *state,
            //     const Utils::ShaderPropertyBag &bag);
            // virtual void deactivateShader(GLRenderState *state);
            virtual Utils::ShaderPropertyBag createDefaultBag() const;

            virtual void ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material);
            virtual void setUniformsFromMatrices(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv);
        };
    }

}
