#include "LineShader.h"
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Components/ComponentCamera.h>

namespace AppKit
{
    namespace GLEngine
    {
        LineShader::LineShader()
        {
            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0 | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec2 aPosition;" // 2d square position
                "attribute vec4 aUV1;"      // line point 1
                "attribute vec4 aUV2;"      // line point 2
                // x = current position lrp in line [0..1]
                // y = line_width_px
                "attribute vec3 aUV3;"
                "attribute vec4 aColor0;"

                "uniform vec2 uScreenSizePx;"
                "uniform vec2 uScreenSizePx_inv;"
                "uniform mat4 uMVP;"

                "varying vec4 color;"
                "varying vec2 p1_px;"
                "varying float p1p2_length;"
                "varying vec2 p1p2_dir_normalized;"
                "varying float line_width_px_half;"

                "mat2 rotation(float angle) {"
                "  float c = cos(angle);"
                "  float s = sin(angle);"
                "  return mat2(c, s, -s, c);"
                "}"

                "void main() {"
                "  vec4 line_p1_ndc = uMVP * aUV1;"
                "  vec4 line_p2_ndc = uMVP * aUV2;"

                "  float line_lrp = aUV3.x;"
                "  float line_width_px = aUV3.y;"

                "  line_width_px_half = line_width_px * 0.5;"

                "  line_p1_ndc /= line_p1_ndc.w;"
                "  line_p1_ndc /= line_p1_ndc.w;"

                "  p1_px = line_p1_ndc.xy * 0.5 + 0.5;"
                "  p1_px *= uScreenSizePx;"

                "  vec4 vert_ndc = mix(line_p1_ndc, line_p2_ndc, line_lrp);"

                "  vec2 p1p2_px = (line_p2_ndc.xy - line_p1_ndc.xy) * 0.5 + 0.5;"
                "  p1p2_px *= uScreenSizePx;"

                "  float angle = atan(p1p2_px.y, p1p2_px.x);"
                "  mat2 rotation_matrix = rotation(angle);"
                "  p1p2_dir_normalized = rotation_matrix[0];"

                "  p1p2_length = dot(p1p2_px, p1p2_dir_normalized);"

                "  vec2 vert_pos_px = (vert_ndc.xy * 0.5 + 0.5) * uScreenSizePx;"
                "  vert_pos_px += rotation_matrix * aPosition * line_width_px;"

                "  vert_ndc.xy = (vert_pos_px * uScreenSizePx_inv - 0.5) * 2.0;"

                "  color = aColor0;"
                "  gl_Position = vert_ndc;"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "uniform vec4 uColor = vec4(1.0,0.0,1.0,1.0);"

                "varying vec4 color;"
                "varying vec2 p1_px;"
                "varying float p1p2_length;"
                "varying vec2 p1p2_dir_normalized;"
                "varying float line_width_px_half;"

                "void main() {"
                "  vec2 pixel_pos_window = gl_FragCoord.xy;"
                "  float aux = dot( pixel_pos_window - p1_px, p1p2_dir_normalized);"
                "  aux = clamp(aux, 0.0, p1p2_length);"
                "  vec2 closest_point_on_line = p1_px + aux * p1p2_dir_normalized;"
                "  float distance_to_line = distance(closest_point_on_line, pixel_pos_window);"
                "  float px_aa_px = 1.0;"
                "  float lrp_distance = smoothstep(line_width_px_half - px_aa_px, line_width_px_half + px_aa_px, distance_to_line);"
                "  if (lrp_distance > 1.0) {"
                "    discard;"
                "  }"
                "  vec4 result = color * uColor;"
                "  result.a *= 1.0 - lrp_distance;"
                "  gl_FragColor = result;"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_screenSizePx = getUniformLocation("uScreenSizePx");
            u_screenSizePx_inv = getUniformLocation("uScreenSizePx_inv");
            u_mvp = getUniformLocation("uMVP");
            u_color = getUniformLocation("uColor");

            // query values
            uScreenSizePx = getUniformVec2(u_screenSizePx);
            uScreenSizePx_inv = getUniformVec2(u_screenSizePx_inv);
            uMVP = getUniformMat4(u_mvp);
            uColor = getUniformVec4(u_color);
        }

        void LineShader::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }

        void LineShader::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        void LineShader::setScreenSizePx(const MathCore::vec2f &screenSizePx)
        {
            if (uScreenSizePx != screenSizePx)
            {
                uScreenSizePx = screenSizePx;
                setUniform(u_screenSizePx, uScreenSizePx);
                uScreenSizePx_inv = 1.0f / uScreenSizePx;
                setUniform(u_screenSizePx_inv, uScreenSizePx_inv);
            }
        }

        void LineShader::activateShaderAndSetPropertiesFromBag(
            Components::ComponentCamera *camera,
            const MathCore::mat4f *mvp,
            const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal, 
            GLRenderState *state,
            const Utils::ShaderPropertyBag &bag
        ) {
            state->CurrentShader = this;

            setMVP(*mvp);
            setScreenSizePx(MathCore::vec2f(camera->viewport.w, camera->viewport.h));
            setColor(bag.getProperty<MathCore::vec4f>("uColor"));


        }

        void LineShader::deactivateShader(GLRenderState *state) {

        }

        Utils::ShaderPropertyBag LineShader::createDefaultBag() const {
            Utils::ShaderPropertyBag bag;
            // bag.addProperty("uScreenSizePx", uScreenSizePx);
            // bag.addProperty("uScreenSizePx_inv", uScreenSizePx_inv);
            // bag.addProperty("uMVP", uMVP);
            bag.addProperty("uColor", uColor); // only color is exposed to the user
            return bag;
        }
    }
}
