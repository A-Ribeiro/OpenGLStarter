#pragma once

#include <InteractiveToolkit/common.h>
// #include <InteractiveToolkit/ITKCommon/StringUtil.h>
// #include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

#include "../Date.h"

namespace ITKCommon
{
    namespace FileSystem
    {

        class File
        {
            public:

            std::string base_path;
            std::string full_path;
            std::string name;
            bool isDirectory;
            bool isFile;

            Date creationTime;
            Date lastWriteTime;

            uint64_t size;

            File()
            {
                isDirectory = false;
                isFile = false;
                size = UINT64_C(0);
            }
        };

    }
}
