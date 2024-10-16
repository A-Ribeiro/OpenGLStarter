#include "VisualList.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const char *DRAG_PAYLOAD_ID_PROJECT_LIST = "PROJECT_LIST";

bool VisualList::CustomImGuiCommand_DrawItem(
    std::shared_ptr<ListElement> &itemSelf,
    int id,
    ImVec2 size,
    ListHolder *listHolder,
    bool *any_click_occured,
    int32_t selected_UID,
    ImGuiID id_sel)
{

    // ImGui::SetWindowFontScale(0.9f);
    ImGui::PushID(id);

    ImVec2 pos = ImGui::GetCursorPos();

    char aux[70];
    // sprintf(aux, "##%s_%i", name, id);
    snprintf(aux, 70, "##%s_%i", prefix_id, id);
    bool result = ImGui::Selectable(aux, itemSelf->selected, ImGuiSelectableFlags_None | ImGuiSelectableFlags_NoPadWithHalfSpacing, size);

    if (itemSelf->scroll_to_this_item)
    {
        itemSelf->scroll_to_this_item = false;
        ImGui::ScrollToItem(ImGuiScrollFlags_KeepVisibleEdgeY);
    }

    bool send_single_click = false;
    bool send_double_click = false;
    bool send_on_select = false;

    if (ImGui::BeginDragDropTarget())
    {
        const ImGuiPayload *payload;
        for (const char *drop_target : this->drop_payload_identifier)
        {
            if (payload = ImGui::AcceptDragDropPayload(drop_target))
            {
                listHolder->OnListDragDrop(drop_target,
                                           (void *)*(intptr_t *)payload->Data,
                                           itemSelf);
            }
        }
        ImGui::EndDragDropTarget();
    }
    // draw line on drag from any tree node.
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 4.0f); // Draw a line between the button and the mouse cursor

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
        if (itemSelf->uid == selected_UID && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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
                send_on_select = (ImGui::GetStateStorage()->GetInt(id_sel, 0) != itemSelf->uid);
                ImGui::GetStateStorage()->SetInt(id_sel, itemSelf->uid);
            }
        }
    }
    itemSelf->hovered.setState(ImGui::IsItemHovered());

    auto font = ImGuiManager::Instance()->font_half_size;

    auto style = ImGui::GetStyle();
    
    ImRect image_rect = ImRect(pos + style.FramePadding, pos + ImVec2(size.x - style.FramePadding.x, size.y * 0.4f));
    ImRect text_rect = ImRect(ImVec2(pos.x + style.FramePadding.x, image_rect.Max.y), pos + size - style.FramePadding);

    ImVec2 image_size = image_rect.GetSize();
    // ImVec2 text_size = text_rect.GetSize();

    // ImGui::GetWindowDrawList()->AddRect(
    //     ImGui::GetItemRectMin(),
    //     ImGui::GetItemRectMin() + size,
    //     IM_COL32(0xff, 0, 0, 0xff));

    // ImGui::GetWindowDrawList()->AddRectFilled(
    //     ImGui::GetItemRectMin() + image_rect.Min - pos,
    //     ImGui::GetItemRectMin() + image_rect.Max - pos,
    //     IM_COL32(255, 0, 255, 64));

    // ImGui::GetWindowDrawList()->AddRectFilled(
    //     ImGui::GetItemRectMin() + text_rect.Min - pos,
    //     ImGui::GetItemRectMin() + text_rect.Max - pos,
    //     IM_COL32(255, 255, 0, 64));

    ImGui::SetCursorPos(image_rect.Min);

    IconType iconToUse = IconType::Small_BoxNode;
    if (itemSelf->data != nullptr)
        iconToUse = itemSelf->data->getIcon();

    bool is_to_stretch_image = ImGuiManager::Instance()->stretch[(int)iconToUse];
    AppKit::OpenGL::GLTexture *texture_ogl = ImGuiManager::Instance()->icons[(int)iconToUse];
    ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)texture_ogl->mTexture;

    float my_tex_w = image_size.x;
    float my_tex_h = image_size.y;

    ImVec2 uv_min = ImVec2(0.0f, 0.0f); // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f); // Lower-right

    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f); // 50% opaque white

    if (is_to_stretch_image)
    {

        float aspect = my_tex_h / my_tex_w;
        uv_max.y = aspect;

        float c_aspect = (aspect - 1.0f) * 0.5f;
        uv_min.y -= c_aspect;
        uv_max.y -= c_aspect;
    }
    else
    {

        float aspect = my_tex_w / my_tex_h;
        uv_max.x = aspect;

        float c_aspect = (aspect - 1.0f) * 0.5f;
        uv_min.x -= c_aspect;
        uv_max.x -= c_aspect;
    }

    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);

    float text_width = size.x;

    

    itemSelf->centeredText.drawText(text_rect.GetCenter(), font);

    // ImGui::PushFont(ImGuiManager::Instance()->font_half_size);

    // ImVec2 text_size = ImGui::CalcTextSize(itemSelf->name, nullptr, false, text_width);

    // ImVec2 aux_pos = size - text_size;
    // aux_pos.x = aux_pos.x * 0.5f;
    // aux_pos.y = aux_pos.y * 0.5f + image_size.y * 0.5f;

    // aux_pos = pos + aux_pos;

    // ImGui::SetCursorPos(aux_pos);
    // ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + text_width);
    // ImGui::Text("%s", itemSelf->name);
    // ImGui::PopTextWrapPos();

    // ImGui::PopFont();

    ImGui::SetCursorPos(pos);
    ImGui::Dummy(size);

    ImGui::PopID();

    // ImGui::SetWindowFontScale(1.0f);

    // event dispatching
    {
        if (itemSelf->hovered.down || itemSelf->hovered.up)
            listHolder->OnListHover(itemSelf, itemSelf->hovered.pressed);
        if (send_single_click)
            listHolder->OnListSingleClick(itemSelf);
        if (send_on_select)
            listHolder->OnListSelect(itemSelf);
        if (send_double_click)
            listHolder->OnListDoubleClick(itemSelf);
    }

    return result;
}

ListElement::ListElement()
{
    uid = 0;
    visualList = nullptr;
    this->setName("-not-set-");
    // this->setIcon(RandomListIcon());
    selected = false;
    scroll_to_this_item = false;
}

ListElement::ListElement(int32_t uid, VisualList *visualList, const char *name, std::shared_ptr<ListData> data)
{
    this->uid = uid;
    this->visualList = visualList;
    this->setName(name);
    // this->setIcon(icon);
    this->data = data;
    selected = false;
    scroll_to_this_item = false;
}

ListElement &ListElement::setName(const char *value)
{
    snprintf(this->name, 64, "%s", value);
    std::string lower_no_accents = ITKCommon::StringUtil::removeAccents(this->name);
    lower_no_accents = ITKCommon::StringUtil::toLower(lower_no_accents);
    snprintf(this->name_tolower_no_accent, 64, "%s", lower_no_accents.c_str());

    auto style = ImGui::GetStyle();

    centeredText.setText(this->name, ImGuiManager::Instance()->font_half_size, visualList->element_rect_size.x - style.FramePadding.x * 2.0f);

    return *this;
}

// ListElement &ListElement::setIcon(const IconType &icon)
// {
//     this->icon = icon;
//     return *this;
// }

std::shared_ptr<ListElement> ListElement::self()
{
    // return visualList->findUID(uid);
    return std::shared_ptr<ListElement>(this->mSelf);
}

std::shared_ptr<ListElement> ListElement::removeSelf()
{
    // std::shared_ptr<ListElement> result = visualList->findUID(uid);
    //    if (result != nullptr)
    auto result = this->self();
    visualList->removeUID(uid);
    return result;
}

void ListElement::makeFirst()
{
    auto result = this->self();

    // auto self = this->visualList->findUID(this->uid);
    // if (self == nullptr)
    //     return;
    this->visualList->removeUID(this->uid);
    this->visualList->items.insert(this->visualList->items.begin(), result);
}

void ListElement::makeLast()
{
    auto result = this->self();

    // auto self = this->visualList->findUID(this->uid);
    // if (self == nullptr)
    //     return;
    this->visualList->removeUID(this->uid);
    this->visualList->items.push_back(result);
}

void ListElement::scrollToThisItem()
{
    scroll_to_this_item = true;
}

void ListElement::render(const char *str_imgui_id_selection, ListHolder *listHolder)
{
}

VisualList::VisualList()
{
    uid_incrementer = 1;

    this->setPrefixID("-not-set-");
    this->setDragPayloadID("-not-set-");
}

// called after GlobalScale Computation
void VisualList::Init()
{

    auto imGuiManager = ImGuiManager::Instance();

    element_rect_size = ImVec2(80, 60) * imGuiManager->GlobalScale;
    element_spacing = ImVec2(4, 4) * imGuiManager->GlobalScale;
}

VisualList &VisualList::setPrefixID(const char *value)
{
    snprintf(this->prefix_id, 64, "%s", value);
    return *this;
}

VisualList &VisualList::setDragPayloadID(const char *value)
{
    snprintf(this->drag_payload_identifier, 32, "%s", value);
    return *this;
}

VisualList &VisualList::setDropPayload(const std::vector<const char *> &value)
{
    this->drop_payload_identifier = value;
    return *this;
}

VisualList &VisualList::addDropPayload(const char *value)
{
    this->drop_payload_identifier.push_back(value);
    return *this;
}

void VisualList::clear()
{
    uid_incrementer = 1;
    items.clear();
}
bool VisualList::removeUID(int32_t uid)
{
    for (auto it = items.begin(); it != items.end(); it++)
    {
        if ((*it)->uid == uid)
        {
            items.erase(it);
            return true;
        }
    }
    return false;
}
std::shared_ptr<ListElement> VisualList::findUID(int32_t uid)
{
    for (auto &chld : items)
    {
        if (chld->uid == uid)
            return chld;
    }
    return nullptr;
}

std::shared_ptr<ListElement> VisualList::addItem(const char *name, std::shared_ptr<ListData> data)
{
    items.push_back(ListElement::CreateShared(uid_incrementer++, this, name, data));
    return items.back();
}

void VisualList::render(const char *str_imgui_id_selection, ListHolder *listHolder)
{
    auto window = ImGui::GetCurrentWindow();
    ImRect window_frame = window->ClipRect;
    window_frame.Expand(-3.5f);
    if (ImGui::BeginDragDropTargetCustom(window_frame, window->ID))
    {
        const ImGuiPayload *payload;
        for (const char *drop_target : this->drop_payload_identifier)
        {
            if (payload = ImGui::AcceptDragDropPayload(drop_target))
            {
                listHolder->OnListDragDrop(drop_target,
                                           (void *)*(intptr_t *)payload->Data,
                                           nullptr);
            }
        }
        ImGui::EndDragDropTarget();
    }

    bool deselect_all = false;

    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows))
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsKeyPressed(ImGuiKey_Escape, false))
        {
            // printf("Clicked on begin...\n");
            deselect_all = true;
        }
    }

    ImGuiID id_sel = ImGui::GetID(str_imgui_id_selection);
    int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);

    bool any_click_occured = false;

    ImVec2 area = ImGui::GetContentRegionAvail();

    ImVec2 icon_size = element_rect_size; // ImVec2(100, 80);
    ImVec2 spacing = element_spacing;     // ImVec2(4, 4);

    // auto imGuiManager = ImGuiManager::Instance();
    // icon_size *= imGuiManager->GlobalScale;
    // spacing *= imGuiManager->GlobalScale;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

    float count_size = icon_size.x;
    for (int i = 0; i < (int)items.size(); i++)
    {
        auto &item = items[i];

        bool element_selected = selected_UID == item->uid;
        if (item->selected != element_selected)
            item->selected = element_selected;

        if (CustomImGuiCommand_DrawItem(
                item,
                i,
                icon_size,
                listHolder,
                &any_click_occured,
                selected_UID,
                id_sel))
        {
            // if (item->selected)
            // {
            //     selected_UID = item->uid;
            //     ImGui::GetStateStorage()->SetInt(id_sel, 0);
            // }

            // for (size_t j = 0; j < items.size(); j++)
            // {
            //     if (i != j && items[j]->selected)
            //         items[j]->selected = false;
            // }
        }

        count_size += spacing.x + icon_size.x;
        if (count_size > area.x)
        {
            count_size = icon_size.x;
        }
        else
            ImGui::SameLine();
    }

    ImGui::PopStyleVar();

    if (any_click_occured)
        deselect_all = false;

    if (deselect_all)
    {
        // printf("reset selection...\n");
        // ImGuiID id_sel = ImGui::GetID("##hierarchy_sel");
        ImGui::GetStateStorage()->SetInt(id_sel, 0);
        listHolder->OnListSelect(nullptr);
    }
}
