#include "Editor.h"
#include "../OpenFolderDialog.h"

#include <InteractiveToolkit/EventCore/ExecuteOnScopeEnd.h>

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
                    MenuBehaviour::SetItemVisibility, // mainMenuBehaviour,

                    "New", // "contextMenuPath"
                    MenuBehaviour::SetItemVisibility // MenuBehaviour contextMenuBehaviour,
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
                    //EventCore::CallbackWrapper(&Editor::createNewSceneOnCurrentDirectory, this)
                    [&](){
                        //activate
                        //printf("New Scene\n");
                        createNewSceneOnCurrentDirectory("NewFile");
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
                    [&](){
                        //activate
                        printf("refresh\n");
                        refreshDirectoryStructure(selectedTreeNode);
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
        imGuiManager->project.clearTreeSelection( ProjectClearMethod::ClearNoCallback );
        imGuiManager->project.clearListSelection( ProjectClearMethod::ClearNoCallback );

        selectedDirectoryInfo = nullptr;
        selectedTreeNode = nullptr;
        selectedFileInfo = nullptr;
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

            if (node == nullptr){
                
                imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");

                if (selectedDirectoryInfo == nullptr || selectedTreeNode == nullptr){
                    selectedTreeNode = project.getTreeRoot();
                    selectedDirectoryInfo = std::dynamic_pointer_cast<FileTreeData>(project.getTreeRoot()->data);
                    project.forceTreeSelection(project.getTreeRoot()->uid);
                } else {
                    project.forceTreeSelection(selectedTreeNode->uid);
                    project.clearListSelection(ProjectClearMethod::ClearNoCallback);
                    return;
                }

                // if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || 
                //     ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                //     imGuiManager->shortcutManager.setActionShortCutByCategory("Action:None");
                // }
            }
            else {
                std::shared_ptr<FileTreeData> directoryInfo = std::dynamic_pointer_cast<FileTreeData>(node->data);
                selectedTreeNode = node;
                selectedDirectoryInfo = directoryInfo;
                imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");
            }

            project.clearListSelection(ProjectClearMethod::ClearNoCallback);
            visualList.clear();

            if (!selectedDirectoryInfo->has_files)
                return;

            Directory dir = Directory::FromFile(selectedDirectoryInfo->file);
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
                if (selectedDirectoryInfo != nullptr){
                    imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");
                } else {
                    imGuiManager->shortcutManager.setActionShortCutByCategory("Action:None");
                }
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                    if (selectedDirectoryInfo != nullptr){
                        //imGuiManager->shortcutManager.setActionShortCutByCategory("Action:FolderOps");
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

    imGuiManager->PostAction.add([&](){
        imGuiManager->project.OnTreeSelect(
            imGuiManager->project.getTreeRoot()
        );
        imGuiManager->project.forceTreeSelection(
            imGuiManager->project.getTreeRoot()->uid
        );
    });
    

}

#include <sstream>

void Editor::createNewSceneOnCurrentDirectory(const std::string &fileName) {

    if (selectedDirectoryInfo == nullptr)
        return;

    ImGuiManager::Instance()->dialogs.showEnterText_OKCancel(fileName, 
        [&](const std::string &new_str){

            printf("createNewSceneOnCurrentDirectory\n");

            if (selectedDirectoryInfo == nullptr)
                return;
            
            lastError = "";
            EventCore::ExecuteOnScopeEnd _exec_on_end([&](){
                printf("this->lastError.length(): %i\n", (int)this->lastError.length() );
                if (this->lastError.length() == 0)
                    return;
                this->showErrorAndRetry(
                    lastError,
                    [&](){
                        this->createNewSceneOnCurrentDirectory(this->fileNameToCreate);
                    }
                );
            });
            
            // remove all slashes
            std::stringstream s_output;

            //check if file exists
            for(const auto &_c : new_str){
#if defined(_WIN32)
                if (_c == '<' || _c == '>' ||
                _c == ':' || _c == '"' ||
                _c == '/' || _c == '\\' ||
                _c == '|' || _c == '?' ||
                _c == '*' )
                    continue;
#elif defined(__linux__)
                if (_c == '/')
                    continue;
#elif defined(__APPLE__)
                if (_c == '/' || _c == ':' )
                    continue;
#endif
                // ASCII control characters. Only Winows blocks, 
                // but it is better to avoid on all systems
                if (_c <= 31) 
                    continue;

                s_output << _c;
            }
            
            auto aux = ITKCommon::StringUtil::trim(s_output.str());
            // apply trim
            s_output = std::stringstream();
            s_output << aux;
            if ( !ITKCommon::StringUtil::endsWith(aux, ".scene") ){
                if (aux.length() == 0){
                    lastError = "Empty file name supplied";
                    fileNameToCreate = aux;
                    return;
                } else {
                    if (ITKCommon::StringUtil::endsWith(aux, "."))
                        s_output << "scene";
                    else
                        s_output << ".scene";
                }
            }

            std::string file_to_create = s_output.str();
            //file_to_create = ITKCommon::StringUtil::trim(file_to_create);

            if (file_to_create.length() == 0) {
                lastError = "Empty file name supplied";
                fileNameToCreate = aux;
                return;
            }

#if defined(_WIN32)
            // windows reserved words
            const char * win32_reserved_words[] = { 
                "CON", "PRN", "AUX", "NUL", 
                "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
                "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
            };
            const int total_words = 22;
            auto parts = ITKCommon::StringUtil::tokenizer(ITKCommon::StringUtil::toUpper(file_to_create), "." );
            for(const auto &item : parts){                
                if (item.length() != 3 && item.length() != 4)
                    continue;
                for(int i=0;i<total_words;i++){
                    if (item.compare(win32_reserved_words[i]) == 0) {
                        lastError = "Use of windows reserved words";
                        fileNameToCreate = aux;
                        return;
                    }
                }
            }
#endif

            // create file pointed by file_to_create
            printf("Creating file: %s\n", file_to_create.c_str());

            std::string full_path_file = selectedDirectoryInfo->file.full_path + file_to_create;

            //refresh and select
            if (ITKCommon::Path::isFile(full_path_file)){
                lastError = "File already exists";
                fileNameToCreate = aux;
                return;
            }

            auto fout = fopen(full_path_file.c_str(), "wb");
            if (!fout){
                lastError = strerror(errno);
                fileNameToCreate = aux;
                return;
            }
            fclose(fout);
#if defined(__linux__) || defined(__APPLE__)
            chmod(full_path_file.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
#endif

            //ITKCommon::FileSystem::File::FromPath(full_path_file);
            imGuiManager->PostAction.add([&, full_path_file](){
                this->refreshCurrentFilesAndSelectPath(full_path_file);
            });
        },
        DialogPosition::OpenOnScreenCenter
    );
}

void Editor::showErrorAndRetry(const std::string &error, EventCore::Callback<void()> retry_callback) {
    printf("ERROR: %s\n", error.c_str());
    ImGuiManager::Instance()->dialogs.showInfo_OK(
        std::string("Error: ") + error,
        retry_callback,
        DialogPosition::OpenOnScreenCenter
    );
}

void Editor::refreshDirectoryStructure(std::shared_ptr<TreeNode> treeNode) {
    
    if (treeNode == nullptr)
        return;
    
    using namespace ITKCommon::FileSystem;

    auto &project = imGuiManager->project;

    struct _To_insert_struct {
        Directory dir_info;
        std::shared_ptr<TreeNode> insert_where;
        //std::vector< File > file_dir_list;
        //std::map<std::string, bool > keep_children;//not expanded
        std::map<std::string, std::shared_ptr<TreeNode> > childrenMap;
    };

    std::vector< _To_insert_struct > _to_insert;
    {
        auto selectedFile = std::dynamic_pointer_cast<FileTreeData>(treeNode->data);
        _to_insert.push_back(_To_insert_struct{
            Directory::FromFile(selectedFile->file),
            treeNode
        });
    }

    // create children map
    {
        auto &last_inserted_struct = _to_insert.back();
        if (!last_inserted_struct.dir_info.isValid()){
            if (!treeNode->isRoot) {
                auto _parent = treeNode->parent->self();
                // remove in case this folder was removed
                treeNode->removeSelf();
                project.OnTreeSelect(_parent);
                return;
            }
        }
        for(auto children: last_inserted_struct.insert_where->children) {
            //last_inserted_struct.children_collapsed[children->getName()] = !children->expanded.pressed;
            last_inserted_struct.childrenMap[children->getName()] = children;
        }
    }

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
            
            std::shared_ptr<TreeNode> tree_node;

            // check if the node exists
            auto _it = first.childrenMap.find(entry.name);
            if ( _it == first.childrenMap.end()){
                // add element to visual tree
                tree_node = project.createTreeNode( entry.name, FileTreeData::CreateShared( entry ) );
                first.insert_where->addChild(tree_node);
            } else {
                // element already exists
                tree_node = _it->second;
                first.childrenMap.erase(_it);
            }

            // enqueue element to get subdirectories
            _to_insert.push_back(_To_insert_struct{ 
                Directory(entry.full_path), 
                tree_node 
            });

            // create children map
            {
                auto &last_inserted_struct = _to_insert.back();
                for(auto children: last_inserted_struct.insert_where->children) {
                    //last_inserted_struct.children_collapsed[children->getName()] = !children->expanded.pressed;
                    last_inserted_struct.childrenMap[children->getName()] = children;
                }
            }


            max_directories_to_include--;
            if (max_directories_to_include <= 0){
                _to_insert.clear();
                break;
            }
        }

        // remove names that were not removed from the Map
        for(auto element:first.childrenMap){
            element.second->removeSelf();
        }

        first.insert_where->sort();
    }

    project.OnTreeSelect(treeNode);
}

void Editor::refreshCurrentFilesAndSelectPath(const std::string &path_to_select) {

    auto &project = imGuiManager->project;
    auto &visualList = project.getVisualList();

    // check if there is more files on selectedTreeNode
    refreshDirectoryStructure(selectedTreeNode);

    // refresh current list
    // project.OnTreeSelect(selectedTreeNode);
    
    std::shared_ptr<ListElement> to_select;
    for(auto &item:visualList.items){
        std::shared_ptr<FileListData> fileInfo = std::dynamic_pointer_cast<FileListData>(item->data);
        if (fileInfo->file.full_path.compare(path_to_select) == 0){
            to_select = item;
            break;
        }
    }

    if (to_select != nullptr){
        project.OnListSingleClick(to_select);
        project.forceFilesSelection(to_select->uid);
        to_select->scrollToThisItem();
    }

}

Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}
