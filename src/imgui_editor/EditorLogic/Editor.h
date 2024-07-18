#pragma once

#include "OperationsCommon.h"
#include "FolderFileOperations.h"

class Editor : public FolderFileOperations
{
public:

    Editor();

    void init();

    static Editor *Instance();
};
