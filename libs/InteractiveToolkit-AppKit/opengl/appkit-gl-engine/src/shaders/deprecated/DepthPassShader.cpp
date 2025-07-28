#include <appkit-gl-engine/DefaultEngineShader.h>

#include <appkit-gl-engine/shaders/deprecated/DepthPassShader.h>

namespace AppKit
{
    namespace GLEngine
    {
        DepthPassShader::DepthPassShader()
        {
            format = ITKExtension::Model::CONTAINS_POS;

            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 aPosition;"
                "uniform mat4 uMVP;"
                "void main() {"
                "  gl_Position = uMVP * aPosition;"
                "}"
            };

            const char fragmentShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "void main() {"
                //"  gl_FragColor = vec4(vec3( gl_FragCoord.z ),1.0);"
                "  gl_FragColor = vec4(1.0);"
                "}"
            };

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
        }

        void DepthPassShader::setMVP(const MathCore::mat4f &mvp)
        {
            setUniform(u_mvp, mvp);
        }
    }
}
