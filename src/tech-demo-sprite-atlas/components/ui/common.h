#pragma once

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            enum MeshUploadMode
            {
                MeshUploadMode_Direct_OnClone_NoModify,
                MeshUploadMode_Direct,
                MeshUploadMode_Dynamic,
                MeshUploadMode_Static,
            };
        }
    }
}