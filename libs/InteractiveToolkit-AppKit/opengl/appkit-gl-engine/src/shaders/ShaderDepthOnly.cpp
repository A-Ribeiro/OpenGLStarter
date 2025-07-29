#pragma once

#include <appkit-gl-engine/shaders/ShaderDepthOnly.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        ShaderDepthOnly::ShaderDepthOnly()
        {
            format = ITKExtension::Model::CONTAINS_POS;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;\n"
                "uniform mat4 uMVP;\n"
                "void main() {\n"
                "  gl_Position = uMVP * aPosition;\n"
                "}"
            };

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "void main() {\n"
                "  gl_FragColor = vec4(1.0);\n"
                "}"
            };

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");

            uMVP = MathCore::mat4f();

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = this;

            setUniform(u_mvp, uMVP);

            state->CurrentShader = nullptr;
        }

        void ShaderDepthOnly::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }

        void ShaderDepthOnly::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            state->CurrentShader = this;
            state->BlendMode = AppKit::GLEngine::BlendModeDisabled;

            state->clearTextureUnitActivationArray();
        }
        void ShaderDepthOnly::setUniformsFromMatrices(
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