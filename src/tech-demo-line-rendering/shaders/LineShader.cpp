#include "LineShader.h"
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Components/ComponentCamera.h>
#include <appkit-gl-engine/ResourceMap.h>

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

                "uniform float uAntialias;"

                "mat2 rotation(float angle) {"
                "  float c = cos(angle);"
                "  float s = sin(angle);"
                "  return mat2(c, s, -s, c);"
                "}"

                // // Liang-Barsky line clipping algorithm (with branch version)
                // "bool barsky_clip_test(float p, float q, inout float u1, inout float u2) {"
                // "  const float epsilon = 1e-10;"
                // "  bool is_p_zero = abs(p) < epsilon;"
                // "  bool is_p_negative = p < -epsilon;"
                // "  bool is_p_positive = p > epsilon;"
                // // If p is essentially zero, check if q is negative (line is outside plane)
                // "  if (is_p_zero) {"
                // "    return q >= 0.0;" // if q < 0, line is completely outside
                // "  }"
                // "  float r = q / p;"
                // // If p < 0 (entering), update u1 if r is valid
                // "  if (is_p_negative) {"
                // "    if (r > u2) return false;" // line starts after exit point
                // "    if (r > u1) u1 = r;"
                // "  }"
                // // If p > 0 (exiting), update u2 if r is valid  
                // "  else if (is_p_positive) {"
                // "    if (r < u1) return false;" // line ends before entry point
                // "    if (r < u2) u2 = r;"
                // "  }"
                // "  return u1 <= u2;" // return true if not completely clipped
                // "}"

                // Liang-Barsky line clipping algorithm (simplified branch-less version)
                "bool barsky_clip_test(float p, float q, inout float u1, inout float u2) {"
                "  const float epsilon = 1e-10;"
                
                // Handle p ≈ 0 case: line is parallel to clipping plane
                "  float is_p_zero = step(abs(p), epsilon);"
                "  float parallel_reject = is_p_zero * step(q, -epsilon);" // reject if q < 0
                
                // Calculate intersection parameter r, avoiding division by zero
                "  float safe_p = p + epsilon * sign(p + epsilon);" // ensure non-zero with correct sign
                "  float r = q / safe_p;"
                
                // Determine which parameter to update based on sign of p
                "  float is_entering = step(p, -epsilon);" // p < 0 (entering region)
                "  float is_exiting = step(epsilon, p);"   // p > 0 (exiting region)
                
                // Update u1 (entry) if entering and r > u1
                "  float update_u1 = is_entering * step(u1, r);"
                "  u1 = mix(u1, r, update_u1);"
                
                // Update u2 (exit) if exiting and r < u2  
                "  float update_u2 = is_exiting * step(r, u2);"
                "  u2 = mix(u2, r, update_u2);"
                
                // Check validity: not parallel-rejected AND u1 <= u2
                "  float is_valid = (1.0 - parallel_reject) * step(u1, u2);"
                "  return is_valid > 0.5;"
                "}"

                "void main() {"
                "  vec4 line_p1_clip = uMVP * aUV1;"
                "  vec4 line_p2_clip = uMVP * aUV2;"

                "  float line_lrp = aUV3.x;"
                "  float line_thickness_px = aUV3.y;"

                "  line_thickness_px_half = line_thickness_px * 0.5;"

                // clip test in clip space - liang barsky
                "  vec4 p1p2_clip_dir = line_p2_clip - line_p1_clip;"

                "  float u1 = 0.0;"
                "  float u2 = 1.0;"
                "  const float epsilon = 1e-3;"

                // Near plane clipping: -w <= z <= w, so z >= -w means z + w >= 0
                // min test on lim_min = -(-line_p1_clip.w + epsilon)
                "  bool near_clipped = barsky_clip_test(-p1p2_clip_dir.z - p1p2_clip_dir.w, line_p1_clip.z - (-line_p1_clip.w + epsilon), u1, u2);"
                // Far plane clipping: z <= w means w - z >= 0  
                // max test on lim_max = (line_p1_clip.w - epsilon)
                "  bool far_clipped = barsky_clip_test(p1p2_clip_dir.z - p1p2_clip_dir.w, (line_p1_clip.w - epsilon) - line_p1_clip.z, u1, u2);"

                // Check if the line segment is completely clipped
                // early discard vertex by putting it out of the NDC view
                "  if (!near_clipped || !far_clipped) {"
                "    gl_Position = vec4(0.0, 0.0, -2.0, 1.0);"
                "    return;"
                "  }"

                "  line_p2_clip = line_p1_clip + p1p2_clip_dir * u2;"
                "  line_p1_clip = line_p1_clip + p1p2_clip_dir * u1;"

                // Now perform perspective division on the clipped points
                "  vec4 line_p1_ndc = line_p1_clip / line_p1_clip.w;"
                "  vec4 line_p2_ndc = line_p2_clip / line_p2_clip.w;"

                // Recalculate direction in NDC space for screen space calculations
                "  vec2 p1p2_ndc_dir = line_p2_ndc.xy - line_p1_ndc.xy;"

                "  vec4 vert_clip = mix(line_p1_clip, line_p2_clip, line_lrp);"
                "  vec4 vert_ndc = mix(line_p1_ndc, line_p2_ndc, line_lrp);"

                "  vec2 p1p2_px = p1p2_ndc_dir.xy * 0.5 * uScreenSizePx;"

                "  float angle = atan(p1p2_px.y, p1p2_px.x);"
                "  mat2 rotation_matrix = rotation(angle);"

                "  p1p2_length_px = dot(p1p2_px, rotation_matrix[0]);"

                // intel HD3000 hack... 
                // avoid artifacts due to 16bit float on fragment shader
                // use the point that is near the screen center as reference of the line
                // this will keep the numbers, and line distance calculation
                // with a non-artifact range in screen space.
                "  float choose_pt = step(dot(line_p2_ndc.xy,line_p2_ndc.xy), dot(line_p1_ndc.xy,line_p1_ndc.xy));"
                "  p1_px = (mix(line_p1_ndc.xy, line_p2_ndc.xy, choose_pt) * 0.5 + 0.5) * uScreenSizePx;"
                "  p1p2_dir_normalized = mix(rotation_matrix[0], -rotation_matrix[0], choose_pt);"

                "  vec2 vert_pos_px = (vert_ndc.xy * 0.5 + 0.5) * uScreenSizePx;"
                "  vert_pos_px += rotation_matrix * aPosition * (line_thickness_px_half + uAntialias);"

                "  vert_ndc.xy = (vert_pos_px * uScreenSizePx_inv - 0.5) * 2.0;"

                // Convert the modified NDC back to clip space
                "  vert_clip.xy = vert_ndc.xy * vert_clip.w;"

                "  color = aColor0;"
                "  aa_px = uAntialias;"
                "  gl_Position = vert_clip;"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "uniform vec4 uColor;"

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
                "    lrp_distance = step(distance_to_line, line_thickness_px_half);"
                "  else"
                "    lrp_distance = smoothstep(line_thickness_px_half + aa_px, line_thickness_px_half - aa_px, distance_to_line);"
                "  if (lrp_distance <= 0.0)"
                "    discard;"
                "  vec4 result = color * uColor;"
                "  result.a *= lrp_distance;"
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
            // uScreenSizePx = getUniformVec2(u_screenSizePx);
            // uScreenSizePx_inv = getUniformVec2(u_screenSizePx_inv);
            // uMVP = getUniformMat4(u_mvp);
            // uColor = getUniformVec4(u_color);
            // uAntialias = getUniformFloat(u_antialias);

            // initialize uniforms

            uScreenSizePx = MathCore::vec2f(1.0f, 1.0f);
            uScreenSizePx_inv = MathCore::vec2f(1.0f, 1.0f);
            uMVP = MathCore::mat4f();
            uColor = MathCore::vec4f(1.0,1.0,1.0,1.0);
            uAntialias = 0.0f;

            GLRenderState *state = GLRenderState::Instance();
            GLShader* old_shader = state->CurrentShader;
            state->CurrentShader = this;

            setUniform(u_screenSizePx, uScreenSizePx);
            setUniform(u_screenSizePx_inv, uScreenSizePx_inv);
            setUniform(u_mvp, uMVP);
            setUniform(u_color, uColor);
            setUniform(u_antialias, uAntialias);

            state->CurrentShader = old_shader;
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

        void LineShader::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;
            state->CurrentShader = this;

            float aa = materialBag.getProperty<float>("uAntialias");
            if (aa < 0.001f)
                state->BlendMode =  AppKit::GLEngine::BlendModeDisabled;
            else
                state->BlendMode = AppKit::GLEngine::BlendModeAlpha;
            setAntialias(aa);
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));
        }

        void LineShader::setUniformsFromMatrices(
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
            setScreenSizePx(MathCore::vec2f(camera->viewport.w, camera->viewport.h));
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
