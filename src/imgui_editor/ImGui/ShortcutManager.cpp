#include "ShortcutManager.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"



void ShortCutCategory::createMenus() {

    // main menu entries
    int increment = 0;
    for(auto& shortcut : shortCuts) {
        if (shortcut.mainMenuPath.compare("")==0)
            continue;
        if ( ITKCommon::StringUtil::endsWith(shortcut.mainMenuPath, "<<>>") )
            increment += 10000;
        ImGuiMenu::Instance()->AddMenu(
            increment,
		    shortcut.mainMenuPath, 
            shortcut.shortcutStr, 
            nullptr
        );
        if ( ITKCommon::StringUtil::endsWith(shortcut.mainMenuPath, "<<>>") )
            increment += 10000;
    }
    ImGuiMenu::Instance()->UpdateUI();

    // context menu entries
    increment = 0;
    auto &ctxMenu = ImGuiManager::Instance()->contextMenu.imGuiMenu;
    for(auto& shortcut : shortCuts) {
        if (shortcut.contextMenuPath.compare("")==0)
            continue;
        if ( ITKCommon::StringUtil::endsWith(shortcut.contextMenuPath, "<<>>") )
            increment += 10000;
        ctxMenu.AddMenu(
            increment,
            shortcut.contextMenuPath, 
            shortcut.shortcutStr, 
            nullptr
        );
        if ( ITKCommon::StringUtil::endsWith(shortcut.contextMenuPath, "<<>>") )
            increment += 10000;
    }
    ctxMenu.UpdateUI();

    setActive(false);
}

void ShortCutCategory::setActive(bool active) {
    // main menu entries
    for(auto& shortcut : shortCuts) {
        if (shortcut.mainMenuPath.compare("")==0)
            continue;
        auto &controller = ImGuiMenu::Instance()->getController(shortcut.mainMenuPath);
        if (shortcut.mainMenuBehaviour == MenuBehaviour::SetItemVisibility){
            printf("[ShortCutCategory::setActive] individual visible: %i\n", (int)active);
            controller.visible = active;
        }
        else if (shortcut.mainMenuBehaviour == MenuBehaviour::SetItemEnabled){
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

    ImGuiMenu::Instance()->UpdateUI();

    
    // context menu entries
    auto &ctxMenu = ImGuiManager::Instance()->contextMenu.imGuiMenu;
    for(auto& shortcut : shortCuts) {
        if (shortcut.contextMenuPath.compare("")==0)
            continue;
        auto &controller = ctxMenu.getController(shortcut.contextMenuPath);
        if (shortcut.contextMenuBehaviour == MenuBehaviour::SetItemVisibility){
            printf("[ShortCutCategory::setActive] individual visible: %i\n", (int)active);
            controller.visible = active;
        }
        else if (shortcut.contextMenuBehaviour == MenuBehaviour::SetItemEnabled){
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

    ctxMenu.UpdateUI();
    
}

const ViewType ShortcutManager::Type = "ShortcutManager";

ShortcutManager::ShortcutManager(): View(ShortcutManager::Type){
    
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
            {
                // set visibility root
                ShortCut(
                    "Action", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility // mainMenuBehaviour,
                ),

                // ShortCut(
                //     "Action/Add", // "mainMenuPath"
                //     MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                //     "Add", // "contextMenuPath"
                //     MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                //     "Ctrl+A",//shortcutStr
                    
                //     //ctrl,shift,alt,window,
                //     true,false,false,false,
                //     KeyCode::A, //AppKit::Window::Devices::KeyCode keyCode,
                //     [](){
                //         //activate
                //         printf("activate Ctrl+A\n");
                //     },
                //     [](){
                //         //deactivate
                //         printf("deactivate Ctrl+A\n");
                //     }
                // ),
                // ShortCut(
                //     "Action/Remove", // "mainMenuPath"
                //     MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                //     "Remove", // "contextMenuPath"
                //     MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                //     "Ctrl+R",//shortcutStr

                //     //ctrl,shift,alt,window,
                //     true,false,false,false,
                //     KeyCode::R, //AppKit::Window::Devices::KeyCode keyCode,
                //     [](){
                //         //activate
                //         printf("activate Ctrl+R\n");
                //     },
                //     [](){
                //         //deactivate
                //         printf("deactivate Ctrl+R\n");
                //     }
                // ),
                ShortCut(
                    "Action/<<>>", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "<<>>", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),
                ShortCut(
                    "Action/Duplicate", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Duplicate", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+D",//shortcutStr

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
                    "Action/Copy", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Copy", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+C",//shortcutStr

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
                    "Action/Cut", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Cut", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+X",//shortcutStr

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

                    "Action/Paste", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Paste", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+V",//shortcutStr

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

        setActionShortCutByCategory("Action:Template");

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
        const std::vector<ShortCut> &shortcuts
){
    auto &added = shortCutMap[category] = ShortCutCategory(
        category,
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
