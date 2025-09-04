#pragma once

#include <InteractiveToolkit-Extension/io/Writer.h>
#include <InteractiveToolkit-Extension/io/Reader.h>

#include <vector>
#include <inttypes.h>
#include <string.h>

namespace AppOptions
{

    struct AppOptions_v1
    {
        struct
        {
            char Input[64];
            char Movement[64];
        } Control;
        static constexpr const char *ControlInput[3] = {"Steam 1", "Keyboard", "Steam 1 + Keyboard"};
        static constexpr const char *ControlMovement[2] = {"Fluid", "Legacy"};

        struct
        {
            char EffectsVolume[64];
            char MusicVolume[64];
        } Audio;
        static constexpr const char *AudioVolume[11] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"};

        struct
        {
            char WindowMode[64];
            char Resolution[64];
            char Aspect[64];
            char AntiAliasing[64];
            char VSync[64];
        } Video;
        static constexpr const char *VideoWindowMode[3] = {"Window", "Borderless", "Fullscreen"};
        // static constexpr const char *VideoResolution[4] = {"2560x1440", "1920x1080", "1280x720", "854x480"};
        static constexpr const char *VideoAspect[2] = {"16:9", "16:10"};
        static constexpr const char *VideoAntiAliasing[2] = {"MSAA", "OFF"};
        static constexpr const char *VideoVSync[2] = {"ON", "OFF"};

        struct
        {
            char Language[64];
            char ColorScheme[64];
            char MeshCrusher[64];
            char Particles[64];
            char OnGameStats[64];
        } Extra;
        static constexpr const char *ExtraLanguage[2] = {"English", "Português (BR)"};
        static constexpr const char *ExtraColorScheme[4] = {"Purple", "Orange", "Green", "Blue"};
        static constexpr const char *ExtraMeshCrusher[2] = {"ON", "OFF"};
        static constexpr const char *ExtraParticles[4] = {"Low", "Medium", "High", "Ultra"};
        static constexpr const char *ExtraOnGameStats[2] = {"OFF", "FPS"};
    };

    void generate_binary_data_v1(const AppOptions_v1 &options, Platform::ObjectBuffer *output);
    bool parse_binary_data_v1(const Platform::ObjectBuffer &data, AppOptions_v1 *out_options);
    void create_binary_data_v1(AppOptions_v1 *options);


    class OptionsManager
    {
    public:
        AppOptions_v1 currentOptions;
        
    };

}