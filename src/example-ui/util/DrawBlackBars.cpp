#include "DrawBlackBars.h"

#include <appkit-gl-engine/GL/GLRenderState.h>
#include <AppKit/window/Window.h>

using namespace AppKit::GLEngine;

void DrawBlackBars::uploadBuffers()
{
    projectionMatrix = MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
            0.0f,                // Left
            (float)windowSize.x, // Right
            0.0f,                // Bottom
            (float)windowSize.y, // Top
            -1.0f,               // ZNear
            1.0f                 // ZFar
            );

    // glDisable(GL_SCISSOR_TEST);

    MathCore::vec3f vertex[8] = {
        MathCore::vec3f(0, 0, 0),
        MathCore::vec3f(windowSize.x, 0, 0),
        MathCore::vec3f(windowSize.x, windowSize.y, 0),
        MathCore::vec3f(0, windowSize.y, 0),

        MathCore::vec3f(viewport.x, viewport.y, 0),
        MathCore::vec3f(viewport.x + viewport.w, viewport.y, 0),
        MathCore::vec3f(viewport.x + viewport.w, viewport.y + viewport.h, 0),
        MathCore::vec3f(viewport.x, viewport.y + viewport.h, 0)};

    /*
    3     2
      7 6
      4 5
    0     1
    */
    uint32_t indices[24] = {
        0, 1, 5,
        0, 5, 4,

        1, 2, 6,
        1, 6, 5,

        2, 3, 7,
        2, 7, 6,

        3, 0, 4,
        3, 4, 7};

    vbo_pos.uploadData((void *)&vertex[0], sizeof(MathCore::vec3f) * 8, false);
    vbo_index.uploadIndex((void *)&indices[0], sizeof(uint32_t) * 24, false);

    // if (setup_vao)
    // {
    //     vao.enable();
    //     vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    //     vbo_index.setIndex();
    //     vao.disable();
    //     vbo_pos.unsetLayout(0);
    //     vbo_index.unsetIndex();
    // }

    // OPENGL_CMD(glEnableVertexAttribArray(shaderColor.vPosition));
    // OPENGL_CMD(glVertexAttribPointer(shaderColor.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));
    // // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 3));
    // OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

    // OPENGL_CMD(glDisableVertexAttribArray(shaderColor.vPosition));

    // // setup VAO
    // vao.enable();
    // vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    // vbo_index.setIndex();
    // vao.disable();
    // vbo_pos.unsetLayout(0);
    // vbo_index.unsetIndex();
}

DrawBlackBars::DrawBlackBars()
{

    // setup VAO
    vao.enable();
    vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    vbo_index.setIndex();
    vao.disable();
    vbo_pos.unsetLayout(0);
    vbo_index.unsetIndex();
}

DrawBlackBars::~DrawBlackBars()
{
}

void DrawBlackBars::draw(AppKit::Window::GLWindow *window, AppKit::OpenGL::GLShaderColor *shaderColor)
{
    GLRenderState *renderState = GLRenderState::Instance();
    AppKit::GLEngine::iRect viewport = renderState->Viewport;
    MathCore::vec2i windowSize = window->getSize();

    AppKit::GLEngine::DepthTestType oldDepthTest = renderState->DepthTest;
    bool oldDepthTestEnabled = renderState->DepthWrite;
    AppKit::GLEngine::BlendModeType oldBlendMode = renderState->BlendMode;

    bool needs_upload_vbo = false;

    if (windowSize != this->windowSize)
    {
        needs_upload_vbo = true;
        this->windowSize = windowSize;
    }
    if (viewport != this->viewport)
    {
        needs_upload_vbo = true;
        this->viewport = viewport;
    }

    if (needs_upload_vbo)
        uploadBuffers();

    renderState->Viewport = AppKit::GLEngine::iRect(windowSize.x, windowSize.y);

    renderState->CurrentShader = shaderColor;
    renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;
    renderState->DepthWrite = false;
    renderState->BlendMode = AppKit::GLEngine::BlendModeDisabled;

    shaderColor->setMatrix(projectionMatrix);
    shaderColor->setColor(MathCore::vec4f(0, 0, 0, 1));

    // draw vertex_array
    vao.enable();
    vao.drawIndex(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT);
    vao.disable();

    // vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    // vbo_index.setIndex();
    // vbo_index.drawIndex(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, 0);
    // vbo_pos.unsetLayout(0);
    // vbo_index.unsetIndex();

    renderState->DepthTest = oldDepthTest;
    renderState->DepthWrite = oldDepthTestEnabled;
    renderState->BlendMode = oldBlendMode;
    renderState->Viewport = viewport;

    renderState->CurrentShader = nullptr;
}
