#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLVertexBufferObject.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            void setTriangle(std::vector<uint32_t> *indices,
                             std::vector<MathCore::vec3f> *vertices,
                             std::vector<MathCore::vec3f> *uv = nullptr,
                             std::vector<MathCore::vec3f> *normals = nullptr,
                             std::vector<MathCore::vec3f> *tangents = nullptr,
                             std::vector<MathCore::vec3f> *binormals = nullptr);

            void setPlane(bool _xy,
                          bool _xz,
                          float _width, float _height,
                          std::vector<uint32_t> *indices,
                          std::vector<MathCore::vec3f> *vertices,
                          std::vector<MathCore::vec3f> *uv = nullptr,
                          std::vector<MathCore::vec3f> *normals = nullptr,
                          std::vector<MathCore::vec3f> *tangents = nullptr,
                          std::vector<MathCore::vec3f> *binormals = nullptr);

            void setBox(const MathCore::vec3f &dimension,
                        std::vector<uint32_t> *indices,
                        std::vector<MathCore::vec3f> *vertices,
                        std::vector<MathCore::vec3f> *uv = nullptr,
                        std::vector<MathCore::vec3f> *normals = nullptr,
                        std::vector<MathCore::vec3f> *tangents = nullptr,
                        std::vector<MathCore::vec3f> *binormals = nullptr);

            void setSphere(float radius, int sectorCount, int stackCount,
                           std::vector<uint32_t> *indices,
                           std::vector<MathCore::vec3f> *vertices,
                           std::vector<MathCore::vec3f> *uv = nullptr,
                           std::vector<MathCore::vec3f> *normals = nullptr,
                           std::vector<MathCore::vec3f> *tangents = nullptr,
                           std::vector<MathCore::vec3f> *binormals = nullptr);

            class ComponentColorMesh : public Component
            {

                AppKit::OpenGL::GLVertexBufferObject *vbo_data;
                AppKit::OpenGL::GLVertexBufferObject *vbo_index;
                int vbo_indexCount;

            public:
                static const ComponentType Type;

                std::vector<MathCore::vec3f> vertices;
                std::vector<uint32_t> indices;

                MathCore::vec4f color;

                ComponentColorMesh() : Component(ComponentColorMesh::Type)
                {
                    vbo_indexCount = 0;
                    vbo_data = nullptr;
                    vbo_index = nullptr;
                }

                ComponentColorMesh(std::vector<MathCore::vec3f> &vertices,
                                   const std::vector<uint32_t> &indices,
                                   const MathCore::vec4f &color) : Component(ComponentColorMesh::Type)
                {

                    this->vertices = vertices;
                    this->indices = indices;

                    this->color = color;

                    vbo_indexCount = 0;
                    vbo_data = nullptr;
                    vbo_index = nullptr;
                }

                void syncVBOStatic()
                {

                    if (vbo_data == nullptr)
                    {
                        vbo_data = new AppKit::OpenGL::GLVertexBufferObject();
                        vbo_index = new AppKit::OpenGL::GLVertexBufferObject();
                    }

                    vbo_indexCount = (int)indices.size();
                    vbo_data->uploadData((void *)&vertices[0], sizeof(MathCore::vec3f) * (int)vertices.size(), false);
                    vbo_index->uploadIndex((void *)&indices[0], (int)indices.size() * sizeof(uint32_t), false);
                }

                void syncVBODynamic()
                {

                    if (vbo_data == nullptr)
                    {
                        vbo_data = new AppKit::OpenGL::GLVertexBufferObject();
                        vbo_index = new AppKit::OpenGL::GLVertexBufferObject();
                    }

                    vbo_indexCount = (int)indices.size();
                    vbo_data->uploadData((void *)&vertices[0], sizeof(MathCore::vec3f) * (int)vertices.size(), true);
                    vbo_index->uploadIndex((void *)&indices[0], (int)indices.size() * sizeof(uint32_t), true);
                }

                ~ComponentColorMesh()
                {
                    if (vbo_data != nullptr)
                        delete vbo_data;
                    if (vbo_index != nullptr)
                        delete vbo_index;
                }

                void setLayoutPointers(GLint positionLayout)
                {

                    if (vbo_indexCount > 0)
                    {
                        vbo_data->setLayout(positionLayout, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                        vbo_index->setIndex();
                    }
                    else if (indices.size() > 0)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(positionLayout));
                        OPENGL_CMD(glVertexAttribPointer(positionLayout, 3, GL_FLOAT, false, (GLsizei)sizeof(MathCore::vec3f), &vertices[0]));
                    }
                }

                void draw()
                {
                    if (vbo_indexCount > 0)
                    {
                        vbo_index->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_INT);
                    }
                    else if (indices.size() > 0)
                    {
                        OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, &indices[0]));
                    }
                }

                void unsetLayoutPointers(GLint positionLayout)
                {
                    if (vbo_indexCount > 0)
                    {
                        vbo_data->unsetLayout(positionLayout);
                        vbo_index->unsetIndex();
                    }
                    else if (indices.size() > 0)
                    {
                        OPENGL_CMD(glDisableVertexAttribArray(positionLayout));
                    }
                }

                // always clone
                std::shared_ptr<Component> duplicate_ref_or_clone(AppKit::GLEngine::ResourceMap *resourceMap, bool force_clone)
                {
                    auto result = Component::CreateShared<ComponentColorMesh>();

                    result->vertices = this->vertices;
                    result->indices = this->indices;
                    result->color = this->color;

                    if (this->vbo_data != nullptr)
                    {
                        if (this->vbo_data->isLastUploadDynamic())
                            result->syncVBODynamic();
                        else
                            result->syncVBOStatic();
                    }

                    return result;
                }
                void fix_internal_references(AppKit::GLEngine::ResourceMap *resourceMap, TransformMapT &transformMap, ComponentMapT &componentMap)
                {
                }

                //
                // Another constructor
                //
                static ComponentColorMesh *createTriangle(const MathCore::vec4f &color)
                {
                    std::vector<MathCore::vec3f> vertices;
                    std::vector<uint32_t> indices;
                    setTriangle(&indices, &vertices);
                    return new ComponentColorMesh(vertices, indices, color);
                }

                static ComponentColorMesh *createPlane(const MathCore::vec4f &color, const MathCore::vec3f &dimension)
                {
                    std::vector<MathCore::vec3f> vertices;
                    std::vector<uint32_t> indices;
                    setPlane(false, true, dimension.x, dimension.z, &indices, &vertices);
                    return new ComponentColorMesh(vertices, indices, color);
                }

                static ComponentColorMesh *createBox(const MathCore::vec4f &color, const MathCore::vec3f &dimension)
                {
                    std::vector<MathCore::vec3f> vertices;
                    std::vector<uint32_t> indices;
                    setBox(dimension, &indices, &vertices);
                    return new ComponentColorMesh(vertices, indices, color);
                }

                static ComponentColorMesh *createSphere(const MathCore::vec4f &color, float radius, int sectorCount, int stackCount)
                {
                    std::vector<MathCore::vec3f> vertices;
                    std::vector<uint32_t> indices;
                    setSphere(radius, sectorCount, stackCount, &indices, &vertices);
                    return new ComponentColorMesh(vertices, indices, color);
                }

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                    writer.StartObject();
                    writer.String("type");
                    writer.String(ComponentColorMesh::Type);
                    writer.String("id");
                    writer.Uint64((intptr_t)self().get());
                    writer.EndObject();
                    
                }
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet){
                    if (!_value.HasMember("type") || !_value["type"].IsString())
                        return;
                    if (!strcmp(_value["type"].GetString(), ComponentColorMesh::Type) == 0)
                        return;
                    
                }

            };

        }
    }

}
