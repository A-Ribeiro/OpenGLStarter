#include "HierarchyOperations.h"

HierarchyOperations::HierarchyOperations()
{
}

void HierarchyOperations::init()
{

    // empty hierarchy list, directory list and file list
    {
        imGuiManager->hierarchy.clearTree();
    }

    // register shortcut behaviour
    {
        using namespace AppKit::Window::Devices;
    }
}

void HierarchyOperations::openFile_HierarchyOperations(const ITKCommon::FileSystem::File &file)
{
    using namespace ITKCommon::FileSystem;

    opened_file = file;
    printf("opened_file: %s\n", opened_file.name.c_str());

    if (!opened_file.isFile)
    {
        this->showErrorAndRetry("Trying to open an invalid file", nullptr);
        return;
    }

    // unselect project tree and project list
    {
        imGuiManager->hierarchy.clearTreeSelection(HierarchyClearMethod::ClearNoCallback);

        selectedTransformInfo = nullptr;
        selectedTreeNode = nullptr;
    }

    // clear all lists and trees
    {
        imGuiManager->hierarchy.clearTree();
    }

    // load file hierarchy
    {
    }

    // on click on transforms
    {
    }
}
