#pragma once

#include "common.h"
#include "all.h"

class Hierarchy : public View, public TreeHolder
{

    //Platform::Time time;

    /*AppKit::OpenGL::GLTexture *texture_transform;
    AppKit::OpenGL::GLTexture *texture_model;

    AppKit::OpenGL::GLTexture *texture_alias[2];*/

    TreeNode root;
    int32_t uid_incrementer;

public:
    static const ViewType Type;

    Hierarchy();

    ~Hierarchy();

    View* Init();

    void RenderAndLogic();

};