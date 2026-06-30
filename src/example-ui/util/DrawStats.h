#pragma once

#include <appkit-gl-engine/Renderer/Fade.h>

#include <appkit-gl-engine/types/iRect.h>

#include <InteractiveToolkit/common.h>

#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/GLVertexArrayObject.h>

#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace Window
    {
        class GLWindow;
    }
    namespace GLEngine
    {
        class RenderWindowRegion;
        class RenderPipeline;
    }
}

class DrawStats
{
    std::string text;
    MathCore::vec2f CameraScreenSize;

    // AppKit::GLEngine::iRect viewport;
    // MathCore::vec2i windowSize;

    MathCore::mat4f mvpMatrix;
    
    // std::shared_ptr<AppKit::OpenGL::GLShaderColor> shaderColor;

    AppKit::OpenGL::GLVertexBufferObject vbo_interleaved_pos_uv_color;
    // AppKit::OpenGL::GLVertexBufferObject vbo_uv;
    // AppKit::OpenGL::GLVertexBufferObject vbo_color;

    AppKit::OpenGL::GLVertexArrayObject vao;

    int vertex_count;

    void uploadBuffers(AppKit::GLEngine::ResourceMap::FontResource *fontResource);

    DrawStats();

public:
    
    DrawStats(const DrawStats &) = delete;
    DrawStats &operator=(const DrawStats &) = delete;

    ~DrawStats();

    void draw(const std::string &text, 
        AppKit::GLEngine::ResourceMap *resourceMap, 
        AppKit::GLEngine::RenderWindowRegion *screenRenderWindow,
        AppKit::GLEngine::RenderPipeline *renderPipeline);

    ITK_DECLARE_CREATE_SHARED(DrawStats)

};

