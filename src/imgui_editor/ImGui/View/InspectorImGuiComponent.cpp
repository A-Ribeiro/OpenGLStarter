#include "InspectorImGuiComponent.h"
#include "all.h"
#include "../ImGuiManager.h"

InspectorImGuiComponent::InspectorImGuiComponent(const std::string& title) {
	this->title = title;
    this->opened = true;
}
InspectorImGuiComponent::~InspectorImGuiComponent() {

}

void InspectorImGuiComponent::renderAndLogic(int count) {
    char aux[64];
    sprintf(aux, "%s##%i", this->title.c_str(), count);

    ImGui::SetNextItemOpen(opened);
    opened = ImGui::CollapsingHeader(aux,
        ImGuiTreeNodeFlags_None |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_AllowItemOverlap |
        ImGuiTreeNodeFlags_Framed);

    sprintf(aux, "[x]##%s_%i", this->title.c_str(), count);
    ImGUIButtonEx buttons[] = {
        ImGUIButtonEx::FromText(aux)
    };
    DrawRightAlignedButtonSet(buttons, 1);
    if (buttons[0].clicked) {
        printf("Clicked...\n");
        ImGuiManager::Instance()->dialogs.showEnterTextOK("Dummy Text", 
            [=](const std::string &new_str){
                printf("new text: %s\n", new_str.c_str());
                //node->name = new_str;

            });
    }
    if (opened) {
        sprintf(aux,"##%s_%i", this->title.c_str(), count);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6, 4));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
        
        // ImGuiStyle& style = ImGui::GetStyle(); 
        // style.FrameRounding = 5.f;

        ImGui::BeginChildCustom(aux);

        this->childRenderAndLogic();

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
    }

}




InspectorImGuiComponent_Transform::InspectorImGuiComponent_Transform():InspectorImGuiComponent("transform") {

}

void InspectorImGuiComponent_Transform::childRenderAndLogic() {

    //ImGui::BeginGroup();

    if (ImGui::DragFloat3Inverted("Pos", p, 0.001f, 0, 0, "%.3f")) {
        printf("Value Pos Changed...\n");
    }

    if (ImGui::DragFloat4Inverted("Quat", r, 0.001f, 0, 0, "%.3f")) {
        printf("Value Quat Changed...\n");
    }
    if (ImGui::DragFloat3Inverted("Scale", s, 0.001f, 0, 0, "%.3f")) {
        printf("Value Scale Changed...\n");
    }

    //ImGui::EndGroup();
    //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    //groupHeight = ImGui::GetItemRectMax().y - ImGui::GetItemRectMin().y;
    ImGui::Separator();

}
