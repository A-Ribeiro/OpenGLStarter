#pragma once

#include "../ImGui/ImGuiManager.h"
#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>

// class DirectoryExt: public ITKCommon::FileSystem::Directory {
//     public:
// };

class Editor
{
public:
    ImGuiManager *imGuiManager;
    ImGuiMenu *imGuiMenu;

    ITKCommon::FileSystem::Directory project_directory;
    bool project_directory_set;

    Editor();

    void init();

    void openFolder(const std::string &path);

    static Editor *Instance();
};
