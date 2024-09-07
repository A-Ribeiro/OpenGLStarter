#pragma once

#include "common.h"
#include "all.h"

class Scene : public View
{

public:
    static const ViewType Type;

    EventCore::Callback<void(const ImVec2 &pos, const ImVec2 &size)> OnImGuiDrawOverlay;

    Scene();

    View* Init();

    void RenderAndLogic();
};