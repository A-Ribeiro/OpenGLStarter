#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/PressReleaseDetector.h>

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
            const std::vector<uint32_t> &static_ids,
            bool *ref_on_ground_called,
            const MathCore::vec2f &position,
            float radius_grounded,
            const EventCore::Callback<void(const Segment2D *on_segment)> &onGrounded);

        void pushOutOfSegments1(
            MathCore::vec2f *ref_b,
            float radius);

        bool pushOutOfSegments(
            MathCore::vec2f point,
            float radius,
            MathCore::vec2f *output,
            MathCore::vec2f *offset,
            MathCore::vec2f *push_normal,
            const MathCore::vec2f &velocity_hint = MathCore::vec2f(0));

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
            const EventCore::Callback<void(const MathCore::vec2f &pos, const Segment2D *on_segment)> &onMoveTouch);

        const float max_velocity = 5000.0f;

        ITK_DECLARE_CREATE_SHARED(PhysicsContainer)
    };

}