#pragma once

#include "common.h"
//#include "all.h"

class TreeHolder
{
    public:

    EventCore::Event<void(TreeNode*, bool)> OnTreeHover;
    EventCore::Event<void(TreeNode*)> OnTreeSingleClick;
    EventCore::Event<void(TreeNode*)> OnTreeDoubleClick;
    EventCore::Event<void(TreeNode*)> OnTreeExpand;
    EventCore::Event<void(TreeNode*)> OnTreeCollapse;
    EventCore::Event<void(TreeNode*)> OnTreeSelect;
};

class TreeNode
{
protected:
    void renderRecursive(TreeHolder *treeHolder, ImGuiID id_sel, int32_t selected_UID, bool* any_click_occured);// , Platform::Time* time);
public:
    TreeNode();
    TreeNode(int32_t uid, IconType type, std::string name = "node");
    
    //AppKit::OpenGL::GLTexture **icon_alias;
    // AppKit::OpenGL::GLTexture *icon;
    // std::string path;
    std::string name;
    int32_t uid;
    std::vector<TreeNode> children;
    IconType type;

    //bool opened;
    EventCore::PressReleaseDetector expanded;
    EventCore::PressReleaseDetector hovered;

    bool isLeaf();
    bool isNode();

    bool removeUIDRecursive(int32_t uid);

    void render(const char* str_imgui_id, TreeHolder *treeHolder);
};

