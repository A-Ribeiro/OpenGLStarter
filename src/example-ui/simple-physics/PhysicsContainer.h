#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>
#include <InteractiveToolkit/Platform/Core/SmartVector.h>

#include "Segment2D.h"
#include "Box2D.h"
#include "Line2D.h"
#include "Structure2D.h"

#include "Quadtree.h"

#include "../components/util/JumpState.h"

namespace SimplePhysics
{
    enum GameAreaSide
    {
        GameAreaSide_Top,
        GameAreaSide_Bottom,
        GameAreaSide_Left,
        GameAreaSide_Right,
        GameAreaSide_Count
    };

    class PhysicsContainer;

    struct PassThroughState
    {
        uint32_t id;
        bool is_active;
    };

    const size_t MAX_ACTIVE_PASS_THROUGH = 4;

    struct PhysicsState
    {
        // aux for pass through platforms
        Platform::SmartVector<PassThroughState> pass_through_active_circular_list;

        // aux for quadtree queries
        std::vector<uint32_t> quadtree_ids;
        std::vector<uint32_t> tmp_array;

        // aux for structure query
        std::vector<const Structure2D *> structure_ptrs;

        PhysicsState() : pass_through_active_circular_list(MAX_ACTIVE_PASS_THROUGH)
        {
            pass_through_active_circular_list.clear();
        }

        inline bool pass_through_is_active(uint32_t idx) const
        {
            auto it = std::find_if(pass_through_active_circular_list.begin(), pass_through_active_circular_list.end(),
                                   [idx](const PassThroughState &state)
                                   { return state.id == idx; });
            return (it != pass_through_active_circular_list.end()) ? it->is_active : false;
        }

        inline bool &pass_through_get_active_ref(uint32_t idx)
        {
            auto it = std::find_if(pass_through_active_circular_list.begin(), pass_through_active_circular_list.end(),
                                   [idx](const PassThroughState &state)
                                   { return state.id == idx; });
            if (it == pass_through_active_circular_list.end())
            {
                if (pass_through_active_circular_list.size() >= MAX_ACTIVE_PASS_THROUGH)
                    pass_through_active_circular_list.pop_front();
                // if not found, add to the active list with inactive state
                pass_through_active_circular_list.push_back({idx, true});
                it = --pass_through_active_circular_list.end();
            }
            else
            {
                // make this it the last
                PassThroughState state = *it;
                pass_through_active_circular_list.erase(it);
                pass_through_active_circular_list.push_back(state);
                it = --pass_through_active_circular_list.end();
            }

            // printf("active pass-through :");
            // for( auto &state : pass_through_active_circular_list)
            //     printf("{id: %u, is_active: %d} ", state.id, state.is_active);
            // printf("\n");

            return it->is_active;
        }

        inline void query_box(
            Quadtree<Structure2D::QuadtreeIntegration> *quadtree, 
            const std::vector<Structure2D> &static_structures,
            const MathCore::vec2f &min, const MathCore::vec2f &max)
        {
            quadtree_ids.clear();
            quadtree->query_box(min, max, &quadtree_ids, &tmp_array);
            structure_ptrs.clear();
            for (uint32_t idx : quadtree_ids)
                structure_ptrs.push_back(&static_structures[idx]);
        }

        inline void query_segment_radius(
            Quadtree<Structure2D::QuadtreeIntegration> *quadtree, 
            const std::vector<Structure2D> &static_structures,
            const MathCore::vec2f &a, const MathCore::vec2f &b, float radius)
        {
            quadtree_ids.clear();
            quadtree->query_segment_radius(a, b, radius, &quadtree_ids, &tmp_array);
            structure_ptrs.clear();
            for (uint32_t idx : quadtree_ids)
                structure_ptrs.push_back(&static_structures[idx]);
        }


    };

    class JumpingController
    {
    public:
        JumpState jumpState;

        MathCore::vec2f velocity;
        MathCore::vec2f acceleration;

        MathCore::vec2f position;

        MathCore::vec2f gravity;
        MathCore::vec2f gravity_dir;
        float gravity_mag;

        float radius;
        float radius_grounded;
        float offset_grounded;

        bool allow_double_jump;

        Segment2D last_collision_segment;
        bool has_last_collision_segment;

        EventCore::PressReleaseDetector move_x_detector;

        PhysicsState physics_state;

        JumpingController();

        static JumpingController fromStaticConfig(
            float radius, float radius_grounded, float offset_grounded,
            float jump_risingVelocity, float jump_minJumpHeight, float jump_maxJumpHeight, float jump_secondJumpHeight,
            const MathCore::vec2f &gravity,
            bool allow_double_jump);

        void update(PhysicsContainer *physicsContainer, Platform::Time *time, float input_x_axis, bool jump_pressed, float max_velocity);

        // set the position and make reset velocity and acceleration, useful for teleporting the player
        void teleport(const MathCore::vec2f &position);

        ITK_DECLARE_CREATE_SHARED(JumpingController)
    };

    class PhysicsContainer
    {
    public:
        std::vector<Structure2D> static_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> static_quadtree;
        std::vector<const Structure2D *> always_check_structures;

        std::vector<Structure2D> dynamic_structures;
        std::unique_ptr<Quadtree<Structure2D::QuadtreeIntegration>> dynamic_quadtree;

        Platform::SmartVector<std::shared_ptr<JumpingController>> jumpingControllerList;

        Line2D game_area_inequality_eq[GameAreaSide_Count];
        Box2D game_area;

        void buildStaticQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);
        void buildDynamicQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);

        void clearStatic();
        void clearDynamic();

        void setGameArea(const Box2D &box);

        Box2D computeStaticStructureBox() const;

        void groundCheck(
            bool *ref_on_ground_called,
            const MathCore::vec2f &position,
            float radius_grounded,
            const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded,
            PhysicsState &physics_state);

        // void pushOutOfSegments1(
        //     MathCore::vec2f *ref_b,
        //     float radius);

        bool pushOutOfSegments(
            MathCore::vec2f point,
            float radius,
            MathCore::vec2f *output,
            MathCore::vec2f *offset,
            MathCore::vec2f *push_normal,
            const MathCore::vec2f &velocity_hint,
            PhysicsState &physics_state);

        // returns last collision segment if collision occurs, otherwise returns nullptr
        void movePlayer(
            const MathCore::vec2f &position,
            float radius,
            float radius_grounded,
            float offset_grounded,
            MathCore::vec2f *out_position,
            MathCore::vec2f *out_velocity,
            float delta_time,
            const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded,
            const EventCore::Callback<void(const MathCore::vec2f &pos, const Segment2D *on_segment)> &onMoveTouch,
            PhysicsState &physics_state);

        const float max_velocity = 5000.0f;

        ITK_DECLARE_CREATE_SHARED(PhysicsContainer)
    };

}