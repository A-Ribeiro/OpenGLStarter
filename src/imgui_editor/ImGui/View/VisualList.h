#pragma once

#include "common.h"
// #include "all.h"

//
// Drag source is of type: VisualList* VisualElement*
//
extern const char *DRAG_PAYLOAD_ID_PROJECT_LIST;

class ListElement;

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

    EventCore::Event<void(const char *drag_payload, void *src, std::shared_ptr<ListElement>target)> OnListDragDrop;

    friend class TreeNode;
};

class VisualList;

// class ListElement
// {
//     // avoid copy, using copy constructors
//     ListElement(const ListElement &) {}
//     void operator=(const ListElement &) {}

//     ListElement();
    

// protected:

//     char name[64];
//     char name_tolower_no_accent[64];

//     VisualList *parent;
// public:

//     ListElement(int32_t uid, const char *name);

//     ListElement &setName(const char *value);
//     inline const char* getName()const;
//     inline const char* getNameToLowerNoAccents()const;

    
//     int32_t uid;
    
//     IconType getIcon()const;

//     EventCore::PressReleaseDetector hovered;

//     std::shared_ptr<ListElement> self();
//     std::shared_ptr<ListElement> removeSelf();

//     void makeFirst();
//     void makeLast();

//     void render(const char *str_imgui_id_selection, ListHolder *listHolder);

//     static inline std::shared_ptr<ListElement> CreateShared(int32_t uid, TreeNodeIconType iconType, const char *name){
//         return std::make_shared<ListElement>(uid, iconType, name);
//     }

//     friend class VisualList;
// };

// class VisualList
// {
//     public:

//     std::vector<std::shared_ptr<ListElement>> items;

//     char prefix_id[64];
//     char drag_payload_identifier[32];

//     std::vector<const char *> drop_payload_identifier;

//     VisualList &setPrefixID(const char *value);
//     VisualList &setDragPayloadID(const char *value);
//     VisualList &setDropPayload(const std::vector<const char *> &value);
//     VisualList &addDropPayload(const char *value);

//     std::shared_ptr<VisualList> self();
//     std::shared_ptr<VisualList> removeSelf();
//     void makeFirst();
//     void makeLast();

//     bool removeUID(int32_t uid);
//     std::shared_ptr<VisualList> findUID(int32_t uid);

//     ListElement &addChild(std::shared_ptr<ListElement> listElement);


//     void render(const char *str_imgui_id_selection, ListHolder *listHolder);

//     void sort(const std::function<int(const std::shared_ptr<ListElement>&,const std::shared_ptr<ListElement>&)> &comparer = Comparer_ASC){
//         std::sort(
//             items.begin(),items.end(),
//             comparer
//         );
//     }

//     static int Comparer_ASC (const std::shared_ptr<ListElement>&a,const std::shared_ptr<ListElement>&b){
//         return strcmp(a->name_tolower_no_accent, b->name_tolower_no_accent);
//     }
//     static int Comparer_DESC (const std::shared_ptr<ListElement>&a,const std::shared_ptr<ListElement>&b){
//         return strcmp(b->name_tolower_no_accent, a->name_tolower_no_accent);
//     }

// };