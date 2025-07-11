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
            format = ITKExtension::Model::CONTAINS_POS | 
                        ITKExtension::Model::CONTAINS_UV1 | ITKExtension::Model::CONTAINS_UV2 | ITKExtension::Model::CONTAINS_UV3 | 
                        ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec2 aPosition;" // 2d square position
                "attribute vec4 aUV1;"      // line point 1
                "attribute vec4 aUV2;"      // line point 2
                // x = current position lrp in line [0..1]
                // y = line_thickness_px
                "attribute vec2 aUV3;"
                "attribute vec4 aColor0;"

                "uniform vec2 uScreenSizePx;"
                "uniform vec2 uScreenSizePx_inv;"
                "uniform mat4 uMVP;"

                "varying vec4 color;"
                "varying vec2 p1_px;"
                "varying float p1p2_length_px;"
                "varying vec2 p1p2_dir_normalized;"
                "varying float line_thickness_px_half;"
                "varying float aa_px;"

                "uniform float uAntialias = 0.0;"

                "mat2 rotation(float angle) {"
                "  float c = cos(angle);"
                "  float s = sin(angle);"
                "  return mat2(c, s, -s, c);"
                "}"

                // "void barsky_clip_test(float p, float q, inout float u1, inout float u2) {"
                // "  if (p < 0.0) {"
                // "    float r = q / p;"
                // "    if (r > u1 && r < u2)"
                // "      u1 = r;"
                // "  } else if (p > 0.0) {"
                // "    float r = q / p;"
                // "    if (r < u2 && r > u1)"
                // "      u2 = r;"
                // "  }"
                // "}"

                // liang barsky clip test no branching
                "void barsky_clip_test(float p, float q, inout float u1, inout float u2) {"
                // avoid -inf, +inf due to division by zero, clamping to a valid range
                "  float r = clamp(q / p, -1e20, 1e20);"
                "  float is_p_negative = step(p, -0.0);"// p > -0 = 0 -> p <= -0 -> 1
                "  float is_p_positive = step(0.0, p);"// p < 0 = 0 -> p >= 0 -> 1
                "  float r_in_range = step(u1, r) * step(r, u2);" // r >= u1 && r <= u2
                "  u1 = mix(u1, r, is_p_negative * r_in_range);"
                "  u2 = mix(u2, r, is_p_positive * r_in_range);"
                "}"

                "void main() {"
                "  vec4 line_p1_ndc = uMVP * aUV1;"
                "  vec4 line_p2_ndc = uMVP * aUV2;"

                "  float line_lrp = aUV3.x;"
                "  float line_thickness_px = aUV3.y;"

                "  line_thickness_px_half = line_thickness_px * 0.5;"

                "  line_p1_ndc /= line_p1_ndc.w;"
                "  line_p2_ndc /= line_p2_ndc.w;"

                // clip test - liang barsky
                "  vec3 p1p2_dir = line_p2_ndc.xyz - line_p1_ndc.xyz;"
                "  float u1 = 0;"
                "  float u2 = 1;"
                "  const float epsilon = 1e-6;"
                "  barsky_clip_test(-p1p2_dir.z, line_p1_ndc.z - (-1.0 + epsilon), u1, u2);"
                "  barsky_clip_test(p1p2_dir.z, (1.0 - epsilon) - line_p1_ndc.z, u1, u2);"
                "  line_p2_ndc.xyz = line_p1_ndc.xyz + p1p2_dir * u2;"
                "  line_p1_ndc.xyz = line_p1_ndc.xyz + p1p2_dir * u1;"
                "  p1p2_dir = line_p2_ndc.xyz - line_p1_ndc.xyz;"

                "  p1_px = line_p1_ndc.xy * 0.5 + 0.5;"
                "  p1_px *= uScreenSizePx;"

                "  vec4 vert_ndc = mix(line_p1_ndc, line_p2_ndc, line_lrp);"

                "  vec2 p1p2_px = p1p2_dir.xy * 0.5 * uScreenSizePx;"

                "  float angle = atan(p1p2_px.y, p1p2_px.x);"
                "  mat2 rotation_matrix = rotation(angle);"
                "  p1p2_dir_normalized = rotation_matrix[0];"

                "  p1p2_length_px = dot(p1p2_px, p1p2_dir_normalized);"

                "  vec2 vert_pos_px = (vert_ndc.xy * 0.5 + 0.5) * uScreenSizePx;"
                "  vert_pos_px += rotation_matrix * aPosition * (line_thickness_px_half + uAntialias);"

                "  vert_ndc.xy = (vert_pos_px * uScreenSizePx_inv - 0.5) * 2.0;"

                "  color = aColor0;"
                "  aa_px = uAntialias;"
                "  gl_Position = vert_ndc;"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "uniform vec4 uColor = vec4(1.0,1.0,1.0,1.0);"

                "varying vec4 color;"
                "varying vec2 p1_px;"
                "varying float p1p2_length_px;"
                "varying vec2 p1p2_dir_normalized;"
                "varying float line_thickness_px_half;"
                "varying float aa_px;"

                "void main() {"
                "  vec2 pixel_pos_window = gl_FragCoord.xy;"
                "  float aux = dot( pixel_pos_window - p1_px, p1p2_dir_normalized);"
                "  aux = clamp(aux, 0.0, p1p2_length_px );"
                "  vec2 closest_point_on_line = p1_px + aux * p1p2_dir_normalized;"
                "  float distance_to_line = distance(closest_point_on_line, pixel_pos_window);"
                "  float lrp_distance;"
                "  if (aa_px < 0.001)"
                "    lrp_distance = step(line_thickness_px_half, distance_to_line);"
                "  else"
                "    lrp_distance = smoothstep(line_thickness_px_half - aa_px, line_thickness_px_half + aa_px, distance_to_line);"
                "  if (lrp_distance >= 1.0)"
                "    discard;"
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
            u_antialias = getUniformLocation("uAntialias");

            // query values
            uScreenSizePx = getUniformVec2(u_screenSizePx);
            uScreenSizePx_inv = getUniformVec2(u_screenSizePx_inv);
            uMVP = getUniformMat4(u_mvp);
            uColor = getUniformVec4(u_color);
            uAntialias = getUniformFloat(u_antialias);
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

        void LineShader::setAntialias(float antialias)
        {
            if (uAntialias != antialias)
            {
                uAntialias = antialias;
                setUniform(u_antialias, uAntialias);
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

            if (uAntialias < 0.001f)
                state->BlendMode =  AppKit::GLEngine::BlendModeDisabled;
            else
                state->BlendMode = AppKit::GLEngine::BlendModeAlpha;

            setMVP(*mvp);
            setScreenSizePx(MathCore::vec2f(camera->viewport.w, camera->viewport.h));
            setColor(bag.getProperty<MathCore::vec4f>("uColor"));
            setAntialias(bag.getProperty<float>("uAntialias"));

        }

        void LineShader::deactivateShader(GLRenderState *state) {

        }

        Utils::ShaderPropertyBag LineShader::createDefaultBag() const {
            Utils::ShaderPropertyBag bag;
            // bag.addProperty("uScreenSizePx", uScreenSizePx);
            // bag.addProperty("uScreenSizePx_inv", uScreenSizePx_inv);
            // bag.addProperty("uMVP", uMVP);
            bag.addProperty("uColor", uColor); // only color is exposed to the user
            bag.addProperty("uAntialias", uAntialias);
            return bag;
        }
    }
}
