#ifndef __depth_renderer__h_
#define __depth_renderer__h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>

#include <mini-gl-engine/UnlitPassShader.h>
#include <mini-gl-engine/GLRenderState.h>

#include <mini-gl-engine/ComponentMesh.h>

namespace GLEngine {

    class DepthShader : public DefaultEngineShader{
        int u_mvp;
    public:
        DepthShader();
        
        void setMVP(const aRibeiro::mat4 &mvp);

    };

    class _SSE2_ALIGN_PRE DepthRenderer {

        void OnWindowSizeChange(aRibeiro::Property<sf::Vector2i> *prop);

        bool traverse_singlepass_render(Transform *element, void* userData);

    public:

        // Debug info
        DepthShader depthShader;

        //method 1 -- copy the depth from current framebuffer to a depthTexture
        openglWrapper::GLTexture depthTexture;

        //method II - render just the depth to the depth texture
        openglWrapper::GLDynamicFBO fbo;
        openglWrapper::GLTexture colorTexture;

        //openglWrapper::GLTexture depthTexture;
        //openglWrapper::GLRenderBuffer realDepthBuffer24;
        
        DepthRenderer();

        virtual ~DepthRenderer();

        // dont need another rendering pass...
        void method1_copy_from_current_framebuffer();

        void method2_render_just_depth(Transform*root, Components::ComponentCamera *camera);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
