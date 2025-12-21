// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include <appkit-gl-engine/Components/2d/ComponentRectangle.h>
// #include "../../shaders/ShaderUnlitVertexColorWithMask.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>
#include "RectangleCorner.h"

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
                    if (mask != nullptr && camera != nullptr)
                    {
                        auto new_material = resourceMap->mask_query_or_create_rectangle(camera, mask);
                        material = transform->addComponent(new_material);
                    }
                    else
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
                {
                    last_local_box.makeEmpty();
                    return;
                }

                getTransform()->skip_traversing = false;

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
                               StrokeModeNone,
                               0.0f, // stroke offset
                               segment_count,
                               stroke_thickness,
                               DrawStroke_Stroke);

                if (drop_shadow_thickness > 0.0f && drop_shadow_color.a > 0.0f)
                {
                    // float offset = stroke_thickness * 0.5f;
                    // if (stroke_mode == StrokeModeGrowOutside)
                    //     offset = stroke_thickness;
                    // else if (stroke_mode == StrokeModeGrowInside)
                    //     offset = 0.0f;

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
                               stroke_mode,
                               0.0f, // stroke offset
                               segment_count,
                               stroke_thickness,
                               DrawStroke_DropShadow_Internal);

                    drawStroke(size,
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, _40_percent), // internal color
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.0f),        // external color
                               radius,
                               StrokeModeGrowOutside,
                               drop_shadow_thickness_60_percent,
                               stroke_mode,
                               drop_shadow_thickness_40_percent, // stroke offset
                               segment_count,
                               stroke_thickness,
                               DrawStroke_DropShadow_External);
                }

                auto size_half = size * 0.5f;
                last_local_box = CollisionCore::AABB<MathCore::vec3f>(
                    -MathCore::vec3f(size_half, 0.0f),
                    MathCore::vec3f(size_half, 0.0f));

                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        -size_half,
                        size_half),
                    true);
            }

            const CollisionCore::AABB<MathCore::vec3f> &ComponentRectangle::getLastLocalBox() const
            {
                return last_local_box;
            }

            void ComponentRectangle::clearMesh()
            {
                mesh->pos.clear();
                mesh->color[0].clear();
                // mesh->uv[0].clear();
                mesh->indices.clear();

                idx_start_inside =
                    idx_start_stroke_external =
                        idx_start_stroke_internal =
                            idx_start_drop_shadow_min_external =
                                idx_start_drop_shadow_min_internal =
                                    idx_start_drop_shadow_max_external =
                                        idx_start_drop_shadow_max_internal =
                                            idx_end = 0;

                precomputeMaskParameters(
                    MathCore::vec2f(0),
                    MathCore::vec4f(0),
                    StrokeModeGrowInside,
                    0);

                getTransform()->skip_traversing = true;

                last_local_box.makeEmpty();
            }

            void ComponentRectangle::precomputeMaskParameters(const MathCore::vec2f &size,
                                                              const MathCore::vec4f &radius,
                                                              StrokeModeEnum stroke_mode,
                                                              float ignore_stroke_thickness)
            {
                RectangleCorner corner;
                corner.setSize(size);

                MathCore::vec2f center_internal[Order_Count];
                MathCore::vec4f radius_internal;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-ignore_stroke_thickness);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(0);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowMiddle)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-ignore_stroke_thickness * 0.5f);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }

                for (int i = 0; i < Order_Count; i++)
                {
                    mask_corner[i] = center_internal[i];
                    mask_radius[i] = radius_internal[i];
                }
            }

            void ComponentRectangle::drawInside(const MathCore::vec2f &size,
                                                const MathCore::vec4f &color_internal,
                                                const MathCore::vec4f &color_external,
                                                const MathCore::vec4f &radius_,
                                                StrokeModeEnum stroke_mode,
                                                float ignore_stroke_thickness,
                                                uint32_t segment_count_ref)
            {

                MathCore::vec4f radius = radius_;
                MathCore::vec4u radius_segment_count_i = RectangleCorner::computeSegmentCounts(radius + ignore_stroke_thickness, segment_count_ref);

                RectangleCorner corner;
                corner.setSize(size);

                MathCore::vec2f center_internal[Order_Count];
                MathCore::vec4f radius_internal;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-ignore_stroke_thickness);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(0);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowMiddle)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-ignore_stroke_thickness * 0.5f);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];
                }

                auto genArc = [](std::vector<MathCore::vec3f> *polygon,
                                 const MathCore::vec2f &center,
                                 float angle_start_deg, float angle_end_deg,
                                 float radius_min,
                                 uint32_t segment_count,
                                 MathCore::vec3f *centroid)
                {
                    polygon->push_back(MathCore::vec3f(center, 0.0f));

                    float angle_start_rad = MathCore::OP<float>::deg_2_rad(angle_start_deg);
                    float angle_end_rad = MathCore::OP<float>::deg_2_rad(angle_end_deg);

                    for (uint32_t i = 0; i <= segment_count; i++)
                    {
                        float lrp = (float)i / (float)(segment_count ? segment_count : 1);
                        float angl = MathCore::OP<float>::lerp(angle_start_rad, angle_end_rad, lrp);
                        MathCore::vec2f pos = MathCore::vec2f(MathCore::OP<float>::cos(angl), MathCore::OP<float>::sin(angl)) * radius_min + center;
                        auto item = MathCore::vec3f(pos, 0.0f);
                        *centroid += item;
                        polygon->push_back(item);
                    }
                };

                std::vector<MathCore::vec3f> polygon;

                int centroid_places[Order_Count];
                int total_centroid_elements = 0;
                MathCore::vec3f centroid;

                for (int i = 0; i < Order_Count; i++)
                {
                    centroid_places[i] = (int)polygon.size();
                    total_centroid_elements += radius_segment_count_i[i] + 1;
                    genArc(&polygon,
                           center_internal[i],
                           corner.start_deg[i], corner.end_deg[i],
                           radius_internal[i], radius_segment_count_i[i],
                           &centroid);
                }

                centroid /= total_centroid_elements;
                for (int i = 0; i < Order_Count; i++)
                    polygon[centroid_places[i]] = 0;

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
                idx_start_stroke_external =
                    idx_start_stroke_internal =
                        idx_start_drop_shadow_min_external =
                            idx_start_drop_shadow_min_internal =
                                idx_start_drop_shadow_max_external =
                                    idx_start_drop_shadow_max_internal =
                                        idx_end = mesh->color[0].size();

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
                                                const MathCore::vec4f &radius_,
                                                StrokeModeEnum stroke_mode,
                                                float stroke_thickness,
                                                StrokeModeEnum base_offset_stroke_mode,
                                                float stroke_drop_shadow_offset,
                                                uint32_t segment_count_ref,
                                                float outside_stroke_thickness_ref,
                                                DrawStrokeEnum drawStrokeMode)
            {
                MathCore::vec4f radius = radius_;
                MathCore::vec4u radius_segment_count_i = RectangleCorner::computeSegmentCounts(radius + outside_stroke_thickness_ref, segment_count_ref);

                RectangleCorner corner;
                corner.setSize(size);

                MathCore::vec2f center_internal[Order_Count];
                MathCore::vec4f radius_internal;

                MathCore::vec2f center_external[Order_Count];
                MathCore::vec4f radius_external;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-stroke_thickness);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];

                    corner.setRadius(radius * corner.output_radius_factor);
                    corner.applyOffset(0);
                    radius_external = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_external[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    float offset = 0;

                    if (drawStrokeMode == DrawStroke_DropShadow_Internal)
                    {
                        if (base_offset_stroke_mode == StrokeModeGrowInside)
                        {

                            corner.setRadius(radius);
                            corner.applyOffset(-outside_stroke_thickness_ref);
                            // corner.setRadius(radius *= corner.output_radius_factor);
                            // corner.applyOffset(0);
                            radius *= corner.output_radius_factor;
                            offset = 0;
                        }
                        else if (base_offset_stroke_mode == StrokeModeGrowOutside)
                        {
                            corner.setRadius(radius);
                            corner.applyOffset(0);
                            radius *= corner.output_radius_factor;
                            offset = outside_stroke_thickness_ref;
                        }
                        if (base_offset_stroke_mode == StrokeModeGrowMiddle)
                        {

                            corner.setRadius(radius);
                            corner.applyOffset(-outside_stroke_thickness_ref * 0.5f);
                            // corner.setRadius(radius *= corner.output_radius_factor);
                            // corner.applyOffset(outside_stroke_thickness_ref * 0.5f);
                            radius *= corner.output_radius_factor;
                            offset = outside_stroke_thickness_ref * 0.5f;
                        }
                    }
                    else if (drawStrokeMode == DrawStroke_DropShadow_External)
                    {
                        if (base_offset_stroke_mode == StrokeModeGrowInside)
                        {
                            corner.setRadius(radius);
                            corner.applyOffset(-outside_stroke_thickness_ref);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(0);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            // corner.applyOffset(outside_stroke_thickness_ref);
                            // corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(stroke_drop_shadow_offset);
                            radius *= corner.output_radius_factor;
                            offset = stroke_drop_shadow_offset;
                        }
                        else if (base_offset_stroke_mode == StrokeModeGrowOutside)
                        {
                            corner.setRadius(radius);
                            corner.applyOffset(0);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(outside_stroke_thickness_ref);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(stroke_drop_shadow_offset);
                            radius *= corner.output_radius_factor;
                            offset = outside_stroke_thickness_ref + stroke_drop_shadow_offset;
                        }
                        else if (base_offset_stroke_mode == StrokeModeGrowMiddle)
                        {
                            corner.setRadius(radius);
                            corner.applyOffset(-outside_stroke_thickness_ref * 0.5f);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(outside_stroke_thickness_ref * 0.5f);
                            corner.setRadius(radius *= corner.output_radius_factor);
                            corner.applyOffset(stroke_drop_shadow_offset);
                            radius *= corner.output_radius_factor;
                            offset = outside_stroke_thickness_ref * 0.5f + stroke_drop_shadow_offset;
                        }
                    }

                    corner.setRadius(radius);
                    corner.applyOffset(offset);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];

                    corner.setRadius(radius * corner.output_radius_factor);
                    corner.applyOffset(offset + stroke_thickness);
                    radius_external = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_external[i] = corner.output_center[i];
                }
                else if (stroke_mode == StrokeModeGrowMiddle)
                {
                    corner.setRadius(radius);
                    corner.applyOffset(-stroke_thickness * 0.5f);
                    radius_internal = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_internal[i] = corner.output_center[i];

                    corner.setRadius(radius * corner.output_radius_factor);
                    corner.applyOffset(stroke_thickness * 0.5f);
                    radius_external = corner.output_radius;
                    for (int i = 0; i < Order_Count; i++)
                        center_external[i] = corner.output_center[i];
                }

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

                for (int i = 0; i < Order_Count; i++)
                    genArc(&polygon, &hole,
                           center_internal[i], // center
                           center_external[i], // center_max
                           corner.start_deg[i], corner.end_deg[i],
                           radius_internal[i], radius_external[i],
                           radius_segment_count_i[i]);

                uint32_t vert_start_idx = (uint32_t)mesh->pos.size();

                mesh->pos.insert(mesh->pos.end(), polygon.begin(), polygon.end());
                mesh->color[0].insert(mesh->color[0].end(), polygon.size(), color_external);
                if (drawStrokeMode == DrawStroke_Stroke)
                    idx_start_stroke_internal =
                        idx_start_drop_shadow_min_external =
                            idx_start_drop_shadow_min_internal =
                                idx_start_drop_shadow_max_external =
                                    idx_start_drop_shadow_max_internal =
                                        idx_end = mesh->color[0].size();
                else if (drawStrokeMode == DrawStroke_DropShadow_Internal)
                    idx_start_drop_shadow_min_internal =
                        idx_start_drop_shadow_max_external =
                            idx_start_drop_shadow_max_internal =
                                idx_end = mesh->color[0].size();
                else if (drawStrokeMode == DrawStroke_DropShadow_External)
                    idx_start_drop_shadow_max_internal =
                        idx_end = mesh->color[0].size();

                uint32_t hole_start_idx = (uint32_t)mesh->pos.size();

                mesh->pos.insert(mesh->pos.end(), hole.begin(), hole.end());
                mesh->color[0].insert(mesh->color[0].end(), hole.size(), color_internal);
                if (drawStrokeMode == DrawStroke_Stroke)
                    idx_start_drop_shadow_min_external =
                        idx_start_drop_shadow_min_internal =
                            idx_start_drop_shadow_max_external =
                                idx_start_drop_shadow_max_internal =
                                    idx_end = mesh->color[0].size();
                else if (drawStrokeMode == DrawStroke_DropShadow_Internal)
                    idx_start_drop_shadow_max_external =
                        idx_start_drop_shadow_max_internal =
                            idx_end = mesh->color[0].size();
                else if (drawStrokeMode == DrawStroke_DropShadow_External)
                    idx_end = mesh->color[0].size();

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

            void ComponentRectangle::setColor(
                const MathCore::vec4f &color,
                const MathCore::vec4f &stroke_color,
                const MathCore::vec4f &drop_shadow_color)
            {
                // idx_start_inside,
                // idx_start_stroke_external,
                // idx_start_stroke_internal,
                // idx_start_drop_shadow_min_external,
                // idx_start_drop_shadow_min_internal,
                // idx_start_drop_shadow_max_external,
                // idx_start_drop_shadow_max_internal,
                // idx_end
                for (size_t i = idx_start_inside; i < idx_start_stroke_external; i++)
                    mesh->color[0][i] = color;
                for (size_t i = idx_start_stroke_external; i < idx_start_drop_shadow_min_external; i++)
                    mesh->color[0][i] = stroke_color;

                const float _60_percent = MathCore::CONSTANT<float>::RPHI;
                const float _40_percent = 1.0f - MathCore::CONSTANT<float>::RPHI;
                auto drop_shadow_color_middle = drop_shadow_color * MathCore::vec4f(1, 1, 1, _40_percent);
                auto drop_shadow_color_external = drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.0f);

                for (size_t i = idx_start_drop_shadow_min_external; i < idx_start_drop_shadow_min_internal; i++)
                    mesh->color[0][i] = drop_shadow_color_middle;
                for (size_t i = idx_start_drop_shadow_min_internal; i < idx_start_drop_shadow_max_external; i++)
                    mesh->color[0][i] = drop_shadow_color;

                for (size_t i = idx_start_drop_shadow_max_external; i < idx_start_drop_shadow_max_internal; i++)
                    mesh->color[0][i] = drop_shadow_color_external;
                for (size_t i = idx_start_drop_shadow_max_internal; i < idx_end; i++)
                    mesh->color[0][i] = drop_shadow_color_middle;
            }

            void ComponentRectangle::setLinearColorVertical(
                const MathCore::vec4f &color_bottom,
                const MathCore::vec4f &color_top)
            {
                float min_y = MathCore::FloatTypeInfo<float>::max;
                float max_y = -MathCore::FloatTypeInfo<float>::max;

                for (size_t i = 0; i < mesh->pos.size(); i++)
                {
                    min_y = MathCore::OP<float>::minimum(min_y, mesh->pos[i].y);
                    max_y = MathCore::OP<float>::maximum(max_y, mesh->pos[i].y);
                }

                float delta_y = MathCore::OP<float>::maximum(max_y - min_y, MathCore::EPSILON<float>::low_precision);

                for (size_t i = 0; i < mesh->pos.size(); i++)
                {
                    float lrp = MathCore::OP<float>::clamp((mesh->pos[i].y - min_y) / delta_y, 0, 1);
                    mesh->color[0][i] = MathCore::OP<MathCore::vec4f>::lerp(color_bottom, color_top, lrp);
                }
            }

            void ComponentRectangle::setMask(AppKit::GLEngine::ResourceMap *resourceMap,
                                             std::shared_ptr<ComponentCamera> &camera,
                                             std::shared_ptr<ComponentRectangle> &mask)
            {
                this->camera = camera;
                this->mask = mask;
                auto transform = getTransform();
                if (mask == nullptr)
                    material = transform->replaceComponent<ComponentMaterial>(material, resourceMap->defaultUnlitVertexColorAlphaMaterial);
                else
                {
                    // auto new_material = Component::CreateShared<Components::ComponentMaterial>();
                    // new_material->always_clone = true;
                    // new_material->setShader(resourceMap->shaderUnlitVertexColorWithMask);
                    // new_material->property_bag.getProperty("BlendMode").set<int>((int)AppKit::GLEngine::BlendModeAlpha);
                    // new_material->property_bag.getProperty("ComponentRectangle").set<std::weak_ptr<Component>>(mask);
                    // new_material->property_bag.getProperty("ComponentCamera").set<std::weak_ptr<Component>>(camera);
                    auto new_material = resourceMap->mask_query_or_create_rectangle(camera, mask);
                    material = transform->replaceComponent<ComponentMaterial>(material, new_material);
                }
            }

            ComponentRectangle::ComponentRectangle() : Component(ComponentRectangle::Type)
            {
                always_clone = true;

                idx_start_inside =
                    idx_start_stroke_external =
                        idx_start_stroke_internal =
                            idx_start_drop_shadow_min_external =
                                idx_start_drop_shadow_min_internal =
                                    idx_start_drop_shadow_max_external =
                                        idx_start_drop_shadow_max_internal =
                                            idx_end = 0;
            }

            ComponentRectangle::~ComponentRectangle()
            {
            }

            // always clone
            std::shared_ptr<Component> ComponentRectangle::duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentRectangle>();

                result->always_clone = this->always_clone;

                result->material = this->material;
                result->mesh = this->mesh;
                result->meshWrapper = this->meshWrapper;

                result->camera = this->camera;
                result->mask = this->mask;

                for (int i = 0; i < MaskOrder_Count; i++)
                    result->mask_corner[i] = this->mask_corner[i];
                result->mask_radius = this->mask_radius;

                result->last_local_box = this->last_local_box;

                result->idx_start_inside = this->idx_start_inside;
                result->idx_start_stroke_external = this->idx_start_stroke_external;
                result->idx_start_stroke_internal = this->idx_start_stroke_internal;
                result->idx_start_drop_shadow_min_external = this->idx_start_drop_shadow_min_external;
                result->idx_start_drop_shadow_min_internal = this->idx_start_drop_shadow_min_internal;
                result->idx_start_drop_shadow_max_external = this->idx_start_drop_shadow_max_external;
                result->idx_start_drop_shadow_max_internal = this->idx_start_drop_shadow_max_internal;
                result->idx_end = this->idx_end;

                return result;
            }
            void ComponentRectangle::fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
            {
                if (componentMap.find(material) != componentMap.end())
                    material = std::dynamic_pointer_cast<ComponentMaterial>(componentMap[material]);
                if (componentMap.find(mesh) != componentMap.end())
                    mesh = std::dynamic_pointer_cast<ComponentMesh>(componentMap[mesh]);
                if (componentMap.find(meshWrapper) != componentMap.end())
                    meshWrapper = std::dynamic_pointer_cast<ComponentMeshWrapper>(componentMap[meshWrapper]);
                if (componentMap.find(camera) != componentMap.end())
                    camera = std::dynamic_pointer_cast<ComponentCamera>(componentMap[camera]);
                if (componentMap.find(mask) != componentMap.end())
                {
                    mask = std::dynamic_pointer_cast<ComponentRectangle>(componentMap[mask]);
                    if (camera != nullptr)
                    {
                        // auto camera = std::dynamic_pointer_cast<ComponentCamera>(material->property_bag.getProperty("ComponentCamera").get<std::weak_ptr<Component>>().lock());
                        auto new_material = resourceMap->mask_query_or_create_rectangle(camera, mask);
                        // for (auto &entry : mTransform)
                        // {
                        //     auto transform = entry.weak_ptr.lock();
                        //     if (!transform)
                        //     {
                        //         printf("[ComponentRectangle] error, clonning null transform list");
                        //         continue;
                        //     }
                        //     transform->replaceComponent(material, new_material);
                        // }
                        getTransform()->replaceComponent(material, new_material);

                        material = new_material;
                    }
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
