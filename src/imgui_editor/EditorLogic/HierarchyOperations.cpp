#include "HierarchyOperations.h"

HierarchyOperations::HierarchyOperations()
{
}

void HierarchyOperations::init()
{

    // empty hierarchy list, directory list and file list
    {
        imGuiManager->hierarchy.clearTree();
    }

    // register shortcut behaviour
    {
        using namespace AppKit::Window::Devices;

        imGuiManager->shortcutManager.addShortcut(
            "Action:TransformOps",//category
            {
                // set visibility root
                ShortCut(
                    "Action", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility // mainMenuBehaviour,
                ),

                ShortCut(
                    "Action/Focus", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Focus", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "F",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::F, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("focus\n");
                        hierarchyDoFocus(this->selectedTreeNode);
                        
                        // if (this->selectedFileInfo == nullptr)
                        //     return;
                        
                        // renameSelectedFile( this->selectedFileInfo->file.name );
                    }
                ),

                ShortCut(
                    "Action/Make First", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Make First", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::Unknown, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("Make First\n");
                        hierarchyMakeFirst(this->selectedTreeNode);

                        
                        // if (this->selectedFileInfo == nullptr)
                        //     return;
                        
                        // renameSelectedFile( this->selectedFileInfo->file.name );
                    }
                ),

                ShortCut(
                    "Action/Make Last", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Make Last", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::Unknown, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("Make Last\n");
                        hierarchyMakeLast(this->selectedTreeNode);


                        // if (this->selectedFileInfo == nullptr)
                        //     return;
                        
                        // renameSelectedFile( this->selectedFileInfo->file.name );
                    }
                ),

                ShortCut(
                    "Action/Rename", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Rename", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "F2",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::F2, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("rename\n");
                        hierarchyRename(this->selectedTreeNode);

                        
                        // if (this->selectedFileInfo == nullptr)
                        //     return;
                        
                        // renameSelectedFile( this->selectedFileInfo->file.name );
                    }
                ),
                ShortCut(
                    "Action/Remove", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Remove", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Delete",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::Delete, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("delete\n");
                        hierarchyRemove(this->selectedTreeNode);
                        // if (this->selectedFileInfo == nullptr)
                        //     return;
                        
                        // deleteSelectedFile();
                    }
                ),
                ShortCut(
                    "Action/<<>>", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "<<>>", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),
                ShortCut(
                    "Action/Duplicate", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Duplicate", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+D",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::D, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("file duplicate\n");
                        // if (this->selectedFileInfo != nullptr){
                        //     std::string file_to_select_after_refresh = this->selectedFileInfo->file.full_path;
                        //     duplicateFile(this->selectedFileInfo,
                        //     [&, file_to_select_after_refresh](){
                        //         // on success
                        //         clipboardState = nullptr;
                        //         // refresh
                        //         imGuiManager->PostAction.add([&, file_to_select_after_refresh](){
                        //             refreshCurrentFilesAndSelectPath(file_to_select_after_refresh);
                        //         });
                        //     });
                        // }
                        hierarchyDuplicate(this->selectedTreeNode);
                    }
                ),
                ShortCut(
                    "Action/Copy", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Copy", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+C",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::C, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("copy\n");
                        if (this->selectedTreeNode != nullptr)
                            clipboardState = CopyTransformOP::CreateShared(this->selectedTreeNode);
                        else
                            clipboardState = nullptr;
                    }
                ),
                ShortCut(
                    "Action/Cut", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Cut", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+X",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::X, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("cut\n");
                        if (this->selectedTreeNode != nullptr)
                            clipboardState = CutTransformOP::CreateShared(this->selectedTreeNode);
                        else
                            clipboardState = nullptr;
                    }
                ),
                ShortCut(

                    "Action/Paste", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Paste", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+V",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::V, //AppKit::Window::Devices::KeyCode keyCode,
                    [&](){
                        //activate
                        printf("file paste\n");
                        if(this->selectedTreeNode == nullptr)
                            return;
                        if (clipboardState == nullptr)
                            return;
                        
                        if (clipboardState->compareType(CopyTransformOP::Type)){
                            auto copyTransform = std::dynamic_pointer_cast<CopyTransformOP>(clipboardState);
                            this->hierarchyPasteFromCopy(copyTransform->treeNode, this->selectedTreeNode);

                        } else if (clipboardState->compareType(CutTransformOP::Type)){
                            auto cutTransform = std::dynamic_pointer_cast<CutTransformOP>(clipboardState);
                            this->hierarchyPasteFromCopy(cutTransform->treeNode, this->selectedTreeNode);
                        }
                    }
                )
            }
        );
    }

    {
        imGuiManager->hierarchy.OnTreeSelect.clear();
        imGuiManager->hierarchy.OnTreeSingleClick.clear();
    }
}

void HierarchyOperations::openFile_HierarchyOperations(const ITKCommon::FileSystem::File &file)
{
    using namespace ITKCommon::FileSystem;

    opened_file = file;
    printf("opened_file: %s\n", opened_file.name.c_str());

    if (!opened_file.isFile)
    {
        this->showErrorAndRetry("Trying to open an invalid file", nullptr);
        return;
    }

    // unselect project tree and project list
    {
        imGuiManager->hierarchy.clearTreeSelection(HierarchyClearMethod::ClearNoCallback);

        selectedTransformInfo = nullptr;
        selectedTreeNode = nullptr;
    }

    // clear all lists and trees
    {
        imGuiManager->hierarchy.clearTree();
    }

    // load file hierarchy from scene file
    {
    }

    // on click on transforms
    {
        imGuiManager->hierarchy.OnTreeSelect.clear();
        imGuiManager->hierarchy.OnTreeSelect.add([&](std::shared_ptr<TreeNode> node){

            auto &hierarchy = imGuiManager->hierarchy;

            if (node == nullptr){
                imGuiManager->shortcutManager.setActionShortCutByCategory("Action:None");
                hierarchy.clearTreeSelection(HierarchyClearMethod::ClearNoCallback);
                this->selectedTreeNode = nullptr;
                this->selectedTransformInfo = nullptr;
            }

        });
        imGuiManager->hierarchy.OnTreeSingleClick.clear();
        imGuiManager->hierarchy.OnTreeSingleClick.add([&](std::shared_ptr<TreeNode> node){
            
            this->selectedTreeNode = node;
            this->selectedTransformInfo = std::dynamic_pointer_cast<HierarchyTreeData>(node->data);

            imGuiManager->shortcutManager.setActionShortCutByCategory("Action:TransformOps");

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                ImGuiManager::Instance()->contextMenu.open();
        });
    }
}

void HierarchyOperations::hierarchyDoFocus(std::shared_ptr<TreeNode> src)
{
    
}
void HierarchyOperations::hierarchyMakeFirst(std::shared_ptr<TreeNode> src)
{
    
}
void HierarchyOperations::hierarchyMakeLast(std::shared_ptr<TreeNode> src)
{
    
}
void HierarchyOperations::hierarchyRename(std::shared_ptr<TreeNode> src)
{
    
}
void HierarchyOperations::hierarchyRemove(std::shared_ptr<TreeNode> src)
{
    
}
void HierarchyOperations::hierarchyDuplicate(std::shared_ptr<TreeNode> src)
{

}
void HierarchyOperations::hierarchyPasteFromCopy(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    
}
void HierarchyOperations::hierarchyPasteFromCut(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    
}

void HierarchyOperations::hierarchyDragMove(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{

}

