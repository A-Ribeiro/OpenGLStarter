#pragma once

#include "../ImGui/ImGuiManager.h"
//#include <InteractiveToolkit/ITKCommon/FileSystem/Directory.h>
#include "Clipboard.h"

class SceneGUI;
class Scene3D;

class OperationsCommon { // : public EventCore::HandleCallback {

public:
    ImGuiManager *imGuiManager;
    ImGuiMenu *imGuiMenu;
    SceneGUI *sceneGUI;
    Scene3D *scene3D;

    std::string lastError;
    std::string _tmp_str;

    // last clipboard operation
    std::shared_ptr<ClipboardOperation> clipboardState;


    OperationsCommon();

    void init();
    void finalize();

    // shared methods that can be called from any subclass
    virtual void openFolder(const std::string &path) = 0;

    virtual void tryToOpenFile(const ITKCommon::FileSystem::File &full_path) = 0;
    
    void showErrorAndRetry(const std::string &error, EventCore::Callback<void()> retry_callback);

};

