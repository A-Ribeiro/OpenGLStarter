#pragma once

#include <appkit-gl-engine/shaders/ShaderDepthOnly.h>
#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {

        ShaderDepthOnly::ShaderDepthOnly()
        {
            format = ITKExtension::Model::CONTAINS_POS;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;"
                "uniform mat4 uMVP;"
                "void main() {"
                "  gl_Position = uMVP * aPosition;"
                "}"
            };

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "void main() {"
                "  gl_FragColor = vec4(1.0);"
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

    }
}