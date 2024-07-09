#include "Editor.h"
#include "../OpenFolderDialog.h"

Editor::Editor()
{
    //project_directory_set = false;
}

void Editor::init()
{
    imGuiManager = ImGuiManager::Instance();
    imGuiMenu = ImGuiMenu::Instance();

    {
        using namespace ITKCommon::FileSystem;
        project_directory = Directory("./");

        auto &controller = imGuiMenu->getController("File/Open Folder");
        controller.callback = [&]() {
            std::string out;
            if (Native::OpenFolderDialog(project_directory.getBasePath(), &out) ){
                openFolder(ITKCommon::Path::getAbsolutePath(out));

                // project_directory = Directory(ITKCommon::Path::getAbsolutePath(out));
                // printf("project_directory: %s\n", project_directory.getBasePath().c_str());
                // project_directory_set = true;
                // time.update();
            }       
        };
        imGuiMenu->UpdateUI();
    }

    // empty hierarchy list, directory list and file list
    {
        imGuiManager->hierarchy.clearTree();
        imGuiManager->project.clearTree();
        imGuiManager->project.getVisualList().clear();
    }

    // register shortcut behaviour
    {
        using namespace AppKit::Window::Devices;

        imGuiManager->shortcutManager.addShortcut(
            "Action:FolderOps",//category
            {
                // set visibility root
                ShortCut(
                    "Action", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility // mainMenuBehaviour,
                ),

                ShortCut(
                    "Action/New", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility // mainMenuBehaviour,
                ),

                ShortCut(
                    "Action/New/Scene", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "New/Scene", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                    "Ctrl+N",//shortcutStr
                    
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::N, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("New Scene\n");

                    }
                ),
                ShortCut(
                    "Action/Refresh", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Refresh", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "F5",//shortcutStr

                    //ctrl,shift,alt,window,
                    false,false,false,false,
                    KeyCode::F5, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("refresh\n");
                    }
                ),
                ShortCut(
                    "Action/<<>>", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "<<>>", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),
                ShortCut(

                    "Action/Paste", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Paste", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+V",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::V, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("paste folder\n");
                    }
                )
            }
        );

        using namespace AppKit::Window::Devices;

        imGuiManager->shortcutManager.addShortcut(
            "Action:FileOps",//category
            {
                // set visibility root
                ShortCut(
                    "Action", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility // mainMenuBehaviour,
                ),

                ShortCut(
                    "Action/Open", // "mainMenuPath"
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "Open", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,
                    
                    "Ctrl+O",//shortcutStr
                    
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::O, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("open \n");
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
                    [](){
                        //activate
                        printf("rename\n");
                    }
                ),
                ShortCut(
                    "Action/<<>>", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "<<>>", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "",//shortcutStr
                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::Unknown //AppKit::Window::Devices::KeyCode keyCode,
                ),
                ShortCut(
                    "Action/Duplicate", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Duplicate", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+D",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::D, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("file copy\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+C\n");
                    }
                ),
                ShortCut(
                    "Action/Copy", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Copy", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+C",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::C, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("file copy\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+C\n");
                    }
                ),
                ShortCut(
                    "Action/Cut", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Cut", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+X",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::X, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("file cut\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+X\n");
                    }
                ),
                ShortCut(

                    "Action/Paste", // "mainMenuPath"
                    MenuBehaviour::SetItemEnabled, // mainMenuBehaviour,

                    "Paste", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility, // MenuBehaviour contextMenuBehaviour,

                    "Ctrl+V",//shortcutStr

                    //ctrl,shift,alt,window,
                    true,false,false,false,
                    KeyCode::V, //AppKit::Window::Devices::KeyCode keyCode,
                    [](){
                        //activate
                        printf("file paste\n");
                    },
                    [](){
                        //deactivate
                        printf("deactivate Ctrl+V\n");
                    }
                )
            }
        );

    }

}

void Editor::openFolder(const std::string &path) {
    using namespace ITKCommon::FileSystem;

    project_directory = Directory(ITKCommon::Path::getAbsolutePath(path));
    printf("project_directory: %s\n", project_directory.getBasePath().c_str());
    //project_directory_set = true;

    // unselect project tree and project list
    {
        imGuiManager->project.clearTreeSelection( ProjectClearMethod::ClearAndCallback );
        imGuiManager->project.clearListSelection( ProjectClearMethod::ClearAndCallback );
    }

    // clear all lists and trees
    {
        imGuiManager->hierarchy.clearTree();
        imGuiManager->project.clearTree();
        imGuiManager->project.getVisualList().clear();
    }

    // load directories
    {
        auto &project = imGuiManager->project;
        auto &root = project.getTreeRoot();
        root->setName(project_directory.getName().c_str());

        root->data = FileTreeData::CreateShared(
            project_directory.toFile()
        );

        selectedDirectoryInfo = std::dynamic_pointer_cast<FileTreeData>(root->data);

        struct _To_insert_struct {
            Directory dir_info;
            std::shared_ptr<TreeNode> insert_where;
            //std::vector< File > file_dir_list;
        };

        std::vector< _To_insert_struct > _to_insert;

        _to_insert.push_back(_To_insert_struct{
            project_directory,
            root
        });

        int max_directories_to_include = 4096;
        while (_to_insert.size() > 0) {

            auto first = _to_insert[0];
            _to_insert.erase(_to_insert.begin());

            std::shared_ptr<FileTreeData> firstData = std::dynamic_pointer_cast<FileTreeData>(first.insert_where->data);
            firstData->has_files = false;

            for(auto &entry: first.dir_info){
                firstData->has_files = true;
                if (!entry.isDirectory)
                    continue;
                    // add element to visual tree
                    auto tree_node = project.createTreeNode( entry.name, FileTreeData::CreateShared( entry ) );
                    first.insert_where->addChild(tree_node);
                    // enqueue element to get subdirectories
                    _to_insert.push_back(_To_insert_struct{ 
                        Directory(entry.full_path), 
                        tree_node 
                    });
                    max_directories_to_include--;
                    if (max_directories_to_include <= 0){
                        _to_insert.clear();
                        break;
                    }
            }

            first.insert_where->sort();
        }
    }

    // on click on directory
    {
        imGuiManager->project.OnTreeSelect.clear();
        imGuiManager->project.OnTreeSelect.add([&](std::shared_ptr<TreeNode> node){

            auto &project = imGuiManager->project;
            auto &visualList = project.getVisualList();

            project.clearListSelection(ProjectClearMethod::ClearNoCallback);
            visualList.clear();

            if (node == nullptr){
                printf("Tree event\n");
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || 
                    ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    imGuiManager->shortcutManager.setActionShortCutByCategory("Action:None");
                }
                return;
            }

            std::shared_ptr<FileTreeData> directoryInfo = std::dynamic_pointer_cast<FileTreeData>(node->data);
            selectedDirectoryInfo = directoryInfo;
            imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");

            if (!directoryInfo->has_files)
                return;

            Directory dir = Directory::FromFile(directoryInfo->file);
            int max_files_to_include = 4096;
            for( auto &entry : dir ) {
                if (!entry.isFile)
                    continue;
                visualList.addItem( entry.name.c_str(), FileListData::CreateShared(entry) );
                max_files_to_include--;
                if (max_files_to_include <= 0)
                    break;
            }

            visualList.sort();
        });
        imGuiManager->project.OnTreeSingleClick.add([&](std::shared_ptr<TreeNode> node){
            
            std::shared_ptr<FileTreeData> directoryInfo = std::dynamic_pointer_cast<FileTreeData>(node->data);
            selectedDirectoryInfo = directoryInfo;
            imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                ImGuiManager::Instance()->contextMenu.open();
        });
    }

    // on click on file
    {
        imGuiManager->project.OnListDoubleClick.clear();
        imGuiManager->project.OnListDoubleClick.add([&](std::shared_ptr<ListElement> element){
            if (element == nullptr)
                return;
            std::shared_ptr<FileListData> fileInfo = std::dynamic_pointer_cast<FileListData>(element->data);
            printf("Clicked in the file: %s\n", fileInfo->file.name.c_str());
            printf("%s\n", fileInfo->file.full_path.c_str());

        });

        imGuiManager->project.OnListSingleClick.clear();
        imGuiManager->project.OnListSingleClick.add([&](std::shared_ptr<ListElement> element){
            std::shared_ptr<FileListData> fileInfo = std::dynamic_pointer_cast<FileListData>(element->data);
            selectedFileInfo = fileInfo;
            imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FileOps");
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
                ImGuiManager::Instance()->contextMenu.open();
            }
        });

        imGuiManager->project.OnListSelect.clear();
        imGuiManager->project.OnListSelect.add([&](std::shared_ptr<ListElement> element){
            if (element == nullptr){
                printf("List event\n");
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    if (selectedDirectoryInfo != nullptr){
                        imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");
                        ImGuiManager::Instance()->contextMenu.open();
                    }
                } 
                // else {
                //     std::shared_ptr<FileListData> fileInfo = std::dynamic_pointer_cast<FileListData>(element->data);
                //     selectedFileInfo = fileInfo;
                //     imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FileOps");
                // }
            }
        });
    }

}

Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}
