#include "VisualTree.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const char *DRAG_PAYLOAD_ID_HIERARCHY_TREE = "HIERARCHY_TREE";
const char *DRAG_PAYLOAD_ID_PROJECT_TREE = "PROJECT_TREE";

TreeNode::TreeNode()
{
    this->uid = 0;
    // this->icon_alias = nullptr;
    // this->path = "-not-initialized-";
    //snprintf(this->name, 64, "-not-initialized-");
    setName("-not-initialized-");

    //this->iconType = TreeNodeIconType::Hierarchy;
    this->expanded.setState(true);
    this->hovered.setState(false);

    this->isRoot = false;
    snprintf(this->prefix_id, 64, "-not-set-");
    snprintf(this->drag_payload_identifier, 32, "-not-set-");

    // this->parent = nullptr;
    scroll_to_this_item = false;
}

TreeNode::TreeNode(int32_t uid, std::shared_ptr<TreeData> data, const char *name)
{
    this->uid = uid;
    // this->icon_alias = icon_alias;
    // this->path = path;
    // snprintf(this->name, 64, "%s", name);
    setName(name);

    this->data = data;
    this->expanded.setState(true);
    this->hovered.setState(false);

    this->isRoot = false;
    snprintf(this->prefix_id, 64, "-not-set-");
    snprintf(this->drag_payload_identifier, 32, "-not-set-");

    // this->parent = nullptr;

    scroll_to_this_item = false;
}

bool TreeNode::isLeaf()
{
    return children.size() == 0;
}
bool TreeNode::isNode()
{
    return children.size() > 0;
}

void TreeNode::clear() {
    children.clear();
}

bool TreeNode::removeUID(int32_t uid) {
    for (auto it = children.begin(); it != children.end(); it++)
    {
        if ((*it)->uid == uid)
        {
            children.erase(it);
            return true;
        }
    }
    return false;
}

bool TreeNode::removeUIDRecursive(int32_t uid)
{
    for (auto it = children.begin(); it != children.end(); it++)
    {
        if ((*it)->uid == uid)
        {
            children.erase(it);
            return true;
        }
        else if ((*it)->removeUIDRecursive(uid))
        {
            return true;
        }
    }
    return false;
}

std::shared_ptr<TreeNode> TreeNode::findUID(int32_t uid) {
    for (auto &chld : children)
    {
        if (chld->uid == uid)
            return chld;
        else {
            auto result = chld->findUID(uid);
            if (result != nullptr)
                return result;
        }
    }
    return nullptr;
}

std::shared_ptr<TreeNode> TreeNode::findChildByName(const char* name_param, bool recursive) {
    for (auto &chld : children)
    {
        if ( strcmp(chld->name, name_param) == 0)
            return chld;
        else if (recursive) {
            auto result = chld->findUID(uid);
            if (result != nullptr)
                return result;
        }
    }
    return nullptr;
}

bool TreeNode::isChild(int32_t uid) const
{
    for (const auto &chld : children)
    {
        if (chld->uid == uid){
            return true;
        } else if (chld->isChild(uid))
            return true;
    }
    return false;
}

void TreeNode::renderRecursive(TreeHolder *treeHolder, std::shared_ptr<TreeNode> &self, ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured) //, Platform::Time* time)
{
    auto imGuiManager = ImGuiManager::Instance();

    // ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
    // int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

    ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth; // | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf |
                                    // ImGuiTreeNodeFlags_NoTreePushOnOpen |
                                    ImGuiTreeNodeFlags_SpanAvailWidth; // | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool selected = (selected_UID == this->uid);
    uint32_t selected_uint32 = ImGuiTreeNodeFlags_Selected & -(selected & 0x1);
    parent_flags = (parent_flags & ~ImGuiTreeNodeFlags_Selected) | selected_uint32;
    leaf_flags = (leaf_flags & ~ImGuiTreeNodeFlags_Selected) | selected_uint32;

    ImGuiTreeNodeFlags flag_to_use = parent_flags;
    
    IconType iconToUse = IconType::Small_BoxNode;
    if (data != nullptr){
        iconToUse = data->getIcon();
    }

    // if (iconType == TreeNodeIconType::Hierarchy)
    //     iconToUse = IconType::Small_BoxNode_Filled;
    // else if (iconType == TreeNodeIconType::Folder)
    //     iconToUse = IconType::Small_Folder_Filled;

    if (this->isLeaf())
    {
        flag_to_use = leaf_flags;

        // if (iconType == TreeNodeIconType::Hierarchy)
        //     iconToUse = IconType::Small_BoxNode;
        // else if (iconType == TreeNodeIconType::Folder)
        //     iconToUse = IconType::Small_Folder_Empty;
    }

    ImGui::SetNextItemOpen(expanded.pressed);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2) * imGuiManager->GlobalScale);

    // Tree Node Header
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2) * imGuiManager->GlobalScale); // Tighten spacing
    // ImGui::AlignTextToFramePadding();

    // force tree selection size
    ImGui::GetCurrentContext()->CurrentWindow->DC.CurrLineSize.y = ImGui::GetFrameHeight(); // 16;

    char txt[128];
    snprintf(txt, 128, "##%s_%i", this->prefix_id, uid);
    bool node_open = ImGui::TreeNodeEx(txt, flag_to_use);
    bool send_single_click = false;
    bool send_double_click = false;
    bool send_on_select = false;

    if (scroll_to_this_item){
        scroll_to_this_item = false;
        ImGui::ScrollToItem(ImGuiScrollFlags_KeepVisibleEdgeY);
    }

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload *payload;
        for (const char *drop_target : this->drop_payload_identifier)
        {
            if (payload = ImGui::AcceptDragDropPayload(drop_target))
            {
                // //(const char* drag_payload, void *src, TreeNode*target)
                // // treeHolder->OnTreeDragDrop(drop_target, payload->Data, this);
                // treeHolder->tree_drop_payload_id = drop_target;
                
                // treeHolder->tree_drop_child = (void*)*(intptr_t*)payload->Data;

                // //memcpy(&treeHolder->tree_drop_child, &payload->Data, sizeof(intptr_t));
                // //treeHolder->tree_drop_child = payload->Data;
                // treeHolder->tree_drop_new_parent = self;

                treeHolder->OnTreeDragDrop(drop_target, 
                    (void*)*(intptr_t*)payload->Data,
                    self
                );
            }
        }
        ImGui::EndDragDropTarget();
    }
    // draw line on drag from any tree node.
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImVec4(1,0,0,1)), 4.0f); // Draw a line between the button and the mouse cursor

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip))
        {
            intptr_t ptr_this = (intptr_t)this;
            ImGui::SetDragDropPayload(this->drag_payload_identifier, &ptr_this, sizeof(intptr_t), ImGuiCond_Once);
            ImGui::EndDragDropSource();
        }
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
    { // && !ImGui::IsItemToggledOpen()) {
        *any_click_occured = true;
        // deselect_all = false;
        // printf("click root\n");
        // time->update();
        if (this->uid == selected_UID && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            // double click
            // printf("double click\n");
            // ImGui::GetStateStorage()->SetInt(id_sel, this->uid);
            send_double_click = true;
        }
        else
        {
            // printf("single click\n");
            send_single_click = true;

            // if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                send_on_select = (ImGui::GetStateStorage()->GetInt(id_sel, 0) != this->uid);
                ImGui::GetStateStorage()->SetInt(id_sel, this->uid);
            }
        }
    }
    hovered.setState(ImGui::IsItemHovered());

    ImGui::SameLine();

    AppKit::OpenGL::GLTexture *texture_ogl = ImGuiManager::Instance()->icons[(int)iconToUse];
    ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)texture_ogl->mTexture;
    float my_tex_w = (float)texture_ogl->width * 0.5f;
    float my_tex_h = (float)texture_ogl->height * 0.5f;
    const ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    const ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    const ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f); // 50% opaque white

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetFrameHeight() - my_tex_h * imGuiManager->GlobalScale) * 0.5f);
    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h) * imGuiManager->GlobalScale, uv_min, uv_max, tint_col, border_col);

    ImGui::SameLine();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", this->name);

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    expanded.setState(node_open);

    // event dispatching
    {
        if (hovered.down || hovered.up)
            treeHolder->OnTreeHover(self, hovered.pressed);
        if (send_single_click)
            treeHolder->OnTreeSingleClick(self);
        if (send_on_select)
            treeHolder->OnTreeSelect(self);
        if (send_double_click)
            treeHolder->OnTreeDoubleClick(self);
        if (expanded.down)
            treeHolder->OnTreeExpand(self);
        if (expanded.up)
            treeHolder->OnTreeCollapse(self);
    }

    if (node_open)
    {
        for (auto &child : children)
            child->renderRecursive(treeHolder, child, id_sel, selected_UID, any_click_occured); // , time);

        // if (this->isNode())
        ImGui::TreePop();
    }
}

// void TreeNode::afterChild(TreeHolder *treeHolder) {

//     // auto window = ImGui::GetCurrentWindow();
//     // //ImRect window_frame = window->ContentRegionRect;

//     // ImVec2 padding = ImGui::GetStyle().WindowPadding;// + ImGui::GetStyle().FramePadding;

//     // ImRect window_frame;
//     // window_frame.Min = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin() - padding;
//     // window_frame.Max = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMax() + padding;

//     // //if (ImGui::BeginDragDropTargetCustom( window_frame, window->ID ))
//     // if (ImGui::BeginDragDropTarget())
//     // {
//     //     const ImGuiPayload *payload;
//     //     for (const char *drop_target : this->drop_payload_identifier)
//     //     {
//     //         if (payload = ImGui::AcceptDragDropPayload(drop_target))
//     //         {
//     //             treeHolder->OnTreeDragDrop(drop_target, 
//     //                 (void*)*(intptr_t*)payload->Data,
//     //                 nullptr
//     //             );
//     //         }
//     //     }
//     //     ImGui::EndDragDropTarget();
//     // }


// }

void TreeNode::render(const char *str_imgui_id_selection, TreeHolder *treeHolder)
{
    if (!isRoot)// || getParent()->children.size() != 1)
        return;

    auto window = ImGui::GetCurrentWindow();
    ImRect window_frame = window->ClipRect;
    window_frame.Expand(-3.5f);

    if (ImGui::BeginDragDropTargetCustom( window_frame, window->ID ))
    {
        const ImGuiPayload *payload;
        for (const char *drop_target : this->drop_payload_identifier)
        {
            if (payload = ImGui::AcceptDragDropPayload(drop_target))
            {
                treeHolder->OnTreeDragDrop(drop_target, 
                    (void*)*(intptr_t*)payload->Data,
                    nullptr
                );
            }
        }
        ImGui::EndDragDropTarget();
    }


    // treeHolder->tree_drop_payload_id = nullptr;
    // treeHolder->tree_drop_child = nullptr;
    // treeHolder->tree_drop_new_parent = nullptr;
    // treeHolder->aux_dragdrop = nullptr;

    bool deselect_all = false;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
    {
        if (ImGui::IsMouseClicked( ImGuiMouseButton_Left ) 
        || ImGui::IsMouseClicked( ImGuiMouseButton_Right ) 
        || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            // printf("Clicked on begin...\n");
            deselect_all = true;
        }
    }

    ImGuiID id_sel = ImGui::GetID(str_imgui_id_selection);
    int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

    bool any_click_occured = false;
    //std::shared_ptr<TreeNode> &self_root = this->parent->children[0];
    
    std::shared_ptr<TreeNode> self_root = this->self();
    
    this->renderRecursive(treeHolder, self_root, id_sel, selected_UID, &any_click_occured); // , & time);
    if (any_click_occured)
        deselect_all = false;

    if (deselect_all)
    {
        // printf("reset selection...\n");
        // ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
        ImGui::GetStateStorage()->SetInt(id_sel, 0);
        treeHolder->OnTreeSelect(nullptr);
    }

    // if (treeHolder->tree_drop_payload_id != nullptr &&
    //     treeHolder->tree_drop_child != nullptr &&
    //     treeHolder->tree_drop_new_parent != nullptr)
    // {
    //     treeHolder->OnTreeDragDrop(
    //         treeHolder->tree_drop_payload_id,
    //         treeHolder->tree_drop_child,
    //         treeHolder->tree_drop_new_parent);
    // }
}

TreeNode &TreeNode::setIsRoot(bool is_root)
{
    this->isRoot = is_root;
    return *this;
}
TreeNode &TreeNode::setPrefixID(const char *value)
{
    snprintf(this->prefix_id, 64, "%s", value);
    return *this;
}
TreeNode &TreeNode::setDragPayloadID(const char *value)
{
    snprintf(this->drag_payload_identifier, 32, "%s", value);
    return *this;
}
TreeNode &TreeNode::setDropPayload(const std::vector<const char *> &value)
{
    this->drop_payload_identifier = value;
    return *this;
}
TreeNode &TreeNode::addDropPayload(const char *value)
{
    this->drop_payload_identifier.push_back(value);
    return *this;
}

TreeNode &TreeNode::setName(const char *value) {
    snprintf(this->name, 64, "%s", value);
    std::string lower_no_accents = ITKCommon::StringUtil::removeAccents(this->name);
    lower_no_accents = ITKCommon::StringUtil::toLower(lower_no_accents);
    snprintf(this->name_tolower_no_accent, 64, "%s", lower_no_accents.c_str());
    return *this;
}

std::shared_ptr<TreeNode> TreeNode::self() {
    // if (this->parent != nullptr)
    //     return this->parent->findUID(this->uid);
    // return nullptr;
    return std::shared_ptr<TreeNode>(this->mSelf);
}

std::shared_ptr<TreeNode> TreeNode::removeSelf(){

    auto parent = getParent();
    auto result = this->self();

    if (parent != nullptr){
        parent->removeUID(this->uid);

        // result = this->parent->findUID(this->uid);
        // if (result != nullptr)
        //     this->parent->removeUID(this->uid);
    }

    return result;
}

void TreeNode::makeFirst(){
    auto parent = getParent();

    if (parent != nullptr){

        // auto self = this->parent->findUID(this->uid);
        // if (self == nullptr)
        //     return;
        auto self = this->self();

        parent->removeUID(this->uid);
        parent->children.insert(parent->children.begin(), self);
    }
}
void TreeNode::makeLast(){
    auto parent = getParent();

    if (parent != nullptr){
        // auto self = this->parent->findUID(this->uid);
        // if (self == nullptr)
        //     return;
        auto self = this->self();

        parent->removeUID(this->uid);
        parent->children.push_back(self);
    }
}

void TreeNode::scrollToThisItem() {
    scroll_to_this_item = true;
}

std::shared_ptr<TreeNode> TreeNode::addChild(std::shared_ptr<TreeNode> treeNode, int before_uid)
{
    treeNode->removeSelf();
    treeNode->setParent(this->self());

    if (before_uid != -1){
        auto it = children.begin();
        while (it != children.end() && (*it)->uid != before_uid){
            it++;
        }
        children.insert(it,treeNode);
    } else
        children.push_back(treeNode);

    auto result = children.back();

    result->setIsRoot(false).setPrefixID(this->prefix_id).setDragPayloadID(this->drag_payload_identifier).setDropPayload(this->drop_payload_identifier);
    
    return result;
}

bool TreeNode::Reparent(std::shared_ptr<TreeNode> child, std::shared_ptr<TreeNode> new_parent)
{
    if (child->isChild(new_parent->uid))
        return false;

    // TreeNode child_copy = child;
    // root->removeUIDRecursive(child_copy.uid);
    // TreeNode *new_parent_node = root->findUID(new_parent_uid);

    new_parent->addChild(child);

    return true;
}
