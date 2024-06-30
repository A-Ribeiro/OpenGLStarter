#pragma once

#include "common.h"
// #include "all.h"

//
// Drag source is of type: VisualList* VisualElement*
//
extern const char *DRAG_PAYLOAD_ID_PROJECT_LIST;

class ListElement;
class VisualList;

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

    EventCore::Event<void(const char *drag_payload, void *src, std::shared_ptr<ListElement> target)> OnListDragDrop;

    friend class VisualList;
    friend class ListElement;
};


IconType RandomListIcon()
{
    IconType _icons[] = {
        IconType::Big_File_Generic,
        IconType::Big_Folder_Filled,
        IconType::Big_Folder_Empty,
        IconType::Big_File_Generic};

    int index = ITKCommon::Random::Instance()->getRange(0, 3);
    return _icons[index];
}

class ListElement
{
    // avoid copy, using copy constructors
    ListElement(const ListElement &) {}
    void operator=(const ListElement &) {}

    ListElement();

protected:
    char name[64];
    char name_tolower_no_accent[64];

public:
    ListElement(int32_t uid, VisualList *visualList, const char *name, const IconType &icon);

    ListElement &setName(const char *value);
    inline const char *getName() const { return name; }
    inline const char *getNameToLowerNoAccents() const { return name_tolower_no_accent; }

    ListElement &setIcon(const IconType &icon);

    int32_t uid;
    VisualList *visualList;

    IconType icon;

    EventCore::PressReleaseDetector hovered;

    std::shared_ptr<ListElement> self();
    std::shared_ptr<ListElement> removeSelf();

    void makeFirst();
    void makeLast();

    void render(const char *str_imgui_id_selection, ListHolder *listHolder);

    static inline std::shared_ptr<ListElement> CreateShared(int32_t uid, VisualList *visualList, const char *name, const IconType &icon)
    {
        return std::make_shared<ListElement>(uid, visualList, name, icon);
    }

    friend class VisualList;
};

class VisualList
{
protected:

    char prefix_id[64];
    char drag_payload_identifier[32];

    std::vector<const char *> drop_payload_identifier;

    int32_t uid_incrementer;

public:
    std::vector<std::shared_ptr<ListElement>> items;

    VisualList();

    VisualList &setPrefixID(const char *value);
    VisualList &setDragPayloadID(const char *value);
    VisualList &setDropPayload(const std::vector<const char *> &value);
    VisualList &addDropPayload(const char *value);

    void clear();
    bool removeUID(int32_t uid);
    std::shared_ptr<ListElement> findUID(int32_t uid);

    std::shared_ptr<ListElement> addItem(const char *name, const IconType &icon);

    void render(const char *str_imgui_id_selection, ListHolder *listHolder);

    void sort(const std::function<int(const std::shared_ptr<ListElement> &, const std::shared_ptr<ListElement> &)> &comparer = Comparer_ASC)
    {
        std::sort(items.begin(), items.end(),comparer);
    }
    
    static int Comparer_ASC(const std::shared_ptr<ListElement> &a, const std::shared_ptr<ListElement> &b)
    {
        return strcmp(a->name_tolower_no_accent, b->name_tolower_no_accent);
    }
    static int Comparer_DESC(const std::shared_ptr<ListElement> &a, const std::shared_ptr<ListElement> &b)
    {
        return strcmp(b->name_tolower_no_accent, a->name_tolower_no_accent);
    }

    friend class ListElement;
};