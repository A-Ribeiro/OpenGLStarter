#include "Editor.h"
#include "../OpenFolderDialog.h"

#include <InteractiveToolkit/EventCore/ExecuteOnScopeEnd.h>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

Editor::Editor() : OperationsCommon(),
                   FolderFileOperations(),
                   HierarchyOperations()
{
}

void Editor::init()
{
    OperationsCommon::init();
    FolderFileOperations::init();
    HierarchyOperations::init();
}

void Editor::finalize() {
    HierarchyOperations::finalize();
    FolderFileOperations::finalize();
    OperationsCommon::finalize();
}

void Editor::openFolder(const std::string &path)
{
    openFolder_FolderFileOperations(path);
    clear_HierarchyOperations();
}

void Editor::tryToOpenFile(const ITKCommon::FileSystem::File &file)
{
    if (!ITKCommon::StringUtil::endsWith(file.name, ".scene.json"))
    {
        ImGuiManager::Instance()->dialogs.showInfo_OK(
            ITKCommon::PrintfToStdString("'%s' file type not supported", file.getExtension().c_str()),
            nullptr,
            DialogPosition::OpenOnScreenCenter);
        return;
    }
    openFile_HierarchyOperations(file);
}

Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}

#if defined(_WIN32)
#pragma warning(pop)
#endif
