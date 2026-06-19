#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
// #include <InteractiveToolkit/EventCore/PressReleaseDetector.h>
// #include <InteractiveToolkit/Platform/Core/SmartVector.h>

#include <appkit-physics/core/Box2D.h>
#include <appkit-physics/core/Line2D.h>
#include <appkit-physics/core/Segment2D.h>

#include <appkit-physics/container/Structure2D.h>
#include <appkit-physics/container/ObjectState2D.h>
#include <appkit-physics/container/ThreadState2D.h>

#include <appkit-physics/util/Quadtree.h>
#include <appkit-physics/util/Uuid.h>
// #include <appkit-physics/velocity-helpers/JumpState.h>

#include <InteractiveToolkit/EventCore/Callback.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            enum GameAreaSide
            {
                GameAreaSide_Top,
                GameAreaSide_Bottom,
                GameAreaSide_Left,
                GameAreaSide_Right,
                GameAreaSide_Count
            };

            class Container2D
            {
                Util::UuidUint32 uuid;

                std::vector<std::unique_ptr<Structure2D>> static_structures;
                std::unique_ptr<Util::Quadtree<Structure2D::QuadtreeIntegration>> static_quadtree;
                std::vector<const Structure2D *> static_always_check;

                std::vector<std::unique_ptr<Structure2D>> dynamic_structures;
                std::unique_ptr<Util::Quadtree<Structure2D::QuadtreeIntegration>> dynamic_quadtree;
                std::vector<const Structure2D *> dynamic_always_check;

            public:
                std::vector<std::unique_ptr<Structure2D>> &getStaticStructures();
                std::vector<std::unique_ptr<Structure2D>> &getDynamicStructures();

                Structure2D *addStaticStructure(const Structure2D &structure);
                void removeStaticStructure(uint32_t idx);
                Structure2D *getStaticStructure(uint32_t idx);

                Structure2D *addDynamicStructure(const Structure2D &structure);
                void removeDynamicStructure(uint32_t idx);
                Structure2D *getDynamicStructure(uint32_t idx);

                // Platform::SmartVector<std::shared_ptr<JumpingController>> jumpingControllerList;

                Core::Line2D game_area_inequality_eq[GameAreaSide_Count];
                Core::Box2D game_area;

                void buildStaticQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);
                void buildDynamicQuadtree(int32_t maxDepth_ = 8, int32_t minPointThresholdToSubdivide_ = 16);

                void clearStatic();
                void clearDynamic();

                void setGameArea(const Core::Box2D &box);

                Core::Box2D computeStaticStructureBox() const;

                void groundCheck(
                    bool *ref_on_ground_called,
                    const MathCore::vec2f &position,
                    float radius_grounded,
                    const EventCore::Callback<void(const Core::Segment2D *on_segment)> &onGrounded,
                    ThreadState2D &thread_state,
                    ObjectState2D &object_state);

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
                    ThreadState2D &thread_state,
                    ObjectState2D &object_state);

                // returns last collision segment if collision occurs, otherwise returns nullptr
                void moveObject(
                    const MathCore::vec2f &in_position,
                    float radius,
                    float radius_grounded,
                    float offset_grounded,
                    MathCore::vec2f *out_position,
                    MathCore::vec2f *out_velocity,
                    float delta_time,
                    const EventCore::Callback<void(const Core::Segment2D *on_segment)> &onGrounded,
                    const EventCore::Callback<void(const MathCore::vec2f &pos, const Core::Segment2D *on_segment)> &onMoveTouch,
                    ThreadState2D &thread_state,
                    ObjectState2D &object_state,
                    float skin_width = 1e-2f);

                const float max_velocity = 5000.0f;

                ITK_DECLARE_CREATE_SHARED(Container2D)

                void dumpContent(const EventCore::Callback<void(const MathCore::vec2f &a, const MathCore::vec2f &b, bool is_pass_through)> &onLine,
                                 const EventCore::Callback<void(const MathCore::vec2f &center, const MathCore::vec2f &size)> &onBox,
                                 const EventCore::Callback<void(const MathCore::vec2f &center, float radius)> &onCircle,
                                 const EventCore::Callback<void(const MathCore::vec2f &center, const MathCore::vec2f &size)> &onGameArea);
            };

        }
    }
}