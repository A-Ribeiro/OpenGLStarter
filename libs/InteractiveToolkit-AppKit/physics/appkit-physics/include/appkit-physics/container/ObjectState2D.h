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
                // aux for pass through platforms
                Platform::SmartVector<PassThroughState> pass_through_active_circular_list;

                ObjectState2D();

                void pass_through_remove_id(uint32_t idx);
                bool pass_through_is_active(uint32_t idx) const;
                bool &pass_through_get_active_ref(uint32_t idx);

            };

        }
    }
}