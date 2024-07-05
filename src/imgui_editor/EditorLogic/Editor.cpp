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
                project_directory = Directory(ITKCommon::Path::getAbsolutePath(out));
                printf("project_directory: %s\n", project_directory.getBasePath().c_str());
                project_directory_set = true;
                // time.update();
            }       
        };
        imGuiMenu->UpdateUI();
    }

    // empty hierarchy list, directory list and file list
    {
        imGuiManager->hierarchy.getTreeRoot()->clear();
        imGuiManager->project.getTreeRoot()->clear();
        imGuiManager->project.getVisualList().clear();
    }

}

Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}
