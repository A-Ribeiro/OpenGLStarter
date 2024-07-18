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

    // shared methods that can be called from any subclass
    virtual void openFolder(const std::string &path) = 0;

    virtual void tryToOpenFile(const ITKCommon::FileSystem::File &full_path) = 0;
    
    void showErrorAndRetry(const std::string &error, EventCore::Callback<void()> retry_callback){
        printf("ERROR: %s\n", error.c_str());
        ImGuiManager::Instance()->dialogs.showInfo_OK(
            std::string("Error: ") + error,
            retry_callback,
            DialogPosition::OpenOnScreenCenter
        );
    }

};

