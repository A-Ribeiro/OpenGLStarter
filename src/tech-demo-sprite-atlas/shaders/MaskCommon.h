#pragma once

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
    "  vec2 ptn_step = step(vec2(0.0), ptn_local_space);\n"                                                                       \
    "  int quadrant = int((1.0 - ptn_step.x) * ( 2.0 + ptn_step.y ) + ptn_step.x * (1.0 - ptn_step.y));\n"                        \
    "  vec2 quadrant_sign = sign(ptn_step - 0.5);\n"                                                                              \
    "  ptn_local_space *= quadrant_sign;\n"                                                                                       \
    "  vec2 corner_abs_space = abs(uMask_corner[quadrant]);\n"                                                                    \
    "  vec2 size = corner_abs_space + vec2(uMask_radius[quadrant]);\n"                                                            \
    "  vec2 ptn_on_proj_vec = ptn_local_space - corner_abs_space;\n"                                                              \
    "  float valid = 1.0;\n" /* Branchless version */                                                                             \
    "  float in_x_region = step(ptn_local_space.x, corner_abs_space.x);\n"                                                        \
    "  float in_y_region = step(ptn_local_space.y, corner_abs_space.y);\n"                                                        \
    "  float in_corner_region = (1.0 - in_x_region) * (1.0 - in_y_region);\n"                                                     \
    "  float case1_valid = step(ptn_local_space.y, size.y);\n"                                                                    \
    "  float case2_valid = step(ptn_local_space.x, size.x);\n"                                                                    \
    "  float case3_valid = step(length(ptn_on_proj_vec), uMask_radius[quadrant]);\n"                                              \
    "  valid *= mix(\n"                                                                                                           \
    "    mix(case3_valid, case2_valid, in_y_region * (1.0 - in_x_region)),\n"                                                     \
    "    case1_valid,\n"                                                                                                          \
    "    in_x_region\n"                                                                                                           \
    "  );\n"                                                                                                                      \
    "  return valid;\n"                                                                                                           \
    "}\n"

#define MASKSHADER_COMPUTE_MASK_FUNCTION_SMOOTH                                                                                            \
    "float compute_mask(float margin){\n"                                                                                                  \
    "  vec2 ptn_local_space = (uTransformToMask * vec4(gl_FragCoord.x, gl_FragCoord.y, (gl_FragCoord.z - 0.5) * 2.0, 1.0)).xy;\n"          \
    "  vec2 ptn_step = step(vec2(0.0), ptn_local_space);\n"                                                                                \
    "  int quadrant = int((1.0 - ptn_step.x) * ( 2.0 + ptn_step.y ) + ptn_step.x * (1.0 - ptn_step.y));\n"                                 \
    "  vec2 quadrant_sign = sign(ptn_step - 0.5);\n"                                                                                       \
    "  ptn_local_space *= quadrant_sign;\n"                                                                                                \
    "  vec2 corner_abs_space = abs(uMask_corner[quadrant]);\n"                                                                             \
    "  vec2 size = corner_abs_space + vec2(uMask_radius[quadrant]);\n"                                                                     \
    "  vec2 ptn_on_proj_vec = ptn_local_space - corner_abs_space;\n"                                                                       \
    "  float valid = 1.0;\n" /* Branchless version */                                                                                      \
    "  float in_x_region = step(ptn_local_space.x, corner_abs_space.x);\n"                                                                 \
    "  float in_y_region = step(ptn_local_space.y, corner_abs_space.y);\n"                                                                 \
    "  float in_corner_region = (1.0 - in_x_region) * (1.0 - in_y_region);\n"                                                              \
    "  vec2 size_smooth = max(size - margin, vec2(0.0));\n"                                                                                \
    "  float case1_valid = smoothstep(size.y, size_smooth.y, ptn_local_space.y) * smoothstep(size.x, size_smooth.x, ptn_local_space.x);\n" \
    "  float case2_valid = case1_valid;\n"                                                                                                 \
    "  float length_smooth = length(ptn_on_proj_vec);\n"                                                                                   \
    "  float case3_valid = smoothstep(length_smooth, length_smooth + margin,uMask_radius[quadrant]);\n"                                      \
    "  valid *= mix(\n"                                                                                                                    \
    "    mix(case3_valid, case2_valid, in_y_region * (1.0 - in_x_region)),\n"                                                              \
    "    case1_valid,\n"                                                                                                                   \
    "    in_x_region\n"                                                                                                                    \
    "  );\n"                                                                                                                               \
    "  return valid;\n"                                                                                                                    \
    "}\n"
        // with branch implementation
        // "  if (ptn_local_space.x < corner_abs_space.x) {\n" \
        // "    valid *= step(ptn_local_space.y, size.y);\n" \
        // "  } else if (ptn_local_space.y < corner_abs_space.y) {\n" \
        // "    valid *= step(ptn_local_space.x, size.x);\n" \
        // "  } else {\n" \
        // "    valid *= step(length(ptn_on_proj_vec), uMask_radius[quadrant]);\n" \
        // "  }\n" \

    }
}
