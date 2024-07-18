#pragma once

#include "OperationsCommon.h"
#include "FolderFileOperations.h"

class Editor : virtual public OperationsCommon,
               public FolderFileOperations
{
public:
    Editor();

    void init();

    static Editor *Instance();
};
