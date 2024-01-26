#pragma once

#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>

#include <InteractiveToolkit-Extension/model/ModelContainer.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        class Basof2ToResource
        {

            static Transform *nodeTraverse(const std::string &spaces,
                                           int currentIndex,
                                           const ITKExtension::Model::Node &node,
                                           const ITKExtension::Model::ModelContainer *container,
                                           std::map<int, Components::ComponentMesh *> &geometryCache,
                                           std::map<int, Components::ComponentMaterial *> &materialCache,
                                           Transform *root,
                                           Components::ComponentMaterial *defaultMaterial = NULL,
                                           uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);

        public:
            static Transform *loadAndConvert(
                const std::string &filename,
                Components::ComponentMaterial *defaultMaterial = NULL,
                Transform *rootNode = NULL,
                uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);
        };

    }

}