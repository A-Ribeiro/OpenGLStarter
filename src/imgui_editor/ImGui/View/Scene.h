#pragma once

#include "common.h"
#include "all.h"

class Scene : public View
{

public:
    static const ViewType Type;

    ImGuizmo::OPERATION mCurrentGizmoOperation;// = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE mCurrentGizmoMode;// = ImGuizmo::LOCAL;

    bool useSnap;// = true;
    float snap[3];// = {0.01f,0.01f,0.01f};

    EventCore::Callback<void(const ImVec2 &pos, const ImVec2 &size)> OnImGuiDrawOverlay;

    Scene();

    View* Init();

    void RenderAndLogic();
};