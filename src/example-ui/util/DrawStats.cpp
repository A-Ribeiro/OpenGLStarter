#include "DrawStats.h"

#include <appkit-gl-engine/GL/GLRenderState.h>
#include <AppKit/window/Window.h>
#include <appkit-gl-engine/ResourceMap.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>

#include <appkit-ui/common.h>

using namespace AppKit::GLEngine;

void DrawStats::uploadBuffers(AppKit::GLEngine::ResourceMap::FontResource *fontResource)
{
    mvpMatrix =
        MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
            0.0f,                           // Left
            (float)CameraScreenSize.width,  // Right
            0.0f,                           // Bottom
            (float)CameraScreenSize.height, // Top
            -1.0f,                          // ZNear
            1.0f                            // ZFar
            ) *
        MathCore::GEN<MathCore::mat4f>::translateHomogeneous(
            16.0f,
            16.0f);

    auto builder = fontResource->fontBuilder.get();

    builder->size = 64.0f;
    builder->faceColor = AppKit::ui::colorFromHex("#FFFF00FF");
    builder->strokeColor = AppKit::ui::colorFromHex("#000000ff");
    builder->strokeOffset = MathCore::vec3f(0, 0, 0.001f);
    builder->horizontalAlign = AppKit::OpenGL::GLFont2HorizontalAlign_left;
    builder->verticalAlign = AppKit::OpenGL::GLFont2VerticalAlign_bottom;
    builder->lineHeight = 1.0f;
    builder->wrapMode = AppKit::OpenGL::GLFont2WrapMode_NoWrap;
    builder->firstLineHeightMode = AppKit::OpenGL::GLFont2FirstLineHeightMode_UseCharacterMaxHeight;
    builder->wordSeparatorChar = U' ';
    builder->drawFace = true;
    builder->drawStroke = true;

    auto engine = AppKit::GLEngine::Engine::Instance();
    builder->richBuild(
        text.c_str(),
        engine->sRGBCapable,
        -1,
        fontResource->polygonFontCache);

    vertex_count = (int)builder->vertexAttrib.size();

    if (vertex_count == 0)
        return;

    int aPos = 0;   // shader->queryAttribLocation("aPosition");
    int aUV = 1;    // shader->queryAttribLocation("aUV0");
    int aColor = 2; // shader->queryAttribLocation("aColor0");

    vbo_interleaved_pos_uv_color.uploadData((void *)builder->vertexAttrib[0].pos.array, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib) * vertex_count, true);

    // OPENGL_CMD(glEnableVertexAttribArray(aPos));
    // OPENGL_CMD(glVertexAttribPointer(aPos, 3, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].pos.array));
    // OPENGL_CMD(glEnableVertexAttribArray(aUV));
    // OPENGL_CMD(glVertexAttribPointer(aUV, 2, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].uv.array));
    // OPENGL_CMD(glEnableVertexAttribArray(aColor));
    // OPENGL_CMD(glVertexAttribPointer(aColor, 4, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].color.array));

    // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)builder->vertexAttrib.size()));
    // // OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

    // OPENGL_CMD(glDisableVertexAttribArray(aPos));
    // OPENGL_CMD(glDisableVertexAttribArray(aUV));
    // OPENGL_CMD(glDisableVertexAttribArray(aColor));

    // projectionMatrix = MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
    //         0.0f,                // Left
    //         (float)windowSize.x, // Right
    //         0.0f,                // Bottom
    //         (float)windowSize.y, // Top
    //         -1.0f,               // ZNear
    //         1.0f                 // ZFar
    //         );

    // // glDisable(GL_SCISSOR_TEST);

    // MathCore::vec3f vertex[8] = {
    //     MathCore::vec3f(0, 0, 0),
    //     MathCore::vec3f(windowSize.x, 0, 0),
    //     MathCore::vec3f(windowSize.x, windowSize.y, 0),
    //     MathCore::vec3f(0, windowSize.y, 0),

    //     MathCore::vec3f(viewport.x, viewport.y, 0),
    //     MathCore::vec3f(viewport.x + viewport.w, viewport.y, 0),
    //     MathCore::vec3f(viewport.x + viewport.w, viewport.y + viewport.h, 0),
    //     MathCore::vec3f(viewport.x, viewport.y + viewport.h, 0)};

    // /*
    // 3     2
    //   7 6
    //   4 5
    // 0     1
    // */
    // uint32_t indices[24] = {
    //     0, 1, 5,
    //     0, 5, 4,

    //     1, 2, 6,
    //     1, 6, 5,

    //     2, 3, 7,
    //     2, 7, 6,

    //     3, 0, 4,
    //     3, 4, 7};

    // vbo_pos.uploadData((void *)&vertex[0], sizeof(MathCore::vec3f) * 8, false);
    // vbo_index.uploadIndex((void *)&indices[0], sizeof(uint32_t) * 24, false);

    // // if (setup_vao)
    // // {
    // //     vao.enable();
    // //     vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    // //     vbo_index.setIndex();
    // //     vao.disable();
    // //     vbo_pos.unsetLayout(0);
    // //     vbo_index.unsetIndex();
    // // }

    // // OPENGL_CMD(glEnableVertexAttribArray(shaderColor.vPosition));
    // // OPENGL_CMD(glVertexAttribPointer(shaderColor.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));
    // // // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 3));
    // // OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

    // // OPENGL_CMD(glDisableVertexAttribArray(shaderColor.vPosition));

    // // // setup VAO
    // // vao.enable();
    // // vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    // // vbo_index.setIndex();
    // // vao.disable();
    // // vbo_pos.unsetLayout(0);
    // // vbo_index.unsetIndex();
}

DrawStats::DrawStats()
{
    // setup VAO
    int aPos = 0;   // shader->queryAttribLocation("aPosition");
    int aUV = 1;    // shader->queryAttribLocation("aUV0");
    int aColor = 2; // shader->queryAttribLocation("aColor0");

    vao.enable();
    vbo_interleaved_pos_uv_color.setLayout(aPos, 3, GL_FLOAT, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), 0);
    vbo_interleaved_pos_uv_color.setLayout(aUV, 2, GL_FLOAT, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), offsetof(AppKit::OpenGL::GLFont2Builder_VertexAttrib, uv));
    vbo_interleaved_pos_uv_color.setLayout(aColor, 4, GL_FLOAT, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), offsetof(AppKit::OpenGL::GLFont2Builder_VertexAttrib, color));
    vao.disable();

    vbo_interleaved_pos_uv_color.unsetLayout(aPos);
    vbo_interleaved_pos_uv_color.unsetLayout(aUV);
    vbo_interleaved_pos_uv_color.unsetLayout(aColor);

    vertex_count = 0;
}

DrawStats::~DrawStats()
{
}

void DrawStats::draw(const std::string &text,
                     AppKit::GLEngine::ResourceMap *resourceMap,
                     AppKit::GLEngine::RenderWindowRegion *screenRenderWindow,
                     AppKit::GLEngine::RenderPipeline *renderPipeline)
{
    auto renderState = GLRenderState::Instance();
    // AppKit::GLEngine::iRect viewport = renderState->Viewport;
    // MathCore::vec2f CameraScreenSize = screenRenderWindow->CameraScreenSize;

    bool needs_reconstruction = false;
    if (this->text != text)
    {
        this->text = text;
        needs_reconstruction = true;
    }
    if (this->CameraScreenSize != screenRenderWindow->CameraScreenSize)
    {
        this->CameraScreenSize = screenRenderWindow->CameraScreenSize;
        needs_reconstruction = true;
    }

    auto engine = AppKit::GLEngine::Engine::Instance();
    std::shared_ptr<AppKit::GLEngine::ResourceMap::FontResource> fontResource =
        resourceMap->getTextureFont("resources/Roboto-Regular-100.basof2", engine->sRGBCapable);

    if (!fontResource->material->shader->compareType(AppKit::GLEngine::ShaderUnlitTextureVertexColorAlpha::Type))
        return;

    if (needs_reconstruction)
        uploadBuffers(fontResource.get());

    if (vertex_count == 0)
        return;

    auto shader = (AppKit::GLEngine::ShaderUnlitTextureVertexColorAlpha *)fontResource->material->shader.get();

    shader->ActiveShader_And_SetUniformsFromMaterial(
        renderState, resourceMap,
        renderPipeline,
        fontResource->material.get());

    shader->setMVP(mvpMatrix);

    AppKit::GLEngine::DepthTestType oldDepthTest = renderState->DepthTest;
    bool oldDepthTestEnabled = renderState->DepthWrite;

    renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;
    renderState->DepthWrite = false;

    // int aPos = 0;   // shader->queryAttribLocation("aPosition");
    // int aUV = 1;    // shader->queryAttribLocation("aUV0");
    // int aColor = 2; // shader->queryAttribLocation("aColor0");

    // OPENGL_CMD(glEnableVertexAttribArray(aPos));
    // OPENGL_CMD(glVertexAttribPointer(aPos, 3, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].pos.array));
    // OPENGL_CMD(glEnableVertexAttribArray(aUV));
    // OPENGL_CMD(glVertexAttribPointer(aUV, 2, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].uv.array));
    // OPENGL_CMD(glEnableVertexAttribArray(aColor));
    // OPENGL_CMD(glVertexAttribPointer(aColor, 4, GL_FLOAT, false, sizeof(AppKit::OpenGL::GLFont2Builder_VertexAttrib), builder->vertexAttrib[0].color.array));

    // OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)builder->vertexAttrib.size()));
    // // OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)(sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, &indices[0]));

    // OPENGL_CMD(glDisableVertexAttribArray(aPos));
    // OPENGL_CMD(glDisableVertexAttribArray(aUV));
    // OPENGL_CMD(glDisableVertexAttribArray(aColor));

    // draw vertex_array
    vao.enable();
    vao.drawArrays(GL_TRIANGLES, (GLsizei)vertex_count);
    vao.disable();

    //     // vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
    //     // vbo_index.setIndex();
    //     // vbo_index.drawIndex(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, 0);
    //     // vbo_pos.unsetLayout(0);
    //     // vbo_index.unsetIndex();

    renderState->DepthTest = oldDepthTest;
    renderState->DepthWrite = oldDepthTestEnabled;

    renderState->clearTextureUnitActivationArray();
    renderState->CurrentShader = nullptr;

    // for (size_t i = 0; i < builder->vertexAttrib.size(); i++)
    // {
    //     mesh->pos.push_back(builder->vertexAttrib[i].pos);
    //     mesh->uv[0].push_back(MathCore::vec3f(builder->vertexAttrib[i].uv, 0.0f));
    //     mesh->color[0].push_back(builder->vertexAttrib[i].color);

    //     // keep CCW orientation
    //     if ((i % 3) == 0)
    //     {
    //         mesh->indices.push_back((uint16_t)i);
    //         mesh->indices.push_back((uint16_t)(i + 1));
    //         mesh->indices.push_back((uint16_t)(i + 2));
    //     }
    // }
}

// {
//     GLRenderState *renderState = GLRenderState::Instance();
//     AppKit::GLEngine::iRect viewport = renderState->Viewport;
//     MathCore::vec2i windowSize = window->getSize();

//     AppKit::GLEngine::DepthTestType oldDepthTest = renderState->DepthTest;
//     bool oldDepthTestEnabled = renderState->DepthWrite;
//     AppKit::GLEngine::BlendModeType oldBlendMode = renderState->BlendMode;

//     bool needs_upload_vbo = false;

//     if (windowSize != this->windowSize)
//     {
//         needs_upload_vbo = true;
//         this->windowSize = windowSize;
//     }
//     if (viewport != this->viewport)
//     {
//         needs_upload_vbo = true;
//         this->viewport = viewport;
//     }

//     if (needs_upload_vbo)
//         uploadBuffers();

//     renderState->Viewport = AppKit::GLEngine::iRect(windowSize.x, windowSize.y);

//     renderState->CurrentShader = shaderColor;
//     renderState->DepthTest = AppKit::GLEngine::DepthTestDisabled;
//     renderState->DepthWrite = false;
//     renderState->BlendMode = AppKit::GLEngine::BlendModeDisabled;

//     shaderColor->setMatrix(projectionMatrix);
//     shaderColor->setColor(MathCore::vec4f(1, 0, 1, 1));

//     // draw vertex_array
//     vao.enable();
//     vao.drawIndex(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT);
//     vao.disable();

//     // vbo_pos.setLayout(0, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
//     // vbo_index.setIndex();
//     // vbo_index.drawIndex(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, 0);
//     // vbo_pos.unsetLayout(0);
//     // vbo_index.unsetIndex();

//     renderState->DepthTest = oldDepthTest;
//     renderState->DepthWrite = oldDepthTestEnabled;
//     renderState->BlendMode = oldBlendMode;
//     renderState->Viewport = viewport;
// }
