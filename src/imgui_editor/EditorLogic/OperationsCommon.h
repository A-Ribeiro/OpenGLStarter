#pragma once

#include "../ImGui/ImGuiManager.h"
//#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>
#include "Clipboard.h"


class OperationsCommon: public EventCore::HandleCallback {

public:
    ImGuiManager *imGuiManager;
    ImGuiMenu *imGuiMenu;

    std::string lastError;
    std::string _tmp_str;

    // last clipboard operation
    std::shared_ptr<ClipboardOperation> clipboardState;


    OperationsCommon() {
        imGuiManager = NULL;
        imGuiMenu = NULL;
    }

    void init() {
        imGuiManager = ImGuiManager::Instance();
        imGuiMenu = ImGuiMenu::Instance();
    }

};

