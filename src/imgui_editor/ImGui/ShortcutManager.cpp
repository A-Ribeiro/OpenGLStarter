#include "ShortcutManager.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"



void ShortCutCategory::createMenus() {

    for(auto& shortcut : shortCuts) {
        ImGuiMenu::Instance()->AddMenu(
		shortcut.menuPath, 
        shortcut.shortcutStr, 
        nullptr);
    }
    ImGuiMenu::Instance()->UpdateUI();

    setActive(false);
}

void ShortCutCategory::setActive(bool active) {
    for(auto& shortcut : shortCuts) {
        auto &controller = ImGuiMenu::Instance()->getController(shortcut.menuPath);
        if (shortcut.menuSetItemVisible){
            printf("[ShortCutCategory::setActive] individual visible: %i\n", (int)active);
            controller.visible = active;
        }
        if (shortcut.menuSetItemEnabled){
            printf("[ShortCutCategory::setActive] individual enabled: %i\n", (int)active);
            controller.enabled = active;
        }

        if (active){
            controller.callback = [&shortcut]()
            { 
                if (shortcut.shortCutState.pressed)
                    return;
                shortcut.shortCutState.setState(true);
                shortcut.shortCutState.setState(false);
            };}
        else {
            controller.callback = nullptr;
        }

    }

    if (menuSetVisible.length() != 0){
        printf("[ShortCutCategory::setActive] update visible\n");
        auto &controller = ImGuiMenu::Instance()->getController(menuSetVisible);
        controller.visible = active;
    }

    if (menuSetEnabled.length() != 0){
        printf("[ShortCutCategory::setActive] update enabled\n");
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
    	// ImGuiMenu::Instance()->AddMenu(
		// "Actions/Add", "Ctrl+A", [&]()
		// { 
        //     printf("Actions/Add\n");
        // });
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

        using namespace AppKit::Window::Devices;

        addShortcut(
            "Action:Template",//category
            "",//menuSetVisible
            "",//menuSetEnabled
            {
                ShortCut(
                    "Action/Add",//menuPath
                    "Ctrl+A",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::A, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("activate Ctrl+A\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+A\n");
                    }
                ),
                ShortCut(
                    "Action/Remove",//menuPath
                    "Ctrl+D",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::D, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("activate Ctrl+D\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+D\n");
                    }
                ),
                ShortCut(
                    "Action/<<>>",//menuPath
                    "",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),
                ShortCut(
                    "Action/Copy",//menuPath
                    "Ctrl+C",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::C, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("activate Ctrl+C\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+C\n");
                    }
                ),
                ShortCut(
                    "Action/Cut",//menuPath
                    "Ctrl+X",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::X, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("activate Ctrl+X\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+X\n");
                    }
                ),
                ShortCut(
                    "Action/Paste",//menuPath
                    "Ctrl+V",//shortcutStr
                    false,//menuSetItemVisible
                    true,//menuSetItemEnabled
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::V, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("activate Ctrl+V\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+V\n");
                    }
                )
            }
        );

        //setActionShortCutByCategory("Action:Template");

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
        if (shortcut.keyCode == KeyCode::Unknown)
            continue;

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
        // bool menuSetItemVisible,
        // bool menuSetItemEnabled,
        const std::vector<ShortCut> &shortcuts
){
    auto &added = shortCutMap[category] = ShortCutCategory(
        category,
        menuSetVisible,
        menuSetEnabled,
        // menuSetItemVisible,
        // menuSetItemEnabled,
        shortcuts);
    
    added.createMenus();

}

void ShortcutManager::setActionShortCutByCategory(
    const std::string &category
){
    if (actionMenu.name.compare(category) == 0)
        return;

    actionMenu.setActive(false);

    auto it = shortCutMap.find(category);
    if (it != shortCutMap.end()){
        actionMenu = it->second;
        actionMenu.setActive(true);
    } else
        actionMenu = ShortCutCategory();
}
