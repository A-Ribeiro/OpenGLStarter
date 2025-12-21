#pragma once

#include "../common.h"

namespace ui
{
    class OsciloscopeWithTrigger
    {

        float countdown_increase_speed_for_secs_and_trigger_action;
        float osciloscope;

        float osciloscope_normal_hz;
        float osciloscope_selected_hz;
        float osciloscope_countdown_trigger_secs;

        bool osciloscope_locked;

    protected:
        virtual void onOsciloscopeAction() = 0;
        virtual void onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin) = 0;
    public:

        OsciloscopeWithTrigger(
            float osciloscope_normal_hz = 1.0f,
            float osciloscope_selected_hz = 6.0f,
            float osciloscope_countdown_trigger_secs = 0.5f);
        ~OsciloscopeWithTrigger() = default;

        void osciloscopeUpdate(Platform::Time *elapsed);

        // set lock true in the action
        void osciloscopeTriggerAction();

        void osciloscopeResetLock();
        bool osciloscopeIsLocked() const;
    };

}
