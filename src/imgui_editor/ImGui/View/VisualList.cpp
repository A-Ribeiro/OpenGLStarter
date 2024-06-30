#include "VisualList.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const char *DRAG_PAYLOAD_ID_PROJECT_LIST = "PROJECT_LIST";


bool CustomImGuiCommand_DrawItem(int id, const char *prefix_name, const char *name, bool *selected, IconType icon, ImVec2 size)
{

    // ImGui::SetWindowFontScale(0.9f);
    ImGui::PushID(id);

    ImVec2 pos = ImGui::GetCursorPos();
    // ImVec2 size = ImVec2(100,80);
    // ImVec2 spacing = ImVec2(5,5);

    ImVec2 image_size = size * 0.4f;
    // ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5,1.0) );

    // ImGui::SetCursorPos(pos);
    char aux[64];
    //sprintf(aux, "##%s_%i", name, id);
    sprintf(aux, "%s_%i", prefix_name, id);
    bool result = ImGui::Selectable(aux, selected, ImGuiSelectableFlags_None | ImGuiSelectableFlags_NoPadWithHalfSpacing, size);

    // ImGui::PopStyleVar();
    // ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0xff, 0, 0, 0xff));
    // printf("%f\n", ImGui::GetItemRectMax().x - ImGui::GetItemRectMin().x);
    // ImGui::SameLine();

    ImGui::SetCursorPos(pos + ImVec2((size.x - image_size.x) * 0.5f, 0));

    bool is_to_stretch_image = ImGuiManager::Instance()->stretch[(int)icon];
    AppKit::OpenGL::GLTexture *texture_ogl = ImGuiManager::Instance()->icons[(int)icon];
    ImTextureID my_tex_id = (ImTextureID)(ogltex_to_imguitex)texture_ogl->mTexture;
    // float my_tex_w = (float)texture_ogl->width * 0.5f;
    // float my_tex_h = (float)texture_ogl->height * 0.5f;
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
        // tint_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

        float aspect = my_tex_w / my_tex_h;
        uv_max.x = aspect;

        float c_aspect = (aspect - 1.0f) * 0.5f;
        uv_min.x -= c_aspect;
        uv_max.x -= c_aspect;
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (ImGui::GetFrameHeight() - 16.0f) * 0.5f);
    ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);

    float text_width = size.x;

    ImVec2 text_size = ImGui::CalcTextSize(name, NULL, false, text_width);

    ImVec2 aux_pos = size - text_size;
    aux_pos.x = aux_pos.x * 0.5f;
    aux_pos.y = aux_pos.y * 0.5f + image_size.y * 0.5f;

    aux_pos = pos + aux_pos;

    ImGui::SetCursorPos(aux_pos);
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + text_width);
    ImGui::Text("%s", name);
    ImGui::PopTextWrapPos();

    ImGui::SetCursorPos(pos);
    ImGui::Dummy(size);
    // ImGui::TextWrapped("fileName.jpg")

    ImGui::PopID();

    // ImGui::SetWindowFontScale(1.0f);

    return result;
}


ListElement::ListElement() {
    uid = 0;
    visualList = NULL;
    this->setName("-not-set-");
    this->setIcon(RandomListIcon());
    selected = false;
}

ListElement::ListElement(int32_t uid, VisualList *visualList, const char *name, const IconType &icon){
    this->uid = uid;
    this->visualList = visualList;
    this->setName(name);
    this->setIcon(icon);
    selected = false;
}

ListElement &ListElement::setName(const char *value){
    snprintf(this->name, 64, "%s", value);
    std::string lower_no_accents = ITKCommon::StringUtil::removeAccents(this->name);
    lower_no_accents = ITKCommon::StringUtil::toLower(lower_no_accents);
    snprintf(this->name_tolower_no_accent, 64, "%s", lower_no_accents.c_str());
    return *this;
}

ListElement &ListElement::setIcon(const IconType &icon){
    this->icon = icon;
    return *this;
}

std::shared_ptr<ListElement> ListElement::self(){
    return visualList->findUID(uid);
}

std::shared_ptr<ListElement> ListElement::removeSelf(){
    std::shared_ptr<ListElement> result = visualList->findUID(uid);
    if (result != nullptr)
        visualList->removeUID(uid);
    return result;
}

void ListElement::makeFirst(){
    auto self = this->visualList->findUID(this->uid);
    if (self == nullptr)
        return;
    this->visualList->removeUID(this->uid);
    this->visualList->items.insert(this->visualList->items.begin(), self);
}

void ListElement::makeLast(){
    auto self = this->visualList->findUID(this->uid);
    if (self == nullptr)
        return;
    this->visualList->removeUID(this->uid);
    this->visualList->items.push_back(self);
}

void ListElement::render(const char *str_imgui_id_selection, ListHolder *listHolder) {

}


VisualList::VisualList(){
    uid_incrementer = 1;

    this->setPrefixID("-not-set-");
    this->setDragPayloadID("-not-set-");

}


VisualList &VisualList::setPrefixID(const char *value){
    snprintf(this->prefix_id, 64, "%s", value);
    return *this;
}

VisualList &VisualList::setDragPayloadID(const char *value){
    snprintf(this->drag_payload_identifier, 32, "%s", value);
    return *this;
}

VisualList &VisualList::setDropPayload(const std::vector<const char *> &value){
    this->drop_payload_identifier = value;
    return *this;
}

VisualList &VisualList::addDropPayload(const char *value){
    this->drop_payload_identifier.push_back(value);
    return *this;
}

void VisualList::clear(){
    uid_incrementer = 1;
    items.clear();
}
bool VisualList::removeUID(int32_t uid){
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
std::shared_ptr<ListElement> VisualList::findUID(int32_t uid){
    for (auto &chld : items)
    {
        if (chld->uid == uid)
            return chld;
    }
    return nullptr;
}

std::shared_ptr<ListElement> VisualList::addItem(const char *name, const IconType &icon){
    items.push_back(ListElement::CreateShared(uid_incrementer++, this, name, icon));
    return items.back();
}

void VisualList::render(const char *str_imgui_id_selection, ListHolder *listHolder){

        ImVec2 area = ImGui::GetContentRegionAvail();

        ImVec2 icon_size = ImVec2(100, 80);
        ImVec2 spacing = ImVec2(4, 4);

        auto imGuiManager = ImGuiManager::Instance();

        icon_size *= imGuiManager->GlobalScale;
        spacing *= imGuiManager->GlobalScale;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

        float count_size = icon_size.x;
        for (size_t i = 0; i < items.size(); i++)
        {
            auto &item = items[i];

            if (CustomImGuiCommand_DrawItem(i, 
                prefix_id,
                item->name, 
                &item->selected, 
                item->icon, 
                icon_size))
            {
                for (size_t j = 0; j < items.size(); j++)
                {
                    if (i != j && items[j]->selected)
                        items[j]->selected = false;
                }
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

}
