#include "Game.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

Game::Game() : View()
{
}

View* Game::Init()
{
	ImGuiMenu::Instance()->AddMenu(
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
		// display_order.push_back(ImGui::GetCurrentWindow());
	}
	ImGui::End();
}