#include <appkit-gl-engine/shaders/LineShader.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <appkit-gl-engine/Components/Core/ComponentCamera.h>
#include <appkit-gl-engine/ResourceMap.h>

#include <appkit-gl-engine/shaders/BasicShadersDefinitions.inl>

#define USE_ONLY_Z_CLIPPING 1
#define USE_XYZ_CLIPPING 2

#define CLIPPING_BARSKY_ALGORITHM USE_ONLY_Z_CLIPPING

namespace AppKit
{
    namespace GLEngine
    {
        const AppKit::OpenGL::ShaderType LineShader::Type = "LineShader";

        LineShader::LineShader() : DefaultEngineShader(LineShader::Type)
        {
            format = ITKExtension::Model::CONTAINS_POS |
                     ITKExtension::Model::CONTAINS_UV1 | ITKExtension::Model::CONTAINS_UV2 | ITKExtension::Model::CONTAINS_UV3 |
                     ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec2 aPosition;\n" // 2d square position
                "attribute vec4 aUV1;\n"      // line point 1
                "attribute vec4 aUV2;\n"      // line point 2
                // x = current position lrp in line [0..1]
                // y = line_thickness_px
                "attribute vec2 aUV3;\n"
                "attribute vec4 aColor0;\n"

                "uniform vec2 uScreenSizePx;\n"
                "uniform vec2 uScreenSizePx_inv;\n"
                "uniform mat4 uMVP;\n"
                "uniform vec4 uWindowViewportPx;\n"

                "varying vec4 windowViewportToScreenSizePx;\n"
                "varying vec4 color;\n"
                "varying vec2 p1_px;\n"
                "varying float p1p2_length_px;\n"
                "varying vec2 p1p2_dir_normalized;\n"
                "varying float line_thickness_px_half;\n"
                "varying float aa_px;\n"

                "uniform float uAntialias;\n"

                "mat2 rotation(float angle) {\n"
                "  float c = cos(angle);\n"
                "  float s = sin(angle);\n"
                "  return mat2(c, s, -s, c);\n"
                "}\n"

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

            // // Liang-Barsky line clipping algorithm (simplified branch-less version)
            // "bool barsky_clip_test(float p, float q, inout float u1, inout float u2) {\n"
            // "  const float epsilon = 1e-10;\n"

            // // Handle p ≈ 0 case: line is parallel to clipping plane
            // "  float is_p_zero = step(abs(p), epsilon);\n"
            // "  float parallel_reject = is_p_zero * step(q, -epsilon);\n" // reject if q < 0

            // // Calculate intersection parameter r, avoiding division by zero
            // "  float safe_p = p + epsilon * sign(p + epsilon);\n" // ensure non-zero with correct sign
            // "  float r = q / safe_p;\n"

            // // Determine which parameter to update based on sign of p
            // "  float is_entering = step(p, -epsilon);\n" // p < 0 (entering region)
            // "  float is_exiting = step(epsilon, p);\n"   // p > 0 (exiting region)

            // // Update u1 (entry) if entering and r > u1
            // "  float update_u1 = is_entering * step(u1, r);\n"
            // "  u1 = mix(u1, r, update_u1);\n"

            // // Update u2 (exit) if exiting and r < u2
            // "  float update_u2 = is_exiting * step(r, u2);\n"
            // "  u2 = mix(u2, r, update_u2);\n"

            // // Check validity: not parallel-rejected AND u1 <= u2
            // "  float is_valid = (1.0 - parallel_reject) * step(u1, u2);\n"
            // "  return is_valid > 0.5;\n"
            // "}\n"
#if CLIPPING_BARSKY_ALGORITHM == USE_ONLY_Z_CLIPPING
                barsky_clip_test_vec2
#elif CLIPPING_BARSKY_ALGORITHM == USE_XYZ_CLIPPING
                barsky_clip_test_vec3
#else
#error "Unsupported CLIPPING_BARSKY_ALGORITHM value"
#endif

                "void main() {\n"
                "  windowViewportToScreenSizePx.xy = uWindowViewportPx.xy;\n"
                "  windowViewportToScreenSizePx.zw = uWindowViewportPx.zw * uScreenSizePx.xy;\n"

                "  vec4 line_p1_clip = uMVP * aUV1;\n"
                "  vec4 line_p2_clip = uMVP * aUV2;\n"

                "  float line_lrp = aUV3.x;\n"
                "  float line_thickness_px = aUV3.y;\n"

                "  line_thickness_px_half = line_thickness_px * 0.5;\n"

                // clip test in clip space - liang barsky
                "  vec4 p1p2_clip_dir = line_p2_clip - line_p1_clip;\n"

                "  float u1 = 0.0;\n"
                "  float u2 = 1.0;\n"
                "  const float epsilon = 1e-3;\n"

            // // Near plane clipping: -w <= z <= w, so z >= -w means z + w >= 0
            // // min test on lim_min = -(-line_p1_clip.w + epsilon)
            // "  bool near_clipped = barsky_clip_test(-p1p2_clip_dir.z - p1p2_clip_dir.w, line_p1_clip.z - (-line_p1_clip.w + epsilon), u1, u2) > 0.5;\n"
            // // Far plane clipping: z <= w means w - z >= 0
            // // max test on lim_max = (line_p1_clip.w - epsilon)
            // "  bool far_clipped = barsky_clip_test(p1p2_clip_dir.z - p1p2_clip_dir.w, (line_p1_clip.w - epsilon) - line_p1_clip.z, u1, u2) > 0.5;\n"

            // Near plane clipping: -w <= z <= w, so z >= -w means z + w >= 0
            // min test on lim_min = -(-line_p1_clip.w + epsilon)
            // Far plane clipping: z <= w means w - z >= 0
            // max test on lim_max = (line_p1_clip.w - epsilon)
#if CLIPPING_BARSKY_ALGORITHM == USE_ONLY_Z_CLIPPING
                "  vec2 u1_array = vec2(0.0);\n"
                "  vec2 u2_array = vec2(1.0);\n"
                "  vec2 coord_set_dir = vec2(-p1p2_clip_dir.z, p1p2_clip_dir.z);\n"
                "  vec2 coord_set_lim = vec2(line_p1_clip.z, - line_p1_clip.z);\n"
                "  vec2 near_far_valid = barsky_clip_test(coord_set_dir - p1p2_clip_dir.w, coord_set_lim + (line_p1_clip.w - epsilon), u1_array, u2_array);\n"

                // Check if the line segment is completely clipped
                // early discard vertex by putting it out of the NDC view
                "  if ( any( lessThan( near_far_valid, vec2(0.5) ) ) ) {\n"
                "    gl_Position = vec4(0.0, 0.0, -2.0, 1.0);\n"
                "    return;\n"
                "  }\n"

                "  u1 = max(u1_array.x, u1_array.y);\n"
                "  u2 = min(u2_array.x, u2_array.y);\n"
#elif CLIPPING_BARSKY_ALGORITHM == USE_XYZ_CLIPPING
                "  vec3 u1_array[2] = vec3[]( vec3(0.0), vec3(0.0) );\n"
                "  vec3 u2_array[2] = vec3[]( vec3(1.0), vec3(1.0) );\n"
                "  vec3 coord_set_dir[2] = vec3[]( -p1p2_clip_dir.xyz,  p1p2_clip_dir.xyz );\n"
                "  vec3 coord_set_lim[2] = vec3[](  line_p1_clip.xyz , -line_p1_clip.xyz  );\n"
                "  vec3 near_far_valid[2];\n"
                "  near_far_valid[0] = barsky_clip_test(coord_set_dir[0] - p1p2_clip_dir.w, coord_set_lim[0] + (line_p1_clip.w - epsilon), u1_array[0], u2_array[0]);\n"
                "  near_far_valid[1] = barsky_clip_test(coord_set_dir[1] - p1p2_clip_dir.w, coord_set_lim[1] + (line_p1_clip.w - epsilon), u1_array[1], u2_array[1]);\n"

                // Check if the line segment is completely clipped
                // early discard vertex by putting it out of the NDC view
                "  bvec3 check1 = lessThan(near_far_valid[0], vec3(0.5));\n"
                "  bvec3 check2 = lessThan(near_far_valid[1], vec3(0.5));\n"
                "  if ( any(check1) || any(check2) ) {\n"
                "    gl_Position = vec4(0.0, 0.0, -2.0, 1.0);\n"
                "    return;\n"
                "  }\n"

                "  vec3 u1_max = max(u1_array[0], u1_array[1]);\n"
                "  vec3 u2_min = min(u2_array[0], u2_array[1]);\n"

                "  u1 = max(u1_max.x, max(u1_max.y, u1_max.z) );\n"
                "  u2 = min(u2_min.x, min(u2_min.y, u2_min.z) );\n"
#endif

                "  line_p2_clip = line_p1_clip + p1p2_clip_dir * u2;\n"
                "  line_p1_clip = line_p1_clip + p1p2_clip_dir * u1;\n"

                // Now perform perspective division on the clipped points
                "  vec4 line_p1_ndc = line_p1_clip / line_p1_clip.w;\n"
                "  vec4 line_p2_ndc = line_p2_clip / line_p2_clip.w;\n"

                // Recalculate direction in NDC space for screen space calculations
                "  vec2 p1p2_ndc_dir = line_p2_ndc.xy - line_p1_ndc.xy;\n"

                "  vec4 vert_clip = mix(line_p1_clip, line_p2_clip, line_lrp);\n"
                "  vec4 vert_ndc = mix(line_p1_ndc, line_p2_ndc, line_lrp);\n"

                "  vec2 p1p2_px = p1p2_ndc_dir.xy * 0.5 * uScreenSizePx;\n"

                "  float angle = atan(p1p2_px.y, p1p2_px.x);\n"
                "  mat2 rotation_matrix = rotation(angle);\n"

                "  p1p2_length_px = dot(p1p2_px, rotation_matrix[0]);\n"

                // intel HD3000 hack...
                // avoid artifacts due to 16bit float on fragment shader
                // use the point that is near the screen center as reference of the line
                // this will keep the numbers, and line distance calculation
                // with a non-artifact range in screen space.
                "  float choose_pt = step(dot(line_p2_ndc.xy,line_p2_ndc.xy), dot(line_p1_ndc.xy,line_p1_ndc.xy));\n"
                "  p1_px = (mix(line_p1_ndc.xy, line_p2_ndc.xy, choose_pt) * 0.5 + 0.5) * uScreenSizePx;\n"
                "  p1p2_dir_normalized = mix(rotation_matrix[0], -rotation_matrix[0], choose_pt);\n"

                "  vec2 vert_pos_px = (vert_ndc.xy * 0.5 + 0.5) * uScreenSizePx;\n"
                "  vert_pos_px += rotation_matrix * aPosition * (line_thickness_px_half + uAntialias);\n"

                "  vert_ndc.xy = (vert_pos_px * uScreenSizePx_inv - 0.5) * 2.0;\n"

                // Convert the modified NDC back to clip space
                "  vert_clip.xy = vert_ndc.xy * vert_clip.w;\n"

                "  color = aColor0;\n"
                "  aa_px = uAntialias;\n"
                "  gl_Position = vert_clip;\n"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "uniform vec4 uColor;\n"

                "varying vec4 windowViewportToScreenSizePx;\n"
                "varying vec4 color;\n"
                "varying vec2 p1_px;\n"
                "varying float p1p2_length_px;\n"
                "varying vec2 p1p2_dir_normalized;\n"
                "varying float line_thickness_px_half;\n"
                "varying float aa_px;\n"

                "void main() {\n"
                "  vec2 pixel_pos_window = (gl_FragCoord.xy - windowViewportToScreenSizePx.xy) * windowViewportToScreenSizePx.zw;\n"
                "  float aux = dot( pixel_pos_window - p1_px, p1p2_dir_normalized);\n"
                "  aux = clamp(aux, 0.0, p1p2_length_px );\n"
                "  vec2 closest_point_on_line = p1_px + aux * p1p2_dir_normalized;\n"
                "  float distance_to_line = distance(closest_point_on_line, pixel_pos_window);\n"
                "  float lrp_distance;\n"
                "  if (aa_px < 0.5)\n"
                "    lrp_distance = step(distance_to_line, line_thickness_px_half);\n"
                "  else\n"
                "    lrp_distance = smoothstep(line_thickness_px_half + aa_px, line_thickness_px_half - aa_px, distance_to_line);\n"
                "  if (lrp_distance <= 0.0)\n"
                "    discard;\n"
                "  vec4 result = color * uColor;\n"
                "  result.a *= lrp_distance;\n"
                "  gl_FragColor = result;\n"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_screenSizePx = getUniformLocation("uScreenSizePx");
            u_screenSizePx_inv = getUniformLocation("uScreenSizePx_inv");
            u_mvp = getUniformLocation("uMVP");
            u_window_viewport_px = getUniformLocation("uWindowViewportPx");
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
            uColor = MathCore::vec4f(1.0, 1.0, 1.0, 1.0);
            uAntialias = 0.0f;

            GLRenderState *state = GLRenderState::Instance();
            GLShader *old_shader = state->CurrentShader;
            state->CurrentShader = this;

            setUniform(u_screenSizePx, uScreenSizePx);
            setUniform(u_screenSizePx_inv, uScreenSizePx_inv);
            setUniform(u_mvp, uMVP);
            setUniform(u_window_viewport_px, uWindowViewportPx);
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

        void LineShader::setWindowViewportPx(const MathCore::vec4f &windowViewportPx)
        {
            if (uWindowViewportPx != windowViewportPx)
            {
                uWindowViewportPx = windowViewportPx;
                setUniform(u_window_viewport_px, uWindowViewportPx);
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
            if (aa <= 0.0f)
                state->BlendMode = AppKit::GLEngine::BlendModeDisabled;
            else
                state->BlendMode = AppKit::GLEngine::BlendModeAlpha;
            setAntialias(aa);
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));

            AppKit::GLEngine::iRect window_viewport = state->Viewport;

            setWindowViewportPx(MathCore::vec4f(
                (float)window_viewport.x,
                (float)window_viewport.y,
                1.0f / (float)window_viewport.w,
                1.0f / (float)window_viewport.h));
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
            setScreenSizePx(camera->projectionAreaSizePx);
        }

        Utils::ShaderPropertyBag LineShader::createDefaultBag() const
        {
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
