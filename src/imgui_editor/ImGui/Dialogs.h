#pragma once

#include "./View/common.h"
#include "./View/all.h"
#include "./ImGuiMenu.h"

#include "./ImGuiExt/CenteredText.h"

enum class DialogPosition : uint8_t {
    None,
    OpenOnMouse,
    OpenOnScreenCenter,

    ForceCentering,
};

class Dialogs : public View
{
    DialogPosition open_EnterText_OKCancel;
    char aux_str_1024_EnterText_OKCancel[1024];
    EventCore::Callback<void(const std::string &v)> callback_EnterText_OKCancel;
    ImVec2 pos_EnterText_OKCancel;

    DialogPosition open_showInfo_OK;
    //char aux_str_1024_showInfo_OK[1024];
    CenteredText text_showInfo_OK;
    EventCore::Callback<void()> callback_showInfo_OK;
    ImVec2 pos_showInfo_OK;
    bool show_Cancel_showInfo_OK;

public:
    static const ViewType Type;

    Dialogs();

    ~Dialogs();

    View* Init();

    void RenderAndLogic();

    void showEnterText_OKCancel(const std::string &startText, const EventCore::Callback<void(const std::string &v)> &callback, DialogPosition dialogPosition = DialogPosition::OpenOnMouse);

    void showInfo_OK(const std::string &infoText, const EventCore::Callback<void()> &callback, DialogPosition dialogPosition = DialogPosition::OpenOnMouse);

    void showInfo_OKCancel(const std::string &infoText, const EventCore::Callback<void()> &callback, DialogPosition dialogPosition = DialogPosition::OpenOnMouse);

};