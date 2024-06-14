#pragma once

#include "common.h"
// #include "all.h"

//
// Drag source is of type: TreeNode*
//
extern const char *DRAG_PAYLOAD_ID_HIERARCHY_TREE;
extern const char *DRAG_PAYLOAD_ID_PROJECT_TREE;

class TreeNode;

class TreeHolder
{
protected:
    const char *tree_drop_payload_id;
    void *tree_drop_child;
    TreeNode *tree_drop_new_parent;

public:
    EventCore::Event<void(TreeNode *, bool)> OnTreeHover;
    EventCore::Event<void(TreeNode *)> OnTreeSingleClick;
    EventCore::Event<void(TreeNode *)> OnTreeDoubleClick;
    EventCore::Event<void(TreeNode *)> OnTreeExpand;
    EventCore::Event<void(TreeNode *)> OnTreeCollapse;
    EventCore::Event<void(TreeNode *)> OnTreeSelect;

    EventCore::Event<void(const char *drag_payload, void *src, TreeNode *target)> OnTreeDragDrop;

    friend class TreeNode;
};

enum class TreeNodeIconType : uint8_t
{
    Hierarchy = 0,
    Folder
};

class TreeNode
{
protected:
    void renderRecursive(TreeHolder *treeHolder, ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured); // , Platform::Time* time);
public:
    TreeNode();
    TreeNode(int32_t uid, TreeNodeIconType iconType, const char *name);

    TreeNode &setIsRoot(bool is_root);
    TreeNode &setPrefixID(const char *value);
    TreeNode &setDragPayloadID(const char *value);

    TreeNode &setDropPayload(const std::vector<const char *> &value);

    TreeNode &addDropPayload(const char *value);

    char name[64];
    int32_t uid;
    std::vector<TreeNode> children;
    
    TreeNodeIconType iconType;

    // the container needs to specify if this is a root node of not...
    bool isRoot;
    // identify the nodes created in this tree
    char prefix_id[64];
    char drag_payload_identifier[32];

    std::vector<const char *> drop_payload_identifier;

    EventCore::PressReleaseDetector expanded;
    EventCore::PressReleaseDetector hovered;

    bool isLeaf();
    bool isNode();

    bool removeUIDRecursive(int32_t uid);
    TreeNode * findUID(int32_t uid);

    bool isChild(int32_t uid) const;

    void render(const char *str_imgui_id, TreeHolder *treeHolder);

    TreeNode &addChild(const TreeNode &treeNode);

    static bool Reparent(TreeNode *root, const TreeNode & child, int32_t new_parent_uid);
};
