#include "Dialogs.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"

const ViewType Dialogs::Type = "Dialogs";

Dialogs::Dialogs() : View(Dialogs::Type)
{
	open_EnterText_OKCancel = DialogPosition::None;
    open_showInfo_OK = DialogPosition::None;

    show_Cancel_showInfo_OK = false;
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
    pos_EnterText_OKCancel = ImVec2(-1,-1);
    pos_showInfo_OK = ImVec2(-1,-1);

	return this;
}

void Dialogs::RenderAndLogic()
{
    ImGuiManager *imGuiManager = ImGuiManager::Instance();

    ImGui::SetNextWindowSize( ImVec2(300.0f * imGuiManager->GlobalScale,-1), ImGuiCond_Appearing );
    ImGui::SetNextWindowPos( ImGui::GetIO().MousePos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (pos_EnterText_OKCancel.x >= 0){
        ImGui::SetNextWindowPos( pos_EnterText_OKCancel, ImGuiCond_Always);
        pos_EnterText_OKCancel = ImVec2(-1,-1);
    }

    if (open_EnterText_OKCancel == DialogPosition::ForceCentering){
        open_EnterText_OKCancel == DialogPosition::None;
        ImGui::SetNextWindowPos( ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }

	if (ImGui::BeginPopupModal("EnterTextOK", nullptr, 
        ImGuiWindowFlags_NoSavedSettings 
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        ))
	{
        imGuiManager->shortcutManager.input_blocked = true;
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
            pos_EnterText_OKCancel = w_pos;
            //printf("-----updating----\n");
        }
        
        ImGui::PushItemWidth(-1);
        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();
        ImGui::InputText("##label", aux_str_1024_EnterText_OKCancel, 1024, ImGuiInputTextFlags_AutoSelectAll);
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
            std::string input_string = aux_str_1024_EnterText_OKCancel;
            if (callback_EnterText_OKCancel != nullptr){
                auto _tmp = callback_EnterText_OKCancel;
                callback_EnterText_OKCancel = nullptr;
                _tmp(input_string);
            }
            ImGui::CloseCurrentPopup(); 
            imGuiManager->shortcutManager.input_blocked = false;
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
            imGuiManager->shortcutManager.input_blocked = false;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

		ImGui::EndPopup();
	}
	// configure popup
	// ImGui::OpenPopupOnItemClick("DefaultContextMenu", ImGuiPopupFlags_NoOpenOverItems);

	if (open_EnterText_OKCancel != DialogPosition::None &&
        open_EnterText_OKCancel != DialogPosition::ForceCentering)
	{
		if (open_EnterText_OKCancel == DialogPosition::OpenOnScreenCenter)
            open_EnterText_OKCancel = DialogPosition::ForceCentering;
        else
    		open_EnterText_OKCancel = DialogPosition::None;
		ImGui::OpenPopup("EnterTextOK");
	}

    


    ImGui::SetNextWindowSize( ImVec2(300.0f * imGuiManager->GlobalScale,-1), ImGuiCond_Appearing );
//    ImGui::SetNextWindowPos( ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowPos( ImGui::GetIO().MousePos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (pos_showInfo_OK.x >= 0){
        ImGui::SetNextWindowPos( pos_showInfo_OK, ImGuiCond_Always);
        pos_showInfo_OK = ImVec2(-1,-1);
    }

    if (open_showInfo_OK == DialogPosition::ForceCentering){
        open_showInfo_OK == DialogPosition::None;
        ImGui::SetNextWindowPos( ImGui::GetIO().DisplaySize * 0.5f, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    }

	if (ImGui::BeginPopupModal("InfoOK", nullptr, 
        ImGuiWindowFlags_NoSavedSettings 
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoTitleBar
        ))
	{
        imGuiManager->shortcutManager.input_blocked = true;

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
            pos_showInfo_OK = w_pos;
            //printf("-----updating----\n");
        }

        //ImGui::Text("%s", aux_str_1024_showInfo_OK);
        auto saved_pos = ImGui::GetCursorPos();
        auto offset_to_center = ImVec2(
            (ImGui::GetContentRegionAvail().x - text_showInfo_OK.text_size.x) * 0.5f,
            0
        );
        
        text_showInfo_OK.drawText(
            saved_pos + text_showInfo_OK.text_size_2 + offset_to_center,
            ImGui::GetCurrentContext()->Font
        );

        ImGui::SetCursorPos(saved_pos + offset_to_center);
        ImGui::Dummy(text_showInfo_OK.text_size);
        //ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));

        ImGui::Separator();

        auto area = ImGui::GetContentRegionAvail();

        if (show_Cancel_showInfo_OK)
            ImGui::Dummy(ImVec2(area.x - 120 - 120 - ImGui::GetStyle().ItemSpacing.x,0));
        else
            ImGui::Dummy(ImVec2(area.x - 120,0));
        ImGui::SameLine(0.0f,0.0f);

        ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.425f, 0.98f*0.7f, 0.425f, 0.40f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0.425f, 0.98f*0.7f, 0.425f, 1.00f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0.295f, 0.98f*0.7f, 0.295f, 1.00f));
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            if (callback_showInfo_OK != nullptr){
                auto _tmp = callback_showInfo_OK;
                callback_showInfo_OK = nullptr;
                _tmp();
            }
            ImGui::CloseCurrentPopup(); 
            imGuiManager->shortcutManager.input_blocked = false;
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        if (show_Cancel_showInfo_OK){
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0.98f*0.7f, 0.425f, 0.425f, 0.40f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0.98f*0.7f, 0.425f, 0.425f, 1.00f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0.98f*0.7f, 0.295f, 0.295f, 1.00f));
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup(); 
                imGuiManager->shortcutManager.input_blocked = false;
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
        }

		ImGui::EndPopup();
	}


    if (open_showInfo_OK != DialogPosition::None && 
        open_showInfo_OK != DialogPosition::ForceCentering)
	{
        if (open_showInfo_OK == DialogPosition::OpenOnScreenCenter)
            open_showInfo_OK = DialogPosition::ForceCentering;
        else
    		open_showInfo_OK = DialogPosition::None;

		ImGui::OpenPopup("InfoOK");
	}
}

void Dialogs::showEnterText_OKCancel(const std::string &startText, const EventCore::Callback<void(const std::string &v)> &callback, DialogPosition dialogPosition)
{
    snprintf(aux_str_1024_EnterText_OKCancel, 1024, "%s", startText.c_str());
    callback_EnterText_OKCancel = callback;
	open_EnterText_OKCancel = dialogPosition;
}

void Dialogs::showInfo_OK(const std::string &infoText, const EventCore::Callback<void()> &callback, DialogPosition dialogPosition) {
    //snprintf(aux_str_1024_showInfo_OK, 1024, "%s", infoText.c_str());
    text_showInfo_OK.setText(
        infoText,
        ImGui::GetCurrentContext()->Font,
        300.0f * ImGuiManager::Instance()->GlobalScale - 
        ImGui::GetStyle().WindowPadding.x * 2.0f
    );
    callback_showInfo_OK = callback;
	open_showInfo_OK = dialogPosition;
    show_Cancel_showInfo_OK = false;
}

void Dialogs::showInfo_OKCancel(const std::string &infoText, const EventCore::Callback<void()> &callback, DialogPosition dialogPosition){
    text_showInfo_OK.setText(
        infoText,
        ImGui::GetCurrentContext()->Font,
        300.0f * ImGuiManager::Instance()->GlobalScale - 
        ImGui::GetStyle().WindowPadding.x * 2.0f
    );
    callback_showInfo_OK = callback;
	open_showInfo_OK = dialogPosition;
    show_Cancel_showInfo_OK = true;
}
