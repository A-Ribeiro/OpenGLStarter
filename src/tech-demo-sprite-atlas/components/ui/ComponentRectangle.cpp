// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentRectangle.h"
#include "../../shaders/ShaderUnlitVertexColorWithMask.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentRectangle::Type = "ComponentRectangle";

            void ComponentRectangle::checkOrCreateAuxiliaryComponents(
                AppKit::GLEngine::ResourceMap *resourceMap)
            {

                if (material != nullptr || mesh != nullptr || meshWrapper != nullptr)
                    return;
                auto transform = getTransform();

                if (material == nullptr)
                {
                    material = transform->addComponent(resourceMap->defaultUnlitVertexColorAlphaMaterial);
                }

                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_COLOR0;
                    mesh->always_clone = true;
                }

                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
                    // meshWrapper->always_clone = true;
                }
            }

            // void ComponentRectangle::setQuadFromMinMax(
            //     AppKit::GLEngine::ResourceMap *resourceMap,
            //     const MathCore::vec2f &min,
            //     const MathCore::vec2f &max,
            //     const MathCore::vec4f &color,
            //     const MathCore::vec4f &radius,
            //     StrokeModeEnum stroke_mode,
            //     float ignore_stroke_thickness,
            //     uint32_t segment_count)
            // {
            //     setQuadFromCenterSize(
            //         resourceMap,
            //         (max + min) * 0.5f,
            //         max - min,
            //         color,
            //         radius,
            //         stroke_mode,
            //         ignore_stroke_thickness,
            //         segment_count);
            // }

            void ComponentRectangle::setQuad(
                AppKit::GLEngine::ResourceMap *resourceMap,
                // const MathCore::vec2f &center,
                const MathCore::vec2f &size,
                const MathCore::vec4f &color,
                const MathCore::vec4f &radius,
                StrokeModeEnum stroke_mode,
                float stroke_thickness,
                const MathCore::vec4f &stroke_color,
                float drop_shadow_thickness,
                const MathCore::vec4f &drop_shadow_color,
                MeshUploadMode meshUploadMode,
                uint32_t segment_count)
            {
                checkOrCreateAuxiliaryComponents(resourceMap);
                bool onCloneNoModify =
                    meshUploadMode == MeshUploadMode_Direct_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Dynamic_OnClone_NoModify ||
                    meshUploadMode == MeshUploadMode_Static_OnClone_NoModify;

                mesh->always_clone = !onCloneNoModify;
                this->always_clone = !onCloneNoModify;

                // uint32_t segment_count = 3;
                // float ignore_stroke_thickness = 0.0f;
                // StrokeModeEnum stroke_mode = StrokeModeNone;

                clearMesh();

                if (size.width <= 0.0f || size.height <= 0.0f)
                    return;

                precomputeMaskParameters(
                    size,
                    radius,
                    stroke_mode,
                    stroke_thickness);

                if (color.a > 0.0f)
                    drawInside(size,
                               color, // internal color
                               color, // external color
                               radius,
                               stroke_mode,
                               stroke_thickness,
                               segment_count);

                if (stroke_thickness > 0.0f && stroke_color.a > 0.0f)
                    drawStroke(size,
                               stroke_color, // internal color
                               stroke_color, // external color
                               radius,
                               stroke_mode,
                               stroke_thickness,
                               0.0f, // stroke offset
                               segment_count);

                if (drop_shadow_thickness > 0.0f && drop_shadow_color.a > 0.0f)
                {
                    float offset = stroke_thickness * 0.5f;
                    if (stroke_mode == StrokeModeGrowOutside)
                        offset = stroke_thickness;
                    else if (stroke_mode == StrokeModeGrowInside)
                        offset = 0.0f;

                    // drawStroke(center,
                    //            size,
                    //            drop_shadow_color,                                 // internal color
                    //            drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.0), // external color
                    //            radius,
                    //            StrokeModeGrowOutside,
                    //            drop_shadow_thickness,
                    //            offset, // stroke offset
                    //            segment_count);

                    const float _60_percent = MathCore::CONSTANT<float>::RPHI;
                    const float _40_percent = 1.0f - MathCore::CONSTANT<float>::RPHI;
                    float drop_shadow_thickness_40_percent = drop_shadow_thickness * _40_percent;
                    float drop_shadow_thickness_60_percent = drop_shadow_thickness - drop_shadow_thickness_40_percent;
                    // 1.0 30% -> 0.5      0.0
                    // |      x             |

                    // drop shadow test
                    drawStroke(size,
                               drop_shadow_color,                                         // internal color
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, _40_percent), // external color
                               radius,
                               StrokeModeGrowOutside,
                               drop_shadow_thickness_40_percent,
                               offset, // stroke offset
                               segment_count);

                    drawStroke(size,
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, _40_percent), // internal color
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.0f),        // external color
                               radius,
                               StrokeModeGrowOutside,
                               drop_shadow_thickness_60_percent,
                               offset + drop_shadow_thickness_40_percent, // stroke offset
                               segment_count);
                }

                auto size_half = size * 0.5f;
                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        -size_half,
                        size_half),
                    true);
            }

            void ComponentRectangle::clearMesh()
            {
                mesh->pos.clear();
                mesh->color[0].clear();
                // mesh->uv[0].clear();
                mesh->indices.clear();
            }

            void ComponentRectangle::precomputeMaskParameters(const MathCore::vec2f &size,
                                                              const MathCore::vec4f &radius,
                                                              StrokeModeEnum stroke_mode,
                                                              float ignore_stroke_thickness)
            {
                enum Order
                {
                    Order_topRight = 0,
                    Order_bottomRight,
                    Order_bottomLeft,
                    Order_topLeft,
                    Order_Count
                };

                auto size_half = size * 0.5f;

                float min_size_half = MathCore::OP<MathCore::vec2f>::minimum(size_half);

                // MathCore::vec4f radius_aux = MathCore::OP<MathCore::vec4f>::minimum(radius, min_size_half);
                MathCore::vec4f radius_aux = MathCore::OP<MathCore::vec4f>::clamp(radius, 0, min_size_half);

                float min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, ignore_stroke_thickness * 0.5f);

                MathCore::vec4f radius_min = radius_aux - min_ignore_stroke_subtract;
                MathCore::vec4f radius_max = radius_aux + ignore_stroke_thickness * 0.5f;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, ignore_stroke_thickness);
                    radius_max = radius_aux;
                    radius_min = radius_aux - min_ignore_stroke_subtract;
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    radius_max = radius_aux + ignore_stroke_thickness;
                    radius_min = radius_aux;
                }

                radius_min = MathCore::OP<MathCore::vec4f>::minimum(radius_min, min_size_half);
                // avoid negative radius
                radius_min = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_min);

                for (int i = 0; i < Order_Count; i++)
                    if (radius_min[i] < 0.0f)
                    {
                        radius_aux[i] -= radius_min[i];
                        radius_min[i] = 0.0f;
                    }

                // radius_aux = MathCore::OP<MathCore::vec4f>::maximum(0, radius_aux);

                float start_deg[Order_Count] = {90, 0, -90, -180};
                float end_deg[Order_Count] = {0, -90, -180, -270};
                MathCore::vec2f rad_factor[Order_Count] = {MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1), MathCore::vec2f(1, 1), MathCore::vec2f(1, -1)};
                MathCore::vec2f size_factor[Order_Count] = {MathCore::vec2f(1, 1), MathCore::vec2f(1, -1), MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1)};

                for (int i = 0; i < Order_Count; i++)
                {
                    mask_corner[i] = size_half * size_factor[i] + radius_aux[i] * rad_factor[i];
                    mask_radius[i] = radius_min[i];
                }
            }

            void ComponentRectangle::drawInside(const MathCore::vec2f &size,
                                                const MathCore::vec4f &color_internal,
                                                const MathCore::vec4f &color_external,
                                                const MathCore::vec4f &radius,
                                                StrokeModeEnum stroke_mode,
                                                float ignore_stroke_thickness,
                                                uint32_t segment_count_ref)
            {
                enum Order
                {
                    Order_topRight = 0,
                    Order_bottomRight,
                    Order_bottomLeft,
                    Order_topLeft,
                    Order_Count
                };

                // const float perimeter_90 = MathCore::OP<float>::deg_2_rad(90);
                const float base_radius = 64.0f;
                float segment_factor = (float)segment_count_ref / base_radius;

                MathCore::vec4f radius_segment_count_f = radius * segment_factor;
                radius_segment_count_f = MathCore::OP<MathCore::vec4f>::ceil(radius_segment_count_f) + 0.5f;
                MathCore::vec4u radius_segment_count_i = (MathCore::vec4u)radius_segment_count_f;
                // radius_segment_count_i = MathCore::OP<MathCore::vec4u>::maximum(radius_segment_count_i, segment_count_ref / 2);
                // for (int i = 0; i < 4; i++)
                //     if (radius_segment_count_i[i] == 0 && radius[i] == 0.0f)
                //         radius_segment_count_i[i] = segment_count_ref / 2;

                // uint32_t segment_count = MathCore::OP<MathCore::vec4u>::maximum(radius_segment_count_i);

                auto size_half = size * 0.5f;

                float min_size_half = MathCore::OP<MathCore::vec2f>::minimum(size_half);

                // MathCore::vec4f radius_aux = MathCore::OP<MathCore::vec4f>::minimum(radius, min_size_half);
                MathCore::vec4f radius_aux = MathCore::OP<MathCore::vec4f>::clamp(radius, 0, min_size_half);

                float min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, ignore_stroke_thickness * 0.5f);

                MathCore::vec4f radius_min = radius_aux - min_ignore_stroke_subtract;
                MathCore::vec4f radius_max = radius_aux + ignore_stroke_thickness * 0.5f;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, ignore_stroke_thickness);
                    radius_max = radius_aux;
                    radius_min = radius_aux - min_ignore_stroke_subtract;
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    radius_max = radius_aux + ignore_stroke_thickness;
                    radius_min = radius_aux;
                }

                radius_min = MathCore::OP<MathCore::vec4f>::minimum(radius_min, min_size_half);
                // avoid negative radius
                radius_min = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_min);

                for (int i = 0; i < Order_Count; i++)
                    if (radius_min[i] < 0.0f)
                    {
                        radius_aux[i] -= radius_min[i];
                        radius_min[i] = 0.0f;
                    }

                // radius_aux = MathCore::OP<MathCore::vec4f>::maximum(0, radius_aux);

                auto genArc = [](std::vector<MathCore::vec3f> *polygon,
                                 const MathCore::vec2f &center,
                                 float angle_start_deg, float angle_end_deg,
                                 float radius_min,
                                 uint32_t segment_count)
                {
                    polygon->push_back(MathCore::vec3f(center, 0.0f));

                    float angle_start_rad = MathCore::OP<float>::deg_2_rad(angle_start_deg);
                    float angle_end_rad = MathCore::OP<float>::deg_2_rad(angle_end_deg);

                    for (uint32_t i = 0; i <= segment_count; i++)
                    {
                        float lrp = (float)i / (float)(segment_count ? segment_count : 1);
                        float angl = MathCore::OP<float>::lerp(angle_start_rad, angle_end_rad, lrp);
                        MathCore::vec2f pos = MathCore::vec2f(MathCore::OP<float>::cos(angl), MathCore::OP<float>::sin(angl)) * radius_min + center;
                        polygon->push_back(MathCore::vec3f(pos, 0.0f));
                    }
                };

                std::vector<MathCore::vec3f> polygon;
                float start_deg[Order_Count] = {90, 0, -90, -180};
                float end_deg[Order_Count] = {0, -90, -180, -270};
                MathCore::vec2f rad_factor[Order_Count] = {MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1), MathCore::vec2f(1, 1), MathCore::vec2f(1, -1)};
                MathCore::vec2f size_factor[Order_Count] = {MathCore::vec2f(1, 1), MathCore::vec2f(1, -1), MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1)};

                for (int i = 0; i < Order_Count; i++)
                    genArc(&polygon, size_half * size_factor[i] + radius_aux[i] * rad_factor[i], start_deg[i], end_deg[i], radius_min[i], radius_segment_count_i[i]);

                // genArc(&polygon, MathCore::vec2f(size_half.x - radius_aux, size_half.y - radius_aux), 90, 0, radius_min, segment_count);
                // genArc(&polygon, MathCore::vec2f(size_half.x - radius_aux, -size_half.y + radius_aux), 0, -90, radius_min, segment_count);
                // genArc(&polygon, MathCore::vec2f(-size_half.x + radius_aux, -size_half.y + radius_aux), -90, -180, radius_min, segment_count);
                // genArc(&polygon, MathCore::vec2f(-size_half.x + radius_aux, size_half.y - radius_aux), -180, -270, radius_min, segment_count);

                uint32_t total_verts_per_quadrant[Order_Count] = {
                    (radius_segment_count_i[Order_topRight] + 1) + 1,
                    (radius_segment_count_i[Order_bottomRight] + 1) + 1,
                    (radius_segment_count_i[Order_bottomLeft] + 1) + 1,
                    (radius_segment_count_i[Order_topLeft] + 1) + 1,
                };

                uint32_t vert_start_position = (uint32_t)mesh->pos.size();
                mesh->pos.insert(mesh->pos.end(), polygon.begin(), polygon.end());
                mesh->color[0].insert(mesh->color[0].end(), polygon.size(), color_external);
                // mesh->uv[0].insert(mesh->uv[0].end(), polygon.size(), MathCore::vec3f(0.0f, 1.0f, 0.0f));

                for (size_t i = (size_t)vert_start_position, j = 0; i < mesh->color[0].size(); i += total_verts_per_quadrant[j++])
                    mesh->color[0][i] = color_internal;

                uint32_t vert_count = (uint32_t)polygon.size();

                int quadrant_start_idx[Order_Count];
                quadrant_start_idx[Order_topRight] = 0;
                quadrant_start_idx[Order_bottomRight] = total_verts_per_quadrant[Order_topRight];
                quadrant_start_idx[Order_bottomLeft] = quadrant_start_idx[Order_bottomRight] + total_verts_per_quadrant[Order_bottomRight];
                quadrant_start_idx[Order_topLeft] = quadrant_start_idx[Order_bottomLeft] + total_verts_per_quadrant[Order_bottomLeft];

                for (uint32_t quadrant = 0; quadrant < 4; quadrant++)
                {
                    uint32_t quadrant_idx = quadrant_start_idx[quadrant];
                    uint32_t quadrant_start_vert = quadrant_idx + 1;
                    for (uint32_t i = 0; i < radius_segment_count_i[quadrant]; i++)
                    {
                        uint32_t a = quadrant_start_vert + i;
                        uint32_t b = quadrant_start_vert + i + 1;
                        mesh->indices.insert(mesh->indices.end(), {quadrant_idx + vert_start_position, b + vert_start_position, a + vert_start_position});
                    }

                    // add next quad
                    // last
                    uint32_t a = (quadrant_start_idx[quadrant] + 1 + radius_segment_count_i[quadrant]) % vert_count;
                    // start
                    uint32_t b = (quadrant_start_idx[(quadrant + 1) % Order_Count] + 1) % vert_count;

                    // q2
                    uint32_t c = (quadrant_start_idx[(quadrant + 1) % Order_Count]) % vert_count;
                    // q1
                    uint32_t d = (quadrant_start_idx[quadrant]) % vert_count;

                    // triangle 1
                    mesh->indices.insert(mesh->indices.end(), {b + vert_start_position, d + vert_start_position, c + vert_start_position});
                    // triangle 2
                    mesh->indices.insert(mesh->indices.end(), {a + vert_start_position, d + vert_start_position, b + vert_start_position});
                }

                // add quad center
                // last
                uint32_t a = (quadrant_start_idx[0]) % vert_count;
                // start
                uint32_t b = (quadrant_start_idx[1]) % vert_count;

                // q2
                uint32_t c = (quadrant_start_idx[2]) % vert_count;
                // q1
                uint32_t d = (quadrant_start_idx[3]) % vert_count;

                // triangle 1
                mesh->indices.insert(mesh->indices.end(), {b + vert_start_position, d + vert_start_position, c + vert_start_position});
                // triangle 2
                mesh->indices.insert(mesh->indices.end(), {a + vert_start_position, d + vert_start_position, b + vert_start_position});
            }

            void ComponentRectangle::drawStroke(const MathCore::vec2f &size,
                                                const MathCore::vec4f &color_internal,
                                                const MathCore::vec4f &color_external,
                                                const MathCore::vec4f &radius,
                                                StrokeModeEnum stroke_mode,
                                                float stroke_thickness,
                                                float stroke_offset,
                                                uint32_t segment_count_ref)
            {
                enum Order
                {
                    Order_topRight = 0,
                    Order_bottomRight,
                    Order_bottomLeft,
                    Order_topLeft,
                    Order_Count
                };

                // const float perimeter_90 = MathCore::OP<float>::deg_2_rad(90);
                const float base_radius = 64.0f;
                float segment_factor = (float)segment_count_ref / base_radius;

                MathCore::vec4f radius_segment_count_f = radius * segment_factor;
                radius_segment_count_f = MathCore::OP<MathCore::vec4f>::ceil(radius_segment_count_f) + 0.5f;
                MathCore::vec4u radius_segment_count_i = (MathCore::vec4u)radius_segment_count_f;
                // radius_segment_count_i = MathCore::OP<MathCore::vec4u>::maximum(radius_segment_count_i, segment_count_ref/2);
                for (int i = 0; i < 4; i++)
                    if (radius_segment_count_i[i] == 0 && radius[i] == 0.0f)
                        radius_segment_count_i[i] = segment_count_ref / 2;

                // uint32_t segment_count = MathCore::OP<MathCore::vec4u>::maximum(radius_segment_count_i);

                auto size_half = size * 0.5f;

                float min_size_half = MathCore::OP<MathCore::vec2f>::minimum(size_half);

                MathCore::vec4f radius_aux = MathCore::OP<MathCore::vec4f>::clamp(radius, 0, min_size_half);

                float min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, stroke_thickness * 0.5f);

                MathCore::vec4f radius_min = radius_aux - min_ignore_stroke_subtract;
                MathCore::vec4f radius_max = radius_aux + stroke_thickness * 0.5f;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    min_ignore_stroke_subtract = MathCore::OP<float>::minimum(min_size_half, stroke_thickness);

                    radius_max = radius_aux;
                    radius_min = radius_aux - min_ignore_stroke_subtract;
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    radius_max = radius_aux + stroke_thickness;
                    radius_min = radius_aux;
                }

                radius_min = MathCore::OP<MathCore::vec4f>::minimum(radius_min, min_size_half);

                radius_min += stroke_offset;
                radius_max += stroke_offset;

                // for(int i=0;i<Order_Count;i++)
                //     if (radius_aux[i] < -0.5f)
                //         radius_max = -radius_max;

                // avoid negative radius
                radius_min = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_min);
                radius_max = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_max);

                MathCore::vec4f radius_aux_max = radius_aux;
                for (int i = 0; i < Order_Count; i++)
                {
                    if (radius[i] < -0.5f)
                    {
                        radius_aux[i] -= radius_min[i];
                        radius_aux_max[i] -= radius_max[i];
                        radius_min[i] = 0.0f;
                        radius_max[i] = 0.0f;
                    }
                    else
                    {
                        if (radius_min[i] < 0.0f)
                        {
                            radius_aux[i] -= radius_min[i];
                            radius_min[i] = 0.0f;
                        }
                        if (radius_max[i] < 0.0f)
                        {
                            radius_aux_max[i] -= radius_max[i];
                            radius_max[i] = 0.0f;
                        }
                    }
                }

                // radius_aux = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_aux);
                // radius_aux_max = MathCore::OP<MathCore::vec4f>::maximum(-min_size_half, radius_aux_max);

                auto genArc = [](std::vector<MathCore::vec3f> *polygon,
                                 std::vector<MathCore::vec3f> *hole,
                                 const MathCore::vec2f &center,
                                 const MathCore::vec2f &center_max,
                                 float angle_start_deg, float angle_end_deg,
                                 float radius_min, float radius_max,
                                 uint32_t segment_count)
                {
                    // polygon->push_back(MathCore::vec3f(center, 0.0f));

                    float angle_start_rad = MathCore::OP<float>::deg_2_rad(angle_start_deg);
                    float angle_end_rad = MathCore::OP<float>::deg_2_rad(angle_end_deg);

                    for (uint32_t i = 0; i <= segment_count; i++)
                    {
                        float lrp = (float)i / (float)(segment_count ? segment_count : 1);
                        float angl = MathCore::OP<float>::lerp(angle_start_rad, angle_end_rad, lrp);
                        MathCore::vec2f dir = MathCore::vec2f(MathCore::OP<float>::cos(angl), MathCore::OP<float>::sin(angl));
                        MathCore::vec2f pos = dir * radius_max + center_max;
                        polygon->push_back(MathCore::vec3f(pos, 0.0f));

                        pos = dir * radius_min + center;
                        hole->push_back(MathCore::vec3f(pos, 0.0f));
                    }
                };

                std::vector<MathCore::vec3f> polygon;
                std::vector<MathCore::vec3f> hole;

                float start_deg[Order_Count] = {90, 0, -90, -180};
                float end_deg[Order_Count] = {0, -90, -180, -270};
                MathCore::vec2f rad_factor[Order_Count] = {MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1), MathCore::vec2f(1, 1), MathCore::vec2f(1, -1)};
                MathCore::vec2f size_factor[Order_Count] = {MathCore::vec2f(1, 1), MathCore::vec2f(1, -1), MathCore::vec2f(-1, -1), MathCore::vec2f(-1, 1)};

                for (int i = 0; i < Order_Count; i++)
                    genArc(&polygon, &hole,
                           size_half * size_factor[i] + radius_aux[i] * rad_factor[i],     // center
                           size_half * size_factor[i] + radius_aux_max[i] * rad_factor[i], // center_max
                           start_deg[i], end_deg[i], radius_min[i], radius_max[i], radius_segment_count_i[i]);

                // genArc(&polygon, &hole, MathCore::vec2f(size_half.x - radius_aux, size_half.y - radius_aux), 90, 0, radius_min, radius_max, segment_count);
                // genArc(&polygon, &hole, MathCore::vec2f(size_half.x - radius_aux, -size_half.y + radius_aux), 0, -90, radius_min, radius_max, segment_count);
                // genArc(&polygon, &hole, MathCore::vec2f(-size_half.x + radius_aux, -size_half.y + radius_aux), -90, -180, radius_min, radius_max, segment_count);
                // genArc(&polygon, &hole, MathCore::vec2f(-size_half.x + radius_aux, size_half.y - radius_aux), -180, -270, radius_min, radius_max, segment_count);

                // int total_verts_per_quadrant = (segment_count + 1) + 1;

                uint32_t vert_start_idx = (uint32_t)mesh->pos.size();

                mesh->pos.insert(mesh->pos.end(), polygon.begin(), polygon.end());
                mesh->color[0].insert(mesh->color[0].end(), polygon.size(), color_external);

                uint32_t hole_start_idx = (uint32_t)mesh->pos.size();

                mesh->pos.insert(mesh->pos.end(), hole.begin(), hole.end());
                mesh->color[0].insert(mesh->color[0].end(), hole.size(), color_internal);

                for (uint32_t i = 0; i < (uint32_t)polygon.size(); i++)
                {
                    uint32_t a = vert_start_idx + i;
                    uint32_t b = vert_start_idx + (i + 1) % polygon.size();

                    uint32_t c = hole_start_idx + i;
                    uint32_t d = hole_start_idx + (i + 1) % hole.size();

                    // triangle 1
                    mesh->indices.insert(mesh->indices.end(), {a, c, b});
                    // triangle 2
                    mesh->indices.insert(mesh->indices.end(), {b, c, d});
                }
            }

            void ComponentRectangle::setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                                             std::shared_ptr<ComponentCamera> &camera,
                                             std::shared_ptr<ComponentRectangle> &mask)
            {
                this->mask = mask;
                auto transform = getTransform();
                if (mask == nullptr)
                    material = transform->replaceComponent<ComponentMaterial>(material, resourceMap->defaultUnlitVertexColorAlphaMaterial);
                else
                {
                    auto new_material = Component::CreateShared<Components::ComponentMaterial>();
                    new_material->always_clone = true;
                    new_material->setShader(std::make_shared<AppKit::GLEngine::ShaderUnlitVertexColorWithMask>());
                    new_material->property_bag.getProperty("BlendMode").set<int>((int)AppKit::GLEngine::BlendModeAlpha);
                    new_material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    new_material->property_bag.getProperty("ComponentCamera").set<std::weak_ptr<Component>>(camera);
                    material = transform->replaceComponent<ComponentMaterial>(material, new_material);
                }
            }

            ComponentRectangle::ComponentRectangle() : Component(ComponentRectangle::Type)
            {
                always_clone = true;
            }

            ComponentRectangle::~ComponentRectangle()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentRectangle::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentRectangle>();

                result->always_clone = this->always_clone;

                result->material = this->material;
                result->mesh = this->mesh;
                result->meshWrapper = this->meshWrapper;

                result->mask = this->mask;

                for (int i = 0; i < MaskOrder_Count; i++)
                    result->mask_corner[i] = this->mask_corner[i];
                result->mask_radius = this->mask_radius;

                return result;
            }
            void ComponentRectangle::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                if (componentMap.find(material) != componentMap.end())
                    material = std::dynamic_pointer_cast<ComponentMaterial>(componentMap[material]);
                if (componentMap.find(mesh) != componentMap.end())
                    mesh = std::dynamic_pointer_cast<ComponentMesh>(componentMap[mesh]);
                if (componentMap.find(meshWrapper) != componentMap.end())
                    meshWrapper = std::dynamic_pointer_cast<ComponentMeshWrapper>(componentMap[meshWrapper]);
                if (componentMap.find(mask) != componentMap.end())
                {
                    mask = std::dynamic_pointer_cast<ComponentRectangle>(componentMap[mask]);
                    // material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                }
            }

            void ComponentRectangle::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
            }
            void ComponentRectangle::Deserialize(rapidjson::Value &_value,
                                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                                 ResourceSet &resourceSet)
            {
            }
        }
    }
}
