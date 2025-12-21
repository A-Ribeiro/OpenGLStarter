#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>

#include <InteractiveToolkit-Extension/model/Bone.h>

#include <appkit-gl-engine/DefaultEngineShader.h>

#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/GLVertexArrayObject.h>



namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentMesh : public Component
            {

                AppKit::OpenGL::GLVertexBufferObject *vbo_pos;
                AppKit::OpenGL::GLVertexBufferObject *vbo_normals;
                AppKit::OpenGL::GLVertexBufferObject *vbo_tangent;
                AppKit::OpenGL::GLVertexBufferObject *vbo_binormal;
                AppKit::OpenGL::GLVertexBufferObject *vbo_uv[8];
                AppKit::OpenGL::GLVertexBufferObject *vbo_color[8];

                // skin data
                AppKit::OpenGL::GLVertexBufferObject *vbo_skin_index;
                AppKit::OpenGL::GLVertexBufferObject *vbo_skin_weights;

                AppKit::OpenGL::GLVertexBufferObject *vbo_index;

                AppKit::OpenGL::GLVertexArrayObject *vao;
                ITKExtension::Model::BitMask vao_format;

                int vbo_indexCount;

                ITKExtension::Model::BitMask last_model_dynamic_upload;
                ITKExtension::Model::BitMask last_model_static_upload;

                void allocateVBO();

                void uploadVBO(ITKExtension::Model::BitMask model_dynamic_upload, ITKExtension::Model::BitMask model_static_upload, bool index = true);

            public:
                static const ComponentType Type;

                // Model::VertexFormat
                ITKExtension::Model::BitMask format;

                std::vector<MathCore::vec3f> pos;
                std::vector<MathCore::vec3f> normals;
                std::vector<MathCore::vec3f> tangent;
                std::vector<MathCore::vec3f> binormal;
                std::vector<MathCore::vec3f> uv[8];
                std::vector<MathCore::vec4f> color[8];
                // std::vector<uint32_t> color[8];//RGBA

                std::vector<uint32_t> indices;

                // store the structure for vertex skinning
                std::vector<ITKExtension::Model::Bone> bones;
                std::vector<MathCore::vec4f> skin_index;
                std::vector<MathCore::vec4f> skin_weights;

                bool always_clone;

                bool usesVBO() const;

                void ComputeFormat(bool skip_if_already_set = true);

                ComponentMesh();

                ~ComponentMesh();

                void clear();
                void concatenate( Transform *toApply, const ComponentMesh *other, const DefaultEngineShader *shader);
                void concatenation_reserve( Transform *toApply, const ComponentMesh *other, const DefaultEngineShader *shader);
                void concatenation_inplace( Transform *toApply, const ComponentMesh *other, const DefaultEngineShader *shader, uint32_t pos_offset, uint32_t pos_count, uint32_t indices_offset, uint32_t indices_count);

                void releaseVBO();

                void syncVBOStatic();

                void syncVBODynamic();

                void syncVBO(ITKExtension::Model::BitMask model_dynamic_upload, ITKExtension::Model::BitMask model_static_upload, bool index = true);

                void setLayoutPointers(const DefaultEngineShader *shader);
                void setLayoutPointers(ITKExtension::Model::BitMask shaderFormat);

                void draw();

                void unsetLayoutPointers(const DefaultEngineShader *shader);
                void unsetLayoutPointers(ITKExtension::Model::BitMask shaderFormat);

                // best option is to return a ref, 
                // but can clone if necessary
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone);
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);


                //
                // Another constructor
                //
                static std::shared_ptr<ComponentMesh> createTriangle();

                static std::shared_ptr<ComponentMesh> createPlaneXZ(float _width, float _height);

                static std::shared_ptr<ComponentMesh> createPlaneXY(float _width, float _height);

                static std::shared_ptr<ComponentMesh> createBox(const MathCore::vec3f &dimension);

                static std::shared_ptr<ComponentMesh> createSphere(float radius, int sectorCount, int stackCount);
            };

        }
    }

}