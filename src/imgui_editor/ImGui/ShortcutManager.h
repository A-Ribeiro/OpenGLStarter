#pragma once

#include "./View/common.h"
#include "./View/all.h"

enum class MenuBehaviour : uint8_t {
    None,
    SetItemVisibility,
    SetItemEnabled
};

class ShortCut{
public: 
    bool ctrl;
    bool shift;
    bool alt;
    bool window;
    
    AppKit::Window::Devices::KeyCode keyCode;
    EventCore::PressReleaseDetector keyCodeState;

    EventCore::PressReleaseDetector shortCutState;

    std::string mainMenuPath;//"Actions/Add"
    MenuBehaviour mainMenuBehaviour;

    std::string contextMenuPath;//"Add"
    MenuBehaviour contextMenuBehaviour;

    std::string shortcutStr;//"Ctrl+A"

    ShortCut() {
        ctrl = false;
        shift = false;
        alt = false;
        window = false;

        keyCode = AppKit::Window::Devices::KeyCode::Unknown;

        mainMenuPath = "";
        mainMenuBehaviour = MenuBehaviour::None;

        contextMenuPath = "";
        contextMenuBehaviour = MenuBehaviour::None;

        shortcutStr = "";
    }

    ShortCut(
        const std::string &mainMenuPath,//"Actions/Add"
        MenuBehaviour mainMenuBehaviour,

        const std::string &contextMenuPath = "",//"Add"
        MenuBehaviour contextMenuBehaviour = MenuBehaviour::None,

        const std::string &shortcutStr = "",//"Ctrl+A"

        bool ctrl = false,
        bool shift = false,
        bool alt = false,
        bool window = false,

    AppKit::Window::Devices::KeyCode keyCode = AppKit::Window::Devices::KeyCode::Unknown,

    const EventCore::Callback<void()> &activate = nullptr,
    const EventCore::Callback<void()> &deactivate = nullptr
    ) {
        this->mainMenuPath = mainMenuPath;
        this->mainMenuBehaviour = mainMenuBehaviour;

        this->contextMenuPath = contextMenuPath;
        this->contextMenuBehaviour = contextMenuBehaviour;

        this->shortcutStr = shortcutStr;
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
    std::vector<ShortCut> shortCuts;

    ShortCutCategory() {
    }

    ShortCutCategory(
        std::string name,
        const std::vector<ShortCut> &shortcuts){
        this->name = name;
        this->shortCuts = shortcuts;
    }

    void createMenus();
    void setActive(bool active);
};

class ShortcutManager : public View
{
    std::unordered_map<
        std::string, 
        ShortCutCategory
    > shortCutMap;

    int lock_change_action_category;

public:
    static const ViewType Type;

    ShortCutCategory actionMenu;

    void lockChangeActionCategory();
    void unlockChangeActionCategory();

    bool input_blocked;

    ShortcutManager();

    ~ShortcutManager();

    View* Init();

    void RenderAndLogic();

    void addShortcut(
        const std::string &category,
        const std::vector<ShortCut> &shortcuts
    );

    void setActionShortCutByCategory(
        const std::string &category
    );
    
};