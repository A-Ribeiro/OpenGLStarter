#include "Scene.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"
#include "../../InnerViewport.h"

const ViewType Scene::Type = "Scene";

Scene::Scene() : View(Scene::Type),
                 mCurrentGizmoOperation(ImGuizmo::TRANSLATE),
                 mCurrentGizmoMode(ImGuizmo::LOCAL),
                 useSnap(true),
                 snap{0.01f, 0.01f, 0.01f}
{
}

View *Scene::Init()
{
    ImGuiMenu::Instance()->AddMenu(
        0,
        "Window/Scene", "", [this]()
        { printf("Window/Scene\n"); },
        &this->active);
    return this;
}

void Scene::RenderAndLogic()
{
    if (!active)
        return;
    auto flags = ImGuiWindowFlags_NoCollapse; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1, 0, 1, 1.0));
    if (ImGui::Begin("Scene", nullptr, flags))
    {
        auto &io = ImGui::GetIO();

        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10) * ImGuiManager::Instance()->GlobalScale);
        ImGui::BeginChild("scene_toolbar", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY, 0 /*ImGuiWindowFlags_HorizontalScrollbar*/);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10) * ImGuiManager::Instance()->GlobalScale);
        if (ImGui::Button("T")||ImGui::IsKeyPressed(ImGuiKey_W))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::Button("R")||ImGui::IsKeyPressed(ImGuiKey_E))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::Button("S")||ImGui::IsKeyPressed(ImGuiKey_R)) // ImGui::SameLine();
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        ImGui::PopStyleVar();

        // ImGui::Text("X: %f Y: %f", io.MousePos.x, io.MousePos.y);
        // if (ImGuizmo::IsUsing())
        // {
        //     ImGui::Text("Using gizmo");
        // }
        // else
        // {
        //     ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
        //     ImGui::SameLine();
        //     ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
        //     ImGui::SameLine();
        //     ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
        //     ImGui::SameLine();
        //     ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
        // }

        ImGui::EndChild();
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::BeginChild("scene_child", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding, 0 /*ImGuiWindowFlags_HorizontalScrollbar*/);

        // if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
        // {
        //     if (ImGui::IsMouseClicked(0))
        //     {
        //         // printf("Clicked on begin...\n");
        //         ImGuiManager::Instance()->innerViewport->fadeFromBlack();
        //     }
        // }
        // display_order.push_back(ImGui::GetCurrentWindow());

        ImVec2 min = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        ImVec2 max = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();

        ImVec2 pos = min;        // ImGui::GetWindowPos();
        ImVec2 size = max - min; // ImGui::GetWindowSize();

        // ImGuiIO &io = ImGui::GetIO();

        pos *= io.DisplayFramebufferScale;
        size *= io.DisplayFramebufferScale;

        // ImGuiViewport *viewport = ImGui::GetMainViewport();
        if (size.x > 0 && size.y > 0)
        {
            ImGuiManager::Instance()->innerViewport->setVisible(true);
            auto RenderWindowRegion = ImGuiManager::Instance()->innerViewport->renderWindow;
            // resize viewport
            iRect newViewport = iRect(pos.x, pos.y, size.x, size.y);
            bool viewportChanged = RenderWindowRegion->WindowViewport.c_val() != newViewport;
            RenderWindowRegion->WindowViewport = newViewport;
            // image draw
            if (RenderWindowRegion->fbo)
            {

                if (viewportChanged)
                {
                    // need render the modified FBO again, or it will give a black texture
                    Platform::Time time_aux;
                    ImGuiManager::Instance()->innerViewport->OnUpdate(&time_aux);
                }

                ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)RenderWindowRegion->fbo->colorAttachment[0]->mTexture;
                // float my_tex_w = (float)texture_ogl->width * 0.5f;
                // float my_tex_h = (float)texture_ogl->height * 0.5f;
                const ImVec2 uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
                const ImVec2 uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right
                const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
                const ImVec4 border_col = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 50% opaque white

                // {
                //     // Clear in Pink
                //     ImVec2 sz = size / io.DisplayFramebufferScale;
                //     auto *draw_list = ImGui::GetWindowDrawList();
                //     draw_list->AddRectFilled(
                //         pos,
                //         pos + sz,
                //         ImGui::GetColorU32(ImVec4(1.0f, 0.0f, 1.0f, 1.0f)),
                //         0.0f, // rounding
                //         ImDrawFlags_None
                //     );
                // }

                ImGui::Image(my_tex_id, size / io.DisplayFramebufferScale, uv_min, uv_max, tint_col, border_col);

                if (OnImGuiDrawOverlay != nullptr)
                    OnImGuiDrawOverlay(pos / io.DisplayFramebufferScale, size / io.DisplayFramebufferScale);
            }
            // else
            // {
            //     RenderWindowRegion->WindowViewport = newViewport;
            // }
        }
        else
        {
            ImGuiManager::Instance()->innerViewport->setVisible(false);
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
    }
    else
    {
        ImGuiManager::Instance()->innerViewport->setVisible(false);

        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }
    ImGui::End();
    // ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    computeOnHoverAndOnFocus();
}
