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

    // debug
    {
        OnHover.add([](HierarchyTreeNode* node, bool hovered){
            printf("OnHover on %s: %i\n", node->name.c_str(), hovered);
        });
        OnSingleClick.add([](HierarchyTreeNode* node){
            printf("OnSingleClick on %s\n", node->name.c_str());
        });
        OnDoubleClick.add([](HierarchyTreeNode* node){
            printf("OnDoubleClick on %s\n", node->name.c_str());
        });
        OnExpand.add([](HierarchyTreeNode* node){
            printf("OnExpand on %s\n", node->name.c_str());
        });
        OnCollapse.add([](HierarchyTreeNode* node){
            printf("OnCollapse on %s\n", node->name.c_str());
        });
        OnSelect.add([](HierarchyTreeNode* node){
            auto &controller = ImGuiMenu::Instance()->getController("Actions/Node");
            printf("Modifing: %s\n", controller.parentName.c_str());

            if (node == NULL) {
                for(auto &chld: controller.childrenMap)
                    chld.second.enabled = false;
                printf("OnSelect on NULL\n");
            } else {
                for(auto &chld: controller.childrenMap)
                    chld.second.enabled = true;
                printf("OnSelect on %s\n", node->name.c_str());
            }

            ImGuiMenu::Instance()->UpdateUI();
        });
    }

    // hierachy menu
    {
    	ImGuiMenu::Instance()->AddMenu(
		"Actions/Node/Add", "Ctrl+A", [&]()
		{ 
            printf("Actions/Node/Add\n");
        });
        ImGuiMenu::Instance()->AddMenu(
		"Actions/Node/Cut", "Ctrl+X", [&]()
		{ 
            printf("Actions/Node/Cut\n");
        });
        ImGuiMenu::Instance()->AddMenu(
		"Actions/Node/Copy", "Ctrl+C", [&]()
		{ 
            printf("Actions/Node/Copy\n");
        });
        ImGuiMenu::Instance()->AddMenu(
		"Actions/Node/Paste", "Ctrl+V", [&]()
		{ 
            printf("Actions/Node/Paste\n");
        });
    }

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
            this->OnSelect(NULL);
		}
	}
	ImGui::End();

    using namespace AppKit::Window::Devices;

    bool ctrl_pressed = Keyboard::isPressed(KeyCode::LControl) || Keyboard::isPressed(KeyCode::RControl);

    A_detector.setState(Keyboard::isPressed(KeyCode::A));
    X_detector.setState(Keyboard::isPressed(KeyCode::X));
    C_detector.setState(Keyboard::isPressed(KeyCode::C));
    V_detector.setState(Keyboard::isPressed(KeyCode::V));

    if (ctrl_pressed && A_detector.down){
        printf("[shortcut Ctrl+A]\n");
    }

}
