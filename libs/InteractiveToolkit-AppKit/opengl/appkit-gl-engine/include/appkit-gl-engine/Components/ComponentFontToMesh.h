#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/font/GLFont2Builder.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

// #include <appkit-gl-engine/SharedPointer/SharedPointer.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>
#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMaterial.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentFontToMesh : public Component
            {

            public:
                static const ComponentType Type;

                std::shared_ptr<ComponentMaterial> material;
                std::shared_ptr<ComponentMesh> mesh;

                void toMesh(AppKit::OpenGL::GLFont2Builder &builder, bool dynamic);

                ComponentFontToMesh();
                ~ComponentFontToMesh();

                void createAuxiliaryComponents();

                void attachToTransform(std::shared_ptr<Transform> t);
                void detachFromTransform(std::shared_ptr<Transform> t);
                
                void start();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value, std::unordered_map<intptr_t, std::shared_ptr<Transform>> &transform_map, std::unordered_map<intptr_t, std::shared_ptr<Component>> &component_map);

            };

        }
    }

}