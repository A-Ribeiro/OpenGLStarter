#include "Hierarchy.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const ViewType Hierarchy::Type = "Hierarchy";

Hierarchy::Hierarchy() : View(Hierarchy::Type)
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
    self_root = TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "-empty-");
	root = TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "root");
    self_root->addChild(root);

    root->setIsRoot(true).
        setPrefixID("HierarchyTree").
        setDragPayloadID(DRAG_PAYLOAD_ID_HIERARCHY_TREE).
        setDropPayload({
            DRAG_PAYLOAD_ID_HIERARCHY_TREE,
            DRAG_PAYLOAD_ID_PROJECT_TREE
        });

    root->addChild(TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "child1"));
	root->addChild(TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "child2"));
    root->children.back()->addChild(TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "child2leaf"));
	root->addChild(TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Hierarchy, "child3"));

	ImGuiMenu::Instance()->AddMenu(
		"Window/Hierarchy", "", [this]()
		{ printf("Window/Hierarchy\n"); },
		&this->active);

    // debug
    {
        OnTreeHover.add([](std::shared_ptr<TreeNode> node, bool hovered){
            printf("[Hierarchy][Tree] OnHover on %s: %i\n", node->name, hovered);
        });
        OnTreeSingleClick.add([](std::shared_ptr<TreeNode> node){
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                ImGuiManager::Instance()->contextMenu.open();

            printf("[Hierarchy][Tree] OnSingleClick on %s\n", node->name);
        });
        OnTreeDoubleClick.add([=](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnDoubleClick on %s\n", node->name);
            ImGuiManager::Instance()->dialogs.showEnterTextOK(node->name, 
            [=](const std::string &new_str){
                printf("new text: %s\n", new_str.c_str());
                //node->name = new_str;

            });
        });
        OnTreeExpand.add([](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnExpand on %s\n", node->name);
        });
        OnTreeCollapse.add([](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnCollapse on %s\n", node->name);
        });
        OnTreeSelect.add([](std::shared_ptr<TreeNode> node){
            if (node == NULL) {
                printf("[Hierarchy][Tree] OnSelect on NULL\n");
            } else {
                printf("[Hierarchy][Tree] OnSelect on %s\n", node->name);
            }
        });
        OnTreeDragDrop.add([&](const char* drag_payload, void *src, std::shared_ptr<TreeNode>target){
            printf("[Hierarchy][Tree] OnTreeDragDrop. drag_payload: %s\n", drag_payload);
            if (drag_payload == DRAG_PAYLOAD_ID_HIERARCHY_TREE){
                std::shared_ptr<TreeNode> source_node = ((TreeNode*)src)->self();
                printf("                  Before PostAction %s to %s !\n", source_node->name, target->name);

                PostAction.add([=](){
                    printf("                  [PostAction]\n");

                    printf("                  Trying to reparent %s to %s !\n", source_node->name, target->name);
                    if (TreeNode::Reparent(source_node, target)){
                        printf("                  Reparent OK!\n");
                    }else {
                        printf("                  Reparent Fail!\n");
                    }
                });
            }
        });
        
    }

	return this;
}

void Hierarchy::RenderAndLogic()
{
	if (!active)
		return;
	
	auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
	if (ImGui::Begin("Hierarchy", NULL, flags))
	{
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

		root->render("##hierarchy_sel", this, root);

	} else {
        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }

	ImGui::End();

    computeOnHoverAndOnFocus();

    PostAction();
    PostAction.clear();
}
