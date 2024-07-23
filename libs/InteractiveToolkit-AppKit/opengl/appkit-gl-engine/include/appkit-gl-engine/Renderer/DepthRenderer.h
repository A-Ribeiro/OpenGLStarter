#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/shaders/UnlitPassShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>

#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentCamera.h>

namespace AppKit
{
    namespace GLEngine
    {

        class DepthShader : public DefaultEngineShader
        {
            int u_mvp;

        public:
            DepthShader();

            void setMVP(const MathCore::mat4f &mvp);
        };

        class DepthRenderer: public EventCore::HandleCallback
        {

            // void OnWindowSizeChange(EventCore::Property<MathCore::vec2i> *prop);

            bool traverse_singlepass_render(std::shared_ptr<Transform> element, void *userData);

        public:
            // Debug info
            DepthShader depthShader;

            // method 1 -- copy the depth from current framebuffer to a depthTexture
            AppKit::OpenGL::GLTexture depthTexture;

            // method II - render just the depth to the depth texture
            AppKit::OpenGL::GLDynamicFBO fbo;
            AppKit::OpenGL::GLTexture colorTexture;

            // AppKit::OpenGL::GLTexture depthTexture;
            // AppKit::OpenGL::GLRenderBuffer realDepthBuffer24;

            DepthRenderer();

            ~DepthRenderer();

            // dont need another rendering pass...
            void method1_copy_from_current_framebuffer(RenderWindowRegion *renderWindowRegion = NULL);

            void method2_render_just_depth(Transform *root, Components::ComponentCamera *camera);
        };

    }

}
