#include "Hierarchy.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

TreeDataType HierarchyTreeData::Type = "HierarchyTreeData";

const ViewType Hierarchy::Type = "Hierarchy";

Hierarchy::Hierarchy() : View(Hierarchy::Type)
{
    force_tree_selection = 0;

    uid_incrementer = 1;

    clear_tree_selection = HierarchyClearMethod::None;
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
    //self_root = TreeNode::CreateShared(uid_incrementer++, nullptr, "-empty-");
	root = TreeNode::CreateShared(uid_incrementer++, nullptr, "root");
    //self_root->addChild(root);

    root->setIsRoot(true).
        setPrefixID("HierarchyTree").
        setDragPayloadID(DRAG_PAYLOAD_ID_HIERARCHY_TREE).
        setDropPayload({
            DRAG_PAYLOAD_ID_HIERARCHY_TREE,
            DRAG_PAYLOAD_ID_PROJECT_TREE
        });

    root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child1"));
	root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child2"));
    root->children.back()->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child2leaf"));
	root->addChild(TreeNode::CreateShared(uid_incrementer++, nullptr, "child3"));

	ImGuiMenu::Instance()->AddMenu(
        0,
		"Window/Hierarchy", "", [this]()
		{ printf("Window/Hierarchy\n"); },
		&this->active);

    // debug
    {
        OnTreeHover.add([](std::shared_ptr<TreeNode> node, bool hovered){
            printf("[Hierarchy][Tree] OnHover on %s: %i\n", node->getName(), hovered);
        });
        OnTreeSingleClick.add([](std::shared_ptr<TreeNode> node){
            // if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            //     ImGuiManager::Instance()->contextMenu.open();
            printf("[Hierarchy][Tree] OnSingleClick on %s\n", node->getName());
        });
        OnTreeDoubleClick.add([=](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnDoubleClick on %s\n", node->getName());
            // if (node->isRoot)
            //     return;
            // ImGuiManager::Instance()->dialogs.showEnterText_OKCancel(node->getName(), 
            // [=](const std::string &new_str){
            //     printf("new text: %s\n", new_str.c_str());
            //     if (new_str.compare(node->getName()) != 0)
            //         node->setName(new_str.c_str());
            // });
        });
        OnTreeExpand.add([](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnExpand on %s\n", node->getName());
        });
        OnTreeCollapse.add([](std::shared_ptr<TreeNode> node){
            printf("[Hierarchy][Tree] OnCollapse on %s\n", node->getName());
        });
        OnTreeSelect.add([](std::shared_ptr<TreeNode> node){
            if (node == nullptr) {
                printf("[Hierarchy][Tree] OnSelect on nullptr\n");
                // if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                //     ImGuiManager::Instance()->contextMenu.open();
            } else {
                printf("[Hierarchy][Tree] OnSelect on %s\n", node->getName());
            }
        });
        OnTreeDragDrop.add([&](const char* drag_payload, void *src, std::shared_ptr<TreeNode>target){
            printf("[Hierarchy][Tree] OnTreeDragDrop. drag_payload: %s\n", drag_payload);
            // if (drag_payload == DRAG_PAYLOAD_ID_HIERARCHY_TREE){
            //     if (target != nullptr){
            //         std::shared_ptr<TreeNode> source_node = ((TreeNode*)src)->self();
            //         printf("                  Before PostAction %s to %s !\n", source_node->getName(), target->getName());
            //         ImGuiManager::Instance()->PostAction.add([=](){
            //             printf("                  [PostAction]\n");

            //             printf("                  Trying to reparent %s to %s !\n", source_node->getName(), target->getName());
            //             if (TreeNode::Reparent(source_node, target)){
            //                 printf("                  Reparent OK!\n");
            //             }else {
            //                 printf("                  Reparent Fail!\n");
            //             }
            //         });
            //     }
            // }
        });
        
    }

	return this;
}

void Hierarchy::RenderAndLogic()
{
	if (!active)
		return;
	
	auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10)*ImGuiManager::Instance()->GlobalScale);

    bool window_opened = ImGui::Begin("Hierarchy", nullptr, flags);

    if (clear_tree_selection != HierarchyClearMethod::None){
        auto action = clear_tree_selection;
        clear_tree_selection = HierarchyClearMethod::None;

        ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
        ImGui::GetStateStorage()->SetInt(id_sel, 0);

        if (action == HierarchyClearMethod::ClearAndCallback)
            this->OnTreeSelect(nullptr);
    }

    if (force_tree_selection != 0){
        ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
        ImGui::GetStateStorage()->SetInt(id_sel, force_tree_selection);

        force_tree_selection = 0;
    }

	if (window_opened)
	{
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

		root->render("##hierarchy_sel", this);

	} else {
        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }

	ImGui::End();
    ImGui::PopStyleVar();
    computeOnHoverAndOnFocus();

    // PostAction();
    // PostAction.clear();
}

std::shared_ptr<TreeNode> Hierarchy::cloneTreeNode(std::shared_ptr<TreeNode> src)
{
    using namespace AppKit::GLEngine;

    using TreeNodeMapT = std::unordered_map<std::shared_ptr<TreeNode>, std::shared_ptr<TreeNode>>;

    Transform::TransformMapT transformMap;
    Transform::ComponentMapT componentMap;
    TreeNodeMapT treeNodeMap;

    auto treeData = std::dynamic_pointer_cast<HierarchyTreeData>(src->data);
    // clone current transform
    auto cloned_transform = treeData->transform->clone(false, &transformMap, &componentMap);

    auto result = createTreeNode(
        src->getName(), 
        HierarchyTreeData::CreateShared(transformMap[treeData->transform])
    );

    struct _clone_structT {
        std::shared_ptr<TreeNode> cloneSrc;
        std::shared_ptr<TreeNode> cloneDst;
    };

    std::list<_clone_structT> to_clone;
    {
        to_clone.push_back(_clone_structT{src,result});
        treeNodeMap[src] = result;
    }

    while (to_clone.size() > 0) {
        auto entry = to_clone.front();
        to_clone.pop_front();

        for(auto &src_treenode: entry.cloneSrc->children ){
    
            treeData = std::dynamic_pointer_cast<HierarchyTreeData>(src_treenode->data);

            auto new_treenode = entry.cloneDst->addChild(
                createTreeNode(
                    src_treenode->getName(), 
                    HierarchyTreeData::CreateShared(transformMap[treeData->transform])
                ) 
            );

            {
                to_clone.push_back(_clone_structT{src_treenode,new_treenode});
                treeNodeMap[src_treenode] = new_treenode;
            }
        }
    }



    

    // auto currentData = std::dynamic_pointer_cast<HierarchyTreeData>(src->data);
    // auto new_transform = Transform::CreateShared();
    // new_transform->setLocalPosition(currentData->transform->getLocalPosition());
    // new_transform->setLocalRotation(currentData->transform->getLocalRotation());
    // new_transform->setLocalScale(currentData->transform->getLocalScale());
    // auto result = 
    //     createTreeNode(src->getName(), 
    //         HierarchyTreeData::CreateShared(
    //             new_transform
    //         )
    //     );

    // struct _child {
    //     std::shared_ptr<TreeNode> cloneSrc;
    //     //std::vector<std::shared_ptr<TreeNode>> cloneSrcChildren;
    //     std::shared_ptr<TreeNode> target;
    // };

    // std::vector<_child> to_clone;
    // to_clone.push_back(_child{
    //     src,
    //     result
    // });
    // while (to_clone.size() > 0){
    //     auto element = *to_clone.begin();
    //     to_clone.erase(to_clone.begin());

    //     for(auto srcChild: element.cloneSrc->children){
    //         auto currentData = std::dynamic_pointer_cast<HierarchyTreeData>(srcChild->data);
    //         auto new_transform = Transform::CreateShared();
    //         new_transform->setLocalPosition(currentData->transform->getLocalPosition());
    //         new_transform->setLocalRotation(currentData->transform->getLocalRotation());
    //         new_transform->setLocalScale(currentData->transform->getLocalScale());
    //         auto target_child = createTreeNode(srcChild->getName(), 
    //             HierarchyTreeData::CreateShared(
    //                 new_transform
    //             )
    //         );
    //         element.target->addChild(target_child);
    //         to_clone.push_back(_child{
    //             srcChild,
    //             target_child
    //         });
    //     }

    // }


    return result;
}