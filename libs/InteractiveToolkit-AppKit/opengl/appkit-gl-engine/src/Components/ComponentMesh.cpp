#include <appkit-gl-engine/Components/ComponentMesh.h>
#include <appkit-gl-engine/Components/ComponentMeshWrapper.h>

#include <InteractiveToolkit-Extension/model/Geometry.h>

// using namespace AppKit::GLEngine;
// using namespace AppKit::GLEngine::Components;

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            const ComponentType ComponentMesh::Type = "ComponentMesh";

            void ComponentMesh::allocateVBO()
            {
                if (vbo_index != nullptr)
                    return;
                if (format & ITKExtension::Model::CONTAINS_POS)
                    vbo_pos = new AppKit::OpenGL::GLVertexBufferObject();
                if (format & ITKExtension::Model::CONTAINS_NORMAL)
                    vbo_normals = new AppKit::OpenGL::GLVertexBufferObject();
                if (format & ITKExtension::Model::CONTAINS_TANGENT)
                    vbo_tangent = new AppKit::OpenGL::GLVertexBufferObject();
                if (format & ITKExtension::Model::CONTAINS_BINORMAL)
                    vbo_binormal = new AppKit::OpenGL::GLVertexBufferObject();
                for (int i = 0; i < 8; i++)
                {
                    if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                        vbo_uv[i] = new AppKit::OpenGL::GLVertexBufferObject();
                    if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                        vbo_color[i] = new AppKit::OpenGL::GLVertexBufferObject();
                }
                if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                {
                    vbo_skin_index = new AppKit::OpenGL::GLVertexBufferObject();
                    vbo_skin_weights = new AppKit::OpenGL::GLVertexBufferObject();
                }
                vbo_index = new AppKit::OpenGL::GLVertexBufferObject();
            }

            void ComponentMesh::uploadVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload)
            {
                last_model_dynamic_upload = model_dynamic_upload;
                last_model_static_upload = model_static_upload;

                for (int i = 0; i < getTransformCount(); i++)
                {
                    auto transform = getTransform(i);

                    auto meshWrapper = transform->findComponent<Components::ComponentMeshWrapper>();
                    if (meshWrapper != nullptr)
                    {
                        if (meshWrapper->wrapShape == WrapShapeAABB)
                            meshWrapper->updateMeshAABB();
                        else if (meshWrapper->wrapShape == WrapShapeOBB)
                            meshWrapper->updateMeshOBB();
                        else if (meshWrapper->wrapShape == WrapShapeSphere)
                            meshWrapper->updateMeshSphere();
                        // printf("sync min %f %f %f\n", meshWrapper->aabb.min_box.x,meshWrapper->aabb.min_box.y,meshWrapper->aabb.min_box.z);
                        // printf("     max %f %f %f\n", meshWrapper->aabb.max_box.x,meshWrapper->aabb.max_box.y,meshWrapper->aabb.max_box.z);
                    }
                }

                bool _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_POS) != 0;
                bool _static = (model_static_upload & ITKExtension::Model::CONTAINS_POS) != 0;
                bool canSet = _dynamic || _static;
                bool contains_attribute = (format & ITKExtension::Model::CONTAINS_POS) != 0;
                if (contains_attribute && canSet)
                    vbo_pos->uploadData((void *)&pos[0], sizeof(MathCore::vec3f) * (int)pos.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                if (contains_attribute && canSet)
                    vbo_normals->uploadData((void *)&normals[0], sizeof(MathCore::vec3f) * (int)normals.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                if (contains_attribute && canSet)
                    vbo_tangent->uploadData((void *)&tangent[0], sizeof(MathCore::vec3f) * (int)tangent.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                if (contains_attribute && canSet)
                    vbo_binormal->uploadData((void *)&binormal[0], sizeof(MathCore::vec3f) * (int)binormal.size(), _dynamic);

                uint32_t bit_flag;
                for (int i = 0; i < 8; i++)
                {
                    bit_flag = (ITKExtension::Model::CONTAINS_UV0 << i);
                    _dynamic = (model_dynamic_upload & bit_flag) != 0;
                    _static = (model_static_upload & bit_flag) != 0;
                    canSet = _dynamic || _static;
                    contains_attribute = (format & bit_flag) != 0;
                    if (contains_attribute && canSet)
                        vbo_uv[i]->uploadData((void *)&uv[i][0], sizeof(MathCore::vec3f) * (int)uv[i].size(), _dynamic);

                    bit_flag = (ITKExtension::Model::CONTAINS_COLOR0 << i);
                    _dynamic = (model_dynamic_upload & bit_flag) != 0;
                    _static = (model_static_upload & bit_flag) != 0;
                    canSet = _dynamic || _static;
                    contains_attribute = (format & bit_flag) != 0;
                    if (contains_attribute && canSet)
                        vbo_color[i]->uploadData((void *)&color[i][0], sizeof(MathCore::vec4f) * (int)color[i].size(), _dynamic);
                }

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                if (contains_attribute && canSet)
                {
                    vbo_skin_index->uploadData((void *)&skin_index[0], sizeof(MathCore::vec4f) * (int)skin_index.size(), _dynamic);
                    vbo_skin_weights->uploadData((void *)&skin_weights[0], sizeof(MathCore::vec4f) * (int)skin_weights.size(), _dynamic);
                }

                vbo_indexCount = (int)indices.size();
                vbo_index->uploadIndex((void *)&indices[0], (int)indices.size() * sizeof(uint32_t), false);
            }

            void ComponentMesh::ComputeFormat()
            {
                if (format != 0)
                    return;

                if (pos.size() > 0)
                    format |= ITKExtension::Model::CONTAINS_POS;
                if (normals.size() > 0)
                    format |= ITKExtension::Model::CONTAINS_NORMAL;
                if (tangent.size() > 0)
                    format |= ITKExtension::Model::CONTAINS_TANGENT;
                if (binormal.size() > 0)
                    format |= ITKExtension::Model::CONTAINS_BINORMAL;
                for (int i = 0; i < 8; i++)
                {
                    if (uv[i].size() > 0)
                        format |= ITKExtension::Model::CONTAINS_UV0 << i;
                    if (color[i].size() > 0)
                        format |= ITKExtension::Model::CONTAINS_COLOR0 << i;
                }

                // skinned mesh, works with any uniform matrix size...
                if (skin_index.size() > 0)
                    format |= ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY;
            }

            ComponentMesh::ComponentMesh() : Component(ComponentMesh::Type)
            {
                vbo_indexCount = 0;

                vbo_pos = nullptr;
                vbo_normals = nullptr;
                vbo_tangent = nullptr;
                vbo_binormal = nullptr;
                for (int i = 0; i < 8; i++)
                {
                    vbo_uv[i] = nullptr;
                    vbo_color[i] = nullptr;
                }

                vbo_skin_index = nullptr;
                vbo_skin_weights = nullptr;

                vbo_index = nullptr;

                format = 0;

                always_clone = false;

                last_model_dynamic_upload = 0;
                last_model_static_upload = 0;
            }

            ComponentMesh::~ComponentMesh()
            {
                if (vbo_pos != nullptr)
                    delete vbo_pos;
                if (vbo_normals != nullptr)
                    delete vbo_normals;
                if (vbo_tangent != nullptr)
                    delete vbo_tangent;
                if (vbo_binormal != nullptr)
                    delete vbo_binormal;
                for (int i = 0; i < 8; i++)
                {
                    if (vbo_uv[i] != nullptr)
                        delete vbo_uv[i];
                    if (vbo_color[i] != nullptr)
                        delete vbo_color[i];
                }

                if (vbo_skin_index != nullptr)
                    delete vbo_skin_index;
                if (vbo_skin_weights != nullptr)
                    delete vbo_skin_weights;

                if (vbo_index != nullptr)
                    delete vbo_index;
            }

            void ComponentMesh::syncVBOStatic()
            {
                ComputeFormat();
                if (pos.size() == 0 || indices.size() == 0)
                {
                    vbo_indexCount = 0;
                    return;
                }
                else
                    ITK_ABORT(!format, "mesh without vertex\n.");
                allocateVBO();
                uploadVBO(0, 0xffffffff);
            }

            void ComponentMesh::syncVBODynamic()
            {
                ComputeFormat();
                if (pos.size() == 0 || indices.size() == 0)
                {
                    vbo_indexCount = 0;
                    return;
                }
                else
                    ITK_ABORT(!format, "mesh without vertex\n.");
                allocateVBO();
                uploadVBO(0xffffffff, 0);
            }

            void ComponentMesh::syncVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload)
            {
                ComputeFormat();
                if (pos.size() == 0 || indices.size() == 0)
                {
                    vbo_indexCount = 0;
                    return;
                }
                else
                    ITK_ABORT(!format, "mesh without vertex\n.");
                allocateVBO();
                uploadVBO(model_dynamic_upload, model_static_upload);
            }

            void ComponentMesh::setLayoutPointers(const DefaultEngineShader *shader)
            {
                uint32_t shaderFormat = shader->format;
                ComputeFormat();
                if (!format)
                    return;

                ITK_ABORT(((format ^ shaderFormat) & shaderFormat), "Shader not compatible with this mesh.\n");

                if (vbo_indexCount > 0)
                {
                    int count = 0;
                    if (shaderFormat & ITKExtension::Model::CONTAINS_POS)
                        vbo_pos->setLayout(count++, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_NORMAL)
                        vbo_normals->setLayout(count++, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_TANGENT)
                        vbo_tangent->setLayout(count++, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_BINORMAL)
                        vbo_binormal->setLayout(count++, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                    for (int i = 0; i < 8; i++)
                    {
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_UV0 << i))
                            vbo_uv[i]->setLayout(count++, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                            vbo_color[i]->setLayout(count++, 4, GL_FLOAT, sizeof(MathCore::vec4f), 0);
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                    {
                        vbo_skin_index->setLayout(count++, 4, GL_FLOAT, sizeof(MathCore::vec4f), 0);
                        vbo_skin_weights->setLayout(count++, 4, GL_FLOAT, sizeof(MathCore::vec4f), 0);
                    }

                    vbo_index->setIndex();
                }
                else if (indices.size() > 0)
                {
                    int count = 0;

                    if (shaderFormat & ITKExtension::Model::CONTAINS_POS)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &pos[0]));
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_NORMAL)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &normals[0]));
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_TANGENT)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &tangent[0]));
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_BINORMAL)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &binormal[0]));
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_UV0 << i))
                        {
                            OPENGL_CMD(glEnableVertexAttribArray(count));
                            OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &uv[i][0]));
                        }
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                        {
                            OPENGL_CMD(glEnableVertexAttribArray(count));
                            OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(MathCore::vec4f), &color[i][0]));
                        }
                    }

                    // skinned mesh information
                    if (shaderFormat & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(MathCore::vec4f), &skin_index[0]));

                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(MathCore::vec4f), &skin_weights[0]));
                    }
                }
            }

            void ComponentMesh::draw()
            {
                ComputeFormat();
                if (!format)
                    return;
                if (vbo_indexCount > 0)
                {
                    vbo_index->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_INT);
                }
                else if (indices.size() > 0)
                {
                    OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, &indices[0]));
                }
            }

            void ComponentMesh::unsetLayoutPointers(const DefaultEngineShader *shader)
            {
                uint32_t shaderFormat = shader->format;

                ComputeFormat();
                if (!format)
                    return;
                if (vbo_indexCount > 0)
                {
                    int count = 0;

                    if (shaderFormat & ITKExtension::Model::CONTAINS_POS)
                        vbo_pos->unsetLayout(count++);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_NORMAL)
                        vbo_normals->unsetLayout(count++);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_TANGENT)
                        vbo_tangent->unsetLayout(count++);
                    if (shaderFormat & ITKExtension::Model::CONTAINS_BINORMAL)
                        vbo_binormal->unsetLayout(count++);
                    for (int i = 0; i < 8; i++)
                    {
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_UV0 << i))
                            vbo_uv[i]->unsetLayout(count++);
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                            vbo_color[i]->unsetLayout(count++);
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                    {
                        vbo_skin_index->unsetLayout(count++);
                        vbo_skin_weights->unsetLayout(count++);
                    }

                    vbo_index->unsetIndex();
                }
                else if (indices.size() > 0)
                {
                    int count = 0;

                    if (shaderFormat & ITKExtension::Model::CONTAINS_POS)
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    if (shaderFormat & ITKExtension::Model::CONTAINS_NORMAL)
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    if (shaderFormat & ITKExtension::Model::CONTAINS_TANGENT)
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    if (shaderFormat & ITKExtension::Model::CONTAINS_BINORMAL)
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    for (int i = 0; i < 8; i++)
                    {
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_UV0 << i))
                            OPENGL_CMD(glDisableVertexAttribArray(count++));
                        if (shaderFormat & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                            OPENGL_CMD(glDisableVertexAttribArray(count++));
                    }
                    if (shaderFormat & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                    {
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    }
                }
            }

            // best option to ref,
            // but can clone if necessary
            std::shared_ptr<Component> ComponentMesh::duplicate_ref_or_clone(bool force_clone)
            {
                if (!always_clone && !force_clone)
                    return self();
                auto result = Component::CreateShared<ComponentMesh>();

                result->format = this->format;

                result->pos = this->pos;
                result->normals = this->normals;
                result->tangent = this->tangent;
                result->binormal = this->binormal;
                for (int i = 0; i < 8; i++)
                {
                    result->uv[i] = this->uv[i];
                    result->color[i] = this->color[i];
                }
                // std::vector<uint32_t> color[8];//RGBA

                result->indices = this->indices;

                // store the structure for vertex skinning
                result->bones = this->bones;
                result->skin_index = this->skin_index;
                result->skin_weights = this->skin_weights;

                result->always_clone = this->always_clone;

                // check VBO
                if(this->vbo_indexCount > 0){
                    result->syncVBO(this->last_model_dynamic_upload, this->last_model_static_upload);
                }

                return result;
            }
            void ComponentMesh::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            //
            // Another constructor
            //
            std::shared_ptr<ComponentMesh> ComponentMesh::createTriangle()
            {
                auto result = Component::CreateShared<ComponentMesh>();
                setTriangle(
                    &result->indices,
                    &result->pos,
                    &result->uv[0],
                    &result->normals,
                    &result->tangent,
                    &result->binormal);
                return result;
            }

            std::shared_ptr<ComponentMesh> ComponentMesh::createPlaneXZ(float _width, float _height)
            {
                auto result = Component::CreateShared<ComponentMesh>();
                setPlane(false, true,
                         _width, _height,
                         &result->indices,
                         &result->pos,
                         &result->uv[0],
                         &result->normals,
                         &result->tangent,
                         &result->binormal);
                return result;
            }

            std::shared_ptr<ComponentMesh> ComponentMesh::createPlaneXY(float _width, float _height)
            {
                auto result = Component::CreateShared<ComponentMesh>();
                setPlane(true, false,
                         _width, _height,
                         &result->indices,
                         &result->pos,
                         &result->uv[0],
                         &result->normals,
                         &result->tangent,
                         &result->binormal);
                return result;
            }

            std::shared_ptr<ComponentMesh> ComponentMesh::createBox(const MathCore::vec3f &dimension)
            {
                auto result = Component::CreateShared<ComponentMesh>();
                setBox(dimension,
                       &result->indices,
                       &result->pos,
                       &result->uv[0],
                       &result->normals,
                       &result->tangent,
                       &result->binormal);
                return result;
            }

            std::shared_ptr<ComponentMesh> ComponentMesh::createSphere(float radius, int sectorCount, int stackCount)
            {
                auto result = Component::CreateShared<ComponentMesh>();
                setSphere(radius, sectorCount, stackCount,
                          &result->indices,
                          &result->pos,
                          &result->uv[0],
                          &result->normals,
                          &result->tangent,
                          &result->binormal);
                return result;
            }

        }
    }
}