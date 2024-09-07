#pragma once

#include "OperationsCommon.h"

//#include "../ImGui/ImGuiManager.h"
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>
//#include "Clipboard.h"


class FolderFileOperations : virtual public OperationsCommon, public EventCore::HandleCallback
{
    public:

    // loaded project directory
    ITKCommon::FileSystem::Directory project_directory;

    // directory tree
    std::shared_ptr<FileTreeData> selectedDirectoryInfo;
    std::shared_ptr<TreeNode> selectedTreeNode;

    // file list
    std::shared_ptr<FileListData> selectedFileInfo;

    FolderFileOperations();

    void init();
    void finalize();

    std::string removeOSForbiddenPathPattern(const std::string &in);
    bool isUsingOSReservedWords(const std::string &in);

    std::string removeSpacesBetweenLastDotParts(const std::string &in);

    void openFolder_FolderFileOperations(const std::string &path);

    void createNewSceneOnCurrentDirectory(const std::string &fileName);

    void createNewDirectoryOnCurrentDirectory(const std::string &fileName);

    void refreshCurrentFilesAndSelectPath(const std::string &path_to_select);

    void refreshDirectoryStructure(std::shared_ptr<TreeNode> treeNode, bool ignore_tree_select = false);

    void renameSelectedFile(const std::string &newfileName);
    void renameSelectedDirectory(const std::string &newdirname);


    void copyFile(std::shared_ptr<FileListData> inFile, const std::string &outFileName, EventCore::Callback<void()> OnSuccess = nullptr);
    void moveFile(std::shared_ptr<FileListData> inFile, const std::string &outFileName, EventCore::Callback<void()> OnSuccess = nullptr);

    void duplicateFile(std::shared_ptr<FileListData> inFile, EventCore::Callback<void()> OnSuccess = nullptr);

    void deleteSelectedFile();

    void deleteSelectedDirectory();

};

