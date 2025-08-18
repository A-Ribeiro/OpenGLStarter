#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <memory>
#include <vector>
#include <unordered_map>

// Forward declarations
namespace AppKit {
    namespace OpenGL {
        class GLTexture;
    }
    namespace GLEngine {
        class Transform;
        class DefaultEngineShader;
        struct ResourceSet;
    }
}


namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentMaterial : public Component
            {
            public:
                static const ComponentType Type;

                // used for mesh skinning
                bool skin_gradient_matrix_dirty;
                std::vector<MathCore::mat4f> *skin_gradient_matrix;
                uint32_t skin_shader_matrix_size_bitflag; // skin hint to help to select shader
                
                // always custom shader
                std::shared_ptr<DefaultEngineShader> shader;
                Utils::ShaderPropertyBag property_bag;

                // set shader and initialize property bag
                void setShader(std::shared_ptr<DefaultEngineShader> shader);

                bool always_clone;

                ComponentMaterial();
                ~ComponentMaterial();

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone) override;
                
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap) override;

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) override;
                
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet) override;
            };
        }
    }

}