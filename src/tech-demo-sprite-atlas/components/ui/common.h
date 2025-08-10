#pragma once

#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            enum MeshUploadMode
            {
                MeshUploadMode_Direct,
                MeshUploadMode_Dynamic,
                MeshUploadMode_Static,
                MeshUploadMode_Direct_OnClone_NoModify,
                MeshUploadMode_Dynamic_OnClone_NoModify,
                MeshUploadMode_Static_OnClone_NoModify
            };
        }
    }
}