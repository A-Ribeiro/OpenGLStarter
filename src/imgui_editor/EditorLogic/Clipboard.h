#pragma once

#include "../ImGui/ImGuiManager.h"

typedef const char* ClipboardOperationType;

class ClipboardOperation {
public:
    inline bool compareType(ClipboardOperationType type) const{
        return this->type == type;
    }
protected:
    ClipboardOperationType type;
    ClipboardOperation(ClipboardOperationType type) {
        this->type = type;
    }
    virtual ~ClipboardOperation(){

    }
};

class CopyFileOP : public ClipboardOperation {
    public:

    static ClipboardOperationType Type;
    std::shared_ptr<FileListData> fileRef;
    CopyFileOP(std::shared_ptr<FileListData> fileRef): ClipboardOperation(CopyFileOP::Type) {
        this->fileRef = fileRef;
    }

    static std::shared_ptr<CopyFileOP> CreateShared(std::shared_ptr<FileListData> fileRef) {
        return std::make_shared<CopyFileOP>(fileRef);
    }
};

class CutFileOP : public ClipboardOperation {
    public:

    static ClipboardOperationType Type;

    std::shared_ptr<FileListData> fileRef;

    std::shared_ptr <TreeNode> treeNodeSource;

    CutFileOP(std::shared_ptr <TreeNode> treeNodeSource,
              std::shared_ptr<FileListData> fileRef): ClipboardOperation(CutFileOP::Type) {
        this->treeNodeSource = treeNodeSource;
        this->fileRef = fileRef;
    }

    static std::shared_ptr<CutFileOP> CreateShared(std::shared_ptr <TreeNode> treeNodeSource, std::shared_ptr<FileListData> fileRef) {
        return std::make_shared<CutFileOP>(treeNodeSource, fileRef);
    }
};
