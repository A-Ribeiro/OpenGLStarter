#pragma once

#include "./View/common.h"
#include "./View/all.h"

class ShortCut{
public: 
    bool ctrl;
    bool shift;
    bool alt;
    bool window;
    
    AppKit::Window::Devices::KeyCode keyCode;
    EventCore::PressReleaseDetector keyCodeState;

    EventCore::PressReleaseDetector shortCutState;

    std::string menuPath;//"Actions/Add"
    std::string shortcutStr;//"Ctrl+A"

    bool menuSetItemVisible;
    bool menuSetItemEnabled;

    ShortCut() {
        ctrl = false;
        shift = false;
        alt = false;
        window = false;

        keyCode = AppKit::Window::Devices::KeyCode::Unknown;

        menuPath = "";
        shortcutStr = "";

        menuSetItemVisible = false;
        menuSetItemEnabled = false;
    }

    ShortCut(
        const std::string &menuPath,//"Actions/Add"
        const std::string &shortcutStr,//"Ctrl+A"
        bool menuSetItemVisible,
        bool menuSetItemEnabled,
        bool ctrl,
    bool shift,
    bool alt,
    bool window,
    AppKit::Window::Devices::KeyCode keyCode,
    const EventCore::Callback<void()> &activate = nullptr,
    const EventCore::Callback<void()> &deactivate = nullptr
    ) {
        this->menuPath = menuPath;
        this->shortcutStr = shortcutStr;
        this->menuSetItemVisible = menuSetItemVisible;
        this->menuSetItemEnabled = menuSetItemEnabled;
        this->ctrl = ctrl;
        this->shift = shift;
        this->alt = alt;
        this->window = window;
        this->keyCode = keyCode;
        if (activate != nullptr)
            this->shortCutState.OnDown.add(activate);
        if (deactivate != nullptr)
            this->shortCutState.OnUp.add(deactivate);
    }
};

class ShortCutCategory{
    public:
    std::string name;
    std::string menuSetVisible;//"Actions"
    std::string menuSetEnabled;//"Actions"
    // bool menuSetItemVisible;
    // bool menuSetItemEnabled;
    std::vector<ShortCut> shortCuts;

    ShortCutCategory() {
        menuSetVisible = "";
        menuSetEnabled = "";
    }

    ShortCutCategory(
        std::string name,
        const std::string &menuSetVisible,
        const std::string &menuSetEnabled,
        // bool menuSetItemVisible,
        // bool menuSetItemEnabled,
        const std::vector<ShortCut> &shortcuts){
        this->name = name;
        this->menuSetVisible = menuSetVisible;
        this->menuSetEnabled = menuSetEnabled;
        // this->menuSetItemVisible = menuSetItemVisible;
        // this->menuSetItemEnabled = menuSetItemEnabled;
        this->shortCuts = shortcuts;
    }

    void createMenus();
    void setActive(bool active);
};

class ShortcutManager : public View
{
    std::map<
        std::string, 
        ShortCutCategory
    > shortCutMap;

public:

    ShortCutCategory actionMenu;

    ShortcutManager();

    ~ShortcutManager();

    View* Init();

    void RenderAndLogic();

    void addShortcut(
        const std::string &category,
        const std::string &menuSetVisible,
        const std::string &menuSetEnabled,
        // bool menuSetItemVisible,
        // bool menuSetItemEnabled,
        const std::vector<ShortCut> &shortcuts
    );

    void setActionShortCutByCategory(
        const std::string &category
    );
    
};