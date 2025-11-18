#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            enum Order
            {
                Order_topRight = 0,
                Order_bottomRight,
                Order_bottomLeft,
                Order_topLeft,
                Order_Count
            };

            class RectangleCorner
            {
            public:
                MathCore::vec2f size;
                MathCore::vec4f radius;

                float start_deg[Order_Count];
                float end_deg[Order_Count];
                MathCore::vec2f rad_factor[Order_Count];
                MathCore::vec2f size_factor[Order_Count];

                MathCore::vec2f output_center[Order_Count];
                MathCore::vec4f output_radius;
                MathCore::vec4f output_radius_factor;

                MathCore::vec2f output_size;

                RectangleCorner()
                {

                    const float start_deg_[Order_Count] = {90, 0, -90, -180};
                    const float end_deg_[Order_Count] = {0, -90, -180, -270};
                    const MathCore::vec2f rad_factor_[Order_Count] = {MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1), MathCore::vec2f(1, 1), MathCore::vec2f(1, -1)};
                    const MathCore::vec2f size_factor_[Order_Count] = {MathCore::vec2f(1, 1), MathCore::vec2f(1, -1), MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1)};

                    for (int i = 0; i < Order_Count; i++)
                    {
                        start_deg[i] = start_deg_[i];
                        end_deg[i] = end_deg_[i];
                        rad_factor[i] = rad_factor_[i];
                        size_factor[i] = size_factor_[i];
                    }
                }

                void setSize(const MathCore::vec2f &size)
                {
                    this->size = size;
                }

                void setRadius(const MathCore::vec4f &radius)
                {
                    this->radius = radius;
                }

                void applyOffset(float stroke_offset)
                {
                    output_size = size + stroke_offset * 2.0f;
                    output_size = MathCore::OP<MathCore::vec2f>::maximum(0.0f, output_size);

                    output_radius = radius + stroke_offset;
                    output_radius = MathCore::OP<MathCore::vec4f>::maximum(0.0f, output_radius);

                    float rad_r_y_factor = (output_size.height) / (output_radius[Order_topRight] + output_radius[Order_bottomRight]);
                    float rad_b_x_factor = (output_size.width) / (output_radius[Order_bottomRight] + output_radius[Order_bottomLeft]);
                    float rad_l_y_factor = (output_size.height) / (output_radius[Order_bottomLeft] + output_radius[Order_topLeft]);
                    float rad_t_x_factor = (output_size.width) / (output_radius[Order_topLeft] + output_radius[Order_topRight]);

                    
                    output_radius_factor[Order_topRight] = MathCore::OP<float>::minimum(rad_t_x_factor, rad_r_y_factor);
                    output_radius_factor[Order_bottomRight] = MathCore::OP<float>::minimum(rad_r_y_factor, rad_b_x_factor);
                    output_radius_factor[Order_bottomLeft] = MathCore::OP<float>::minimum(rad_b_x_factor, rad_l_y_factor);
                    output_radius_factor[Order_topLeft] = MathCore::OP<float>::minimum(rad_l_y_factor, rad_t_x_factor);

                    output_radius_factor = MathCore::OP<MathCore::vec4f>::clamp(output_radius_factor, 0.0f, 1.0f);

                    output_radius = output_radius_factor * output_radius;

                    auto size_half = output_size * 0.5f;
                    for (int i = 0; i < Order_Count; i++) {
                        if (radius[i] <= -0.5f) {
                            output_radius[i] = 0.0f;
                            output_radius_factor[i] = 1.0f;
                        }
                        output_center[i] = size_half * size_factor[i] - output_radius[i] * size_factor[i];
                    }
                }

                static MathCore::vec4u computeSegmentCounts(const MathCore::vec4f &radius, uint32_t segment_count_ref, float base_radius = 64.0f)
                {
                    float segment_factor = (float)segment_count_ref / base_radius;
                    MathCore::vec4f radius_segment_count_f = radius * segment_factor;
                    radius_segment_count_f = MathCore::OP<MathCore::vec4f>::ceil(radius_segment_count_f) + 0.5f;
                    MathCore::vec4u radius_segment_count_i = (MathCore::vec4u)radius_segment_count_f;
                    for (int i = 0; i < 4; i++)
                        if (radius_segment_count_i[i] == 0 && radius[i] == 0.0f)
                            radius_segment_count_i[i] =  (int)(32.0f * segment_factor + 0.5f);// segment_count_ref / 2;
                    return radius_segment_count_i;
                }
            };

        }
    }
}