#pragma once

#include "common.h"
#include "all.h"

class Console : public View
{
    Platform::Tool::DebugConsoleIPC *console;

    std::vector<std::string> history;
    int max_history_lines;

    int scroll_last_item;

public:
    static const ViewType Type;

    bool autoScroll;

    Console();

    ~Console();

    View* Init();

    void RenderAndLogic();
};