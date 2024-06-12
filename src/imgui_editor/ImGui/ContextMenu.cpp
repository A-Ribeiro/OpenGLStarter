#include "ContextMenu.h"
#include "./View/Hierarchy.h"
#include "ImGuiMenu.h"
#include "ImGuiManager.h"

const ViewType ContextMenu::Type = "ContextMenu";

ContextMenu::ContextMenu() : View(ContextMenu::Type)
{
	openContextMenuTriggered = false;
}

ContextMenu::~ContextMenu()
{

}

View* ContextMenu::Init()
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


	return this;
}

void ContextMenu::RenderAndLogic()
{

	if (ImGui::BeginPopup("DefaultContextMenu"))
	{

		imGuiMenu.RenderAndLogic(true);

		// ImGui::Text("ContextMenu");
		// ImGui::Separator();
		// if (ImGui::Selectable("Option 1"))
		// {
		// }
		// if (ImGui::Selectable("Option 2"))
		// {
		// }
		// ImGui::Separator();
		// if (ImGui::Selectable("Option 3"))
		// {
		// }

		// if (ImGui::BeginMenu("Menu", true))
		// {
		//     if (ImGui::MenuItem("Custom", NULL, false))
		//     {
		//         // ImGui::CloseCurrentPopup();
		//     }
		// }

		ImGui::EndPopup();
	}
	// configure popup
	// ImGui::OpenPopupOnItemClick("DefaultContextMenu", ImGuiPopupFlags_NoOpenOverItems);

	if (openContextMenuTriggered)
	{
		openContextMenuTriggered = false;
		ImGui::OpenPopup("DefaultContextMenu");
	}
}

void ContextMenu::open()
{
	openContextMenuTriggered = true;
}
