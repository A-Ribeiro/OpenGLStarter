#include "InspectorImGuiComponent.h"
#include "../all.h"
#include "../../ImGuiManager.h"

InspectorImGuiComponent::InspectorImGuiComponent(const std::string& title) {
	this->title = title;
    this->opened = true;
}
InspectorImGuiComponent::~InspectorImGuiComponent() {

}

void InspectorImGuiComponent::renderAndLogic(int count) {
    char aux[64];
    snprintf(aux, 64, "%s##%i", this->title.c_str(), count);

    ImGui::SetNextItemOpen(opened);
    opened = ImGui::CollapsingHeader(aux,
        ImGuiTreeNodeFlags_None |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_AllowOverlap |
        ImGuiTreeNodeFlags_Framed);

    snprintf(aux, 64, "[x]##%s_%i", this->title.c_str(), count);
    ImGUIButtonEx buttons[] = {
        ImGUIButtonEx::FromText(aux)
    };
    DrawRightAlignedButtonSet(buttons, 1);
    if (buttons[0].clicked) {
        printf("Clicked...\n");
        // ImGuiManager::Instance()->dialogs.showEnterText_OKCancel("Dummy Text", 
        //     [=](const std::string &new_str){
        //         printf("new text: %s\n", new_str.c_str());
        //         //node->name = new_str;

        //     });
    }
    if (opened) {
        snprintf(aux, 64,"##%s_%i", this->title.c_str(), count);
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
