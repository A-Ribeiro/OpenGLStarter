#pragma once

#include <InteractiveToolkit/Platform/Core/SmartVector.h>

namespace AppKit
{
    namespace Physics
    {
        namespace Container
        {
            struct PassThroughState
            {
                uint32_t id;
                bool is_active;
            };

            const size_t MAX_ACTIVE_PASS_THROUGH = 4;

            struct ObjectState2D
            {
                private:
                float time_acc_sec;
                public:
                // aux for pass through platforms
                Platform::SmartVector<PassThroughState> pass_through_active_circular_list;

                ObjectState2D();

                void pass_through_remove_id(uint32_t idx);
                bool pass_through_is_active(uint32_t idx) const;
                // bool &pass_through_get_active_ref(uint32_t idx);

                bool pass_through_get_active(uint32_t idx);
                void pass_through_set_active(uint32_t idx, bool active);

                void temporarily_turn_off_pass_through(float amount_time_sec = .050f);

                void elapsed_time(float elapsed_sec);

            };

        }
    }
}