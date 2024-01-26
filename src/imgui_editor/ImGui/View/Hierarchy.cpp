#include "Hierarchy.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

Hierarchy::Hierarchy() : View()
{
	//texture_transform = NULL;
	//texture_model = NULL;
	uid_incrementer = 1;
}

Hierarchy::~Hierarchy()
{

	//delete texture_transform);
	//delete texture_model);
}

View* Hierarchy::Init()
{
	//texture_transform = AppKit::OpenGL::GLTexture::loadFromFile("./resources/node_transform.png");
	//texture_model = AppKit::OpenGL::GLTexture::loadFromFile("./resources/node_model.png");

	//texture_alias[Icon_Transform] = texture_transform;
	//texture_alias[Icon_Model] = texture_model;

	// creating testing node
	root = HierarchyTreeNode(uid_incrementer++, IconType::Small_BoxNode, "root");
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_BoxNode, "child1"));
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_BoxNode_Filled, "child2"));
	root.children[root.children.size() - 1].children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_BoxNode, "child2leaf"));
	root.children.push_back(HierarchyTreeNode(uid_incrementer++, IconType::Small_BoxNode, "child3"));

	ImGuiMenu::Instance()->AddMenu(
		"Window/Hierarchy", "", [this]()
		{ printf("Window/Hierarchy\n"); },
		&this->active);
	return this;
}

void Hierarchy::RenderAndLogic()
{
	if (!active)
		return;
	bool deselect_all = false;
	auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
	if (ImGui::Begin("Hierarchy", NULL, flags))
	{
		if (ImGui::IsWindowHovered())
		{
			if (ImGui::IsMouseClicked(0) || ImGui::IsKeyDown(ImGuiKey_Escape))
			{
				// printf("Clicked on begin...\n");
				deselect_all = true;
			}
		}

		ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
		int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

		bool any_click_occured = false;
		root.renderRecursive(id_sel, selected_UID, &any_click_occured);// , & time);
		if (any_click_occured)
			deselect_all = false;

		if (deselect_all)
		{
			// printf("reset selection...\n");
			// ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
			ImGui::GetStateStorage()->SetInt(id_sel, 0);
		}
	}
	ImGui::End();
}
