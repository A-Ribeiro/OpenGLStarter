#pragma once

#include "./View/common.h"
#include "./View/all.h"


class ContextMenu : public View
{
    bool openContextMenuTriggered;

public:
    static const ViewType Type;

    ContextMenu();

    ~ContextMenu();

    View* Init();

    void RenderAndLogic();

    void open();
    
};