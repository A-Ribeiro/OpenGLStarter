#include "ShortcutManager.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"



void ShortCutCategory::createMenus() const {

    for(auto& shortcut : shortCuts) {
        ImGuiMenu::Instance()->AddMenu(
		shortcut.menuPath, 
        shortcut.shortcutStr, 
        [&]()
		{ 
            printf("-not-implemented-menu-action-\n");
        });
    }
    ImGuiMenu::Instance()->UpdateUI();

    setActive(false);
}

void ShortCutCategory::setActive(bool active) const {
    for(auto& shortcut : shortCuts) {
        auto &controller = ImGuiMenu::Instance()->getController(shortcut.menuPath);
        if (menuSetItemVisible)
            controller.visible = active;
        if (menuSetItemEnabled)
            controller.enabled = active;
    }

    if (menuSetVisible.length() != 0){
        auto &controller = ImGuiMenu::Instance()->getController(menuSetVisible);
        controller.visible = active;
    }

    if (menuSetEnabled.length() != 0){
        auto &controller = ImGuiMenu::Instance()->getController(menuSetEnabled);
        controller.enabled = active;
    }

    ImGuiMenu::Instance()->UpdateUI();
}


ShortcutManager::ShortcutManager(){
    
}

ShortcutManager::~ShortcutManager(){

}

View* ShortcutManager::Init(){

    // hierachy menu
    {
    	ImGuiMenu::Instance()->AddMenu(
		"Actions/Add", "Ctrl+A", [&]()
		{ 
            printf("Actions/Add\n");
        });
        // ImGuiMenu::Instance()->AddMenu(
		// "Actions/Cut", "Ctrl+X", [&]()
		// { 
        //     printf("Actions/Cut\n");
        // });
        // ImGuiMenu::Instance()->AddMenu(
		// "Actions/Copy", "Ctrl+C", [&]()
		// { 
        //     printf("Actions/Copy\n");
        // });
        // ImGuiMenu::Instance()->AddMenu(
		// "Actions/Paste", "Ctrl+V", [&]()
		// { 
        //     printf("Actions/Paste\n");
        // });


    }

    return this;
}

void ShortcutManager::RenderAndLogic(){

    using namespace AppKit::Window::Devices;

    bool ctrl_pressed = Keyboard::isPressed(KeyCode::LControl) || Keyboard::isPressed(KeyCode::RControl);
    bool shift_pressed = Keyboard::isPressed(KeyCode::LShift) || Keyboard::isPressed(KeyCode::RShift);
    bool alt_pressed = Keyboard::isPressed(KeyCode::LAlt) || Keyboard::isPressed(KeyCode::RAlt);
    bool window_pressed = Keyboard::isPressed(KeyCode::LSystem) || Keyboard::isPressed(KeyCode::RSystem);

    for(auto& shortcut : actionMenu.shortCuts){
        shortcut.keyCodeState.setState(Keyboard::isPressed(shortcut.keyCode));
        
        bool match_special_keys = 
        shortcut.ctrl == ctrl_pressed && 
        shortcut.shift == shift_pressed && 
        shortcut.alt == alt_pressed && 
        shortcut.window == window_pressed;

        bool pressed_state = match_special_keys && shortcut.keyCodeState.pressed;

        if (shortcut.keyCodeState.down || shortcut.shortCutState.pressed){
            shortcut.shortCutState.setState(pressed_state);
        }
    }

}

void ShortcutManager::addShortcut(
        const std::string &category,
        const std::string &menuSetVisible,
        const std::string &menuSetEnabled,
        bool menuSetItemVisible,
        bool menuSetItemEnabled,
        const std::vector<ShortCut> &shortcuts
){
    const auto &added = shortCutMap[category] = ShortCutCategory(
        menuSetVisible,
        menuSetEnabled,
        menuSetItemVisible,
        menuSetItemEnabled,
        shortcuts);
    
}

void ShortcutManager::setActionShortCutByCategory(
    const std::string &category
){
    auto it = shortCutMap.find(category);
    if (it != shortCutMap.end())
        actionMenu = it->second;
    else
        actionMenu = ShortCutCategory();
}
