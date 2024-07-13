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

    std::string lastError;
    std::string _tmp_str;

    Editor();

    void init();

    void openFolder(const std::string &path);

    void createNewSceneOnCurrentDirectory(const std::string &fileName);

    void showErrorAndRetry(const std::string &error, EventCore::Callback<void()> callback);

    void refreshCurrentFilesAndSelectPath(const std::string &path_to_select);

    void refreshDirectoryStructure(std::shared_ptr<TreeNode> treeNode);

    void renameSelectedFile(const std::string &newfileName);

    static Editor *Instance();
};
