#pragma once

#include "common.h"
#include "all.h"

class Hierarchy : public View
{

    //Platform::Time time;

    /*AppKit::OpenGL::GLTexture *texture_transform;
    AppKit::OpenGL::GLTexture *texture_model;

    AppKit::OpenGL::GLTexture *texture_alias[2];*/

    HierarchyTreeNode root;
    int32_t uid_incrementer;

public:
    Hierarchy();

    ~Hierarchy();

    View* Init();

    void RenderAndLogic();

    EventCore::Event<void(HierarchyTreeNode*, bool)> OnHover;
    EventCore::Event<void(HierarchyTreeNode*)> OnSingleClick;
    EventCore::Event<void(HierarchyTreeNode*)> OnDoubleClick;
    EventCore::Event<void(HierarchyTreeNode*)> OnExpand;
    EventCore::Event<void(HierarchyTreeNode*)> OnCollapse;
    EventCore::Event<void(HierarchyTreeNode*)> OnSelect;


};