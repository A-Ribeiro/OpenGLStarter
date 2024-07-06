#pragma once

#include "common.h"
#include "all.h"


// enum class FileRefType:uint8_t {
//     File=0,
//     Folder,
//     EmptyFolder
// };

// class FileRef {
//     public:
//     FileRefType type;
//     std::string name;
//     bool selected;

//     FileRef(FileRefType type,
//         const char *name) {
//         this->type = type;
//         this->name = name;
//         selected = false;
//     }

//     IconType getIcon()const {
//         if (type == FileRefType::File){
//             return IconType::Big_File_Generic;
//         }
//         else if (type == FileRefType::Folder){
//             return IconType::Big_Folder_Filled;
//         }
//         else if (type == FileRefType::EmptyFolder){
//             return IconType::Big_Folder_Empty;
//         } else {
//             return IconType::Big_File_Generic;
//         }
//     }

// };

class FileTreeData:public TreeData
{
protected:
public:
    static TreeDataType Type;

    ITKCommon::FileSystem::File file;
    //IconType iconToUse;
    bool has_files;

    FileTreeData(const ITKCommon::FileSystem::File &file):TreeData(FileTreeData::Type){
        this->file = file;
        //iconToUse = IconType::Small_Folder_Filled;
        has_files = false;
    }

    virtual IconType getIcon(){
        if (has_files)
            return IconType::Small_Folder_Filled;
        else
            return IconType::Small_Folder_Empty;
    }

    static inline std::shared_ptr<FileTreeData> CreateShared(const ITKCommon::FileSystem::File &file)
    {
        return std::make_shared<FileTreeData>(file);
    }

};


class Project : public View, public TreeHolder, public ListHolder
{

    // trick to make root have a self() valid method call
    std::shared_ptr<TreeNode> self_root;

    std::shared_ptr<TreeNode> root;
    int32_t uid_incrementer;

    VisualList visualList;

    bool clear_tree_selection;
    bool clear_list_selection;

public:
    static const ViewType Type;

    //std::vector<FileRef> files;

    Project();

    View* Init();

    void RenderAndLogic();

    std::shared_ptr<TreeNode> & getTreeRoot() {
        return root;
    }

    void clearTree() {
        root->clear();
        uid_incrementer = 10;
    }

    std::shared_ptr<TreeNode> createTreeNode(const std::string &name, std::shared_ptr<FileTreeData> data)
    {
        return TreeNode::CreateShared(uid_incrementer++, data, name.c_str());
    }

    VisualList & getVisualList() {
        return visualList;
    }

    void clearTreeSelection() {
        
        // tree
        clear_tree_selection = true;
    }

    void clearListSelection() {
        
        // visual list
        clear_list_selection = true;
    }

};