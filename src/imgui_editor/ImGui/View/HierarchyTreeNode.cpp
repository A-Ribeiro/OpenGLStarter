#include "HierarchyTreeNode.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

HierarchyTreeNode::HierarchyTreeNode()
{
    this->uid = 0;
    // this->icon_alias = NULL;
    // this->path = "-not-initialized-";
    this->name = "-not-initialized-";
    this->type = IconType::Small_BoxNode;
    this->expanded.setState(true);
    this->hovered.setState(false);
}
HierarchyTreeNode::HierarchyTreeNode(int32_t uid, IconType type, std::string name)
{
    this->uid = uid;
    // this->icon_alias = icon_alias;
    // this->path = path;
    this->name = name;
    this->type = type;
    this->expanded.setState(true);
    this->hovered.setState(false);
}

bool HierarchyTreeNode::isLeaf()
{
    return children.size() == 0;
}
bool HierarchyTreeNode::isNode()
{
    return children.size() > 0;
}

bool HierarchyTreeNode::removeUIDRecursive(int32_t uid)
{
    for (auto it = children.begin(); it != children.end(); it++)
    {
        if ((*it).uid == uid)
        {
            children.erase(it);
            return true;
        }
        else if ((*it).removeUIDRecursive(uid))
        {
            return true;
        }
    }
    return false;
}

void HierarchyTreeNode::renderRecursive(ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured) //, Platform::Time* time)
{
    auto imGuiManager = ImGuiManager::Instance();

    // ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
    // int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

    ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;                                                               // | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth; // | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool selected = (selected_UID == this->uid);
    uint32_t selected_uint32 = ImGuiTreeNodeFlags_Selected & -(selected & 0x1);
    parent_flags = (parent_flags & ~ImGuiTreeNodeFlags_Selected) | selected_uint32;
    leaf_flags = (leaf_flags & ~ImGuiTreeNodeFlags_Selected) | selected_uint32;

    ImGuiTreeNodeFlags flag_to_use = parent_flags;
    if (this->isLeaf())
    {
        flag_to_use = leaf_flags;
    }

    ImGui::SetNextItemOpen(expanded.pressed);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2) * imGuiManager->GlobalScale);

    // Tree Node Header
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 2) * imGuiManager->GlobalScale); // Tighten spacing
    // ImGui::AlignTextToFramePadding();

    // force tree selection size
    ImGui::GetCurrentContext()->CurrentWindow->DC.CurrLineSize.y = ImGui::GetFrameHeight(); // 16;

    bool node_open = ImGui::TreeNodeEx("##hierarchy_root", flag_to_use);
    bool send_single_click = false;
    bool send_double_click = false;
    bool send_on_select = false;

    if (ImGui::IsItemClicked())
    { // && !ImGui::IsItemToggledOpen()) {
        *any_click_occured = true;
        // deselect_all = false;
        // printf("click root\n");
        // time->update();
        if (this->uid == selected_UID && ImGui::IsMouseDoubleClicked(0))
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
            
            send_on_select = (ImGui::GetStateStorage()->GetInt(id_sel,0) != this->uid);
            ImGui::GetStateStorage()->SetInt(id_sel, this->uid);
        }
    }
    hovered.setState(ImGui::IsItemHovered());

    ImGui::SameLine();

    AppKit::OpenGL::GLTexture *texture_ogl = ImGuiManager::Instance()->icons[(int)this->type];
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
    ImGui::Text("%s", this->name.c_str());

    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

    expanded.setState(node_open);

    if (node_open)
    {
        for (auto &child : children)
            child.renderRecursive(id_sel, selected_UID, any_click_occured); // , time);

        if (this->isNode())
            ImGui::TreePop();
    }

    if (hovered.down || hovered.up)
        imGuiManager->hierarchy.OnHover(this, hovered.pressed);
    if (send_single_click)
        imGuiManager->hierarchy.OnSingleClick(this);
    if (send_on_select)
        imGuiManager->hierarchy.OnSelect(this);
    if (send_double_click)
        imGuiManager->hierarchy.OnDoubleClick(this);
    if (expanded.down)
        imGuiManager->hierarchy.OnExpand(this);
    if (expanded.up)
        imGuiManager->hierarchy.OnCollapse(this);
}
