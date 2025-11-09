#pragma once
#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {

#define MASKSHADER_FRAGMENT_UNIFORM    \
    "uniform mat4 uTransformToMask;\n" \
    "uniform vec2 uMask_corner[4];\n"  \
    "uniform vec4 uMask_radius;\n"

#define MASKSHADER_COMPUTE_MASK_FUNCTION                                                                                          \
    "float compute_mask(){\n"                                                                                                     \
    "  vec2 ptn_local_space = (uTransformToMask * vec4(gl_FragCoord.x, gl_FragCoord.y, (gl_FragCoord.z - 0.5) * 2.0, 1.0)).xy;\n" \
    "  vec2 box_min = uMask_corner[2] - uMask_radius[2];\n"                                                                       \
    "  vec2 box_max = uMask_corner[0] + uMask_radius[0];\n"                                                                       \
    "  vec2 box_size_half = (box_max - box_min) * 0.5;\n"                                                                         \
    "  vec2 box_center = (box_min + box_max) * 0.5;\n"                                                                            \
    "  vec2 box_pt_offset = abs( ptn_local_space - box_center );\n"                                                               \
    "  vec2 box_valid = step(box_pt_offset, box_size_half);\n"                                                                    \
    "  float valid = box_valid.x * box_valid.y;\n"                                                                                \
    "  vec2 qdrant_sign[4];\n"                                                                                                    \
    "  qdrant_sign[0] = vec2( 1.0,  1.0);\n"                                                                                      \
    "  qdrant_sign[1] = vec2( 1.0, -1.0);\n"                                                                                      \
    "  qdrant_sign[2] = vec2(-1.0, -1.0);\n"                                                                                      \
    "  qdrant_sign[3] = vec2(-1.0,  1.0);\n"                                                                                      \
    "  for(int i = 0;i < 4;i++){\n"                                                                                               \
    "    vec2 item_offset = ptn_local_space - uMask_corner[i];\n"                                                                 \
    "    vec2 item_qdrant = step(item_offset * qdrant_sign[i], vec2( 0.0 ));\n"                                                   \
    "    float item_qdrant_valid = max(item_qdrant.x, item_qdrant.y);\n"                                                          \
    "    float item_valid = step(dot(item_offset,item_offset), uMask_radius[i]*uMask_radius[i]);\n"                               \
    "    valid *= max(item_qdrant_valid, item_valid);\n"                                                                          \
    "  }\n"                                                                                                                       \
    "  return valid;\n"                                                                                                           \
    "}\n"

// #define MASKSHADER_COMPUTE_MASK_FUNCTION_SMOOTH                                                                                            \
//     "float compute_mask(float margin){\n"                                                                                                  \
//     "  vec2 ptn_local_space = (uTransformToMask * vec4(gl_FragCoord.x, gl_FragCoord.y, (gl_FragCoord.z - 0.5) * 2.0, 1.0)).xy;\n"          \
//     "  vec2 ptn_step = step(vec2(0.0), ptn_local_space);\n"                                                                                \
//     "  int quadrant = int((1.0 - ptn_step.x) * ( 2.0 + ptn_step.y ) + ptn_step.x * (1.0 - ptn_step.y) + 0.5);\n"                           \
//     "  vec2 quadrant_sign = sign(ptn_step - 0.5);\n"                                                                                       \
//     "  ptn_local_space *= quadrant_sign;\n"                                                                                                \
//     "  vec2 corner_abs_space = abs(uMask_corner[quadrant]);\n"                                                                             \
//     "  vec2 size = corner_abs_space + vec2(uMask_radius[quadrant]);\n"                                                                     \
//     "  vec2 ptn_on_proj_vec = ptn_local_space - corner_abs_space;\n"                                                                       \
//     "  float valid = 1.0;\n" /* Branchless version */                                                                                      \
//     "  float in_x_region = step(ptn_local_space.x, corner_abs_space.x);\n"                                                                 \
//     "  float in_y_region = step(ptn_local_space.y, corner_abs_space.y);\n"                                                                 \
//     "  float in_corner_region = (1.0 - in_x_region) * (1.0 - in_y_region);\n"                                                              \
//     "  vec2 size_smooth = max(size - margin, vec2(0.0));\n"                                                                                \
//     "  float case1_valid = smoothstep(size.y, size_smooth.y, ptn_local_space.y) * smoothstep(size.x, size_smooth.x, ptn_local_space.x);\n" \
//     "  float case2_valid = case1_valid;\n"                                                                                                 \
//     "  float length_smooth = length(ptn_on_proj_vec);\n"                                                                                   \
//     "  float case3_valid = smoothstep(length_smooth, length_smooth + margin,uMask_radius[quadrant]);\n"                                    \
//     "  valid *= mix(\n"                                                                                                                    \
//     "    mix(case3_valid, case2_valid, in_y_region * (1.0 - in_x_region)),\n"                                                              \
//     "    case1_valid,\n"                                                                                                                   \
//     "    in_x_region\n"                                                                                                                    \
//     "  );\n"                                                                                                                               \
//     "  return valid;\n"                                                                                                                    \
//     "}\n"
//         // with branch implementation
//         // "  if (ptn_local_space.x < corner_abs_space.x) {\n" \
//         // "    valid *= step(ptn_local_space.y, size.y);\n" \
//         // "  } else if (ptn_local_space.y < corner_abs_space.y) {\n" \
//         // "    valid *= step(ptn_local_space.x, size.x);\n" \
//         // "  } else {\n" \
//         // "    valid *= step(length(ptn_on_proj_vec), uMask_radius[quadrant]);\n" \
//         // "  }\n" \


        class AddShaderRectangleMask : public DefaultEngineShader
        {
        protected:
            int u_transform_to_mask;
            int u_mask_corner;
            int u_mask_radius;

            MathCore::mat4f uTransformToMask;
            MathCore::vec2<float, MathCore::SIMD_TYPE::NONE> uMask_corner[4];
            MathCore::vec4f uMask_radius;

            void setMaskFromPropertyBag(const AppKit::GLEngine::Utils::ShaderPropertyBag &bag);

            void mask_query_uniform_locations_and_set_default_values();

            Utils::ShaderPropertyBag mask_default_bag() const;

        public:
            // friend class AppKit::GLEngine::Components::ComponentRectangle;

            void setMask_ScreenToLocalTransform(const MathCore::mat4f &uTransformToMask);
            void setMask_Corner_Centers(const MathCore::vec2f uMask_corner[4]);
            void setMask_Corner_Radius(const MathCore::vec4f &uMask_radius);
        };

    }
}
