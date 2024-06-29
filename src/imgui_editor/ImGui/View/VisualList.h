#pragma once

#include "common.h"
// #include "all.h"

//
// Drag source is of type: VisualList* VisualElement*
//
extern const char *DRAG_PAYLOAD_ID_PROJECT_LIST;

class ListElement;

class ListHolder
{
protected:
public:
    EventCore::Event<void(std::shared_ptr<ListElement>, bool)> OnListHover;
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListSingleClick;
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListDoubleClick;
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListExpand;
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListCollapse;
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListSelect;

    EventCore::Event<void(const char *drag_payload, void *src, std::shared_ptr<ListElement>target)> OnListDragDrop;

    friend class TreeNode;
};


// class ListElement
// {
//     // avoid copy, using copy constructors
//     ListElement(const ListElement &) {}
//     void operator=(const ListElement &) {}

//     ListElement();
    

// protected:
//     void renderRecursive(TreeHolder *treeHolder, std::shared_ptr<TreeNode> &self, ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured); // , Platform::Time* time);
// public:

//     TreeNode(int32_t uid, TreeNodeIconType iconType, const char *name);
    

//     TreeNode &setIsRoot(bool is_root);
//     TreeNode &setPrefixID(const char *value);
//     TreeNode &setDragPayloadID(const char *value);

//     TreeNode &setDropPayload(const std::vector<const char *> &value);

//     TreeNode &addDropPayload(const char *value);

//     TreeNode &setName(const char *value);

//     TreeNode *parent;
//     char name[64];
//     int32_t uid;
//     std::vector<std::shared_ptr<TreeNode>> children;
    
//     TreeNodeIconType iconType;

//     // the container needs to specify if this is a root node of not...
//     bool isRoot;
//     // identify the nodes created in this tree
//     char prefix_id[64];
//     char drag_payload_identifier[32];

//     std::vector<const char *> drop_payload_identifier;

//     EventCore::PressReleaseDetector expanded;
//     EventCore::PressReleaseDetector hovered;

//     bool isLeaf();
//     bool isNode();

//     bool removeUID(int32_t uid);
//     bool removeUIDRecursive(int32_t uid);
//     std::shared_ptr<TreeNode> findUID(int32_t uid);

//     std::shared_ptr<TreeNode> self();
//     std::shared_ptr<TreeNode> removeSelf();
//     void makeFirst();
//     void makeLast();

//     bool isChild(int32_t uid) const;

//     void render(const char *str_imgui_id, TreeHolder *treeHolder, std::shared_ptr<TreeNode> &self_root);

//     TreeNode &addChild(std::shared_ptr<TreeNode> treeNode);

//     static bool Reparent(std::shared_ptr<TreeNode> child, std::shared_ptr<TreeNode> new_parent);

//     static inline std::shared_ptr<ListElement> CreateShared(int32_t uid, TreeNodeIconType iconType, const char *name){
//         return std::make_shared<ListElement>(uid, iconType, name);
//     }
// };