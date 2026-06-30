#pragma once

#include <appkit-gl-engine/Renderer/Fade.h>

#include <appkit-gl-engine/types/iRect.h>

#include <InteractiveToolkit/common.h>

#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/GLVertexArrayObject.h>

namespace AppKit
{
    namespace Window
    {
        class GLWindow;
    }
}

class DrawBlackBars
{

    AppKit::GLEngine::iRect viewport;
    MathCore::vec2i windowSize;

    MathCore::mat4f projectionMatrix;
    
    // std::shared_ptr<AppKit::OpenGL::GLShaderColor> shaderColor;

    AppKit::OpenGL::GLVertexBufferObject vbo_pos;
    AppKit::OpenGL::GLVertexBufferObject vbo_index;
    AppKit::OpenGL::GLVertexArrayObject vao;

    void uploadBuffers();

    DrawBlackBars();

public:
    
    DrawBlackBars(const DrawBlackBars &) = delete;
    DrawBlackBars &operator=(const DrawBlackBars &) = delete;

    ~DrawBlackBars();

    void draw(AppKit::Window::GLWindow *window, AppKit::OpenGL::GLShaderColor *shaderColor);

    ITK_DECLARE_CREATE_SHARED(DrawBlackBars)

};

