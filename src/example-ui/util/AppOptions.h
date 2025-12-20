#pragma once

#include <InteractiveToolkit-Extension/io/Writer.h>
#include <InteractiveToolkit-Extension/io/Reader.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

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

        struct
        {
            char EffectsVolume[64];
            char MusicVolume[64];
        } Audio;

        struct
        {
            char WindowMode[64];
            char Resolution[64];
            char Aspect[64];
            char AntiAliasing[64];
            char VSync[64];
        } Video;

        struct
        {
            char Language[64];
            char ColorScheme[64];
            char ButtonAppearance[64];
            char UiSize[64];
            char MeshCrusher[64];
            char Particles[64];
            char OnGameStats[64];
        } Extra;
    };

    namespace OptionsConstants
    {
        static constexpr const char *ControlInput[3] = {"Steam 1", "Keyboard", "Steam 1 + Keyboard"};
        static constexpr const char *ControlMovement[2] = {"Fluid", "Legacy"};

        static constexpr const char *AudioVolume[11] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"};

        static constexpr const char *VideoWindowMode[3] = {"Window", "Borderless", "Fullscreen"};
        // static constexpr const char *VideoResolution[4] = {"2560x1440", "1920x1080", "1280x720", "854x480"};
        static constexpr const char *VideoAspect[2] = {"16:9", "16:10"};
        static constexpr const char *VideoAntiAliasing[2] = {"MSAA", "OFF"};
        static constexpr const char *VideoVSync[2] = {"ON", "OFF"};

        static constexpr const char *ExtraLanguage[2] = {"English", "Português (BR)"};
        static constexpr const char *ExtraColorScheme[6] = {"Blush", "Purple", "Orange", "Green", "Blue", "Dark"};
        static constexpr const char *ExtraButtonAppearance[8] = {"Bend Up", "Bend Down", "Round", "Tip Front", "Tip Back", "Tip Up", "Tip Down", "Square"};
        static constexpr const char *ExtraUiSize[5] = {"Extra Small", "Small", "Medium", "Large", "Extra Large"};
        static constexpr const char *ExtraMeshCrusher[2] = {"ON", "OFF"};
        static constexpr const char *ExtraParticles[4] = {"Low", "Medium", "High", "Ultra"};
        static constexpr const char *ExtraOnGameStats[2] = {"OFF", "FPS"};

        static constexpr const char *groups[4] = {"Control", "Audio", "Video", "Extra"};

    };

    void write_binary_data_v1(const AppOptions_v1 &options, Platform::ObjectBuffer *output);
    bool read_binary_data_v1(const Platform::ObjectBuffer &data, AppOptions_v1 *out_options);
    void populate_default_data_v1(AppOptions_v1 *options);

    class OptionsManager
    {
        std::vector<MathCore::vec2i> mainMonitorFullscreenResolutions;

        bool initialized;
    public:

        // DPI::Monitor mainMonitor;
        DPI::Mode mainMonitor_InitialMode;

        AppOptions_v1 currentOptions;

        OptionsManager();

        bool isInitialized() const { return initialized; }
        void initializeDefaults();
        bool loadOptionsFromBuffer(const Platform::ObjectBuffer &data);
        void saveOptionsToBuffer(Platform::ObjectBuffer *output);

        void checkSystemCompatibilityAfterLoad();

        static OptionsManager *Instance();

        std::vector<std::string> getGroups() const;
        std::vector<std::string> getGroupKeys(const std::string &group) const;
        std::vector<std::string> getGroupValuesForKey(const std::string &group, const std::string &key) const;
        char *getGroupValueSelectedForKey(const std::string &group, const std::string &key);
        const char *getGroupValueSelectedForKey(const std::string &group, const std::string &key) const;
        bool setGroupValueSelectedForKey(const std::string &group, const std::string &key, const std::string &value);


        // compare current options with Singleton Instance options
        bool hasChanged(const std::string &group, const std::string &key);
        // compare all available options with Singleton Instance options
        bool hasAnyChange();
        

    };

}