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
    // const char *tree_drop_payload_id;
    // void *tree_drop_child;

    // std::shared_ptr<TreeNode> tree_drop_new_parent;
    // // TreeNode *tree_drop_new_parent;

    // std::shared_ptr<TreeNode> aux_dragdrop;
    //  TreeNode* aux_dragdrop;

public:
    EventCore::Event<void(std::shared_ptr<TreeNode>, bool)> OnTreeHover;
    EventCore::Event<void(std::shared_ptr<TreeNode>)> OnTreeSingleClick;
    EventCore::Event<void(std::shared_ptr<TreeNode>)> OnTreeDoubleClick;
    EventCore::Event<void(std::shared_ptr<TreeNode>)> OnTreeExpand;
    EventCore::Event<void(std::shared_ptr<TreeNode>)> OnTreeCollapse;
    EventCore::Event<void(std::shared_ptr<TreeNode>)> OnTreeSelect;

    EventCore::Event<void(const char *drag_payload, void *src, std::shared_ptr<TreeNode> target)> OnTreeDragDrop;

    friend class TreeNode;
};

enum class TreeNodeIconType : uint8_t
{
    Hierarchy = 0,
    Folder
};

class TreeNode
{
    // avoid copy, using copy constructors
    TreeNode(const TreeNode &) {}
    void operator=(const TreeNode &) {}

    TreeNode();

protected:
    char name[64];
    char name_tolower_no_accent[64];

    // identify the nodes created in this tree
    char prefix_id[64];
    char drag_payload_identifier[32];

    std::vector<const char *> drop_payload_identifier;

    void renderRecursive(TreeHolder *treeHolder, std::shared_ptr<TreeNode> &self, ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured); // , Platform::Time* time);
public:
    TreeNode(int32_t uid, TreeNodeIconType iconType, const char *name);

    TreeNode &setIsRoot(bool is_root);
    TreeNode &setPrefixID(const char *value);
    TreeNode &setDragPayloadID(const char *value);
    TreeNode &setDropPayload(const std::vector<const char *> &value);
    TreeNode &addDropPayload(const char *value);
    TreeNode &setName(const char *value);

    inline const char *getName() const { return name; }
    inline const char *getNameToLowerNoAccents() const { return name_tolower_no_accent; }

    TreeNode *parent;
    int32_t uid;
    std::vector<std::shared_ptr<TreeNode>> children;

    TreeNodeIconType iconType;

    // // the container needs to specify if this is a root node of not...
    bool isRoot;
    
    EventCore::PressReleaseDetector expanded;
    EventCore::PressReleaseDetector hovered;

    bool isLeaf();
    bool isNode();

    bool removeUID(int32_t uid);
    bool removeUIDRecursive(int32_t uid);
    std::shared_ptr<TreeNode> findUID(int32_t uid);

    std::shared_ptr<TreeNode> self();
    std::shared_ptr<TreeNode> removeSelf();
    void makeFirst();
    void makeLast();

    bool isChild(int32_t uid) const;

    void render(const char *str_imgui_id_selection, TreeHolder *treeHolder);
    void afterChild(TreeHolder *treeHolder);

    TreeNode &addChild(std::shared_ptr<TreeNode> treeNode);

    void sort(const std::function<int(const std::shared_ptr<TreeNode>&,const std::shared_ptr<TreeNode>&)> &comparer = Comparer_ASC){
        std::sort(children.begin(),children.end(),comparer);
    }

    static int Comparer_ASC(const std::shared_ptr<TreeNode> &a, const std::shared_ptr<TreeNode> &b)
    {
        return strcmp(a->name_tolower_no_accent, b->name_tolower_no_accent);
    }
    static int Comparer_DESC(const std::shared_ptr<TreeNode> &a, const std::shared_ptr<TreeNode> &b)
    {
        return strcmp(b->name_tolower_no_accent, a->name_tolower_no_accent);
    }

    static bool Reparent(std::shared_ptr<TreeNode> child, std::shared_ptr<TreeNode> new_parent);

    static inline std::shared_ptr<TreeNode> CreateShared(int32_t uid, TreeNodeIconType iconType, const char *name)
    {
        return std::make_shared<TreeNode>(uid, iconType, name);
    }
};
