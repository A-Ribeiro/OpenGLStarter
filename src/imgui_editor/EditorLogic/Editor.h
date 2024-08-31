#pragma once

#include "OperationsCommon.h"
#include "FolderFileOperations.h"
#include "HierarchyOperations.h"

class Editor : virtual public OperationsCommon,
               public FolderFileOperations,
               public HierarchyOperations
{
public:
    Editor();

    void init();
    void finalize();

    void openFolder(const std::string &path);

    void tryToOpenFile(const ITKCommon::FileSystem::File &full_path);


    static Editor *Instance();
};
