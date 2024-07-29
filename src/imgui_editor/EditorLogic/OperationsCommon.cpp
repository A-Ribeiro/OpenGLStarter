#include "OperationsCommon.h"
#include "../App.h"
#include "../InnerViewport.h"
#include "../SceneGUI.h"

OperationsCommon::OperationsCommon() {
    imGuiManager = nullptr;
    imGuiMenu = nullptr;
    sceneGUI = nullptr;
}

void OperationsCommon::init() {
    imGuiManager = ImGuiManager::Instance();
    imGuiMenu = ImGuiMenu::Instance();
    sceneGUI = imGuiManager->innerViewport->sceneGUI;
}

void OperationsCommon::showErrorAndRetry(const std::string &error, EventCore::Callback<void()> retry_callback){
    printf("ERROR: %s\n", error.c_str());
    ImGuiManager::Instance()->dialogs.showInfo_OK(
        std::string("Error: ") + error,
        retry_callback,
        DialogPosition::OpenOnScreenCenter
    );
}
