#include "Dialogs.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"

const ViewType Dialogs::Type = "Dialogs";

Dialogs::Dialogs() : View(Dialogs::Type)
{
	openContextMenuTriggered = false;
}

Dialogs::~Dialogs()
{

}

View* Dialogs::Init()
{


	// imGuiMenu.AddMenu(
	// 	"New/Object", 
	//     "", 
	//     nullptr);
	// imGuiMenu.AddMenu(
	// 	"New/Transform", 
	//     "", 
	//     nullptr);

	// imGuiMenu.AddMenu(
	// 	"<<>>", 
	//     "", 
	//     nullptr);

	// imGuiMenu.AddMenu(
	// 	"Transform/Objectº", 
	//     "", 
	//     nullptr);

	// imGuiMenu.UpdateUI();
    pos = ImVec2(-1,-1);

	return this;
}

void Dialogs::RenderAndLogic()
{
    ImGuiManager *imGuiManager = ImGuiManager::Instance();

    ImGui::SetNextWindowSize( ImVec2(300.0f * imGuiManager->GlobalScale,-1), ImGuiCond_Appearing );
//    ImGui::SetNextWindowPos( ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowPos( ImGui::GetIO().MousePos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (pos.x >= 0){
        ImGui::SetNextWindowPos( pos, ImGuiCond_Always);
        pos = ImVec2(-1,-1);
    }

	if (ImGui::BeginPopupModal("EnterTextOK", NULL, 
        ImGuiWindowFlags_NoSavedSettings 
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        ))
	{
        bool needs_update = false;
        ImVec2 w_pos = ImGui::GetWindowPos();
        ImVec2 w_max = ImGui::GetWindowSize() + w_pos;
        if (w_pos.x < 0){
            w_pos.x = 0;
            needs_update = true;
        }
        if (w_pos.y < 0){
            w_pos.y = 0;
            needs_update = true;
        }
        if (w_max.x > ImGui::GetIO().DisplaySize.x){
            w_pos.x += ImGui::GetIO().DisplaySize.x - w_max.x;
            needs_update = true;
        }
        if (w_max.y > ImGui::GetIO().DisplaySize.y){
            w_pos.y += ImGui::GetIO().DisplaySize.y - w_max.y;
            needs_update = true;
        }
        if (needs_update){
            pos = w_pos;
            printf("-----updating----\n");
        }
        
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();
        ImGui::InputText("##label", aux_str_1024, 1024, ImGuiInputTextFlags_AutoSelectAll);
        ImGui::PopItemWidth();
        //ImGui::SetItemDefaultFocus();

        ImGui::Separator();

        auto area = ImGui::GetContentRegionAvail();

        ImGui::Dummy(ImVec2(area.x - 120 - 120 - ImGui::GetStyle().ItemSpacing.x ,0));
        ImGui::SameLine(0.0f,0.0f);

        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.425f, 0.98f*0.7f, 0.425f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0.425f, 0.98f*0.7f, 0.425f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0.295f, 0.98f*0.7f, 0.295f, 1.00f));
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            std::string input_string = aux_str_1024;
            //if (startText.compare(input_string) != 0){
            if (callback != nullptr){
                auto _tmp = callback;
                callback = nullptr;
                _tmp(input_string);
            }
            //}
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.98f*0.7f, 0.425f, 0.425f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0.98f*0.7f, 0.425f, 0.425f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0.98f*0.7f, 0.295f, 0.295f, 1.00f));
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

		ImGui::EndPopup();
	}
	// configure popup
	// ImGui::OpenPopupOnItemClick("DefaultContextMenu", ImGuiPopupFlags_NoOpenOverItems);

	if (openContextMenuTriggered)
	{
		openContextMenuTriggered = false;
		ImGui::OpenPopup("EnterTextOK");
	}
}

void Dialogs::showEnterTextOK(const std::string &startText, const EventCore::Callback<void(const std::string &v)> &OnChange)
{
    this->startText = startText;
    snprintf(aux_str_1024, 1024, "%s", startText.c_str());
    callback = OnChange;
	openContextMenuTriggered = true;
}
