#include "HierarchyOperations.h"
#include "../App.h"

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
                    "Action/New", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "New", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility // MenuBehaviour contextMenuBehaviour,
                ),

                ShortCut(
                    "Action/<<>>##0", MenuBehaviour::SetItemVisibility, // "mainMenuPath"
                    "<<>>##0", MenuBehaviour::SetItemVisibility, // "contextMenuPath"
                    "" //shortcutStr
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
                    "Action/<<>>##1", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "<<>>##1", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),

                ShortCut(
                    "Action/New/Node", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "New/Node", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                    "Ctrl+N",//shortcutStr
                    
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::N, //AppKit::Window::Devices::KeyCode keyCode,
                    //EventCore::CallbackWrapper(&FolderFileOperations::createNewSceneOnCurrentDirectory, this)
                    [&](){
                        //activate
                        //printf("New Scene\n");
                        hierarchyCreateNewChildOnNode( this->selectedTreeNode ,"NewNode");
                    }
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
                        hierarchyRename(this->selectedTreeNode, this->selectedTreeNode->getName());

                        
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
                    "Action/<<>>##2", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "<<>>##2", // "contextMenuPath"
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
        auto root = imGuiManager->hierarchy.getTreeRoot();


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

    // main save scene
    {
        auto imGuiMenu = ImGuiMenu::Instance();
        auto &controller = imGuiMenu->getController("File/Save Scene");
        controller.callback = [&]() { 
            printf("new save scene\n");
        };
        controller.enabled = true;
        imGuiMenu->UpdateUI();

        auto app = (App*)AppKit::GLEngine::Engine::Instance()->app;
        app->CtrlS_Detector.OnDown.clear();
        app->CtrlS_Detector.OnDown.add([&](){
            printf("new Ctrl+S save scene action\n");
        });
    }
}

void HierarchyOperations::hierarchyCreateNewChildOnNode(std::shared_ptr<TreeNode> src, const std::string &new_name) 
{
    if (src == nullptr)
        return;

    imGuiManager->dialogs.showEnterText_OKCancel(
        new_name,
        [&, src](const std::string &new_str){
            //auto treeData = std::dynamic_pointer_cast< HierarchyTreeData >(src->data);
            if (new_str.length() == 0){
                this->showErrorAndRetry("Empty node name",[&,src,new_str](){
                    hierarchyCreateNewChildOnNode(src, new_str);
                });
                return;
            }
            
            using namespace AppKit::GLEngine;

            auto tree_node = imGuiManager->hierarchy.createTreeNode( 
                new_str, 
                HierarchyTreeData::CreateShared( Transform::CreateShared() ) 
            );
            src->addChild(tree_node);
            
        },
        DialogPosition::OpenOnScreenCenter
    );
}

void HierarchyOperations::hierarchyDoFocus(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;
}
void HierarchyOperations::hierarchyMakeFirst(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;
    src->makeFirst();
}
void HierarchyOperations::hierarchyMakeLast(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;
    src->makeLast();
}
void HierarchyOperations::hierarchyRename(std::shared_ptr<TreeNode> src, const std::string &new_name)
{
    if (src == nullptr)
        return;

    if (src->isRoot) {
        this->showErrorAndRetry("Trying to rename root",nullptr);
        return;
    }
    
    imGuiManager->dialogs.showEnterText_OKCancel(
        new_name,
        [&, src](const std::string &new_str){
            //auto treeData = std::dynamic_pointer_cast< HierarchyTreeData >(src->data);
            if (new_str.compare( src->getName()) == 0)
                return;
            src->setName(new_str.c_str());
        },
        DialogPosition::OpenOnScreenCenter
    );
}
void HierarchyOperations::hierarchyRemove(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;

    if (src->isRoot) {
        this->showErrorAndRetry("Trying to remove root",nullptr);
        return;
    }

    src->removeSelf();

    imGuiManager->hierarchy.clearTreeSelection(HierarchyClearMethod::ClearNoCallback);
    
}
void HierarchyOperations::hierarchyDuplicate(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;

    if (src->isRoot) {
        this->showErrorAndRetry("Trying to duplicate root",nullptr);
        return;
    }

    auto parent = src->getParent();

    std::string new_name;
    int count = 0;
    do {
        new_name = ITKCommon::PrintfToStdString("%s %.3i", src->getName(), count);
        count++;
    } while(parent->findChildByName(new_name.c_str(),false));

    using namespace AppKit::GLEngine;

    // create the new node
    auto tree_node = imGuiManager->hierarchy.createTreeNode( 
        new_name, 
        HierarchyTreeData::CreateShared( Transform::CreateShared() ) 
    );
    src->addChild(tree_node, src->uid);


}
void HierarchyOperations::hierarchyPasteFromCopy(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    clipboardState = nullptr;
    if (src == nullptr || target == nullptr)
        return;
    
    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid copy", nullptr);
        return;
    }

    using namespace AppKit::GLEngine;

    auto tree_node = imGuiManager->hierarchy.createTreeNode( 
        src->getName(), 
        HierarchyTreeData::CreateShared( Transform::CreateShared() ) 
    );
    target->addChild(tree_node, src->uid);
}
void HierarchyOperations::hierarchyPasteFromCut(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    clipboardState = nullptr;
    if (src == nullptr || target == nullptr)
        return;
    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid cut", nullptr);
        return;
    }

    TreeNode::Reparent(src, target);
}

void HierarchyOperations::hierarchyDragMove(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    if (src == nullptr || target == nullptr)
        return;

    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid move", nullptr);
        return;
    }

    TreeNode::Reparent(src, target);
}

