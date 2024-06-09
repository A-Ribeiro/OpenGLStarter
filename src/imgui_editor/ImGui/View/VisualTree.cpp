#include "VisualTree.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

TreeNode::TreeNode()
{
    this->uid = 0;
    // this->icon_alias = NULL;
    // this->path = "-not-initialized-";
    this->name = "-not-initialized-";
    this->type = IconType::Small_BoxNode;
    this->expanded.setState(true);
    this->hovered.setState(false);
    this->isRoot = false;
}
TreeNode::TreeNode(int32_t uid, IconType type, std::string name)
{
    this->uid = uid;
    // this->icon_alias = icon_alias;
    // this->path = path;
    this->name = name;
    this->type = type;
    this->expanded.setState(true);
    this->hovered.setState(false);
}

bool TreeNode::isLeaf()
{
    return children.size() == 0;
}
bool TreeNode::isNode()
{
    return children.size() > 0;
}

bool TreeNode::removeUIDRecursive(int32_t uid)
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

void TreeNode::renderRecursive(TreeHolder *treeHolder, ImGuiID id_sel, int32_t selected_UID, bool *any_click_occured) //, Platform::Time* time)
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

    char txt[64];
    sprintf(txt, "##node_%i", uid);
    bool node_open = ImGui::TreeNodeEx(txt, flag_to_use);
    bool send_single_click = false;
    bool send_double_click = false;
    bool send_on_select = false;

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

    // event dispatching
    {
        if (hovered.down || hovered.up)
            treeHolder->OnTreeHover(this, hovered.pressed);
        if (send_single_click)
            treeHolder->OnTreeSingleClick(this);
        if (send_on_select)
            treeHolder->OnTreeSelect(this);
        if (send_double_click)
            treeHolder->OnTreeDoubleClick(this);
        if (expanded.down)
            treeHolder->OnTreeExpand(this);
        if (expanded.up)
            treeHolder->OnTreeCollapse(this);
    }

    if (node_open)
    {
        for (auto &child : children)
            child.renderRecursive(treeHolder, id_sel, selected_UID, any_click_occured); // , time);

        // if (this->isNode())
        ImGui::TreePop();
    }
}

void TreeNode::render(const char *str_imgui_id, TreeHolder *treeHolder)
{
    bool deselect_all = false;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_RectOnly))
    {
        if (ImGui::IsMouseClicked(0) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            // printf("Clicked on begin...\n");
            deselect_all = true;
        }
    }

    ImGuiID id_sel = ImGui::GetID(str_imgui_id);
    int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

    bool any_click_occured = false;
    this->renderRecursive(treeHolder, id_sel, selected_UID, &any_click_occured); // , & time);
    if (any_click_occured)
        deselect_all = false;

    if (deselect_all)
    {
        // printf("reset selection...\n");
        // ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
        ImGui::GetStateStorage()->SetInt(id_sel, 0);
        treeHolder->OnTreeSelect(NULL);
    }
}
