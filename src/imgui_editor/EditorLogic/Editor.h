#pragma once

#include "OperationsCommon.h"
#include "FolderFileOperations.h"

class Editor : virtual public OperationsCommon,
               public FolderFileOperations
{
public:
    Editor();

    void init();

    void openFolder(const std::string &path);

    void tryToOpenFile(const ITKCommon::FileSystem::File &full_path);


    static Editor *Instance();
};
