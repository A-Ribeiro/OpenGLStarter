#pragma once

#include "common.h"
//#include "all.h"

class HierarchyTreeNode
{
public:
    HierarchyTreeNode();
    HierarchyTreeNode(int32_t uid, IconType type, std::string name = "node");
    
    //AppKit::OpenGL::GLTexture **icon_alias;
    // AppKit::OpenGL::GLTexture *icon;
    // std::string path;
    std::string name;
    int32_t uid;
    std::vector<HierarchyTreeNode> children;
    IconType type;

    //bool opened;
    EventCore::PressReleaseDetector expanded;
    EventCore::PressReleaseDetector hovered;
    

    bool isLeaf();
    bool isNode();

    bool removeUIDRecursive(int32_t uid);

    void renderRecursive(ImGuiID id_sel, int32_t selected_UID, bool* any_click_occured);// , Platform::Time* time);
};

