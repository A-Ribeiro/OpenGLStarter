#pragma once

#include "../ImGui/ImGuiManager.h"
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

// class DirectoryExt: public ITKCommon::FileSystem::Directory {
//     public:
// };

class Editor : public EventCore::HandleCallback
{
public:
    ImGuiManager *imGuiManager;
    ImGuiMenu *imGuiMenu;

    ITKCommon::FileSystem::Directory project_directory;
    //bool project_directory_set;

    std::shared_ptr<FileTreeData> selectedDirectoryInfo;
    std::shared_ptr<TreeNode> selectedTreeNode;


    std::shared_ptr<FileListData> selectedFileInfo;

    Editor();

    void init();

    void openFolder(const std::string &path);

    void createNewSceneOnCurrentDirectory();

    static Editor *Instance();
};
