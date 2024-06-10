#pragma once

#include "./View/common.h"
#include "./View/all.h"
#include "./ImGuiMenu.h"

class ContextMenu : public View
{
    bool openContextMenuTriggered;

public:
    static const ViewType Type;

    ImGuiMenu imGuiMenu;

    ContextMenu();

    ~ContextMenu();

    View* Init();

    void RenderAndLogic();

    void open();
    
};