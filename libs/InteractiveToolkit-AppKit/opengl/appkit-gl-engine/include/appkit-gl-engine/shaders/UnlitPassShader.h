#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

        class UnlitPassShader : public DefaultEngineShader
        {
            int u_mvp;
            int u_color;

        public:
            UnlitPassShader()
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
                    "uniform vec4 uColor;"
                    "void main() {"
                    "  gl_FragColor = uColor;"
                    "}"
                };

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                DefaultEngineShader::setupAttribLocation();
                link(__FILE__, __LINE__);

                u_mvp = getUniformLocation("uMVP");
                u_color = getUniformLocation("uColor");
            }

            void setMVP(const MathCore::mat4f &mvp)
            {
                setUniform(u_mvp, mvp);
            }

            void setColor(const MathCore::vec4f &color)
            {
                setUniform(u_color, color);
            }
        };

        class Unlit_tex_PassShader : public DefaultEngineShader
        {
            int u_mvp;
            int u_texture;
            int u_color;

        public:
            Unlit_tex_PassShader()
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
            }

            void setMVP(const MathCore::mat4f &mvp)
            {
                setUniform(u_mvp, mvp);
            }
            void setTexture(int texunit)
            {
                setUniform(u_texture, texunit);
            }
            void setColor(const MathCore::vec4f &color)
            {
                setUniform(u_color, color);
            }
        };

        class Unlit_tex_vertcolor_PassShader : public DefaultEngineShader
        {
            int u_mvp;
            int u_texture;
            int u_color;

        public:
            Unlit_tex_vertcolor_PassShader()
            {
                format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

                const char vertexShaderCode[] = {
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
                    "varying vec2 uv;"
                    "varying vec4 color;"
                    "uniform vec4 uColor;"
                    "uniform sampler2D uTexture;"
                    "void main() {"
                    "  vec4 texel = texture2D(uTexture, uv);"
                    "  vec4 result = texel * color * uColor;"
                    "  gl_FragColor = result;"
                    "}"};

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                DefaultEngineShader::setupAttribLocation();
                link(__FILE__, __LINE__);

                u_mvp = getUniformLocation("uMVP");
                u_texture = getUniformLocation("uTexture");
                u_color = getUniformLocation("uColor");
            }

            void setMVP(const MathCore::mat4f &mvp)
            {
                setUniform(u_mvp, mvp);
            }
            void setTexture(int texunit)
            {
                setUniform(u_texture, texunit);
            }
            void setColor(const MathCore::vec4f &color)
            {
                setUniform(u_color, color);
            }
        };

        class Unlit_tex_vertcolor_font_PassShader : public DefaultEngineShader
        {
            int u_mvp;
            int u_texture;
            int u_color;

        public:
            Unlit_tex_vertcolor_font_PassShader()
            {
                format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

                const char vertexShaderCode[] = {
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
                    "varying vec2 uv;"
                    "varying vec4 color;"
                    "uniform vec4 uColor;"
                    "uniform sampler2D uTexture;"
                    "void main() {"
                    "  vec4 texel = vec4( 1.0,1.0,1.0, texture2D(uTexture, uv).a);"
                    "  vec4 result = texel * color * uColor;"
                    "  gl_FragColor = result;"
                    "}"};

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                DefaultEngineShader::setupAttribLocation();
                link(__FILE__, __LINE__);

                u_mvp = getUniformLocation("uMVP");
                u_texture = getUniformLocation("uTexture");
                u_color = getUniformLocation("uColor");
            }

            void setMVP(const MathCore::mat4f &mvp)
            {
                setUniform(u_mvp, mvp);
            }
            void setTexture(int texunit)
            {
                setUniform(u_texture, texunit);
            }
            void setColor(const MathCore::vec4f &color)
            {
                setUniform(u_color, color);
            }
        };
    }

}