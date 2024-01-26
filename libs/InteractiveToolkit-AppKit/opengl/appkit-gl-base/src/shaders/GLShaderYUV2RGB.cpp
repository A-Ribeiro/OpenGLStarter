#include <appkit-gl-base/shaders/GLShaderYUV2RGB.h>
#include <appkit-gl-base/platform/PlatformGL.h>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {

        GLShaderYUV2RGB::GLShaderYUV2RGB(bool useBlur) : GLShader()
        {

            const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "attribute vec4 vPosition;"
                "attribute vec2 vUV;"
                "uniform vec2 scale;"
                "varying vec2 uv;"
                "void main() {"
                "  uv = vUV;"
                "  vec4 p = vPosition;"
                "  p.xy *= scale;"
                "  gl_Position = p;"
                "}"
            };

            const char fragmentShaderCode[] = {
            //"precision mediump float;"
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "uniform float fade;"
                "varying vec2 uv;"
                "uniform sampler2D textureY;"
                "uniform sampler2D textureU;"
                "uniform sampler2D textureV;"
                "vec3 yuv2rgb(vec3 yuv) {"
                "yuv.x = yuv.x * 1.1640625 - 0.0710784313725490196078431372549;"
                "yuv.yz -= 0.5;"
                "return yuv.xxx + MathCore::vec3f("
                "2.015625 * yuv.y,"
                "-0.390625 * yuv.y - 0.8125 * yuv.z,"
                "1.59765625 * yuv.z"
                ");"
                "}"
                "void main() {"
                "  vec3 yuv;"
                "  yuv.x = texture2D(textureY, uv).a;"
                "  yuv.y = texture2D(textureU, uv).a;"
                "  yuv.z = texture2D(textureV, uv).a;"
                "  vec4 result = vec4(0.0,0.0,0.0,1.0);"
                "  result.rgb = yuv2rgb(yuv);"
                "  gl_FragColor = result * fade;"
                "}"
            };

            const char blurFragmentShaderCode[] = {
            //"precision mediump float;"
#if !defined(GLAD_GLES2)
                "#version 120\n"
#endif
                "uniform float fade;"
                "varying vec2 uv;"
                "uniform sampler2D textureY;"
                "uniform sampler2D textureU;"
                "uniform sampler2D textureV;"
                "vec3 yuv2rgb(vec3 yuv) {"
                "yuv.x = yuv.x * 1.1640625 - 0.0710784313725490196078431372549;"
                "yuv.yz -= 0.5;"
                "return yuv.xxx + MathCore::vec3f("
                "2.015625 * yuv.y,"
                "-0.390625 * yuv.y - 0.8125 * yuv.z,"
                "1.59765625 * yuv.z"
                ");"
                "}"

                "\n#define SIZE 15\n"
                "\n#define THRESHOLD (30.0/255.0)\n"
                "\n#define THRESHOLD_SQR (THRESHOLD*THRESHOLD)\n"

                "vec3 RGB(vec2 uv) { "
                "  vec3 yuv;"
                "  yuv.x = texture2D(textureY, uv).a;"
                "  yuv.y = texture2D(textureU, uv).a;"
                "  yuv.z = texture2D(textureV, uv).a;"
                "  return yuv2rgb(yuv);"
                "} "

                "void main() { "
                "  vec4 texColor = vec4(0.0,0.0,0.0,1.0);"

                "  texColor.rgb = RGB(uv);"

                "  vec2 dx = dFdx(uv);"
                "  vec2 dy = dFdy(uv);"

                "  vec2 uv1, uv2, uv3, uv4;"
                "  vec2 aux, aux2;"
                "  float faux;"
                "  vec3 acc = MathCore::vec3f(0.0,0.0,0.0);"
                "  float count = 0.0;"
                "  vec3 tex1, tex2, tex3, tex4, cmp1,cmp2,cmp3,cmp4;"

                "  float walk_1 = 1.0;"
                "  float walk_2 = 1.0;"
                "  float walk_3 = 1.0;"
                "  float walk_4 = 1.0;"

                "  for(int i=1;i<SIZE;i++)"
                "  {"
                "    faux = float(i);"
                "    aux = dx * faux;"
                "    aux2 = dy * faux;"

                "    uv1 = uv + aux;"
                "    uv2 = uv - aux;"
                "    uv3 = uv + aux2;"
                "    uv4 = uv - aux2;"

                "    tex1 = RGB(uv1);"
                "    tex2 = RGB(uv2);"
                "    tex3 = RGB(uv3);"
                "    tex4 = RGB(uv4);"

                "    cmp1 = tex1 - texColor.rgb;"
                "    cmp2 = tex2 - texColor.rgb;"
                "    cmp3 = tex3 - texColor.rgb;"
                "    cmp4 = tex4 - texColor.rgb;"

                "    walk_1 *= step( dot(cmp1,cmp1), THRESHOLD_SQR );"
                "    walk_2 *= step( dot(cmp2,cmp2), THRESHOLD_SQR );"
                "    walk_3 *= step( dot(cmp3,cmp3), THRESHOLD_SQR );"
                "    walk_4 *= step( dot(cmp4,cmp4), THRESHOLD_SQR );"

                "    count += walk_1;"
                "    count += walk_2;"
                "    count += walk_3;"
                "    count += walk_4;"

                "    acc += tex1 * walk_1;"
                "    acc += tex2 * walk_2;"
                "    acc += tex3 * walk_3;"
                "    acc += tex4 * walk_4;"
                "  }"

                "  texColor.rgb = ( texColor.rgb + acc ) / ( 1.0 + count ) ;"
                "  gl_FragColor = texColor * fade;"
                "}"
            };

            if (useBlur)
                compile(vertexShaderCode, blurFragmentShaderCode, __FILE__, __LINE__);
            else
                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            bindAttribLocation(GLShaderYUV2RGB::vPosition, "vPosition");
            bindAttribLocation(GLShaderYUV2RGB::vUV, "vUV");
            link(__FILE__, __LINE__);

            // attrib
            // vPosition = getAttribLocation("vPosition");
            // vUV = getAttribLocation("vUV");
            // uniform
            scale = getUniformLocation("scale");
            textureY = getUniformLocation("textureY");
            textureU = getUniformLocation("textureU");
            textureV = getUniformLocation("textureV");
            fade = getUniformLocation("fade");
        }

        void GLShaderYUV2RGB::setScale(float x, float y)
        {
            setUniform(scale, MathCore::vec2f(x, y));
        }

        void GLShaderYUV2RGB::setTextureY(int textureUnit)
        {
            setUniform(textureY, textureUnit);
        }
        void GLShaderYUV2RGB::setTextureU(int textureUnit)
        {
            setUniform(textureU, textureUnit);
        }
        void GLShaderYUV2RGB::setTextureV(int textureUnit)
        {
            setUniform(textureV, textureUnit);
        }

        void GLShaderYUV2RGB::setFade(float v)
        {
            setUniform(fade, v);
        }

    }
}
