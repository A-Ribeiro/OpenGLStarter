#include "HierarchyOperations.h"
#include "../App.h"
#include "../InnerViewport.h"
#include "../SceneGUI.h"
#include "../Scene3D.h"
#include <appkit-gl-engine/Serializer/JSONSceneSerializer.h>


HierarchyOperations::HierarchyOperations()
{
}

void HierarchyOperations::finalize() {
    selectedTransformInfo = nullptr;
    selectedTreeNode = nullptr;
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
                    "Action/Components", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Components", // "contextMenuPath"
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
                    "Action/Components/Clear", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Components/Clear", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                    "",//shortcutStr
                    
                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::Unknown, //AppKit::Window::Devices::KeyCode keyCode,
                    //EventCore::CallbackWrapper(&FolderFileOperations::createNewSceneOnCurrentDirectory, this)
                    [&](){
                        //activate
                        //printf("New Scene\n");
                        componentsClear( this->selectedTreeNode );
                    }
                ),

                ShortCut(
                    "Action/Components/AddCube", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Components/AddCube", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                    "",//shortcutStr
                    
                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::Unknown, //AppKit::Window::Devices::KeyCode keyCode,
                    //EventCore::CallbackWrapper(&FolderFileOperations::createNewSceneOnCurrentDirectory, this)
                    [&](){
                        //activate
                        //printf("New Scene\n");
                        componentsAddCubeAt( this->selectedTreeNode);
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
                            this->hierarchyPasteFromCut(cutTransform->treeNode, this->selectedTreeNode);
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

void HierarchyOperations::clear_HierarchyOperations(){
    imGuiManager->hierarchy.clearTree();
    imGuiManager->hierarchy.OnTreeSelect.clear();
    imGuiManager->hierarchy.OnTreeSingleClick.clear();
    imGuiManager->hierarchy.OnTreeDoubleClick.clear();
    imGuiManager->hierarchy.OnTreeDragDrop.clear();

    auto root = imGuiManager->hierarchy.getTreeRoot();

    root->setName( "root" );
    root->data = nullptr;

    clipboardState = nullptr;

    // clear current root transforms and components
    {
        auto _3d_root = scene3D->getRoot();
        auto root_editor = _3d_root->findTransformByName("_editor_root_",1);
        if (root_editor == nullptr){
            root_editor = _3d_root->addChild( Transform::CreateShared() );
            root_editor->setName("_editor_root_");
        }

        root_editor->clearChildren();
        root_editor->clearComponents();
    }


}

void HierarchyOperations::openFile_HierarchyOperations(const ITKCommon::FileSystem::File &file)
{
    using namespace ITKCommon::FileSystem;

    clear_HierarchyOperations();

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

        root->setName( opened_file.name.c_str() );

        auto _3d_root = scene3D->getRoot();
        auto root_editor = _3d_root->findTransformByName("_editor_root_",1);

        root->data = HierarchyTreeData::CreateShared( root_editor );

        imGuiManager->scene.OnImGuiDrawOverlay = nullptr;
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

                imGuiManager->scene.OnImGuiDrawOverlay = nullptr;
            }

        });
        imGuiManager->hierarchy.OnTreeSingleClick.clear();
        imGuiManager->hierarchy.OnTreeSingleClick.add([&](std::shared_ptr<TreeNode> node){
            
            this->selectedTreeNode = node;
            this->selectedTransformInfo = std::dynamic_pointer_cast<HierarchyTreeData>(node->data);
            
            
            // auto methodRef = &HierarchyOperations::drawImGizmoOverlay;

            // auto OperationsCommonRef = (OperationsCommon*)this;
            // typedef void(OperationsCommon::*refT)(const ImVec2 &pos, const ImVec2 &size);

            imGuiManager->scene.OnImGuiDrawOverlay = EventCore::CallbackWrapper(
                &HierarchyOperations::drawImGizmoOverlay, this
            );

            imGuiManager->shortcutManager.setActionShortCutByCategory("Action:TransformOps");

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                ImGuiManager::Instance()->contextMenu.open();
        });
        imGuiManager->hierarchy.OnTreeDoubleClick.clear();
        imGuiManager->hierarchy.OnTreeDoubleClick.add([&](std::shared_ptr<TreeNode> node){
            if (this->selectedTreeNode == nullptr)
                return;
            
            hierarchyRename(this->selectedTreeNode, this->selectedTreeNode->getName());
            // this->selectedTreeNode = node;
            // this->selectedTransformInfo = std::dynamic_pointer_cast<HierarchyTreeData>(node->data);

            // imGuiManager->shortcutManager.setActionShortCutByCategory("Action:TransformOps");

            // if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            //     ImGuiManager::Instance()->contextMenu.open();
        });
    }

    // drag n drop
    {
        imGuiManager->hierarchy.OnTreeDragDrop.clear();
        imGuiManager->hierarchy.OnTreeDragDrop.add([&](const char* drag_payload, void *src, std::shared_ptr<TreeNode>target){
            printf("[Hierarchy][Tree] OnTreeDragDrop. drag_payload: %s\n", drag_payload);
            if (target != nullptr && drag_payload == DRAG_PAYLOAD_ID_HIERARCHY_TREE){
                std::shared_ptr<TreeNode> source_node = ((TreeNode*)src)->self();
                ImGuiManager::Instance()->PostAction.add([&, source_node, target](){
                    hierarchyDragMove(source_node, target);
                    // if (TreeNode::Reparent(source_node, target)){
                    //     printf("                  Reparent OK!\n");
                    // }else {
                    //     printf("                  Reparent Fail!\n");
                    // }
                });
            }
        });
    }

    // main save scene
    {
        auto imGuiMenu = ImGuiMenu::Instance();
        auto &controller = imGuiMenu->getController("File/Save Scene");
        controller.callback = [&]() { 
            printf("new save scene\n");

            if (!opened_file.isFile)
                return;

            auto _3d_root = scene3D->getRoot();
            auto root_editor = _3d_root->findTransformByName("_editor_root_",1);
            
            if (root_editor == nullptr)
                return;

            auto camera = scene3D->getCamera();

            auto writerSet = JSONSceneSerializer::Begin();
            writerSet->writer.StartObject();

            writerSet->writer.String("Camera");
            JSONSceneSerializer::Serialize(writerSet->writer, camera->getTransform(), true);

            writerSet->writer.String("Scene");
            JSONSceneSerializer::Serialize(writerSet->writer, root_editor, false);

            writerSet->writer.EndObject();
            auto bufferObject = JSONSceneSerializer::End(writerSet);
            
            std::string error;
            if (!opened_file.writeContentFromObjectBuffer(&bufferObject, false, &error)){
                this->showErrorAndRetry( error, nullptr );
            }
        };
        controller.enabled = true;
        imGuiMenu->UpdateUI();

        auto app = (App*)AppKit::GLEngine::Engine::Instance()->app;
        app->CtrlS_Detector.OnDown.clear();
        app->CtrlS_Detector.OnDown.add([&](){
            printf("new Ctrl+S save scene action\n");
            auto imGuiMenu = ImGuiMenu::Instance();
            auto &controller = imGuiMenu->getController("File/Save Scene");
            controller.callback();
        });
    }

    // load scene from JSON
    {
        Platform::ObjectBuffer bufferObject;
        std::string error;
        if (!opened_file.readContentToObjectBuffer(&bufferObject, &error)){
            this->showErrorAndRetry( error, nullptr );
            return;
        }

        if (bufferObject.size > 0){
            auto _3d_root = scene3D->getRoot();
            auto root_editor = _3d_root->findTransformByName("_editor_root_",1);
            
            if (root_editor == nullptr)
                return;

            auto camera = scene3D->getCamera();

            auto readSet = JSONSceneDeserializer::Begin(&bufferObject);

            if (readSet == nullptr){
                this->showErrorAndRetry( "Input file corrupted.", nullptr );
                return;
            }

            JSONSceneDeserializer::Deserialize(readSet->document["Camera"], true, camera->getTransform());
            JSONSceneDeserializer::Deserialize(readSet->document["Scene"], false, root_editor);

            // synchronize the scene with the hierarchy view
            {
                auto root = imGuiManager->hierarchy.getTreeRoot();

                // check root editor is linked to root
                auto rootTreeData = std::dynamic_pointer_cast< HierarchyTreeData >(root->data);
                if (rootTreeData->transform != root_editor) {
                    this->showErrorAndRetry( "Base hierarchy setup with wrong assignment.", nullptr );
                    return;
                }

                struct itemT
                {
                    std::shared_ptr<Transform> transform;
                    std::shared_ptr<TreeNode> treeNode;
                    std::shared_ptr<TreeNode> parentTreeNode;
                };

                std::vector<itemT> to_traverse;

                to_traverse.push_back({root_editor, root, nullptr});

                while (to_traverse.size() > 0)
                {
                    auto item = to_traverse.back();
                    to_traverse.pop_back();

                    if (item.parentTreeNode != nullptr) {
                        item.treeNode = imGuiManager->hierarchy.createTreeNode( 
                            item.transform->getName(), 
                            HierarchyTreeData::CreateShared( item.transform ) 
                        );
                        item.parentTreeNode->addChild(item.treeNode);
                    }

                    for (auto &child : STL_Tools::Reversal(item.transform->getChildren())){
                        // auto newTreeNode = imGuiManager->hierarchy.createTreeNode( 
                        //     child->getName(), 
                        //     HierarchyTreeData::CreateShared( child ) 
                        // );
                        //item.treeNode->addChild(newTreeNode);
                        to_traverse.push_back({child, nullptr, item.treeNode});
                    }
                }
            }
        }

    }
    printHierarchy();
}

void HierarchyOperations::hierarchyCreateNewChildOnNode(std::shared_ptr<TreeNode> src, const std::string &new_name) 
{
    if (src == nullptr)
        return;

    imGuiManager->dialogs.showEnterText_OKCancel(
        new_name,
        [&, src](const std::string &new_str){
            //auto treeData = std::dynamic_pointer_cast< HierarchyTreeData >(src->data);
            // if (new_str.length() == 0){
            //     this->showErrorAndRetry("Empty node name",[&,src,new_str](){
            //         hierarchyCreateNewChildOnNode(src, new_str);
            //     });
            //     return;
            // }
            
            using namespace AppKit::GLEngine;

            auto parent_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
            auto transform = parent_transform->addChild(Transform::CreateShared());
            transform->setName(new_str);

            auto tree_node = imGuiManager->hierarchy.createTreeNode( 
                new_str, 
                HierarchyTreeData::CreateShared( transform ) 
            );
            src->addChild(tree_node);


            this->printHierarchy();
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

    auto parent_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
    parent_transform->makeFirst();
    
    this->printHierarchy();

}
void HierarchyOperations::hierarchyMakeLast(std::shared_ptr<TreeNode> src)
{
    if (src == nullptr)
        return;
    src->makeLast();

    auto parent_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
    parent_transform->makeLast();
    
    this->printHierarchy();
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

            auto parent_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
            parent_transform->setName(new_str);
            this->printHierarchy();

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

    auto parent_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
    parent_transform->removeSelf();

    this->printHierarchy();

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
    auto parent_treeData = std::dynamic_pointer_cast<HierarchyTreeData>(parent->data);


    std::string new_name;
    int count = 0;
    do {
        new_name = ITKCommon::PrintfToStdString("%s %.3i", src->getName(), count);
        count++;
    } while(parent->findChildByName(new_name.c_str(),false));

    using namespace AppKit::GLEngine;

    // create the new node
    // auto tree_node = imGuiManager->hierarchy.createTreeNode( 
    //     new_name, 
    //     HierarchyTreeData::CreateShared( Transform::CreateShared() ) 
    // );
    auto tree_node = imGuiManager->hierarchy.cloneTreeNode(src);
    auto tree_node_treeData = std::dynamic_pointer_cast<HierarchyTreeData>(tree_node->data);

    tree_node->setName(new_name.c_str());
    tree_node_treeData->transform->setName(new_name);

    auto src_treeData = std::dynamic_pointer_cast<HierarchyTreeData>(src->data);

    parent->addChild(tree_node, src->uid);
    parent_treeData->transform->addChild(tree_node_treeData->transform, src_treeData->transform);

    this->printHierarchy();
}
void HierarchyOperations::hierarchyPasteFromCopy(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    clipboardState = nullptr;
    if (src == nullptr || target == nullptr || src == target)
        return;
    
    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid copy", nullptr);
        return;
    }

    using namespace AppKit::GLEngine;

    // needs to clone the entire hierarchy

    // auto tree_node = imGuiManager->hierarchy.createTreeNode( 
    //     src->getName(), 
    //     HierarchyTreeData::CreateShared( Transform::CreateShared() ) 
    // );
    auto tree_node = imGuiManager->hierarchy.cloneTreeNode(src);
    auto tree_node_treeData = std::dynamic_pointer_cast<HierarchyTreeData>(tree_node->data);

    target->addChild(tree_node);//, src->uid);

    auto target_treeData = std::dynamic_pointer_cast<HierarchyTreeData>(target->data);
    target_treeData->transform->addChild(tree_node_treeData->transform);

    this->printHierarchy();
}
void HierarchyOperations::hierarchyPasteFromCut(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    clipboardState = nullptr;
    if (src == nullptr || target == nullptr || src == target)
        return;
    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid cut", nullptr);
        return;
    }

    TreeNode::Reparent(src, target);

    auto src_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
    auto target_transform = std::dynamic_pointer_cast<HierarchyTreeData>(target->data)->transform;

    target_transform->addChild(src_transform);

    this->printHierarchy();

}

void HierarchyOperations::hierarchyDragMove(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target)
{
    if (src == nullptr || target == nullptr || src == target)
        return;

    if (src->isChild(target->uid)){
        this->showErrorAndRetry("Parent/child invalid move", nullptr);
        return;
    }

    TreeNode::Reparent(src, target);

    auto src_transform = std::dynamic_pointer_cast<HierarchyTreeData>(src->data)->transform;
    auto target_transform = std::dynamic_pointer_cast<HierarchyTreeData>(target->data)->transform;

    target_transform->addChild(src_transform);

    this->printHierarchy();
}

void HierarchyOperations::printHierarchy() {
    printf("\nScene3D:\n\n");
    scene3D->printHierarchy();
    printf("\n");
}

void HierarchyOperations::componentsClear(std::shared_ptr<TreeNode> target) {
    if (target == nullptr)
        return;
    auto transform = std::dynamic_pointer_cast<HierarchyTreeData>(target->data)->transform;
    if (transform == nullptr)
        return;
    
    transform->clearComponents();
}

void HierarchyOperations::componentsAddCubeAt(std::shared_ptr<TreeNode> target) {
    if (target == nullptr)
        return;
    if (target->isRoot)
        return;
    auto transform = std::dynamic_pointer_cast<HierarchyTreeData>(target->data)->transform;
    if (transform == nullptr)
        return;
    
    auto resourceHelper = imGuiManager->innerViewport->scene3D->resourceHelper;

    auto material = transform->addNewComponent<ComponentMaterial>();
    transform->addComponent(ComponentMesh::createBox(MathCore::vec3f(1, 1, 1)));
    
    //material->type = MaterialUnlit;
    //material->unlit.color = vec4(0.5f,0.5f,0.5f,1.0f);

    material->type = MaterialPBR;
    material->pbr.albedoColor = MathCore::vec3f(1, 1, 1);
    material->pbr.metallic = 0.0f;
    material->pbr.roughness = 1.0f;
    material->pbr.texAlbedo = resourceHelper->defaultAlbedoTexture;
    material->pbr.texNormal = nullptr;//refCount->add( resourceHelper->defaultNormalTexture );

    resourceHelper->addAABBMesh(transform, false);

}


void HierarchyOperations::drawImGizmoOverlay(const ImVec2 &pos, const ImVec2 &size) {

    // available variables:
    // this->selectedTreeNode;
    // this->selectedTransformInfo;

    if (this->selectedTreeNode->isRoot)
        return;

    // ImVec2 min = ImGui::GetWindowContentRegionMin() + ImGui::GetWindowPos();
    // ImVec2 max = ImGui::GetWindowContentRegionMax() + ImGui::GetWindowPos();

    // ImVec2 pos = min;        // ImGui::GetWindowPos();
    // ImVec2 size = max - min; // ImGui::GetWindowSize();

    if (size.x <= MathCore::EPSILON<float>::low_precision || size.y  <= MathCore::EPSILON<float>::low_precision)
        return;


    // ImGuiIO &io = ImGui::GetIO();
    // pos *= io.DisplayFramebufferScale;
    // size *= io.DisplayFramebufferScale;

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetOrthographic(false);

    // adjust gizmo size in PX
    {
        // size in px
        float gizmo_size_px = 80.0f * imGuiManager->GlobalScale;
        // 10% of the screen
        float gizmo_size_normalized = 0.1f;
        // compute the Gizmo final size based on the minimum window size
        float min_win_size = (size.x < size.y) ? size.x : size.y;
        if (min_win_size > 0.002f)
            gizmo_size_normalized = gizmo_size_px / min_win_size;

        float aspect = size.x / size.y;

        if (aspect > 1.0f)
            ImGuizmo::SetGizmoSizeClipSpace(gizmo_size_normalized / aspect);
        else
            ImGuizmo::SetGizmoSizeClipSpace(gizmo_size_normalized * aspect);
    }

    ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::LOCAL;

    bool useSnap = true;
    float snap[] = {0.01f,0.01f,0.01f};

    auto camera = imGuiManager->innerViewport->scene3D->getCamera();
    camera->precomputeViewProjection(false);

    auto obj_transform = this->selectedTransformInfo->transform;

    auto obj_matrix = obj_transform->getMatrix();

    ImGuizmo::SetID(0);

    ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
    ImGuizmo::Manipulate(
        camera->view.array,
        camera->projection.array, 
        mCurrentGizmoOperation, mCurrentGizmoMode, 
        obj_matrix.array, NULL, useSnap ? &snap[0] : NULL);

    // monitoring translation
    auto translation_vec3 = MathCore::CVT<MathCore::vec4f>::toVec3(
        MathCore::OP<MathCore::mat4f>::extractTranslation(obj_matrix)
    );

    float dst_to_consider_movement = MathCore::EPSILON<float>::high_precision;

    if (MathCore::OP<MathCore::vec3f>::sqrDistance(obj_transform->getPosition(),translation_vec3) > dst_to_consider_movement){
        obj_transform->setPosition(translation_vec3);
        printf(".");fflush(stdout);
    }

}

