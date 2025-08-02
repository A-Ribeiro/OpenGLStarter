// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include "ComponentRectangle.h"

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
                }

                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
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

            void ComponentRectangle::setQuadFromCenterSize(
                AppKit::GLEngine::ResourceMap *resourceMap,
                const MathCore::vec2f &center,
                const MathCore::vec2f &size,
                const MathCore::vec4f &color,
                const MathCore::vec4f &radius,
                StrokeModeEnum stroke_mode,
                float stroke_thickness,
                const MathCore::vec4f &stroke_color,
                float drop_shadow_thickness,
                const MathCore::vec4f &drop_shadow_color,
                uint32_t segment_count)
            {
                checkOrCreateAuxiliaryComponents(resourceMap);

                // uint32_t segment_count = 3;
                // float ignore_stroke_thickness = 0.0f;
                // StrokeModeEnum stroke_mode = StrokeModeNone;

                clearMesh();

                if (color.a > 0.0f)
                    drawInside(center,
                               size,
                               color, // internal color
                               color, // external color
                               radius,
                               stroke_mode,
                               stroke_thickness,
                               segment_count);

                if (stroke_thickness > 0.0f && stroke_color.a > 0.0f)
                    drawStroke(center,
                               size,
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
                    drawStroke(center,
                               size,
                               drop_shadow_color,                                  // internal color
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.5f), // external color
                               radius,
                               StrokeModeGrowOutside,
                               drop_shadow_thickness_40_percent,
                               offset, // stroke offset
                               segment_count);

                    drawStroke(center,
                               size,
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.5f), // internal color
                               drop_shadow_color * MathCore::vec4f(1, 1, 1, 0.0f), // external color
                               radius,
                               StrokeModeGrowOutside,
                               drop_shadow_thickness_60_percent,
                               offset + drop_shadow_thickness_40_percent, // stroke offset
                               segment_count);
                }

                auto size_half = size * 0.5f;
                meshWrapper->setShapeAABB(
                    CollisionCore::AABB<MathCore::vec3f>(
                        center - size_half,
                        center + size_half),
                    true);
            }

            void ComponentRectangle::clearMesh()
            {
                mesh->pos.clear();
                mesh->color[0].clear();
                // mesh->uv[0].clear();
                mesh->indices.clear();
            }

            void ComponentRectangle::drawInside(const MathCore::vec2f &center,
                                                const MathCore::vec2f &size,
                                                const MathCore::vec4f &color_internal,
                                                const MathCore::vec4f &color_external,
                                                const MathCore::vec4f &radius,
                                                StrokeModeEnum stroke_mode,
                                                float ignore_stroke_thickness,
                                                uint32_t segment_count)
            {
                auto size_half = size * 0.5f;

                float min_size_half = MathCore::OP<MathCore::vec2f>::minimum(size_half);

                float radius_aux = MathCore::OP<float>::minimum(radius.x, min_size_half);

                float radius_min = radius_aux - ignore_stroke_thickness * 0.5f;
                float radius_max = radius_aux + ignore_stroke_thickness * 0.5f;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    radius_max = radius_aux;
                    radius_min = radius_aux - ignore_stroke_thickness;
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    radius_max = radius_aux + ignore_stroke_thickness;
                    radius_min = radius_aux;
                }

                radius_min = MathCore::OP<float>::minimum(radius_min, min_size_half);
                // avoid negative radius
                radius_min = MathCore::OP<float>::maximum(0, radius_min);

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
                        float lrp = (float)i / (float)segment_count;
                        float angl = MathCore::OP<float>::lerp(angle_start_rad, angle_end_rad, lrp);
                        MathCore::vec2f pos = MathCore::vec2f(MathCore::OP<float>::cos(angl), MathCore::OP<float>::sin(angl)) * radius_min + center;
                        polygon->push_back(MathCore::vec3f(pos, 0.0f));
                    }
                };

                std::vector<MathCore::vec3f> polygon;

                genArc(&polygon, MathCore::vec2f(size_half.x - radius_aux, size_half.y - radius_aux), 90, 0, radius_min, segment_count);
                genArc(&polygon, MathCore::vec2f(size_half.x - radius_aux, -size_half.y + radius_aux), 0, -90, radius_min, segment_count);
                genArc(&polygon, MathCore::vec2f(-size_half.x + radius_aux, -size_half.y + radius_aux), -90, -180, radius_min, segment_count);
                genArc(&polygon, MathCore::vec2f(-size_half.x + radius_aux, size_half.y - radius_aux), -180, -270, radius_min, segment_count);

                int total_verts_per_quadrant = (segment_count + 1) + 1;

                uint32_t vert_start_position = (uint32_t)mesh->pos.size();
                mesh->pos.insert(mesh->pos.end(), polygon.begin(), polygon.end());
                mesh->color[0].insert(mesh->color[0].end(), polygon.size(), color_external);
                // mesh->uv[0].insert(mesh->uv[0].end(), polygon.size(), MathCore::vec3f(0.0f, 1.0f, 0.0f));
                for (size_t i = (size_t)vert_start_position; i < mesh->color[0].size(); i += total_verts_per_quadrant)
                    mesh->color[0][i] = color_internal;

                uint32_t vert_count = (uint32_t)polygon.size();

                for (uint32_t quadrant = 0; quadrant < 4; quadrant++)
                {
                    uint32_t quadrant_idx = total_verts_per_quadrant * quadrant;
                    uint32_t quadrant_start_vert = quadrant_idx + 1;
                    for (uint32_t i = 0; i < segment_count; i++)
                    {
                        uint32_t a = quadrant_start_vert + i;
                        uint32_t b = quadrant_start_vert + i + 1;
                        mesh->indices.insert(mesh->indices.end(), {quadrant_idx + vert_start_position, b + vert_start_position, a + vert_start_position});
                    }

                    // add next quad
                    // last
                    uint32_t a = (total_verts_per_quadrant * quadrant + 1 + segment_count) % vert_count;
                    // start
                    uint32_t b = (total_verts_per_quadrant * (quadrant + 1) + 1) % vert_count;

                    // q2
                    uint32_t c = (total_verts_per_quadrant * (quadrant + 1)) % vert_count;
                    // q1
                    uint32_t d = (total_verts_per_quadrant * (quadrant)) % vert_count;

                    // triangle 1
                    mesh->indices.insert(mesh->indices.end(), {b + vert_start_position, d + vert_start_position, c + vert_start_position});
                    // triangle 2
                    mesh->indices.insert(mesh->indices.end(), {a + vert_start_position, d + vert_start_position, b + vert_start_position});
                }

                // add quad center
                // last
                uint32_t a = (total_verts_per_quadrant * 0) % vert_count;
                // start
                uint32_t b = (total_verts_per_quadrant * 1) % vert_count;

                // q2
                uint32_t c = (total_verts_per_quadrant * 2) % vert_count;
                // q1
                uint32_t d = (total_verts_per_quadrant * 3) % vert_count;

                // triangle 1
                mesh->indices.insert(mesh->indices.end(), {b + vert_start_position, d + vert_start_position, c + vert_start_position});
                // triangle 2
                mesh->indices.insert(mesh->indices.end(), {a + vert_start_position, d + vert_start_position, b + vert_start_position});
            }

            void ComponentRectangle::drawStroke(const MathCore::vec2f &center,
                                                const MathCore::vec2f &size,
                                                const MathCore::vec4f &color_internal,
                                                const MathCore::vec4f &color_external,
                                                const MathCore::vec4f &radius,
                                                StrokeModeEnum stroke_mode,
                                                float stroke_thickness,
                                                float stroke_offset,
                                                uint32_t segment_count)
            {
                auto size_half = size * 0.5f;

                float min_size_half = MathCore::OP<MathCore::vec2f>::minimum(size_half);

                float radius_aux = MathCore::OP<float>::minimum(radius.x, min_size_half);

                float radius_min = radius_aux - stroke_thickness * 0.5f;
                float radius_max = radius_aux + stroke_thickness * 0.5f;

                if (stroke_mode == StrokeModeGrowInside)
                {
                    radius_max = radius_aux;
                    radius_min = radius_aux - stroke_thickness;
                }
                else if (stroke_mode == StrokeModeGrowOutside)
                {
                    radius_max = radius_aux + stroke_thickness;
                    radius_min = radius_aux;
                }

                radius_min = MathCore::OP<float>::minimum(radius_min, min_size_half);

                radius_min += stroke_offset;
                radius_max += stroke_offset;

                // avoid negative radius
                radius_min = MathCore::OP<float>::maximum(0, radius_min);

                auto genArc = [](std::vector<MathCore::vec3f> *polygon,
                                 std::vector<MathCore::vec3f> *hole,
                                 const MathCore::vec2f &center,
                                 float angle_start_deg, float angle_end_deg,
                                 float radius_min, float radius_max,
                                 uint32_t segment_count)
                {
                    // polygon->push_back(MathCore::vec3f(center, 0.0f));

                    float angle_start_rad = MathCore::OP<float>::deg_2_rad(angle_start_deg);
                    float angle_end_rad = MathCore::OP<float>::deg_2_rad(angle_end_deg);

                    for (uint32_t i = 0; i <= segment_count; i++)
                    {
                        float lrp = (float)i / (float)segment_count;
                        float angl = MathCore::OP<float>::lerp(angle_start_rad, angle_end_rad, lrp);
                        MathCore::vec2f dir = MathCore::vec2f(MathCore::OP<float>::cos(angl), MathCore::OP<float>::sin(angl));
                        MathCore::vec2f pos = dir * radius_max + center;
                        polygon->push_back(MathCore::vec3f(pos, 0.0f));

                        pos = dir * radius_min + center;
                        hole->push_back(MathCore::vec3f(pos, 0.0f));
                    }
                };

                std::vector<MathCore::vec3f> polygon;
                std::vector<MathCore::vec3f> hole;

                genArc(&polygon, &hole, MathCore::vec2f(size_half.x - radius_aux, size_half.y - radius_aux), 90, 0, radius_min, radius_max, segment_count);
                genArc(&polygon, &hole, MathCore::vec2f(size_half.x - radius_aux, -size_half.y + radius_aux), 0, -90, radius_min, radius_max, segment_count);
                genArc(&polygon, &hole, MathCore::vec2f(-size_half.x + radius_aux, -size_half.y + radius_aux), -90, -180, radius_min, radius_max, segment_count);
                genArc(&polygon, &hole, MathCore::vec2f(-size_half.x + radius_aux, size_half.y - radius_aux), -180, -270, radius_min, radius_max, segment_count);

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

            ComponentRectangle::ComponentRectangle() : Component(ComponentRectangle::Type)
            {
                always_clone = false;
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

                return result;
            }
            void ComponentRectangle::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
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
