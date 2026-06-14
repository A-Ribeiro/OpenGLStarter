#pragma once

#include <InteractiveToolkit/EventCore/Callback.h>
#include <InteractiveToolkit/platform/Core/ObjectBuffer.h>
#include <appkit-ui/util/AppOptions.h>
//#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {
        struct EngineWindowConfig;
    }

    namespace ui
    {

        class AppTemplate
        {
            std::vector<OptionsGroup> options_template;
            EventCore::Callback<void(Platform::ObjectBuffer *out_optionsDataRaw)> read_options_raw_data;
            EventCore::Callback<void(const Platform::ObjectBuffer &in_optionsDataRaw)> write_options_raw_data;

            void apply_window_options_to_engine(AppKit::GLEngine::EngineWindowConfig *engineConfig, const EventCore::Callback<void()> &OnAfterAppCreation);

            public:

            void configure(
                const std::vector<OptionsGroup> &options_template,
                const EventCore::Callback<void(Platform::ObjectBuffer *out_optionsDataRaw)> &read_options_raw_data,
                const EventCore::Callback<void(const Platform::ObjectBuffer &in_optionsDataRaw)> &write_options_raw_data
            );

            void load_options();
            
            void save_options();

            void apply_settings_to_window(const EventCore::Callback<void()> &OnAfterAppCreation);

            void reset_monitor_mode_to_default();

            static AppTemplate *Instance();

        };

    }
}
