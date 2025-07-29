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
        class ResourceMap;

        class Basof2ToResource
        {

            static std::shared_ptr<Transform> nodeTraverse(const std::string &spaces,
                                           int currentIndex,
                                           ResourceMap *resourceMap,
                                           const ITKExtension::Model::Node &node,
                                           const ITKExtension::Model::ModelContainer *container,
                                           std::unordered_map<int, std::shared_ptr<Components::ComponentMesh> > &geometryCache,
                                           std::unordered_map<int, std::shared_ptr<Components::ComponentMaterial> > &materialCache,
                                           std::shared_ptr<Transform> root,
                                           std::shared_ptr<Components::ComponentMaterial> defaultMaterial = nullptr,
                                           uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);

        public:
            static std::shared_ptr<Transform> loadAndConvert(
                const std::string &filename,
                ResourceMap *resourceMap,
                std::shared_ptr<Components::ComponentMaterial> defaultMaterial = nullptr,
                std::shared_ptr<Transform> rootNode = nullptr,
                uint32_t model_dynamic_upload = 0, uint32_t model_static_upload = 0xffffffff);
        };

    }

}