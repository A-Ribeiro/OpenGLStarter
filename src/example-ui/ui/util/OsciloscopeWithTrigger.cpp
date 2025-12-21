#include "OsciloscopeWithTrigger.h"

namespace ui
{
    OsciloscopeWithTrigger::OsciloscopeWithTrigger(float osciloscope_normal_hz, float osciloscope_selected_hz, float osciloscope_countdown_trigger_secs)
    {
        this->osciloscope_normal_hz = osciloscope_normal_hz;
        this->osciloscope_selected_hz = osciloscope_selected_hz;
        this->osciloscope_countdown_trigger_secs = osciloscope_countdown_trigger_secs;
        this->countdown_increase_speed_for_secs_and_trigger_action = -1.0f;
        this->osciloscope = MathCore::OP<float>::deg_2_rad(-90.0f);
        this->osciloscope_locked = false;
    }
    // OsciloscopeWithTrigger::~OsciloscopeWithTrigger() = default;

    void OsciloscopeWithTrigger::osciloscopeUpdate(Platform::Time *elapsed)
    {
        float speed = osciloscope_normal_hz;

        if (countdown_increase_speed_for_secs_and_trigger_action > 0.0f)
        {
            speed = osciloscope_selected_hz;
            countdown_increase_speed_for_secs_and_trigger_action -= elapsed->unscaledDeltaTime;
            if (countdown_increase_speed_for_secs_and_trigger_action < 0.0f)
            {
                countdown_increase_speed_for_secs_and_trigger_action = -1.0f;
                onOsciloscopeAction();
            }
        }

        if (!osciloscopeIsLocked() || countdown_increase_speed_for_secs_and_trigger_action > 0.0f)
        {
            const float _360_pi = MathCore::CONSTANT<float>::PI * 2.0f;
            osciloscope = MathCore::OP<float>::fmod(osciloscope + elapsed->unscaledDeltaTime * speed * _360_pi, _360_pi);
            float sin = MathCore::OP<float>::sin(osciloscope) * 0.5f + 0.5f;
            onOsciloscopeSinLerp(elapsed, osciloscope, sin);
        }

    }

    // set lock true in the action
    void OsciloscopeWithTrigger::osciloscopeTriggerAction()
    {
        this->countdown_increase_speed_for_secs_and_trigger_action = this->osciloscope_countdown_trigger_secs;
        this->osciloscope_locked = true;
    }

    void OsciloscopeWithTrigger::osciloscopeResetLock()
    {
        osciloscope_locked = false;
        osciloscope = MathCore::OP<float>::deg_2_rad(-90.0f);
        this->countdown_increase_speed_for_secs_and_trigger_action = -1.0f;
    }

    bool OsciloscopeWithTrigger::osciloscopeIsLocked() const
    {
        return osciloscope_locked;
    }

}
