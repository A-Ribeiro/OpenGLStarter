#pragma once

#include "common.h"
#include "all.h"

class Hierarchy : public View, public TreeHolder
{

    //Platform::Time time;

    /*AppKit::OpenGL::GLTexture *texture_transform;
    AppKit::OpenGL::GLTexture *texture_model;

    AppKit::OpenGL::GLTexture *texture_alias[2];*/

    // trick to make root have a self() valid method call
    std::shared_ptr<TreeNode> self_root;
    std::shared_ptr<TreeNode> root;
    int32_t uid_incrementer;

    EventCore::Event<void()> PostAction;

public:
    static const ViewType Type;

    Hierarchy();

    ~Hierarchy();

    View* Init();

    void RenderAndLogic();

};