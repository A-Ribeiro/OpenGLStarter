// #include <appkit-gl-engine/Components/ComponentSprite.h>
#include <appkit-gl-engine/Components/Core/ComponentLineMounter.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

using namespace AppKit::GLEngine;

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            const ComponentType ComponentLineMounter::Type = "ComponentLineMounter";

            void ComponentLineMounter::OnCameraTransformVisit(std::shared_ptr<Transform> transform)
            {
                force_refresh();
            }

            void ComponentLineMounter::OnBeforeComputeFinalPositions(ComponentMeshWrapper *meshWrapper)
            {
                if (use_max_scale_update_on_change_self_scale)
                {

                    const auto &world_to_local = getTransform()->getMatrixInverse(true);
                    MathCore::vec3f aux_vec3(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[2])));
                    float new_one_time_word_to_local_scale = MathCore::OP<MathCore::vec3f>::minimum(aux_vec3);

                    // printf("+");fflush(stdout);
                    // check if the AABB is dirty or if the max scaled value changed
                    if (!dirty && MathCore::OP<float>::compare_almost_equal(one_time_word_to_local_scale, new_one_time_word_to_local_scale))
                        return;
                    // printf(".");fflush(stdout);
                    dirty = false;
                    one_time_word_to_local_scale = new_one_time_word_to_local_scale;
                    one_time_set_scale = one_time_word_to_local_scale * one_time_camera_scale;

                    AABBType finalAABB = aabb;

                    for (size_t i = 0; i < mesh->pos.size(); i += 8)
                    {
                        const auto &a = mesh->uv[1][i];
                        const auto &b = mesh->uv[2][i];
                        float thickness = mesh->uv[3][i].y;

                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, one_time_set_scale * thickness));
                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, one_time_set_scale * thickness));
                    }

                    meshWrapper->setShapeAABB(finalAABB, true);
                }
                if (camera == nullptr)
                {
                    return; // no camera set, cannot compute AABB
                }
                else if (camera->compareType(Components::ComponentCameraOrthographic::Type))
                {
                    auto ortho = std::dynamic_pointer_cast<Components::ComponentCameraOrthographic>(camera);
                    MathCore::vec3f camera_px_scale =
                        (MathCore::vec3f(ortho->viewport.w, ortho->viewport.h, -1.0f)) *
                        (MathCore::vec3f(ortho->projection.a1, ortho->projection.b2, 1.0f));
                    // camera_px_scale *= 0.5f;
                    camera_px_scale = 1.0f / camera_px_scale;

                    const auto &world_to_local = getTransform()->getMatrixInverse(true);
                    MathCore::vec3f aux_vec3(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[2])));
                    float word_to_local_scale = MathCore::OP<MathCore::vec3f>::minimum(aux_vec3);

                    float max_scaled = word_to_local_scale * MathCore::OP<MathCore::vec3f>::maximum(camera_px_scale);

                    // check if the AABB is dirty or if the max scaled value changed
                    if (!dirty && MathCore::OP<float>::compare_almost_equal(last_max_scaled, max_scaled))
                        return;

                    dirty = false;
                    last_max_scaled = max_scaled;

                    // printf(".");fflush(stdout);

                    AABBType finalAABB = aabb;

                    for (size_t i = 0; i < mesh->pos.size(); i += 8)
                    {
                        const auto &a = mesh->uv[1][i];
                        const auto &b = mesh->uv[2][i];
                        float thickness = mesh->uv[3][i].y;

                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, max_scaled * thickness));
                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, max_scaled * thickness));
                    }

                    meshWrapper->setShapeAABB(finalAABB, true);
                }
                else if (camera->compareType(Components::ComponentCameraPerspective::Type))
                {
                    auto perspective = std::dynamic_pointer_cast<Components::ComponentCameraPerspective>(camera);

                    auto dir = perspective->getTransform()->getRotation(true) * MathCore::vec3f(0, 0, 1);
                    auto cam_pos = perspective->getTransform()->getPosition(true);
                    float tan_half_fov = MathCore::OP<float>::tan(MathCore::OP<float>::deg_2_rad(perspective->fovDegrees) * 0.5f);
                    float near_plane = perspective->nearPlane;
                    float far_plane = perspective->farPlane;
                    // Pre-calculate the conversion factor: 2 * tan_half_fov / viewport_height
                    // after that multiply by 0.5 to pass as radius to the sphere parameter
                    // final factor: tan_half_fov / viewport_height
                    float tan_over_viewport_height = tan_half_fov / (float)perspective->viewport.h;

                    const auto &local_to_world = getTransform()->getMatrix(true);

                    // check if the AABB is dirty or if the max scaled value changed
                    if (!dirty &&
                        last_dir == dir &&
                        last_cam_pos == cam_pos &&
                        MathCore::OP<float>::compare_almost_equal(last_near_plane, near_plane) &&
                        MathCore::OP<float>::compare_almost_equal(last_far_plane, far_plane) &&
                        MathCore::OP<float>::compare_almost_equal(last_tan_over_viewport_height, tan_over_viewport_height) &&
                        last_local_to_world == local_to_world)
                        return;

                    dirty = false;
                    last_dir = dir;
                    last_cam_pos = cam_pos;
                    last_near_plane = near_plane;
                    last_far_plane = far_plane;
                    last_tan_over_viewport_height = tan_over_viewport_height;
                    last_local_to_world = local_to_world;

                    // printf(".");fflush(stdout);

                    const auto &world_to_local = getTransform()->getMatrixInverse(true);

                    // Calculate the uniform scale factor for converting world space thickness to local space
                    MathCore::vec3f scale_vec(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[2])));
                    float world_to_local_scale = MathCore::OP<MathCore::vec3f>::minimum(scale_vec);

                    AABBType finalAABB = aabb;

                    for (size_t i = 0; i < mesh->pos.size(); i += 8)
                    {
                        const auto &a = mesh->uv[1][i];
                        const auto &b = mesh->uv[2][i];
                        const float &thickness_pixels = mesh->uv[3][i].y;

                        float thickness_factor = thickness_pixels * tan_over_viewport_height;

                        // Transform line endpoints to world space
                        auto world_a = MathCore::CVT<MathCore::vec4f>::toVec3(local_to_world * MathCore::CVT<MathCore::vec3f>::toPtn4(a));
                        auto world_b = MathCore::CVT<MathCore::vec4f>::toVec3(local_to_world * MathCore::CVT<MathCore::vec3f>::toPtn4(b));

                        // Calculate distance from camera for both endpoints
                        float dist_a = MathCore::OP<MathCore::vec3f>::dot(world_a - cam_pos, dir);
                        float dist_b = MathCore::OP<MathCore::vec3f>::dot(world_b - cam_pos, dir);

                        dist_a = MathCore::OP<float>::clamp(dist_a, near_plane, far_plane);
                        dist_b = MathCore::OP<float>::clamp(dist_b, near_plane, far_plane);

                        // Process point A
                        // Convert pixel thickness to world space thickness at this distance
                        // At distance d, the world space size per pixel is: (2 * d * tan_half_fov) / viewport_height
                        // float world_thickness_a = thickness_pixels * (2.0f * dist_a * tan_half_fov) / viewport_height;
                        float world_thickness_a = dist_a * thickness_factor;
                        float local_thickness_a = world_thickness_a * world_to_local_scale;
                        // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_a * 0.5f));
                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_a));
                        // if (dist_a > near_plane)
                        // {
                        //     // Convert pixel thickness to world space thickness at this distance
                        //     // At distance d, the world space size per pixel is: (2 * d * tan_half_fov) / viewport_height
                        //     // float world_thickness_a = thickness_pixels * (2.0f * dist_a * tan_half_fov) / viewport_height;
                        //     float world_thickness_a = dist_a * thickness_factor;
                        //     float local_thickness_a = world_thickness_a * world_to_local_scale;
                        //     // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_a * 0.5f));
                        //     finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_a));
                        // }
                        // else
                        // {
                        //     // Point is behind or at near plane, use near plane thickness
                        //     // float world_thickness_near = thickness_pixels * (2.0f * near_plane * tan_half_fov) / viewport_height;
                        //     float world_thickness_near = near_plane * thickness_factor;
                        //     float local_thickness_near = world_thickness_near * world_to_local_scale;
                        //     // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_near * 0.5f));
                        //     finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, local_thickness_near));
                        // }

                        // Process point B
                        // float world_thickness_b = thickness_pixels * (2.0f * dist_b * tan_half_fov) / viewport_height;
                        float world_thickness_b = dist_b * thickness_factor;
                        float local_thickness_b = world_thickness_b * world_to_local_scale;
                        // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_b * 0.5f));
                        finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_b));
                        // if (dist_b > near_plane)
                        // {
                        //     // float world_thickness_b = thickness_pixels * (2.0f * dist_b * tan_half_fov) / viewport_height;
                        //     float world_thickness_b = dist_b * thickness_factor;
                        //     float local_thickness_b = world_thickness_b * world_to_local_scale;
                        //     // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_b * 0.5f));
                        //     finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_b));
                        // }
                        // else
                        // {
                        //     // float world_thickness_near = thickness_pixels * (2.0f * near_plane * tan_half_fov) / viewport_height;
                        //     float world_thickness_near = near_plane * thickness_factor;
                        //     float local_thickness_near = world_thickness_near * world_to_local_scale;
                        //     // finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_near * 0.5f));
                        //     finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, local_thickness_near));
                        // }
                    }

                    meshWrapper->setShapeAABB(finalAABB, true);
                }
            }

            void ComponentLineMounter::force_refresh()
            {
                // one way
                // meshWrapper->getTransform()->visited = false; // force the transform to be visited again

                // another way
                meshWrapper->forceComputeFinalPositions();
            }

            void ComponentLineMounter::checkOrCreateAuxiliaryComponents(AppKit::GLEngine::ResourceMap *resourceMap)
            {
                auto transform = getTransform();
                if (material == nullptr)
                {
                    material = transform->addComponent<ComponentMaterial>(resourceMap->defaultLineMaterial);
                }
                if (mesh == nullptr)
                {
                    mesh = transform->addNewComponent<ComponentMesh>();
                    mesh->always_clone = true;
                }
                if (meshWrapper == nullptr)
                {
                    meshWrapper = transform->addNewComponent<ComponentMeshWrapper>();
                    meshWrapper->OnBeforeComputeFinalPositions = EventCore::CallbackWrapper(&ComponentLineMounter::OnBeforeComputeFinalPositions, this);
                    transform->makeFirstComponent(meshWrapper);
                    // meshWrapper->updateMeshAABB();
                }
                transform->makeLastComponent(self());
            }

            void ComponentLineMounter::one_time_set(std::shared_ptr<ComponentCamera> p_camera)
            {
                one_time_set_scale = 1.0f;
                if (p_camera == nullptr)
                    return;
                use_max_scale_update_on_change_self_scale = true;

                if (p_camera->compareType(Components::ComponentCameraOrthographic::Type))
                {
                    auto ortho = std::dynamic_pointer_cast<Components::ComponentCameraOrthographic>(p_camera);
                    MathCore::vec3f camera_px_scale =
                        (MathCore::vec3f(ortho->viewport.w, ortho->viewport.h, -1.0f)) *
                        (MathCore::vec3f(ortho->projection.a1, ortho->projection.b2, 1.0f));
                    camera_px_scale = 1.0f / camera_px_scale;

                    const auto &world_to_local = getTransform()->getMatrixInverse(false); // compute inverse until root
                    MathCore::vec3f aux_vec3(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[2])));
                    one_time_word_to_local_scale = MathCore::OP<MathCore::vec3f>::minimum(aux_vec3);
                    one_time_camera_scale = MathCore::OP<MathCore::vec3f>::maximum(camera_px_scale);
                    one_time_set_scale = one_time_word_to_local_scale * one_time_camera_scale;
                }
                else if (p_camera->compareType(Components::ComponentCameraPerspective::Type))
                {
                    auto perspective = std::dynamic_pointer_cast<Components::ComponentCameraPerspective>(p_camera);

                    float tan_half_fov = MathCore::OP<float>::tan(MathCore::OP<float>::deg_2_rad(perspective->fovDegrees) * 0.5f);
                    float far_plane = perspective->farPlane;
                    // Pre-calculate the conversion factor: 2 * tan_half_fov / viewport_height
                    // after that multiply by 0.5 to pass as radius to the sphere parameter
                    // final factor: tan_half_fov / viewport_height
                    float tan_over_viewport_height = tan_half_fov / (float)perspective->viewport.h;

                    const auto &world_to_local = getTransform()->getMatrixInverse(false); // compute inverse until root
                    // Calculate the uniform scale factor for converting world space thickness to local space
                    MathCore::vec3f scale_vec(
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[0])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[1])),
                        MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(world_to_local[2])));
                    one_time_word_to_local_scale = MathCore::OP<MathCore::vec3f>::minimum(scale_vec);

                    // float thickness_factor = thickness_pixels * tan_over_viewport_height;
                    float thickness_factor = tan_over_viewport_height;
                    float world_thickness_far = far_plane * thickness_factor;
                    one_time_camera_scale = world_thickness_far;
                    one_time_set_scale = one_time_word_to_local_scale * one_time_camera_scale;
                }

                dirty = false;
                AABBType finalAABB = aabb;

                for (size_t i = 0; i < mesh->pos.size(); i += 8)
                {
                    const auto &a = mesh->uv[1][i];
                    const auto &b = mesh->uv[2][i];
                    float thickness = mesh->uv[3][i].y;

                    finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(a, one_time_set_scale * thickness));
                    finalAABB = AABBType::joinAABB(finalAABB, AABBType::fromSphere(b, one_time_set_scale * thickness));
                }

                meshWrapper->setShapeAABB(finalAABB, true);
            }

            void ComponentLineMounter::setCamera(AppKit::GLEngine::ResourceMap *resourceMap,
                                                 std::shared_ptr<ComponentCamera> camera, bool use_one_time_set)
            {
                checkOrCreateAuxiliaryComponents(resourceMap);

                use_max_scale_update_on_change_self_scale = false;
                if (this->camera != nullptr && this->camera->getTransform() != nullptr)
                    this->camera->getTransform()->OnVisited.remove(
                        EventCore::CallbackWrapper(&ComponentLineMounter::OnCameraTransformVisit, this));
                if (use_one_time_set)
                {
                    this->camera = nullptr;
                    one_time_set(camera);
                    return;
                }
                this->camera = camera;
                if (camera->compareType(Components::ComponentCameraPerspective::Type))
                {
                    camera->getTransform()->OnVisited.add(
                        EventCore::CallbackWrapper(&ComponentLineMounter::OnCameraTransformVisit, this));
                }
            }

            ComponentLineMounter::ComponentLineMounter() : Component(ComponentLineMounter::Type)
            {
                always_clone = false;

                dirty = true;
                last_max_scaled = -1.0f;

                use_max_scale_update_on_change_self_scale = false;
                one_time_set_scale = 1.0f;
            }

            ComponentLineMounter::~ComponentLineMounter()
            {
                if (this->camera != nullptr && this->camera->getTransform() != nullptr)
                    this->camera->getTransform()->OnVisited.remove(
                        EventCore::CallbackWrapper(&ComponentLineMounter::OnCameraTransformVisit, this));
            }

            void ComponentLineMounter::clear()
            {
                mesh->pos.clear();
                mesh->uv[1].clear();
                mesh->uv[2].clear();
                mesh->uv[3].clear();
                mesh->color[0].clear();

                mesh->indices.clear();

                aabb.makeEmpty();
                meshWrapper->clearShape();

                if (use_max_scale_update_on_change_self_scale)
                {
                    dirty = false;
                }
                else
                {
                    dirty = true;
                }
            }

            void ComponentLineMounter::addLine(const MathCore::vec3f &a, const MathCore::vec3f &b,
                                               float thickness,
                                               const MathCore::vec4f &color)
            {

                if (use_max_scale_update_on_change_self_scale)
                {
                    aabb = AABBType::joinAABB(aabb, AABBType::fromSphere(a, one_time_set_scale * thickness));
                    aabb = AABBType::joinAABB(aabb, AABBType::fromSphere(b, one_time_set_scale * thickness));
                }
                else
                    aabb = AABBType::joinAABB(aabb, AABBType::fromLineSegment(a, b));

                // aabb = AABBType::joinAABB( aabb, AABBType::fromSphere(a, thickness * 0.5) );
                // aabb = AABBType::joinAABB( aabb, AABBType::fromSphere(b, thickness * 0.5) );

                // "attribute vec2 aPosition;" // 2d square position
                // "attribute vec4 aUV1;"      // line point 1
                // "attribute vec4 aUV2;"      // line point 2
                // // x = current position lrp in line [0..1]
                // // y = line_thickness_px
                // "attribute vec2 aUV3;"
                // "attribute vec4 aColor0;"

                //
                //(-1,1)  (0,1)         (1,1) (2,1)
                //         *--------------*
                //         |              |
                //        A|              |B
                //         |              |
                //         *--------------*
                //(-1,-1) (0,-1)        (1,-1) (2,-1)
                uint32_t start_index;
                start_index = (uint32_t)mesh->pos.size();

                mesh->pos.push_back(MathCore::vec3f(-1, 1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(-1, -1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(1, 1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                mesh->pos.push_back(MathCore::vec3f(1, -1, 0));
                mesh->uv[1].push_back(a);                                // line point 1
                mesh->uv[2].push_back(b);                                // line point 2
                mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                mesh->color[0].push_back(color);

                // quad 1
                mesh->indices.push_back(start_index);
                mesh->indices.push_back(start_index + 1);
                mesh->indices.push_back(start_index + 2);

                mesh->indices.push_back(start_index + 2);
                mesh->indices.push_back(start_index + 1);
                mesh->indices.push_back(start_index + 3);

                // quad 2
                mesh->indices.push_back(start_index + 2);
                mesh->indices.push_back(start_index + 3);
                mesh->indices.push_back(start_index + 4);

                mesh->indices.push_back(start_index + 4);
                mesh->indices.push_back(start_index + 3);
                mesh->indices.push_back(start_index + 5);

                // quad 3
                mesh->indices.push_back(start_index + 4);
                mesh->indices.push_back(start_index + 5);
                mesh->indices.push_back(start_index + 6);

                mesh->indices.push_back(start_index + 6);
                mesh->indices.push_back(start_index + 5);
                mesh->indices.push_back(start_index + 7);

                // // a ear
                // //
                // //(-1,1)  (0,1)         (1,1) (2,1)
                // //         *--------------*
                // //         |              |
                // //        A|              |B
                // //         |              |
                // //         *--------------*
                // //(-1,-1) (0,-1)        (1,-1) (2,-1)
                // start_index = (uint32_t)mesh->pos.size();

                // mesh->pos.push_back(MathCore::vec3f(-1, -1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(-1, 1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(0, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->indices.push_back(start_index);
                // mesh->indices.push_back(start_index + 1);
                // mesh->indices.push_back(start_index + 2);

                // mesh->indices.push_back(start_index);
                // mesh->indices.push_back(start_index + 2);
                // mesh->indices.push_back(start_index + 3);

                // // b ear
                // //
                // //(-1,1)  (0,1)         (1,1) (2,1)
                // //         *--------------*
                // //         |              |
                // //        A|              |B
                // //         |              |
                // //         *--------------*
                // //(-1,-1) (0,-1)        (1,-1) (2,-1)
                // start_index = (uint32_t)mesh->pos.size();

                // mesh->pos.push_back(MathCore::vec3f(0, -1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(1, -1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(1, 1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->pos.push_back(MathCore::vec3f(0, 1, 0));
                // mesh->uv[1].push_back(a);                                // line point 1
                // mesh->uv[2].push_back(b);                                // line point 2
                // mesh->uv[3].push_back(MathCore::vec3f(1, thickness, 0)); // x = current position lrp in line [0..1], y = line_thickness_px
                // mesh->color[0].push_back(color);

                // mesh->indices.push_back(start_index);
                // mesh->indices.push_back(start_index + 1);
                // mesh->indices.push_back(start_index + 2);

                // mesh->indices.push_back(start_index);
                // mesh->indices.push_back(start_index + 2);
                // mesh->indices.push_back(start_index + 3);

                mesh->format = ITKExtension::Model::CONTAINS_POS |
                               ITKExtension::Model::CONTAINS_UV1 | ITKExtension::Model::CONTAINS_UV2 | ITKExtension::Model::CONTAINS_UV3 |
                               ITKExtension::Model::CONTAINS_COLOR0;

                meshWrapper->setShapeAABB(aabb);

                if (!use_max_scale_update_on_change_self_scale)
                    dirty = true;
            }

            void ComponentLineMounter::addCircle(const MathCore::vec3f &center, float radius,
                                                 float thickness,
                                                 const MathCore::vec4f &color,
                                                 int segment_count,
                                                 const MathCore::quatf &rotation)
            {
                addLine(center, center + MathCore::vec3f(radius, 0, 0), thickness, color);

                for (int i = 0; i < segment_count; i++)
                {
                    float angle_a = ((float)i / (float)segment_count) * MathCore::CONSTANT<float>::PI * 2.0f;
                    float angle_b = ((float)((i + 1) % segment_count) / (float)segment_count) * MathCore::CONSTANT<float>::PI * 2.0f;

                    auto a = center + (rotation * (MathCore::vec3f(MathCore::OP<float>::cos(angle_a), MathCore::OP<float>::sin(angle_a), 0) * radius));
                    auto b = center + (rotation * (MathCore::vec3f(MathCore::OP<float>::cos(angle_b), MathCore::OP<float>::sin(angle_b), 0) * radius));

                    addLine(a, b, thickness, color);
                }
            }

        }
    }
}
