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
    void finalize();

    void clear_HierarchyOperations();

    void openFile_HierarchyOperations(const ITKCommon::FileSystem::File &file);

    void hierarchyCreateNewChildOnNode(std::shared_ptr<TreeNode> src, const std::string &new_name);

    void hierarchyDoFocus(std::shared_ptr<TreeNode> src);
    void hierarchyMakeFirst(std::shared_ptr<TreeNode> src);
    void hierarchyMakeLast(std::shared_ptr<TreeNode> src);
    void hierarchyRename(std::shared_ptr<TreeNode> src, const std::string &new_name);
    void hierarchyRemove(std::shared_ptr<TreeNode> src);
    void hierarchyDuplicate(std::shared_ptr<TreeNode> src);
    void hierarchyPasteFromCopy(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target);
    void hierarchyPasteFromCut(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target);

    void hierarchyDragMove(std::shared_ptr<TreeNode> src, std::shared_ptr<TreeNode> target);

    void printHierarchy();


    // components
    void componentsClear(std::shared_ptr<TreeNode> target);
    void componentsAddCubeAt(std::shared_ptr<TreeNode> target);


};

