#pragma once

#include "./View/common.h"
#include "./View/all.h"
#include "./ImGuiMenu.h"

class Dialogs : public View
{
    bool openContextMenuTriggered;

    std::string startText;
    char aux_str_1024[1024];
    EventCore::Callback<void(const std::string &v)> callback;

    ImVec2 pos;


public:
    static const ViewType Type;

    Dialogs();

    ~Dialogs();

    View* Init();

    void RenderAndLogic();

    void showEnterTextOK(const std::string &startText, const EventCore::Callback<void(const std::string &v)> &OnChange);
    
};