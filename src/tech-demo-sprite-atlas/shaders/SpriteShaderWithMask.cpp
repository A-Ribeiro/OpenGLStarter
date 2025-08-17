
#include "SpriteShaderWithMask.h"
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Components/Core/ComponentCamera.h>

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {
        SpriteShaderWithMask::SpriteShaderWithMask()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;\n"
                "attribute vec3 aUV0;\n"
                "attribute vec4 aColor0;\n"
                "uniform mat4 uMVP;\n"
                "varying vec2 uv;\n"
                "varying vec4 color;\n"
                "void main() {\n"
                "  uv = aUV0.xy;\n"
                "  color = aColor0;\n"
                "  gl_Position = uMVP * aPosition;\n"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120

                MASKSHADER_FRAGMENT_UNIFORM

                MASKSHADER_COMPUTE_MASK_FUNCTION

                "varying vec2 uv;\n"
                "varying vec4 color;\n"
                "uniform vec4 uColor;\n"
                "uniform sampler2D uTexture;\n"
                "void main() {\n"
                "  float mask = compute_mask();\n"
                "  if (mask <= 0.0)\n"
                "    discard;\n"
                "  vec4 texel = texture2D(uTexture, uv);\n"
                "  vec4 result = texel * color * uColor;\n"
                "  result.a *= mask;\n"
                "  if (result.a <= 0.0)\n"
                "    discard;\n"
                "  gl_FragColor = result;\n"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            // u_screenSizePx = getUniformLocation("uScreenSizePx");
            // u_screenSizePx_inv = getUniformLocation("uScreenSizePx_inv");
            u_mvp = getUniformLocation("uMVP");
            u_color = getUniformLocation("uColor");
            u_texture = getUniformLocation("uTexture");

            // initialize uniforms

            uMVP = MathCore::mat4f();
            uColor = MathCore::vec4f(1.0, 1.0, 1.0, 1.0);
            uTexture = 0;

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = this;

            setUniform(u_mvp, uMVP);
            setUniform(u_color, uColor);
            setUniform(u_texture, uTexture); // tex unit 0

            mask_query_uniform_locations_and_set_default_values();

            state->CurrentShader = nullptr;
        }

        void SpriteShaderWithMask::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }

        void SpriteShaderWithMask::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        void SpriteShaderWithMask::setTexture(int texunit)
        {
            if (uTexture != texunit)
            {
                uTexture = texunit;
                setUniform(u_texture, uTexture);
            }
        }

        Utils::ShaderPropertyBag SpriteShaderWithMask::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag = mask_default_bag();

            bag.addProperty("uColor", uColor);
            bag.addProperty("uTexture", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("UseDiscard", false);

            return bag;
        }

        void SpriteShaderWithMask::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;
            state->CurrentShader = this;
            if (materialBag.getProperty<bool>("UseDiscard"))
                state->BlendMode = AppKit::GLEngine::BlendModeDisabled;
            else
                state->BlendMode = AppKit::GLEngine::BlendModeAlpha;
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));

            setMaskFromPropertyBag(materialBag);

            auto tex = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("uTexture");
            if (tex == nullptr)
                tex = resourceMap->defaultAlbedoTexture;

            OpenGL::VirtualTexture* textureUnitActivation[] = {tex.get()};
            state->setTextureUnitActivationArray(textureUnitActivation, 1);

            setTexture(0);
        }
        void SpriteShaderWithMask::setUniformsFromMatrices(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {
            setMVP(*mvp);
        }
    }
}
