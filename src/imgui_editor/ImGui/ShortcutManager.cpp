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
        if ( ITKCommon::StringUtil::contains(shortcut.mainMenuPath, "<<>>") )
            increment += 10000;
        ImGuiMenu::Instance()->AddMenu(
            increment,
		    shortcut.mainMenuPath, 
            shortcut.shortcutStr, 
            nullptr
        );
        if ( ITKCommon::StringUtil::contains(shortcut.mainMenuPath, "<<>>") )
            increment += 10000;
    }
    ImGuiMenu::Instance()->UpdateUI();

    // context menu entries
    increment = 0;
    auto &ctxMenu = ImGuiManager::Instance()->contextMenu.imGuiMenu;
    for(auto& shortcut : shortCuts) {
        if (shortcut.contextMenuPath.compare("")==0)
            continue;
        if ( ITKCommon::StringUtil::contains(shortcut.contextMenuPath, "<<>>") )
            increment += 10000;
        ctxMenu.AddMenu(
            increment,
            shortcut.contextMenuPath, 
            shortcut.shortcutStr, 
            nullptr
        );
        if ( ITKCommon::StringUtil::contains(shortcut.contextMenuPath, "<<>>") )
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
            //printf("[ShortCutCategory::setActive] individual visible: %i\n", (int)active);
            controller.visible = active;
        }
        else if (shortcut.mainMenuBehaviour == MenuBehaviour::SetItemEnabled){
            //printf("[ShortCutCategory::setActive] individual enabled: %i\n", (int)active);
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
            //printf("[ShortCutCategory::setActive] individual visible: %i\n", (int)active);
            controller.visible = active;
        }
        else if (shortcut.contextMenuBehaviour == MenuBehaviour::SetItemEnabled){
            //printf("[ShortCutCategory::setActive] individual enabled: %i\n", (int)active);
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
    lock_change_action_category = 0;
    input_blocked = false;
}

void ShortcutManager::lockChangeActionCategory(){
lock_change_action_category++;
}
void ShortcutManager::unlockChangeActionCategory() {
lock_change_action_category--;
if (lock_change_action_category < 0){
    lock_change_action_category = 0;
    printf("error... negative lock change category");
}
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
                    "Action", MenuBehaviour::SetItemVisibility // "mainMenuPath"
                ),

                ShortCut(
                    "Action/New", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "New", MenuBehaviour::SetItemVisibility // "contextMenuPath"
                ),

                ShortCut(
                    "Action/Components", MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,
                    "Components", MenuBehaviour::SetItemVisibility // MenuBehaviour contextMenuBehaviour,
                ),


                ShortCut(
                    "Action/<<>>##0", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "<<>>##0", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
                ),

                ShortCut(
                    "Action/Make First", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Make First", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
                ),
                ShortCut(
                    "Action/Make Last", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Make Last", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
                ),

                ShortCut(
                    "Action/<<>>##1", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "<<>>##1", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
                ),

                ShortCut(
                    "Action/Focus", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Focus", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "F" //shortcutStr
                ),

                ShortCut(
                    "Action/Open", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Open", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+O" //shortcutStr
                ),

                ShortCut(
                    "Action/New/Scene", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "New/Scene", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+N" //shortcutStr
                ),
                ShortCut(
                    "Action/New/Node", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "New/Node", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+N" //shortcutStr
                ),
                ShortCut(
                    "Action/New/Directory", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "New/Directory", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+N" //shortcutStr
                ),
                ShortCut(
                    "Action/Refresh", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Refresh", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "F5" //shortcutStr
                ),
                ShortCut(
                    "Action/Rename", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Rename", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "F2" //shortcutStr
                ),
                ShortCut(
                    "Action/Remove", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Remove", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Delete" //shortcutStr
                ),

                ShortCut(
                    "Action/<<>>##2", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "<<>>##2", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
                ),
                ShortCut(
                    "Action/Duplicate", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Duplicate", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+D" //shortcutStr
                ),
                ShortCut(
                    "Action/Copy", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Copy", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+C" //shortcutStr
                ),
                ShortCut(
                    "Action/Cut", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Cut", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+X" //shortcutStr
                ),
                ShortCut(
                    "Action/Paste", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "Paste", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "Ctrl+V" //shortcutStr
                )                
            }
        );

        setActionShortCutByCategory("Action:Template");

    }

    return this;
}

void ShortcutManager::RenderAndLogic(){

    if (input_blocked)
        return;

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
    if (lock_change_action_category != 0)
        return;
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
