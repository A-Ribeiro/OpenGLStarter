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

    std::shared_ptr<TreeNode> cloneTreeNode(std::shared_ptr<TreeNode> src)
    {
        using namespace AppKit::GLEngine;

        auto currentData = std::dynamic_pointer_cast<HierarchyTreeData>(src->data);
        auto new_transform = Transform::CreateShared();
        new_transform->setLocalPosition(currentData->transform->getLocalPosition());
        new_transform->setLocalRotation(currentData->transform->getLocalRotation());
        new_transform->setLocalScale(currentData->transform->getLocalScale());
        auto result = 
            createTreeNode(src->getName(), 
                HierarchyTreeData::CreateShared(
                    new_transform
                )
            );

        struct _child {
            std::shared_ptr<TreeNode> cloneSrc;
            //std::vector<std::shared_ptr<TreeNode>> cloneSrcChildren;
            std::shared_ptr<TreeNode> target;
        };

        std::vector<_child> to_clone;
        to_clone.push_back(_child{
            src,
            result
        });
        while (to_clone.size() > 0){
            auto element = *to_clone.begin();
            to_clone.erase(to_clone.begin());

            for(auto srcChild: element.cloneSrc->children){
                auto currentData = std::dynamic_pointer_cast<HierarchyTreeData>(srcChild->data);
                auto new_transform = Transform::CreateShared();
                new_transform->setLocalPosition(currentData->transform->getLocalPosition());
                new_transform->setLocalRotation(currentData->transform->getLocalRotation());
                new_transform->setLocalScale(currentData->transform->getLocalScale());
                auto target_child = createTreeNode(srcChild->getName(), 
                    HierarchyTreeData::CreateShared(
                        new_transform
                    )
                );
                element.target->addChild(target_child);
                to_clone.push_back(_child{
                    srcChild,
                    target_child
                });
            }

        }


        return result;
    }

    void clearTreeSelection(HierarchyClearMethod method) {        
        // tree
        clear_tree_selection = method;
    }

    void forceTreeSelection(int uid){
        force_tree_selection = uid;
    }

};