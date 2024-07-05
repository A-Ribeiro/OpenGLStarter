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

class Project : public View, public TreeHolder, public ListHolder
{

    // trick to make root have a self() valid method call
    std::shared_ptr<TreeNode> self_root;

    std::shared_ptr<TreeNode> root;
    int32_t uid_incrementer;

    VisualList visualList;

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

    std::shared_ptr<TreeNode> createTreeNode(const std::string &name)
    {
        return TreeNode::CreateShared(uid_incrementer++, TreeNodeIconType::Folder, name.c_str());
    }

    VisualList & getVisualList() {
        return visualList;
    }

    void clearSelection() {
        
        // tree
        {
            ImGuiID id_sel = ImGui::GetID("##project_sel");
            //int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);
            ImGui::GetStateStorage()->SetInt(id_sel, 0);
            this->OnTreeSelect(nullptr);
        }

        // visual list
        {
            ImGuiID id_sel = ImGui::GetID("##proj_files_sel");
            //int selected_UID = ImGui::GetStateStorage()->GetInt(id_sel, 0);
            ImGui::GetStateStorage()->SetInt(id_sel, 0);
            this->OnListSelect(nullptr);
        }
    }

};