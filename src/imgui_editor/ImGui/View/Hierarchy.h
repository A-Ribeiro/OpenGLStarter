#pragma once

#include "common.h"
#include "all.h"

#include <appkit-gl-engine/Transform.h>

class HierarchyTreeData : public TreeData
{
protected:
public:
    static TreeDataType Type;
    std::shared_ptr<AppKit::GLEngine::Transform> transform;
    bool has_content;

    HierarchyTreeData(std::shared_ptr<AppKit::GLEngine::Transform> transform) : TreeData(HierarchyTreeData::Type)
    {
        this->transform = transform;
        has_content = false;
    }

    virtual IconType getIcon()
    {
        if (transform->getComponentCount() > 0)
            return IconType::Small_BoxNode_Filled;
        else
            return IconType::Small_BoxNode;
    }

    static inline std::shared_ptr<HierarchyTreeData> CreateShared(std::shared_ptr<AppKit::GLEngine::Transform> transform)
    {
        return std::make_shared<HierarchyTreeData>(transform);
    }
};

enum class HierarchyClearMethod: uint8_t {
    None,
    ClearAndCallback,
    ClearNoCallback
};

class Hierarchy : public View, public TreeHolder
{

    // trick to make root have a self() valid method call
    // std::shared_ptr<TreeNode> self_root;
    std::shared_ptr<TreeNode> root;
    int32_t uid_incrementer;

    HierarchyClearMethod clear_tree_selection;
    int32_t force_tree_selection;

public:
    static const ViewType Type;

    Hierarchy();

    ~Hierarchy();

    View *Init();

    void RenderAndLogic();

    std::shared_ptr<TreeNode> &getTreeRoot()
    {
        return root;
    }

    void clearTree()
    {
        root->clear();
        uid_incrementer = 10;
    }

    std::shared_ptr<TreeNode> createTreeNode(const std::string &name, std::shared_ptr<HierarchyTreeData> data)
    {
        return TreeNode::CreateShared(uid_incrementer++, data, name.c_str());
    }

    std::shared_ptr<TreeNode> cloneTreeNode(std::shared_ptr<TreeNode> src);

    void clearTreeSelection(HierarchyClearMethod method) {        
        // tree
        clear_tree_selection = method;
    }

    void forceTreeSelection(int uid){
        force_tree_selection = uid;
    }

    void Finalize() {
        /*auto tdt = std::dynamic_pointer_cast<HierarchyTreeData>(root->data);
        tdt->transform->clearComponents();*/

        root = nullptr;
    }

};