#pragma once

#include "common.h"
#include "../ImGuiExt/CenteredText.h"

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
    EventCore::Event<void(std::shared_ptr<ListElement>)> OnListSelect;

    EventCore::Event<void(const char *drag_payload, void *src, std::shared_ptr<ListElement> target)> OnListDragDrop;

    friend class VisualList;
    friend class ListElement;
};


static inline IconType RandomListIcon()
{
    IconType _icons[] = {
        IconType::Big_File_Generic,
        IconType::Big_Folder_Filled,
        IconType::Big_Folder_Empty,
        IconType::Big_File_Generic};

    int index = ITKCommon::Random::Instance()->getRange(0, 3);
    return _icons[index];
}

typedef const char *ListDataType;

class ListData
{
protected:
    ListDataType type;
    ListData(ListDataType type){ this->type = type; }
public:
    ListDataType getType() const { return type; }
    bool compareType(ListDataType t) const {return this->type == t;}

    virtual IconType getIcon()=0;
    virtual ~ListData(){}
};

class ListElement
{

    ListElement();

protected:

    std::weak_ptr<ListElement> mSelf;

    char name[64];
    char name_tolower_no_accent[64];

    CenteredText centeredText;

    // used for aux variable to imgui::selectable
    bool selected;

    bool scroll_to_this_item;

public:

    //deleted copy constructor and assign operator, to avoid copy...
    ListElement(const ListElement &) = delete;
    ListElement& operator=(const ListElement &) = delete;

    ListElement(int32_t uid, VisualList *visualList, const char *name, std::shared_ptr<ListData> data);

    ListElement &setName(const char *value);
    inline const char *getName() const { return name; }
    inline const char *getNameToLowerNoAccents() const { return name_tolower_no_accent; }

    std::shared_ptr<ListData> data;

    // ListElement &setIcon(const IconType &icon);

    int32_t uid;
    VisualList *visualList;

    // IconType icon;

    EventCore::PressReleaseDetector hovered;

    std::shared_ptr<ListElement> self();
    std::shared_ptr<ListElement> removeSelf();

    void makeFirst();
    void makeLast();
    void scrollToThisItem();

    void render(const char *str_imgui_id_selection, ListHolder *listHolder);

    static inline std::shared_ptr<ListElement> CreateShared(int32_t uid, VisualList *visualList, const char *name, std::shared_ptr<ListData> data)
    {
        auto result = std::make_shared<ListElement>(uid, visualList, name, data);
        result->mSelf = std::weak_ptr<ListElement>(result);
        return result;
    }

    friend class VisualList;
};

class VisualList
{
protected:

    ImVec2 element_rect_size;
    ImVec2 element_spacing;

    char prefix_id[64];
    char drag_payload_identifier[32];

    std::vector<const char *> drop_payload_identifier;

    int32_t uid_incrementer;

    bool CustomImGuiCommand_DrawItem(
        std::shared_ptr<ListElement> &itemSelf,
        int id, 
        //const char *prefix_name, 
        //const char *name, 
        //bool *selected, 
        //IconType icon, 

        ImVec2 size, 

        ListHolder *listHolder,
        bool *any_click_occured,
        int32_t selected_UID,
        ImGuiID id_sel);

public:
    std::vector<std::shared_ptr<ListElement>> items;

    VisualList();
    
    void Init();

    VisualList &setPrefixID(const char *value);
    VisualList &setDragPayloadID(const char *value);
    VisualList &setDropPayload(const std::vector<const char *> &value);
    VisualList &addDropPayload(const char *value);

    void clear();
    bool removeUID(int32_t uid);
    std::shared_ptr<ListElement> findUID(int32_t uid);

    std::shared_ptr<ListElement> addItem(const char *name, std::shared_ptr<ListData> data);

    void render(const char *str_imgui_id_selection, ListHolder *listHolder);

    // check if a < b
    void sort(const std::function<bool(const std::shared_ptr<ListElement> &, const std::shared_ptr<ListElement> &)> &comparer = Comparer_ASC)
    {
        std::sort(items.begin(), items.end(),comparer);
    }
    
    // check if a < b
    static bool Comparer_ASC(const std::shared_ptr<ListElement> &a, const std::shared_ptr<ListElement> &b)
    {
        return strcmp(a->name_tolower_no_accent, b->name_tolower_no_accent) < 0;
    }

    // check if a < b
    static bool Comparer_DESC(const std::shared_ptr<ListElement> &a, const std::shared_ptr<ListElement> &b)
    {
        return strcmp(b->name_tolower_no_accent, a->name_tolower_no_accent) < 0;
    }

    friend class ListElement;
};