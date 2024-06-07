#include "Scene.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"
#include "../../InnerViewport.h"

const ViewType Scene::Type = "Scene";

Scene::Scene() : View(Scene::Type)
{
}

View *Scene::Init()
{
    ImGuiMenu::Instance()->AddMenu(
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
    if (ImGui::Begin("Scene", NULL, flags))
    {
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_RectOnly));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_RectOnly))
        {
            if (ImGui::IsMouseClicked(0))
            {
                // printf("Clicked on begin...\n");
                ImGuiManager::Instance()->innerViewport->fadeFromBlack();
            }
        }
        // display_order.push_back(ImGui::GetCurrentWindow());

        ImVec2 min = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
        ImVec2 max = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();

        ImVec2 pos = min;        // ImGui::GetWindowPos();
        ImVec2 size = max - min; // ImGui::GetWindowSize();

        ImGuiIO &io = ImGui::GetIO();

        pos *= io.DisplayFramebufferScale;
        size *= io.DisplayFramebufferScale;

        // ImGuiViewport *viewport = ImGui::GetMainViewport();
        if (size.x > 0 && size.y > 0)
        {
            ImGuiManager::Instance()->innerViewport->setVisible(true);
            RenderWindowRegion *RenderWindowRegion = &ImGuiManager::Instance()->innerViewport->renderWindow;
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
            }
            else
            {
                RenderWindowRegion->WindowViewport = newViewport;
            }
        }
        else
        {
            ImGuiManager::Instance()->innerViewport->setVisible(false);
        }
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
