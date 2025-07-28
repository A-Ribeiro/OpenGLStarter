
#include "SpriteShader.h"
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Components/ComponentCamera.h>

#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        SpriteShader::SpriteShader(ResourceMap *resourceMap)
        {
            this->resourceMap = resourceMap;
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;"
                "attribute vec3 aUV0;"
                "attribute vec4 aColor0;"
                "uniform mat4 uMVP;"
                "varying vec2 uv;"
                "varying vec4 color;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  color = aColor0;"
                "  gl_Position = uMVP * aPosition;"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "varying vec2 uv;"
                "varying vec4 color;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  vec4 result = texel * color * uColor;"
                "  if (result.a <= 0.0) {"
                "    discard;"
                "  }"
                "  gl_FragColor = result;"
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

            state->CurrentShader = nullptr;
        }

        void SpriteShader::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }

        void SpriteShader::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        void SpriteShader::setTexture(int texunit)
        {
            if (uTexture != texunit)
            {
                uTexture = texunit;
                setUniform(u_texture, uTexture);
            }
        }

        // void SpriteShader::activateShaderAndSetPropertiesFromBag(
        //     Components::ComponentCamera *camera,
        //     const MathCore::mat4f *mvp,
        //     const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal,
        //     GLRenderState *state,
        //     const Utils::ShaderPropertyBag &bag)
        // {
        //     state->CurrentShader = this;

        //     if (bag.getProperty<bool>("UseDiscard"))
        //         state->BlendMode = AppKit::GLEngine::BlendModeDisabled;
        //     else
        //         state->BlendMode = AppKit::GLEngine::BlendModeAlpha;

        //     setMVP(*mvp);
        //     setColor(bag.getProperty<MathCore::vec4f>("uColor"));

        //     texture_activated = bag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("uTexture");

        //     if (texture_activated == nullptr)
        //     {
        //         bool srgb = GLEngine::Engine::Instance()->sRGBCapable;
        //         // texture_activated = this->resourceMap->getTexture("DEFAULT_ALBEDO",srgb);
        //         texture_activated = this->resourceMap->defaultAlbedoTexture;
        //     }

        //     texture_activated->active(0);
        //     setTexture(0);
        // }

        // void SpriteShader::deactivateShader(GLRenderState *state)
        // {
        //     texture_activated->deactive(0);
        //     texture_activated = nullptr;
        // }

        Utils::ShaderPropertyBag SpriteShader::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("uColor", uColor);
            bag.addProperty("uTexture", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("UseDiscard", false);

            return bag;
        }

        void SpriteShader::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
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


            auto tex = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("uTexture");
            if (tex == nullptr)
                tex = this->resourceMap->defaultAlbedoTexture;

            OpenGL::VirtualTexture* textureUnitActivation[] = {tex.get()};
            state->setTextureUnitActivationArray(textureUnitActivation, 1);

            setTexture(0);
        }
        void SpriteShader::setUniformsFromMatrices(
            GLRenderState *state,
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
