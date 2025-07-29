#pragma once

#include <appkit-gl-engine/shaders/ShaderUnlitVertexColor.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        ShaderUnlitVertexColor::ShaderUnlitVertexColor()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                "attribute vec4 aPosition;"
                "attribute vec4 aColor0;"
                "uniform mat4 uMVP;"
                "varying vec4 color;"
                "void main() {"
                "  color = aColor0;"
                "  gl_Position = uMVP * aPosition;"
                "}"};

            const char fragmentShaderCode[] = {
                "varying vec4 color;"
                "uniform vec4 uColor;"
                "void main() {"
                "  vec4 result = color * uColor;"
                "  gl_FragColor = result;"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
            u_color = getUniformLocation("uColor");

            uMVP = MathCore::mat4f();
            uColor = MathCore::vec4f(1.0, 1.0, 1.0, 1.0);

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = this;

            setUniform(u_mvp, uMVP);
            setUniform(u_color, uColor);

            state->CurrentShader = nullptr;
        }

        void ShaderUnlitVertexColor::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }
        void ShaderUnlitVertexColor::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        Utils::ShaderPropertyBag ShaderUnlitVertexColor::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("uColor", uColor);
            bag.addProperty("BlendMode", (int)AppKit::GLEngine::BlendModeDisabled);

            return bag;
        }

         void ShaderUnlitVertexColor::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;
            state->CurrentShader = this;

            state->BlendMode = (AppKit::GLEngine::BlendModeType)materialBag.getProperty<int>("BlendMode");
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));

            state->clearTextureUnitActivationArray();
        }
        void ShaderUnlitVertexColor::setUniformsFromMatrices(
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
