#include "Editor.h"
#include "../OpenFolderDialog.h"

Editor::Editor()
{
    project_directory_set = false;
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

}

void Editor::openFolder(const std::string &path) {
    using namespace ITKCommon::FileSystem;

    project_directory = Directory(ITKCommon::Path::getAbsolutePath(path));
    printf("project_directory: %s\n", project_directory.getBasePath().c_str());
    project_directory_set = true;

    // unselect project tree and project list
    {
        imGuiManager->project.clearTreeSelection();
        imGuiManager->project.clearListSelection();
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

            project.clearListSelection();
            visualList.clear();
            if (node == nullptr)
                return;

            std::shared_ptr<FileTreeData> directoryInfo = std::dynamic_pointer_cast<FileTreeData>(node->data);
            if (!directoryInfo->has_files)
                return;

            Directory dir = Directory::FromFile(directoryInfo->file);
            int max_files_to_include = 4096;
            for( auto &entry : dir ) {
                if (!entry.isFile)
                    continue;
                visualList.addItem( entry.name.c_str() , IconType::Big_File_Generic);
                max_files_to_include--;
                if (max_files_to_include <= 0)
                    break;
            }

            visualList.sort();
        });

    }

}

Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}
