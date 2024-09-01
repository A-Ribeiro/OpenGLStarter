#pragma once

#include "./View/common.h"

class MenuItemController
{
public:
    std::unordered_map<std::string, MenuItemController> childrenMap;
    std::vector<MenuItemController> childrenSorted;
    std::function<void(void)> callback;
    std::string parentName;
    std::string shortcut;
    bool *check_variable_ptr;
    int index;
    bool visible;
    bool enabled;

    MenuItemController();
    bool IsLeaf();
    void render(const std::string& parentName, bool isRoot = true);

    void UpdateUI();
};

class ImGuiMenu
{

    MenuItemController menu;
    int global_index;

public:
    static ImGuiMenu* Instance();

    ImGuiMenu();

    MenuItemController& AddMenu(int increment_UID, const std::string& path, const std::string& shortcut, std::function<void(void)> callback, bool* check_variable_ptr = nullptr);
    void makeLast(const std::string& path);

    bool GetCheckState(const std::string& path);

    void Clear();

    void UpdateUI();

    void RenderAndLogic(bool isContextMenu = false);

    MenuItemController& getController(const std::string& path);

};
