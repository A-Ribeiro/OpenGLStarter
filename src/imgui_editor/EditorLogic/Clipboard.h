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

class CopyFile : public ClipboardOperation {
    public:

    static ClipboardOperationType Type;
    std::shared_ptr<FileListData> fileRef;
    CopyFile(std::shared_ptr<FileListData> fileRef): ClipboardOperation(CopyFile::Type) {
        this->fileRef = fileRef;
    }

    static std::shared_ptr<CopyFile> CreateShared(std::shared_ptr<FileListData> fileRef) {
        return std::make_shared<CopyFile>(fileRef);
    }
};

class CutFile : public ClipboardOperation {
    public:

    static ClipboardOperationType Type;

    std::shared_ptr<FileListData> fileRef;
    CutFile(std::shared_ptr<FileListData> fileRef): ClipboardOperation(CutFile::Type) {
        this->fileRef = fileRef;
    }

    static std::shared_ptr<CutFile> CreateShared(std::shared_ptr<FileListData> fileRef) {
        return std::make_shared<CutFile>(fileRef);
    }
};
