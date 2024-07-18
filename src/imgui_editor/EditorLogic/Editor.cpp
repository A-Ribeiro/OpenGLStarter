#include "Editor.h"
#include "../OpenFolderDialog.h"

#include <InteractiveToolkit/EventCore/ExecuteOnScopeEnd.h>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

Editor::Editor():FolderFileOperations()
{
}

void Editor::init()
{
    OperationsCommon::init();
    FolderFileOperations::init();
}


Editor *Editor::Instance()
{
    static Editor _editor;
    return &_editor;
}

#if defined(_WIN32)
#pragma warning(pop)
#endif
