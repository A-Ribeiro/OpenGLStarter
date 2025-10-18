#include "./ScreenOptions.h"
#include "./ScreenManager.h"
#include "./Options/TopBar.h"
#include "./Options/OptionSet.h"
#include "./ScreenMessageBox.h"

void save_options();

namespace ui
{
    void ScreenOptions::layoutElements(const MathCore::vec2i &size)
    {

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);

        auto bg = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();
        bg->getTransform()->setLocalPosition(MathCore::vec3f(0, -top_bar_height * 0.5f, 0));
        bg->setQuad(
            uiComponent->resourceMap,
            MathCore::vec2f(valid_size.x, valid_size.y - top_bar_height), // size
            screenManager->colorPalette.primary,                          // color
            MathCore::vec4f(32, 32, 32, 32),                              // radius
            AppKit::GLEngine::Components::StrokeModeGrowInside,           // stroke mode
            screenManager->colorPalette.stroke_thickness,                 // stroke thickness
            screenManager->colorPalette.primary_stroke,                   // stroke color
            0,                                                            // drop shadow thickness
            MathCore::vec4f(0),                                           // drop shadow color
            AppKit::GLEngine::Components::MeshUploadMode_Direct           // meshUploadMode,
        );

        if (topBar)
            topBar->layoutElements(size);

        for (auto &item : optionMap)
            item.second->layoutElements(size);
    }

    void ScreenOptions::activeCurrentTab()
    {
        if (!topBar)
            return;
        auto tab_name = topBar->getSelectedButtonName();
        for (auto &option : this->optionMap)
        {
            if (option.first == tab_name)
                option.second->show();
            else
                option.second->hide();
        }
    }

    const char *ScreenOptions::Name = "ScreenOptions";

    void ScreenOptions::releaseLocalContext()
    {
        localContext.reset();

        if (topBar)
        {
            uiComponent->getItemByComponent(topBar->ui).removeSelf();
            topBar = nullptr;
        }

        for (auto &option : optionMap)
        {
            uiComponent->getItemByComponent(option.second->ui).removeSelf();
        }
        optionMap.clear();
    }
    void ScreenOptions::loadOptionsFromGlobalContext()
    {
        localContext = STL_Tools::make_unique<AppOptions::OptionsManager>();
        *localContext = *AppOptions::OptionsManager::Instance();

        topBar = std::make_shared<TopBar>();
        topBar->initialize(localContext->getGroups(),
                           uiComponent, screenManager);

        auto mask = uiComponent->getItemByName("bg").get<AppKit::GLEngine::Components::ComponentRectangle>();

        for (const auto &group : localContext->getGroups())
        {
            auto optSet = STL_Tools::make_unique<OptionSet>();
            optSet->initialize(group, uiComponent, screenManager, mask);

            for (auto group_key : localContext->getGroupKeys(group))
            {
                auto key_entries = localContext->getGroupValuesForKey(group, group_key);
                auto key_selected_entry = localContext->getGroupValueSelectedForKey(group, group_key);
                optSet->addOption(group_key, key_entries, key_selected_entry);

                auto &optionItem = optSet->getItemByOptionName(group_key);
                optionItem.onChange = [this, group, group_key](const std::string &selected)
                {
                    bool updated = localContext->setGroupValueSelectedForKey(group, group_key, selected);
                    if (updated && group == "Video" && group_key == "WindowMode")
                    {
                        // update resolution options
                        // try to keep the already set option from Global Context
                        auto new_key_entries = localContext->getGroupValuesForKey("Video", "Resolution");
                        const char* new_key_selected_entry = AppOptions::OptionsManager::Instance()->getGroupValueSelectedForKey("Video", "Resolution");
                        if (std::find(new_key_entries.begin(), new_key_entries.end(), new_key_selected_entry) == new_key_entries.end())
                            new_key_selected_entry = new_key_entries.back().c_str();
                        optionMap["Video"]->updateOption("Resolution", new_key_entries, new_key_selected_entry);
                    }
                };

                optionItem.onChange(optionItem.selected);
            }

            optionMap[group] = std::move(optSet);
        }

        activeCurrentTab();
        layoutElements(screenManager->current_size);
    }

    void ScreenOptions::onOsciloscopeAction()
    {
        printf("Action at selection end...");
    }
    void ScreenOptions::onOsciloscopeSinLerp(Platform::Time *elapsed, float osciloscope, float sin)
    {
        if (topBar)
        {
            auto tab_name = topBar->getSelectedButtonName();
            optionMap[tab_name]->update(elapsed, osciloscope, sin);
        }
    }

    ScreenOptions::ScreenOptions() : OsciloscopeWithTrigger(
                                         ScreenOptions::osciloscope_normal_hz,
                                         ScreenOptions::osciloscope_selected_hz,
                                         ScreenOptions::osciloscope_countdown_trigger_secs)
    {
        screenManager = nullptr;
    }

    std::string ScreenOptions::name() const
    {
        return Name;
    }

    void ScreenOptions::resize(const MathCore::vec2i &size)
    {
        printf("    [ScreenOptions] resize %i %i\n", size.width, size.height);
        layoutElements(size);
    }

    void ScreenOptions::update(Platform::Time *elapsed)
    {
        if (uiComponent->items.size() == 0)
            return;
        if (!screenManager->screen_in_top_byInstance(this))
            return;

        osciloscopeUpdate(elapsed);
    }

    std::shared_ptr<AppKit::GLEngine::Transform> ScreenOptions::initializeTransform(
        AppKit::GLEngine::Engine *engine,
        AppKit::GLEngine::ResourceMap *resourceMap,
        MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
        ui::ScreenManager *screenManager,
        const MathCore::vec2i &size)
    {
        if (uiNode)
            return uiNode;
        this->screenManager = screenManager;
        printf("    [ScreenOptions] initializeTransform\n");
        uiNode = AppKit::GLEngine::Transform::CreateShared(name());
        uiNode->skip_traversing = true;

        uiComponent = uiNode->addNewComponent<AppKit::GLEngine::Components::ComponentUI>();
        uiComponent->Initialize(resourceMap);

        auto valid_size = MathCore::vec2f(size.width - ScreenOptions::margin * 2.0f,
                                          size.height - ScreenOptions::margin * 2.0f);
        auto mask = uiComponent->addRectangle(
                                   MathCore::vec2f(0, -top_bar_height * 0.5f),                   // pos
                                   MathCore::vec2f(valid_size.x, valid_size.y - top_bar_height), // size
                                   screenManager->colorPalette.primary,                          // color
                                   MathCore::vec4f(0, 32, 32, 0),                                // radius
                                   AppKit::GLEngine::Components::StrokeModeGrowInside,           // stroke mode
                                   screenManager->colorPalette.stroke_thickness,                 // stroke thickness
                                   screenManager->colorPalette.primary_stroke,                   // stroke color
                                   0,                                                            // drop shadow thickness
                                   MathCore::vec4f(0),                                           // drop shadow color
                                   0,                                                            // z
                                   "bg")
                        .get<AppKit::GLEngine::Components::ComponentRectangle>();

        // topBar = std::make_shared<TopBar>();
        // topBar->initialize({"Control",
        //                     "Audio",
        //                     "Video",
        //                     "Extra"},
        //                    uiComponent, screenManager);

        // // Control
        // {
        //     std::string btn_set_name = "Control";

        //     auto optSet = STL_Tools::make_unique<OptionSet>();
        //     optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

        //     optSet->addOption("Input", {"Steam 1", "Keyboard", "Steam 1 + Keyboard"}, "Steam 1 + Keyboard");
        //     optSet->addOption("Movement", {"Fluid", "Legacy"}, "Fluid");

        //     optionMap[btn_set_name] = std::move(optSet);
        // }
        // // Audio
        // {
        //     std::string btn_set_name = "Audio";

        //     auto optSet = STL_Tools::make_unique<OptionSet>();
        //     optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

        //     optSet->addOption("Effects Volume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100");
        //     optSet->addOption("Music Volume", {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", "100"}, "100");

        //     optionMap[btn_set_name] = std::move(optSet);
        // }
        // // Video
        // {
        //     std::string btn_set_name = "Video";

        //     auto optSet = STL_Tools::make_unique<OptionSet>();
        //     optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

        //     optSet->addOption("Window Mode", {"Window", "Borderless", "Fullscreen"}, "Borderless");
        //     optSet->addOption("Resolution", {"2560x1440", "1920x1080", "1280x720", "854x480"}, "1920x1080");
        //     optSet->addOption("Aspect", {"16:9", "16:10"}, "16:9");
        //     optSet->addOption("Anti-Aliasing", {"MSAA", "OFF"}, "MSAA");
        //     optSet->addOption("VSync", {"ON", "OFF"}, "ON");

        //     optionMap[btn_set_name] = std::move(optSet);
        // }
        // // Extra
        // {
        //     std::string btn_set_name = "Extra";

        //     auto optSet = STL_Tools::make_unique<OptionSet>();
        //     optSet->initialize(btn_set_name, uiComponent, screenManager, mask);

        //     optSet->addOption("Language", {"English", "Português (BR)"}, "English");
        //     optSet->addOption("ColorScheme", {"Purple", "Orange", "Green", "Blue"}, "Purple");
        //     optSet->addOption("MeshCrusher", {"ON", "OFF"}, "ON");
        //     optSet->addOption("Particles", {"Low", "Medium", "High", "Ultra"}, "High");
        //     optSet->addOption("On Game Stats", {"FPS", "OFF"}, "OFF");

        //     optionMap[btn_set_name] = std::move(optSet);
        // }

        activeCurrentTab();

        return uiNode;
    }

    void ScreenOptions::triggerEvent(ui::UIEventEnum event)
    {
        printf("    [ScreenOptions] %s\n", UIEventToStr(event));
        if (event == UIEventEnum::UIEvent_ScreenPush)
        {
            uiNode->skip_traversing = false;

            releaseLocalContext();
            loadOptionsFromGlobalContext();

            osciloscopeResetLock();
        }
        else if (event == UIEventEnum::UIEvent_ScreenPop)
        {
            uiNode->skip_traversing = true;

            releaseLocalContext();
        }
        else if (!osciloscopeIsLocked())
        {
            if (event == UIEventEnum::UIEvent_InputActionEnter)
            {
                // osciloscopeTriggerAction();
            }
            else if (event == UIEventEnum::UIEvent_InputDown)
            {
                if (topBar)
                {
                    auto tab_name = topBar->getSelectedButtonName();
                    optionMap[tab_name]->downButton();
                }
            }
            else if (event == UIEventEnum::UIEvent_InputUp)
            {
                if (topBar)
                {
                    auto tab_name = topBar->getSelectedButtonName();
                    optionMap[tab_name]->upButton();
                }
            }
            else if (event == UIEventEnum::UIEvent_InputLeft)
            {
                if (topBar)
                {
                    auto tab_name = topBar->getSelectedButtonName();
                    optionMap[tab_name]->leftButton();
                }
            }
            else if (event == UIEventEnum::UIEvent_InputRight)
            {
                if (topBar)
                {
                    auto tab_name = topBar->getSelectedButtonName();
                    optionMap[tab_name]->rightButton();
                }
            }
            else if (event == UIEventEnum::UIEvent_InputActionBack)
            {
                screenManager->screen<ScreenMessageBox>()->showMessageBox( //
                    "Are you sure you want to go back?",
                    {"Yes", "No"}, // options
                    "Yes",         // init selected
                    [this](const std::string &option)
                    {
                        if (option == "Yes")
                        {
                            save_options();
                            screenManager->open_screen("ScreenMain");
                        }
                        else
                        {
                            screenManager->pop_screen();
                        }
                    });
            }
            else if (event == UIEventEnum::UIEvent_InputShoulderRight)
            {
                if (topBar)
                    topBar->shoulderNext();
                activeCurrentTab();
            }
            else if (event == UIEventEnum::UIEvent_InputShoulderLeft)
            {
                if (topBar)
                    topBar->shoulderPrevious();
                activeCurrentTab();
            }
        }
    }

}