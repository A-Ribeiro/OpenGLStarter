#include "AppOptions.h"
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

namespace AppOptions
{
    void write_binary_data_v1(const AppOptions_v1 &options, Platform::ObjectBuffer *output)
    {
        ITKExtension::IO::Writer writer;
        writer.writeInt32(0x01); // version
        writer.writeRaw(&options, sizeof(AppOptions_v1));
        writer.writeToBuffer(output);
    }

    bool read_binary_data_v1(const Platform::ObjectBuffer &data, AppOptions_v1 *out_options)
    {
        ITKExtension::IO::Reader reader;

        if (!reader.readFromBuffer(data))
            return false;
        if (reader.readInt32() != 0x01)
            return false;
        reader.readRaw(out_options, sizeof(AppOptions_v1));

        return true;
    }

    void create_default_binary_data_v1(AppOptions_v1 *options)
    {
        memset(options, 0, sizeof(AppOptions_v1));
        snprintf(options->Control.Input, sizeof(options->Control.Input), "%s", OptionsConstants::ControlInput[2]);
        snprintf(options->Control.Movement, sizeof(options->Control.Movement), "%s", OptionsConstants::ControlMovement[0]);

        snprintf(options->Audio.EffectsVolume, sizeof(options->Audio.EffectsVolume), "%s", OptionsConstants::AudioVolume[10]);
        snprintf(options->Audio.MusicVolume, sizeof(options->Audio.MusicVolume), "%s", OptionsConstants::AudioVolume[10]);

        snprintf(options->Video.WindowMode, sizeof(options->Video.WindowMode), "%s", OptionsConstants::VideoWindowMode[0]);
        snprintf(options->Video.Resolution, sizeof(options->Video.Resolution), "%s", "");
        snprintf(options->Video.Aspect, sizeof(options->Video.Aspect), "%s", OptionsConstants::VideoAspect[0]);
        snprintf(options->Video.AntiAliasing, sizeof(options->Video.AntiAliasing), "%s", OptionsConstants::VideoAntiAliasing[0]);
        snprintf(options->Video.VSync, sizeof(options->Video.VSync), "%s", OptionsConstants::VideoVSync[0]);

        snprintf(options->Extra.Language, sizeof(options->Extra.Language), "%s", OptionsConstants::ExtraLanguage[0]);
        snprintf(options->Extra.ColorScheme, sizeof(options->Extra.ColorScheme), "%s", OptionsConstants::ExtraColorScheme[0]);
        snprintf(options->Extra.ButtonAppearance, sizeof(options->Extra.ButtonAppearance), "%s", OptionsConstants::ExtraButtonAppearance[0]);
        snprintf(options->Extra.MeshCrusher, sizeof(options->Extra.MeshCrusher), "%s", OptionsConstants::ExtraMeshCrusher[0]);
        snprintf(options->Extra.Particles, sizeof(options->Extra.Particles), "%s", OptionsConstants::ExtraParticles[2]);
        snprintf(options->Extra.OnGameStats, sizeof(options->Extra.OnGameStats), "%s", OptionsConstants::ExtraOnGameStats[0]);
    }

    std::vector<std::string> OptionsManager::getGroups() const
    {
        return {std::begin(AppOptions::OptionsConstants::groups), std::end(AppOptions::OptionsConstants::groups)};
    }
    std::vector<std::string> OptionsManager::getGroupKeys(const std::string &group) const
    {
        if (group == "Control")
        {
            return {"Input", "Movement"};
        }
        else if (group == "Audio")
        {
            return {"EffectsVolume", "MusicVolume"};
        }
        else if (group == "Video")
        {
            return {"WindowMode", "Resolution", "Aspect", "AntiAliasing", "VSync"};
        }
        else if (group == "Extra")
        {
            return {"Language", "ColorScheme", "ButtonAppearance", "MeshCrusher", "Particles", "OnGameStats"};
        }
        return {};
    }

    std::vector<std::string> OptionsManager::getGroupValuesForKey(const std::string &group, const std::string &key) const
    {
        if (group == "Control")
        {
            if (key == "Input")
                return {std::begin(AppOptions::OptionsConstants::ControlInput), std::end(AppOptions::OptionsConstants::ControlInput)};
            if (key == "Movement")
                return {std::begin(AppOptions::OptionsConstants::ControlMovement), std::end(AppOptions::OptionsConstants::ControlMovement)};
        }
        else if (group == "Audio")
        {
            if (key == "EffectsVolume")
                return {std::begin(AppOptions::OptionsConstants::AudioVolume), std::end(AppOptions::OptionsConstants::AudioVolume)};
            if (key == "MusicVolume")
                return {std::begin(AppOptions::OptionsConstants::AudioVolume), std::end(AppOptions::OptionsConstants::AudioVolume)};
        }
        else if (group == "Video")
        {
            if (key == "WindowMode")
                return {std::begin(AppOptions::OptionsConstants::VideoWindowMode), std::end(AppOptions::OptionsConstants::VideoWindowMode)};
            if (key == "Resolution")
            {
                const char *currWindowMode = getGroupValueSelectedForKey("Video", "WindowMode");
                if (strcmp(currWindowMode, "Borderless") == 0)
                {
                    auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                    return {ITKCommon::PrintfToStdString("%ix%i", defaultMonitor.width, defaultMonitor.height)};
                }
                else if (strcmp(currWindowMode, "Fullscreen") == 0)
                {
                    std::vector<std::string> res;
                    for (const auto &mode : this->mainMonitorFullscreenResolutions)
                        res.push_back(ITKCommon::PrintfToStdString("%ix%i", mode.x, mode.y));
                    return res;
                }
                else // windowed
                    return {"-"};
            }
            if (key == "Aspect")
                return {std::begin(AppOptions::OptionsConstants::VideoAspect), std::end(AppOptions::OptionsConstants::VideoAspect)};
            if (key == "AntiAliasing")
                return {std::begin(AppOptions::OptionsConstants::VideoAntiAliasing), std::end(AppOptions::OptionsConstants::VideoAntiAliasing)};
            if (key == "VSync")
                return {std::begin(AppOptions::OptionsConstants::VideoVSync), std::end(AppOptions::OptionsConstants::VideoVSync)};
        }
        else if (group == "Extra")
        {
            if (key == "Language")
                return {std::begin(AppOptions::OptionsConstants::ExtraLanguage), std::end(AppOptions::OptionsConstants::ExtraLanguage)};
            if (key == "ColorScheme")
                return {std::begin(AppOptions::OptionsConstants::ExtraColorScheme), std::end(AppOptions::OptionsConstants::ExtraColorScheme)};
            if (key == "ButtonAppearance")
                return {std::begin(AppOptions::OptionsConstants::ExtraButtonAppearance), std::end(AppOptions::OptionsConstants::ExtraButtonAppearance)};
            if (key == "MeshCrusher")
                return {std::begin(AppOptions::OptionsConstants::ExtraMeshCrusher), std::end(AppOptions::OptionsConstants::ExtraMeshCrusher)};
            if (key == "Particles")
                return {std::begin(AppOptions::OptionsConstants::ExtraParticles), std::end(AppOptions::OptionsConstants::ExtraParticles)};
            if (key == "OnGameStats")
                return {std::begin(AppOptions::OptionsConstants::ExtraOnGameStats), std::end(AppOptions::OptionsConstants::ExtraOnGameStats)};
        }
        return {};
    }

    const char *OptionsManager::getGroupValueSelectedForKey(const std::string &group, const std::string &key) const
    {
        if (group == "Control")
        {
            if (key == "Input")
                return currentOptions.Control.Input;
            if (key == "Movement")
                return currentOptions.Control.Movement;
        }
        else if (group == "Audio")
        {
            if (key == "EffectsVolume")
                return currentOptions.Audio.EffectsVolume;
            if (key == "MusicVolume")
                return currentOptions.Audio.MusicVolume;
        }
        else if (group == "Video")
        {
            if (key == "WindowMode")
                return currentOptions.Video.WindowMode;
            if (key == "Resolution")
                return currentOptions.Video.Resolution;
            if (key == "Aspect")
                return currentOptions.Video.Aspect;
            if (key == "AntiAliasing")
                return currentOptions.Video.AntiAliasing;
            if (key == "VSync")
                return currentOptions.Video.VSync;
        }
        else if (group == "Extra")
        {
            if (key == "Language")
                return currentOptions.Extra.Language;
            if (key == "ColorScheme")
                return currentOptions.Extra.ColorScheme;
            if (key == "ButtonAppearance")
                return currentOptions.Extra.ButtonAppearance;
            if (key == "MeshCrusher")
                return currentOptions.Extra.MeshCrusher;
            if (key == "Particles")
                return currentOptions.Extra.Particles;
            if (key == "OnGameStats")
                return currentOptions.Extra.OnGameStats;
        }
        return nullptr;
    }

    char *OptionsManager::getGroupValueSelectedForKey(const std::string &group, const std::string &key)
    {
        if (group == "Control")
        {
            if (key == "Input")
                return currentOptions.Control.Input;
            if (key == "Movement")
                return currentOptions.Control.Movement;
        }
        else if (group == "Audio")
        {
            if (key == "EffectsVolume")
                return currentOptions.Audio.EffectsVolume;
            if (key == "MusicVolume")
                return currentOptions.Audio.MusicVolume;
        }
        else if (group == "Video")
        {
            if (key == "WindowMode")
                return currentOptions.Video.WindowMode;
            if (key == "Resolution")
                return currentOptions.Video.Resolution;
            if (key == "Aspect")
                return currentOptions.Video.Aspect;
            if (key == "AntiAliasing")
                return currentOptions.Video.AntiAliasing;
            if (key == "VSync")
                return currentOptions.Video.VSync;
        }
        else if (group == "Extra")
        {
            if (key == "Language")
                return currentOptions.Extra.Language;
            if (key == "ColorScheme")
                return currentOptions.Extra.ColorScheme;
            if (key == "ButtonAppearance")
                return currentOptions.Extra.ButtonAppearance;
            if (key == "MeshCrusher")
                return currentOptions.Extra.MeshCrusher;
            if (key == "Particles")
                return currentOptions.Extra.Particles;
            if (key == "OnGameStats")
                return currentOptions.Extra.OnGameStats;
        }
        return nullptr;
    }

    bool OptionsManager::setGroupValueSelectedForKey(const std::string &group, const std::string &key, const std::string &value)
    {
        char *currentValue = getGroupValueSelectedForKey(group, key);
        if (currentValue && strcmp(currentValue, value.c_str()) != 0)
        {
            snprintf(currentValue, 64, "%s", value.c_str());
            return true;
        }
        return false;
    }

    OptionsManager::OptionsManager()
    {
        initialized = false;
    }

    void OptionsManager::initializeDefaults()
    {
        AppOptions::create_default_binary_data_v1(&currentOptions);

        if (!initialized)
        {
            auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
            mainMonitor_InitialMode = defaultMonitor.getCurrentMode();

            mainMonitorFullscreenResolutions.clear();
            for (const auto &mode : STL_Tools::Reversal(defaultMonitor.modes))
            {
                mainMonitorFullscreenResolutions.push_back(MathCore::vec2i(mode.width, mode.height));
            }

            initialized = true;
        }

        checkSystemCompatibilityAfterLoad();
    }
    bool OptionsManager::loadOptionsFromBuffer(const Platform::ObjectBuffer &data)
    {
        ITKExtension::IO::Reader reader;

        if (!data.size || !reader.readFromBuffer(data, true))
            return false;
        // if (reader.readInt32() != 0x01)
        //     return false;
        // reader.readRaw(out_options, sizeof(AppOptions_v1));
        int32_t groups = reader.readInt32();
        for (int32_t g = 0; g < groups; g++)
        {
            std::string group = reader.readString();
            int32_t keys = reader.readInt32();
            for (int32_t k = 0; k < keys; k++)
            {
                std::string key = reader.readString();
                std::string value = reader.readString();
                setGroupValueSelectedForKey(group, key, value);
            }
        }

        return true;

        // if (!AppOptions::read_binary_data_v1(data, &currentOptions))
        //     return false;
        // checkSystemCompatibilityAfterLoad();
        // return true;
    }
    void OptionsManager::saveOptionsToBuffer(Platform::ObjectBuffer *output)
    {
        // AppOptions::write_binary_data_v1(currentOptions, output);

        ITKExtension::IO::Writer writer;
        // writer.writeInt32(0x01); // version
        // writer.writeRaw(&options, sizeof(AppOptions_v1));

        auto groups = getGroups();
        writer.writeInt32((int32_t)groups.size());
        for (auto group : groups)
        {
            writer.writeString(group);

            auto keys = getGroupKeys(group);
            writer.writeInt32((int32_t)keys.size());
            for (auto key : keys)
            {
                const char* value = getGroupValueSelectedForKey(group, key);
                writer.writeString(key);
                writer.writeString(value);
            }
        }

        writer.writeToBuffer(output, true);
    }

    void OptionsManager::checkSystemCompatibilityAfterLoad()
    {
        // needs to check the window mode and resolution
        auto validWindowModeList = getGroupValuesForKey("Video", "WindowMode");
        const char *currWindowMode = getGroupValueSelectedForKey("Video", "WindowMode");

        auto sel_it = std::find(validWindowModeList.begin(), validWindowModeList.end(), currWindowMode);
        if (sel_it == validWindowModeList.end())
        {
            // If the current window mode is not valid, reset to the first valid mode
            setGroupValueSelectedForKey("Video", "WindowMode", validWindowModeList.front());
        }

        const char *currResolution = getGroupValueSelectedForKey("Video", "Resolution");
        auto validResolutionList = getGroupValuesForKey("Video", "Resolution");
        sel_it = std::find(validResolutionList.begin(), validResolutionList.end(), currResolution);
        if (sel_it == validResolutionList.end())
        {
            // If the current resolution is not valid, reset to the first valid resolution
            setGroupValueSelectedForKey("Video", "Resolution", validResolutionList.front());
        }
    }

    OptionsManager *OptionsManager::Instance()
    {
        static OptionsManager instance;
        return &instance;
    }

    // compare current options with Singleton Instance options
    bool OptionsManager::hasChanged(const std::string &group, const std::string &key)
    {
        OptionsManager *globalOptions = OptionsManager::Instance();
        const char *currentValue = getGroupValueSelectedForKey(group, key);
        const char *globalValue = globalOptions->getGroupValueSelectedForKey(group, key);
        if (currentValue && globalValue)
        {
            return strcmp(currentValue, globalValue) != 0;
        }
        return false;
    }
    // compare all available options with Singleton Instance options
    bool OptionsManager::hasAnyChange()
    {
        for (const auto &group : OptionsConstants::groups)
        {
            for (const auto &key : getGroupKeys(group))
            {
                if (hasChanged(group, key))
                {
                    return true;
                }
            }
        }
        return false;
    }

}