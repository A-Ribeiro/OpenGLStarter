#include "ImGuiMenu.h"

MenuItemController::MenuItemController()
{
	index = 0;
	check_variable_ptr = NULL;
	callback = NULL;
}
bool MenuItemController::IsLeaf()
{
	return childrenSorted.size() == 0;
}
void MenuItemController::render(const std::string& parentName, bool isRoot)
{
	if (IsLeaf()) {
		if (parentName.compare("<<>>") == 0) {
			ImGui::Separator();
		}
		else
			if (ImGui::MenuItem(parentName.c_str(), shortcut.c_str(), (check_variable_ptr) ? check_variable_ptr : NULL))
				callback();
	}
	else {
		if (isRoot) {
			//root level
			for (auto& child : childrenSorted)
				child.render(child.parentName, false);
		}
		else
			if (ImGui::BeginMenu(parentName.c_str())) {
				for (auto& child : childrenSorted)
					child.render(child.parentName, false);
				ImGui::EndMenu();
			}
	}
}

void MenuItemController::UpdateUI() {
	childrenSorted.clear();
	for (auto& child : childrenMap)
	{
		child.second.parentName = child.first;
		childrenSorted.push_back(child.second);
	}
	std::sort(childrenSorted.begin(), childrenSorted.end(),
		// Lambda expression begins
		[](const MenuItemController& a, const MenuItemController& b) {
			return a.index < b.index;
		} // end of lambda expression
	);

	for (auto& child : childrenSorted)
		child.UpdateUI();
}


ImGuiMenu* ImGuiMenu::Instance()
{
	static ImGuiMenu _ImGuiMenu;
	return &_ImGuiMenu;
}

ImGuiMenu::ImGuiMenu() {
	global_index = 0;
}

void ImGuiMenu::AddMenu(const std::string& path, const std::string& shortcut, std::function<void(void)> callback, bool* check_variable_ptr)
{
	auto string_splitted = ITKCommon::StringUtil::tokenizer(path, "/");
	auto* tree_node = &menu;
	for (auto& entry : string_splitted)
		tree_node = &tree_node->childrenMap[entry];
	tree_node->callback = callback;
	tree_node->shortcut = shortcut;
	tree_node->check_variable_ptr = check_variable_ptr;
	tree_node->index = global_index++;
}

bool ImGuiMenu::GetCheckState(const std::string& path) {
	auto string_splitted = ITKCommon::StringUtil::tokenizer(path, "/");
	auto* tree_node = &menu;
	for (auto& entry : string_splitted)
		tree_node = &tree_node->childrenMap[entry];
	return (tree_node->check_variable_ptr) ? *tree_node->check_variable_ptr : true;
}

void ImGuiMenu::Clear() {
	global_index = 0;
	menu.childrenMap.clear();
	menu.childrenSorted.clear();
}

void ImGuiMenu::UpdateUI() {
	menu.UpdateUI();
}

void ImGuiMenu::RenderAndLogic() {
	if (menu.childrenSorted.size() == 0)
		UpdateUI();
	if (ImGui::BeginMainMenuBar()) {
		menu.render("");
		ImGui::EndMainMenuBar();
	}
}