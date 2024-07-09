#include "Game.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const ViewType Game::Type = "Game";

Game::Game() : View(Game::Type)
{
}

View* Game::Init()
{
	ImGuiMenu::Instance()->AddMenu(
        0,
		"Window/Game", "", [this]()
		{ printf("Window/Game\n"); },
		&this->active);
	return this;
}

void Game::RenderAndLogic()
{
	if (!active)
		return;
	auto flags = ImGuiWindowFlags_NoCollapse; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
	if (ImGui::Begin("Game", NULL, flags))
	{
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

		// display_order.push_back(ImGui::GetCurrentWindow());
	} else {
        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }
	ImGui::End();

    computeOnHoverAndOnFocus();
}