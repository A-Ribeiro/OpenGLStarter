#include <appkit-ui/util/AppOptions.h>
#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

namespace AppKit
{
    namespace ui
    {
        // void write_binary_data_v1(const std::vector<OptionsGroup> &options, Platform::ObjectBuffer *output)
        // {
        //     ITKExtension::IO::Writer writer;
        //     writer.writeInt32(0x01); // version
        //     writer.writeRaw(&options, sizeof(AppOptions_v1));
        //     writer.writeToBuffer(output);
        // }

        // bool read_binary_data_v1(const Platform::ObjectBuffer &data, std::vector<OptionsGroup> *out_options)
        // {
        //     ITKExtension::IO::Reader reader;

        //     if (!reader.readFromBuffer(data))
        //         return false;
        //     if (reader.readInt32() != 0x01)
        //         return false;
        //     reader.readRaw(out_options, sizeof(AppOptions_v1));

        //     return true;
        // }

        // void create_default_binary_data_v1(std::vector<OptionsGroup> *options, std::unordered_map<std::string, std::unordered_map<std::string, std::string>> *optionsState)
        // {
        //     (*options) = {{"Control",
        //                    {
        //                        {"Input", {"Steam 1", "Keyboard", "Steam 1 + Keyboard"}, "Steam 1 + Keyboard"},
        //                        {"Movement", {"Fluid", "Legacy"}, "Fluid"},
        //                    }},
        //                   {"Audio",
        //                    {
        //                        {"EffectsVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
        //                        {"MusicVolume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100"},
        //                    }},
        //                   {"Video",
        //                    {
        //                        {"WindowMode", {"Window", "Borderless", "Fullscreen"}, "Window"},
        //                        {"Resolution", {}, ""},
        //                        {"Aspect", {"16:9", "16:10"}, "16:9"},
        //                        {"AntiAliasing", {"MSAA", "OFF"}, "MSAA"},
        //                        {"VSync", {"ON", "OFF"}, "ON"},
        //                    }},
        //                   {"Extra",
        //                    {
        //                        {"Language", {"English", "Português (BR)"}, "English"},
        //                        {"ColorScheme", {"Blush", "Purple", "Orange", "Green", "Blue", "Dark"}, "Blush"},
        //                        {"ButtonAppearance", {"Bend Up", "Bend Down", "Round", "Tip Front", "Tip Back", "Tip Up", "Tip Down", "Square"}, "Bend Up"},
        //                        {"UiSize", {"Extra Small", "Small", "Medium", "Large", "Extra Large"}, "Medium"},
        //                        {"MeshCrusher", {"ON", "OFF"}, "OFF"},
        //                        {"Particles", {"Low", "Medium", "High", "Ultra"}, "High"},
        //                        {"OnGameStats", {"OFF", "FPS"}, "OFF"},
        //                    }}};
        //     (*optionsState).clear();

        //     for (const auto &group : *options)
        //     {
        //         for (const auto &key : group.keys)
        //         {
        //             (*optionsState)[group.group_name][key.key_name] = key.default_option;
        //         }
        //     }

        //     // memset(options, 0, sizeof(AppOptions_v1));
        //     // snprintf(options->Control.Input, sizeof(options->Control.Input), "%s", OptionsConstants::ControlInput[2]);
        //     // snprintf(options->Control.Movement, sizeof(options->Control.Movement), "%s", OptionsConstants::ControlMovement[0]);

        //     // snprintf(options->Audio.EffectsVolume, sizeof(options->Audio.EffectsVolume), "%s", OptionsConstants::AudioVolume[10]);
        //     // snprintf(options->Audio.MusicVolume, sizeof(options->Audio.MusicVolume), "%s", OptionsConstants::AudioVolume[10]);

        //     // snprintf(options->Video.WindowMode, sizeof(options->Video.WindowMode), "%s", OptionsConstants::VideoWindowMode[0]);
        //     // snprintf(options->Video.Resolution, sizeof(options->Video.Resolution), "%s", "");
        //     // snprintf(options->Video.Aspect, sizeof(options->Video.Aspect), "%s", OptionsConstants::VideoAspect[0]);
        //     // snprintf(options->Video.AntiAliasing, sizeof(options->Video.AntiAliasing), "%s", OptionsConstants::VideoAntiAliasing[0]);
        //     // snprintf(options->Video.VSync, sizeof(options->Video.VSync), "%s", OptionsConstants::VideoVSync[0]);

        //     // snprintf(options->Extra.Language, sizeof(options->Extra.Language), "%s", OptionsConstants::ExtraLanguage[0]);
        //     // snprintf(options->Extra.ColorScheme, sizeof(options->Extra.ColorScheme), "%s", OptionsConstants::ExtraColorScheme[0]);
        //     // snprintf(options->Extra.ButtonAppearance, sizeof(options->Extra.ButtonAppearance), "%s", OptionsConstants::ExtraButtonAppearance[0]);
        //     // snprintf(options->Extra.UiSize, sizeof(options->Extra.UiSize), "%s", OptionsConstants::ExtraUiSize[2]);
        //     // snprintf(options->Extra.MeshCrusher, sizeof(options->Extra.MeshCrusher), "%s", OptionsConstants::ExtraMeshCrusher[1]);
        //     // snprintf(options->Extra.Particles, sizeof(options->Extra.Particles), "%s", OptionsConstants::ExtraParticles[2]);
        //     // snprintf(options->Extra.OnGameStats, sizeof(options->Extra.OnGameStats), "%s", OptionsConstants::ExtraOnGameStats[0]);
        // }

        std::vector<std::string> OptionsManager::getGroups() const
        {
            std::vector<std::string> groups;
            for (const auto &group : options)
                groups.push_back(group.group_name);
            return groups;
            // return {std::begin(AppOptions::OptionsConstants::groups), std::end(AppOptions::OptionsConstants::groups)};
        }
        std::vector<std::string> OptionsManager::getGroupKeys(const std::string &group) const
        {
            for (const auto &g : options)
            {
                if (group == g.group_name)
                {
                    std::vector<std::string> keys;
                    for (const auto &key : g.keys)
                        keys.push_back(key.key_name);
                    return keys;
                }
            }
            // if (group == "Control")
            // {
            //     return {"Input", "Movement"};
            // }
            // else if (group == "Audio")
            // {
            //     return {"EffectsVolume", "MusicVolume"};
            // }
            // else if (group == "Video")
            // {
            //     return {"WindowMode", "Resolution", "Aspect", "AntiAliasing", "VSync"};
            // }
            // else if (group == "Extra")
            // {
            //     return {"Language", "ColorScheme", "ButtonAppearance", "UiSize", "MeshCrusher", "Particles", "OnGameStats"};
            // }
            return {};
        }

        std::vector<std::string> OptionsManager::getGroupValuesForKey(const std::string &group, const std::string &key) const
        {
            if (group == "Video" && key == "Resolution")
            {
                const std::string &currWindowMode = getGroupValueSelectedForKey("Video", "WindowMode");
                if (currWindowMode == "Borderless")
                {
                    auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                    return {ITKCommon::PrintfToStdString("%ix%i", defaultMonitor.width, defaultMonitor.height)};
                }
                else if (currWindowMode == "Fullscreen")
                {
                    std::vector<std::string> res;
                    for (const auto &mode : this->mainMonitorFullscreenResolutions)
                        res.push_back(ITKCommon::PrintfToStdString("%ix%i", mode.x, mode.y));
                    return res;
                }
                else // windowed
                    return {"-"};
            }
            else
            {
                for (const auto &g : options)
                {
                    if (group == g.group_name)
                    {
                        for (const auto &k : g.keys)
                        {
                            if (key == k.key_name)
                            {
                                std::vector<std::string> values;
                                for (const auto &value : k.options)
                                    values.push_back(value);
                                return values;
                            }
                        }
                    }
                }
            }

            // if (group == "Control")
            // {
            //     if (key == "Input")
            //         return {std::begin(AppOptions::OptionsConstants::ControlInput), std::end(AppOptions::OptionsConstants::ControlInput)};
            //     if (key == "Movement")
            //         return {std::begin(AppOptions::OptionsConstants::ControlMovement), std::end(AppOptions::OptionsConstants::ControlMovement)};
            // }
            // else if (group == "Audio")
            // {
            //     if (key == "EffectsVolume")
            //         return {std::begin(AppOptions::OptionsConstants::AudioVolume), std::end(AppOptions::OptionsConstants::AudioVolume)};
            //     if (key == "MusicVolume")
            //         return {std::begin(AppOptions::OptionsConstants::AudioVolume), std::end(AppOptions::OptionsConstants::AudioVolume)};
            // }
            // else if (group == "Video")
            // {
            //     if (key == "WindowMode")
            //         return {std::begin(AppOptions::OptionsConstants::VideoWindowMode), std::end(AppOptions::OptionsConstants::VideoWindowMode)};
            //     if (key == "Resolution")
            //     {
            //         const char *currWindowMode = getGroupValueSelectedForKey("Video", "WindowMode");
            //         if (strcmp(currWindowMode, "Borderless") == 0)
            //         {
            //             auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
            //             return {ITKCommon::PrintfToStdString("%ix%i", defaultMonitor.width, defaultMonitor.height)};
            //         }
            //         else if (strcmp(currWindowMode, "Fullscreen") == 0)
            //         {
            //             std::vector<std::string> res;
            //             for (const auto &mode : this->mainMonitorFullscreenResolutions)
            //                 res.push_back(ITKCommon::PrintfToStdString("%ix%i", mode.x, mode.y));
            //             return res;
            //         }
            //         else // windowed
            //             return {"-"};
            //     }
            //     if (key == "Aspect")
            //         return {std::begin(AppOptions::OptionsConstants::VideoAspect), std::end(AppOptions::OptionsConstants::VideoAspect)};
            //     if (key == "AntiAliasing")
            //         return {std::begin(AppOptions::OptionsConstants::VideoAntiAliasing), std::end(AppOptions::OptionsConstants::VideoAntiAliasing)};
            //     if (key == "VSync")
            //         return {std::begin(AppOptions::OptionsConstants::VideoVSync), std::end(AppOptions::OptionsConstants::VideoVSync)};
            // }
            // else if (group == "Extra")
            // {
            //     if (key == "Language")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraLanguage), std::end(AppOptions::OptionsConstants::ExtraLanguage)};
            //     if (key == "ColorScheme")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraColorScheme), std::end(AppOptions::OptionsConstants::ExtraColorScheme)};
            //     if (key == "ButtonAppearance")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraButtonAppearance), std::end(AppOptions::OptionsConstants::ExtraButtonAppearance)};
            //     if (key == "UiSize")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraUiSize), std::end(AppOptions::OptionsConstants::ExtraUiSize)};
            //     if (key == "MeshCrusher")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraMeshCrusher), std::end(AppOptions::OptionsConstants::ExtraMeshCrusher)};
            //     if (key == "Particles")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraParticles), std::end(AppOptions::OptionsConstants::ExtraParticles)};
            //     if (key == "OnGameStats")
            //         return {std::begin(AppOptions::OptionsConstants::ExtraOnGameStats), std::end(AppOptions::OptionsConstants::ExtraOnGameStats)};
            // }
            return {};
        }

        const std::string &OptionsManager::getGroupValueSelectedForKey(const std::string &group, const std::string &key) const
        {
            auto groupIt = optionsState.find(group);
            if (groupIt != optionsState.end())
            {
                auto keyIt = groupIt->second.find(key);
                if (keyIt != groupIt->second.end())
                    return keyIt->second;
            }

            // if (group == "Control")
            // {
            //     if (key == "Input")
            //         return currentOptions.Control.Input;
            //     if (key == "Movement")
            //         return currentOptions.Control.Movement;
            // }
            // else if (group == "Audio")
            // {
            //     if (key == "EffectsVolume")
            //         return currentOptions.Audio.EffectsVolume;
            //     if (key == "MusicVolume")
            //         return currentOptions.Audio.MusicVolume;
            // }
            // else if (group == "Video")
            // {
            //     if (key == "WindowMode")
            //         return currentOptions.Video.WindowMode;
            //     if (key == "Resolution")
            //         return currentOptions.Video.Resolution;
            //     if (key == "Aspect")
            //         return currentOptions.Video.Aspect;
            //     if (key == "AntiAliasing")
            //         return currentOptions.Video.AntiAliasing;
            //     if (key == "VSync")
            //         return currentOptions.Video.VSync;
            // }
            // else if (group == "Extra")
            // {
            //     if (key == "Language")
            //         return currentOptions.Extra.Language;
            //     if (key == "ColorScheme")
            //         return currentOptions.Extra.ColorScheme;
            //     if (key == "ButtonAppearance")
            //         return currentOptions.Extra.ButtonAppearance;
            //     if (key == "UiSize")
            //         return currentOptions.Extra.UiSize;
            //     if (key == "MeshCrusher")
            //         return currentOptions.Extra.MeshCrusher;
            //     if (key == "Particles")
            //         return currentOptions.Extra.Particles;
            //     if (key == "OnGameStats")
            //         return currentOptions.Extra.OnGameStats;
            // }
            // return nullptr;
            static std::string emptyString = "";
            return emptyString;
        }

        // char *OptionsManager::getGroupValueSelectedForKey(const std::string &group, const std::string &key)
        // {
        //     if (group == "Control")
        //     {
        //         if (key == "Input")
        //             return currentOptions.Control.Input;
        //         if (key == "Movement")
        //             return currentOptions.Control.Movement;
        //     }
        //     else if (group == "Audio")
        //     {
        //         if (key == "EffectsVolume")
        //             return currentOptions.Audio.EffectsVolume;
        //         if (key == "MusicVolume")
        //             return currentOptions.Audio.MusicVolume;
        //     }
        //     else if (group == "Video")
        //     {
        //         if (key == "WindowMode")
        //             return currentOptions.Video.WindowMode;
        //         if (key == "Resolution")
        //             return currentOptions.Video.Resolution;
        //         if (key == "Aspect")
        //             return currentOptions.Video.Aspect;
        //         if (key == "AntiAliasing")
        //             return currentOptions.Video.AntiAliasing;
        //         if (key == "VSync")
        //             return currentOptions.Video.VSync;
        //     }
        //     else if (group == "Extra")
        //     {
        //         if (key == "Language")
        //             return currentOptions.Extra.Language;
        //         if (key == "ColorScheme")
        //             return currentOptions.Extra.ColorScheme;
        //         if (key == "ButtonAppearance")
        //             return currentOptions.Extra.ButtonAppearance;
        //         if (key == "UiSize")
        //             return currentOptions.Extra.UiSize;
        //         if (key == "MeshCrusher")
        //             return currentOptions.Extra.MeshCrusher;
        //         if (key == "Particles")
        //             return currentOptions.Extra.Particles;
        //         if (key == "OnGameStats")
        //             return currentOptions.Extra.OnGameStats;
        //     }
        //     return nullptr;
        // }

        bool OptionsManager::setGroupValueSelectedForKey(const std::string &group, const std::string &key, const std::string &value)
        {
            const std::string &currentValue = getGroupValueSelectedForKey(group, key);
            if (currentValue != value)
            {
                optionsState[group][key] = value;
                // currentValue = value;
                return true;
            }
            return false;
        }

        OptionsManager::OptionsManager()
        {
            initialized = false;
        }

        void OptionsManager::initializeDefaults(const std::vector<OptionsGroup> &options_p)
        {
            // AppOptions::create_default_binary_data_v1(&options, &optionsState);
            options = options_p;
            optionsState.clear();
            for (const auto &group : options)
                for (const auto &key : group.keys)
                    optionsState[group.group_name][key.key_name] = key.default_option;

            if (!initialized)
            {
                auto defaultMonitor = DPI::Display::QueryMonitors(true)[0];
                mainMonitor_InitialMode = defaultMonitor.getCurrentMode();

                mainMonitorFullscreenResolutions.clear();
                for (const auto &mode : STL_Tools::Reversal(defaultMonitor.modes))
                    mainMonitorFullscreenResolutions.push_back(MathCore::vec2i(mode.width, mode.height));

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
                    const std::string &value = getGroupValueSelectedForKey(group, key);
                    writer.writeString(key);
                    writer.writeString(value);
                }
            }

            writer.writeToBuffer(output, true);
        }

        void OptionsManager::checkSystemCompatibilityAfterLoad()
        {

            for (const auto &g : options)
            {
                for (const auto &key : g.keys)
                {
                    if (strcmp(g.group_name, "Video") == 0 && strcmp(key.key_name, "Resolution") == 0)
                        continue;
                    auto valid_options = getGroupValuesForKey(g.group_name, key.key_name);
                    const std::string &curr_value = getGroupValueSelectedForKey(g.group_name, key.key_name);
                    auto valid_options_it = std::find(valid_options.begin(), valid_options.end(), curr_value);
                    // if any option not found, set to default
                    if (valid_options_it == valid_options.end())
                        setGroupValueSelectedForKey(g.group_name, key.key_name, key.default_option);
                }
            }

            // // needs to check the window mode and resolution
            // auto validWindowModeList = getGroupValuesForKey("Video", "WindowMode");
            // const std::string &currWindowMode = getGroupValueSelectedForKey("Video", "WindowMode");

            // auto sel_it = std::find(validWindowModeList.begin(), validWindowModeList.end(), currWindowMode);
            // if (sel_it == validWindowModeList.end())
            // {
            //     // If the current window mode is not valid, reset to the first valid mode
            //     setGroupValueSelectedForKey("Video", "WindowMode", validWindowModeList.front());
            // }

            const std::string &currResolution = getGroupValueSelectedForKey("Video", "Resolution");
            auto validResolutionList = getGroupValuesForKey("Video", "Resolution");
            auto validResolutionList_it = std::find(validResolutionList.begin(), validResolutionList.end(), currResolution);
            // If the current resolution is not valid, reset to the first valid resolution
            if (validResolutionList_it == validResolutionList.end())
                setGroupValueSelectedForKey("Video", "Resolution", validResolutionList.front());
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
            const std::string &currentValue = getGroupValueSelectedForKey(group, key);
            const std::string &globalValue = globalOptions->getGroupValueSelectedForKey(group, key);
            if (!currentValue.empty() && !globalValue.empty())
                return currentValue != globalValue;
            return false;
        }
        // compare all available options with Singleton Instance options
        bool OptionsManager::hasAnyChange()
        {
            for (const auto &g : options)
            {
                for (const auto &key : g.keys)
                {
                    if (hasChanged(g.group_name, key.key_name))
                        return true;
                }
            }
            // for (const auto &group : OptionsConstants::groups)
            // {
            //     for (const auto &key : getGroupKeys(group))
            //     {
            //         if (hasChanged(group, key))
            //         {
            //             return true;
            //         }
            //     }
            // }
            return false;
        }
    }
}