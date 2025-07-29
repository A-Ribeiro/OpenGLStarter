#pragma once

#include <appkit-gl-engine/shaders/ShaderUnlitTexture.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        ShaderUnlitTexture::ShaderUnlitTexture()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0;

            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "attribute vec3 aUV0;"
                "uniform mat4 uMVP;"
                "varying vec2 uv;"
                "void main() {"
                "  uv = aUV0.xy;"
                "  gl_Position = uMVP * aPosition;"
                "}"};

            const char fragmentShaderCode[] = {
                "varying vec2 uv;"
                "uniform vec4 uColor;"
                "uniform sampler2D uTexture;"
                "void main() {"
                "  vec4 texel = texture2D(uTexture, uv);"
                "  vec4 result = texel * uColor;"
                "  gl_FragColor = result;"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
            u_texture = getUniformLocation("uTexture");
            u_color = getUniformLocation("uColor");

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

        void ShaderUnlitTexture::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }
        void ShaderUnlitTexture::setTexture(int texunit)
        {
            if (uTexture != texunit)
            {
                uTexture = texunit;
                setUniform(u_texture, uTexture);
            }
        }
        void ShaderUnlitTexture::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        Utils::ShaderPropertyBag ShaderUnlitTexture::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("uColor", uColor);
            bag.addProperty("uTexture", std::shared_ptr<OpenGL::VirtualTexture>(nullptr));
            bag.addProperty("BlendMode", (int)AppKit::GLEngine::BlendModeDisabled);

            return bag;
        }

        void ShaderUnlitTexture::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;
            state->CurrentShader = this;

            state->BlendMode = (AppKit::GLEngine::BlendModeType)materialBag.getProperty<int>("BlendMode");
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));

            auto tex = materialBag.getProperty<std::shared_ptr<OpenGL::VirtualTexture>>("uTexture");
            if (tex == nullptr)
                tex = resourceMap->defaultAlbedoTexture;

            OpenGL::VirtualTexture* textureUnitActivation[] = {tex.get()};
            state->setTextureUnitActivationArray(textureUnitActivation, 1);

            setTexture(0);

        }
        void ShaderUnlitTexture::setUniformsFromMatrices(
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