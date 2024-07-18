#pragma once

#include "OperationsCommon.h"

class HierarchyOperations : virtual public OperationsCommon
{
    public:

    // loaded project directory
    ITKCommon::FileSystem::File opened_file;

    // directory tree
    std::shared_ptr<HierarchyTreeData> selectedTransformInfo;
    std::shared_ptr<TreeNode> selectedTreeNode;


    HierarchyOperations();

    void init();

    void openFile_HierarchyOperations(const ITKCommon::FileSystem::File &file);

};

