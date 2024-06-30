#include "VisualList.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const char *DRAG_PAYLOAD_ID_PROJECT_LIST = "PROJECT_LIST";

ListElement::ListElement() {
    uid = 0;
    visualList = NULL;
    this->setName("-not-set-");
    this->setIcon(RandomListIcon());
}

ListElement::ListElement(int32_t uid, VisualList *visualList, const char *name, const IconType &icon){
    this->uid = uid;
    this->visualList = visualList;
    this->setName(name);
    this->setIcon(icon);
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

}
