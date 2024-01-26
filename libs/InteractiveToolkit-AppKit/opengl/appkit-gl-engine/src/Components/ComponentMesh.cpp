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
                if (vbo_index != NULL)
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

                for (size_t i = 0; i < transform.size(); i++)
                {
                    Transform *transform = this->transform[i];
                    Components::ComponentMeshWrapper *meshWrapper =
                        (Components::ComponentMeshWrapper *)transform->findComponent(Components::ComponentMeshWrapper::Type);
                    if (meshWrapper != NULL)
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
                    vbo_pos->uploadData((void *)&pos[0], sizeof(MathCore::vec3f) * pos.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_NORMAL) != 0;
                if (contains_attribute && canSet)
                    vbo_normals->uploadData((void *)&normals[0], sizeof(MathCore::vec3f) * normals.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_TANGENT) != 0;
                if (contains_attribute && canSet)
                    vbo_tangent->uploadData((void *)&tangent[0], sizeof(MathCore::vec3f) * tangent.size(), _dynamic);

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_BINORMAL) != 0;
                if (contains_attribute && canSet)
                    vbo_binormal->uploadData((void *)&binormal[0], sizeof(MathCore::vec3f) * binormal.size(), _dynamic);

                uint32_t bit_flag;
                for (int i = 0; i < 8; i++)
                {
                    bit_flag = (ITKExtension::Model::CONTAINS_UV0 << i);
                    _dynamic = (model_dynamic_upload & bit_flag) != 0;
                    _static = (model_static_upload & bit_flag) != 0;
                    canSet = _dynamic || _static;
                    contains_attribute = (format & bit_flag) != 0;
                    if (contains_attribute && canSet)
                        vbo_uv[i]->uploadData((void *)&uv[i][0], sizeof(MathCore::vec3f) * uv[i].size(), _dynamic);

                    bit_flag = (ITKExtension::Model::CONTAINS_COLOR0 << i);
                    _dynamic = (model_dynamic_upload & bit_flag) != 0;
                    _static = (model_static_upload & bit_flag) != 0;
                    canSet = _dynamic || _static;
                    contains_attribute = (format & bit_flag) != 0;
                    if (contains_attribute && canSet)
                        vbo_color[i]->uploadData((void *)&color[i][0], sizeof(MathCore::vec4f) * color[i].size(), _dynamic);
                }

                _dynamic = (model_dynamic_upload & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                _static = (model_static_upload & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
                if (contains_attribute && canSet)
                {
                    vbo_skin_index->uploadData((void *)&skin_index[0], sizeof(MathCore::vec4f) * skin_index.size(), _dynamic);
                    vbo_skin_weights->uploadData((void *)&skin_weights[0], sizeof(MathCore::vec4f) * skin_weights.size(), _dynamic);
                }

                vbo_indexCount = indices.size();
                vbo_index->uploadIndex((void *)&indices[0], indices.size() * sizeof(uint32_t), false);
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

                vbo_pos = NULL;
                vbo_normals = NULL;
                vbo_tangent = NULL;
                vbo_binormal = NULL;
                for (int i = 0; i < 8; i++)
                {
                    vbo_uv[i] = NULL;
                    vbo_color[i] = NULL;
                }

                vbo_skin_index = NULL;
                vbo_skin_weights = NULL;

                vbo_index = NULL;

                format = 0;
            }

            ComponentMesh::~ComponentMesh()
            {
                if (vbo_pos != NULL)
                    delete vbo_pos;
                if (vbo_normals != NULL)
                    delete vbo_normals;
                if (vbo_tangent != NULL)
                    delete vbo_tangent;
                if (vbo_binormal != NULL)
                    delete vbo_binormal;
                for (int i = 0; i < 8; i++)
                {
                    if (vbo_uv[i] != NULL)
                        delete vbo_uv[i];
                    if (vbo_color[i] != NULL)
                        delete vbo_color[i];
                }

                if (vbo_skin_index != NULL)
                    delete vbo_skin_index;
                if (vbo_skin_weights != NULL)
                    delete vbo_skin_weights;

                if (vbo_index != NULL)
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
                    OPENGL_CMD(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, &indices[0]));
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

            //
            // Another constructor
            //
            ComponentMesh *ComponentMesh::createTriangle()
            {
                ComponentMesh *result = new ComponentMesh();
                setTriangle(
                    &result->indices,
                    &result->pos,
                    &result->uv[0],
                    &result->normals,
                    &result->tangent,
                    &result->binormal);
                return result;
            }

            ComponentMesh *ComponentMesh::createPlaneXZ(float _width, float _height)
            {
                ComponentMesh *result = new ComponentMesh();
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

            ComponentMesh *ComponentMesh::createPlaneXY(float _width, float _height)
            {
                ComponentMesh *result = new ComponentMesh();
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

            ComponentMesh *ComponentMesh::createBox(const MathCore::vec3f &dimension)
            {
                ComponentMesh *result = new ComponentMesh();
                setBox(dimension,
                       &result->indices,
                       &result->pos,
                       &result->uv[0],
                       &result->normals,
                       &result->tangent,
                       &result->binormal);
                return result;
            }

            ComponentMesh *ComponentMesh::createSphere(float radius, int sectorCount, int stackCount)
            {
                ComponentMesh *result = new ComponentMesh();
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