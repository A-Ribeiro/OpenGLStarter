#include "ContextMenu.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"

const ViewType ContextMenu::Type = "ContextMenu";

ContextMenu::ContextMenu(): View(ContextMenu::Type){
    openContextMenuTriggered = false;
}

ContextMenu::~ContextMenu(){

}

View* ContextMenu::Init(){
    return this;
}

void ContextMenu::RenderAndLogic() {

    if (ImGui::BeginPopupContextVoid("DefaultContextMenu",
    ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_NoReopen
    | ImGuiPopupFlags_MouseButtonMask_
    ))
    {
        ImGui::Text("ContextMenu");
        ImGui::Separator();
        if (ImGui::Selectable("Option 1")){

        }
        if (ImGui::Selectable("Option 2")){

        }
        ImGui::Separator();
        if (ImGui::Selectable("Option 3")){

        }
        ImGui::EndPopup();
    }
    //configure popup
    //ImGui::OpenPopupOnItemClick("DefaultContextMenu", ImGuiPopupFlags_NoOpenOverItems);
    
    if (openContextMenuTriggered){
        openContextMenuTriggered = false;
        ImGui::OpenPopup("DefaultContextMenu");
    }


}

void ContextMenu::open() {
    openContextMenuTriggered = true;
}
