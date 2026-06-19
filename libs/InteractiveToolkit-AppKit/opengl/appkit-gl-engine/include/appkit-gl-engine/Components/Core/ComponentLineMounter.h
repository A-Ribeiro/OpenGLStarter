#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

// #include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
// #include <appkit-gl-engine/util/ReferenceCounter.h>

// #include <appkit-gl-engine/DefaultEngineShader.h>
// #include "SpriteAtlas.h"

#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMaterial.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {
        class ResourceMap;
        namespace Components
        {

            class ComponentLineMounter : public Component
            {
                // make possible to the component to fiz the aabb according the pixel size of the
                // line segments and the camera projection type
                void OnBeforeComputeFinalPositions(ComponentMeshWrapper *meshWrapper);

                void OnCameraTransformVisit(std::shared_ptr<Transform> transform);

                bool dirty;
                // for orthographic camera, the last max scaled value
                float last_max_scaled;

                // for perspective camera, the last camera projection matrix
                MathCore::vec3f last_dir;
                MathCore::vec3f last_cam_pos;
                float last_near_plane;
                float last_far_plane;
                float last_tan_over_viewport_height;
                MathCore::mat4f last_local_to_world;

                std::shared_ptr<ComponentCamera> camera;

                // this calculates the max size the point can be in any projection.
                // for orthographic camera, it is the max scaled value
                // for perspective camera, it is the max scaled value at the far plane
                bool use_max_scale_update_on_change_self_scale;
                float one_time_set_scale;
                float one_time_word_to_local_scale;
                float one_time_camera_scale;
                void one_time_set(std::shared_ptr<ComponentCamera> camera);

                bool is_one_time_set;

            public:
                static const ComponentType Type;

                // bool always_clone; - mesh wrapper needs to be different on each instance... always clone logic is the default

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;
                std::shared_ptr<ComponentMeshWrapper> meshWrapper;

                using AABBType = CollisionCore::AABB<MathCore::vec3f>;

                AABBType aabb;

                void start();

                void checkOrCreateAuxiliaryComponents(AppKit::GLEngine::ResourceMap *resourceMap);

                void setCamera(AppKit::GLEngine::ResourceMap *resourceMap, std::shared_ptr<ComponentCamera> camera, bool one_time_set);

                void force_refresh();

                ComponentLineMounter();

                ~ComponentLineMounter();

                void clear();

                void addLine(const MathCore::vec3f &a, const MathCore::vec3f &b,
                             float thickness,
                             const MathCore::vec4f &color);

                void addCircle(const MathCore::vec3f &center, float radius,
                               float thickness,
                               const MathCore::vec4f &color,
                               int segment_count = 16,
                               const MathCore::quatf &rotation = MathCore::quatf());

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone);
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {}
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet) {}

                void iterate_lines(const EventCore::Callback<void(const MathCore::vec3f &a, const MathCore::vec3f &b, float thickness, const MathCore::vec4f &color)> &callback) const;

                struct LineSegment
                {
                    MathCore::vec3f a;
                    MathCore::vec3f b;
                    float thickness;
                    MathCore::vec4f color;
                };

                // STL-compatible type aliases
                using value_type = LineSegment;
                using size_type = size_t;
                using difference_type = std::ptrdiff_t;
                // using reference = LineSegment &;
                using const_reference = const LineSegment &;
                // using pointer = LineSegment *;
                using const_pointer = const LineSegment *;

                class const_iterator;
                friend class const_iterator;

                class const_iterator
                {
                public:
                    using iterator_category = std::random_access_iterator_tag;
                    using value_type = const LineSegment;
                    using difference_type = std::ptrdiff_t;
                    using pointer = const LineSegment *;
                    using reference = const LineSegment &;

                    inline const_iterator(const ComponentLineMounter *line_mounter, size_t idx_p) noexcept
                        : line_mounter(line_mounter), idx(idx_p)
                    {
                        size_t item_count = line_mounter->mesh->pos.size() >> 3; // each line segment is 8 vertices (2 points with 4 vertices each)
                        if (item_count == 0 || idx == (std::numeric_limits<size_t>::max)())
                        {
                            idx = (std::numeric_limits<size_t>::max)(); // end
                        }
                        else if (idx < item_count)
                        {
                            const auto &a = line_mounter->mesh->uv[1][idx * 8];
                            const auto &b = line_mounter->mesh->uv[2][idx * 8];
                            float thickness = line_mounter->mesh->uv[3][idx * 8].y;
                            const auto &color = line_mounter->mesh->color[0][idx * 8];

                            this->current_line_segment = LineSegment{a, b, thickness, color};
                        }
                        else
                        {
                            idx = (std::numeric_limits<size_t>::max)(); // end
                        }
                    }

                    inline reference operator*() const noexcept { return this->current_line_segment; }
                    inline pointer operator->() const noexcept { return &this->current_line_segment; }

                    // Prefix increment
                    inline const_iterator &operator++() noexcept
                    {
                        if (idx == (std::numeric_limits<size_t>::max)())
                            return *this; // already at end

                        size_t item_count = line_mounter->mesh->pos.size() >> 3;
                        idx++;
                        if (idx >= item_count)
                            idx = (std::numeric_limits<size_t>::max)(); // end
                        else
                        {
                            const auto &a = line_mounter->mesh->uv[1][idx * 8];
                            const auto &b = line_mounter->mesh->uv[2][idx * 8];
                            float thickness = line_mounter->mesh->uv[3][idx * 8].y;
                            const auto &color = line_mounter->mesh->color[0][idx * 8];

                            this->current_line_segment = LineSegment{a, b, thickness, color};
                        }
                        return *this;
                    }

                    // Postfix increment
                    inline const_iterator operator++(int) noexcept
                    {
                        const_iterator tmp = *this;
                        ++(*this);
                        return tmp;
                    }

                    // Prefix decrement
                    inline const_iterator &operator--() noexcept
                    {
                        size_t item_count = line_mounter->mesh->pos.size() >> 3;
                        if (idx == (std::numeric_limits<size_t>::max)() && item_count > 0)
                            idx = item_count - 1;
                        else if (idx > 0)
                            idx--;
                        else
                            idx = 0; // already at begin

                        // if item_count == 0
                        if (idx >= item_count)
                            idx = (std::numeric_limits<size_t>::max)(); // end
                        else
                        {
                            const auto &a = line_mounter->mesh->uv[1][idx * 8];
                            const auto &b = line_mounter->mesh->uv[2][idx * 8];
                            float thickness = line_mounter->mesh->uv[3][idx * 8].y;
                            const auto &color = line_mounter->mesh->color[0][idx * 8];

                            this->current_line_segment = LineSegment{a, b, thickness, color};
                        }

                        return *this;
                    }

                    // Postfix decrement
                    inline const_iterator operator--(int) noexcept
                    {
                        const_iterator tmp = *this;
                        --(*this);
                        return tmp;
                    }

                    inline const_iterator &operator+=(difference_type offset) noexcept
                    {
                        if (offset == 0)
                            return *this;

                        size_t item_count = line_mounter->mesh->pos.size() >> 3;
                        if (offset > 0)
                        {
                            if (idx == (std::numeric_limits<size_t>::max)())
                                return *this; // already at end
                            idx += offset;
                            if (idx >= item_count)
                                idx = (std::numeric_limits<size_t>::max)();
                        }
                        else
                        {
                            difference_type abs_offset = -offset;
                            if (idx == (std::numeric_limits<size_t>::max)())
                            {
                                if ((size_t)abs_offset >= item_count)
                                    idx = 0;
                                else
                                    idx = item_count - abs_offset;
                            }
                            else if ((size_t)abs_offset > idx)
                                idx = 0;
                            else
                                idx -= abs_offset;
                        }

                        if (idx >= item_count)
                            idx = (std::numeric_limits<size_t>::max)(); // end
                        else
                        {
                            const auto &a = line_mounter->mesh->uv[1][idx * 8];
                            const auto &b = line_mounter->mesh->uv[2][idx * 8];
                            float thickness = line_mounter->mesh->uv[3][idx * 8].y;
                            const auto &color = line_mounter->mesh->color[0][idx * 8];

                            this->current_line_segment = LineSegment{a, b, thickness, color};
                        }

                        return *this;
                    }

                    inline const_iterator &operator-=(difference_type offset) noexcept
                    {
                        return *this += (-offset);
                    }

                    inline const_iterator operator+(difference_type offset) const noexcept
                    {
                        const_iterator tmp = *this;
                        tmp += offset;
                        return tmp;
                    }

                    inline const_iterator operator-(difference_type offset) const noexcept
                    {
                        const_iterator tmp = *this;
                        tmp -= offset;
                        return tmp;
                    }

                    inline difference_type operator-(const const_iterator &other) const noexcept
                    {
                        size_t item_count = line_mounter->mesh->pos.size() >> 3;

                        size_t this_pos = (idx == (std::numeric_limits<size_t>::max)()) ? item_count : idx;
                        size_t other_pos = (other.idx == (std::numeric_limits<size_t>::max)()) ? item_count : other.idx;

                        return (difference_type)this_pos - (difference_type)other_pos;
                    }

                    inline reference operator[](difference_type n) const noexcept
                    {
                        const_iterator tmp = *this + n;
                        return *tmp;
                    }

                    inline constexpr bool operator==(const const_iterator &other) const noexcept
                    {
                        return idx == other.idx && line_mounter == other.line_mounter;
                    }
                    inline constexpr bool operator!=(const const_iterator &other) const noexcept
                    {
                        return !(*this == other);
                    }

                    inline bool operator<(const const_iterator &other) const noexcept
                    {
                        if (line_mounter != other.line_mounter)
                            return false;
                        if (idx == (std::numeric_limits<size_t>::max)())
                            return false; // this is at end
                        if (other.idx == (std::numeric_limits<size_t>::max)())
                            return true; // other is at end
                        return idx < other.idx;
                    }

                    inline bool operator<=(const const_iterator &other) const noexcept
                    {
                        return *this < other || *this == other;
                    }

                    inline bool operator>(const const_iterator &other) const noexcept
                    {
                        return !(*this <= other);
                    }

                    inline bool operator>=(const const_iterator &other) const noexcept
                    {
                        return !(*this < other);
                    }

                private:
                    const ComponentLineMounter *line_mounter;
                    size_t idx;

                    LineSegment current_line_segment;

                    friend class ComponentLineMounter;
                };

                // inline iterator begin() noexcept { return iterator(this, (internal_size) ? _start : m_capacity, internal_size); }
                // inline iterator end() noexcept { return iterator(this, m_capacity, 0); }
                inline const_iterator begin() const noexcept { return const_iterator(this, 0); }
                inline const_iterator end() const noexcept { return const_iterator(this, (std::numeric_limits<size_t>::max)()); }
                inline const_iterator cbegin() const noexcept { return begin(); }
                inline const_iterator cend() const noexcept { return end(); }

                // Additional STL-compatible iterator type aliases
                // using iterator_type = iterator;
                using const_iterator_type = const_iterator;
                // using reverse_iterator = std::reverse_iterator<iterator>;
                using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            };
        }
    }
}
