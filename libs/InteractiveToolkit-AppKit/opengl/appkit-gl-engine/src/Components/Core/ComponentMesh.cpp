#include <appkit-gl-engine/Components/Core/ComponentMesh.h>
#include <appkit-gl-engine/Components/Core/ComponentMeshWrapper.h>

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
                vao = new AppKit::OpenGL::GLVertexArrayObject();
            }

            void ComponentMesh::uploadVBO(ITKExtension::Model::BitMask model_dynamic_upload, ITKExtension::Model::BitMask model_static_upload, bool index)
            {
                last_model_dynamic_upload = model_dynamic_upload;
                last_model_static_upload = model_static_upload;

                // printf("uploadVBO getTransformCount(): %i\n", getTransformCount());
                for (int i = 0; i < getTransformCount(); i++)
                {
                    auto transform = getTransform(i);
                    // if (transform == nullptr)
                    //     continue;

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

                if (index)
                {
                    vbo_indexCount = (int)indices.size();
                    vbo_index->uploadIndex((void *)&indices[0], (int)indices.size() * sizeof(uint32_t), false);
                }
            }

            bool ComponentMesh::usesVBO() const
            {
                return vbo_indexCount > 0;
            }

            void ComponentMesh::ComputeFormat(bool skip_if_already_set)
            {
                if (skip_if_already_set && format != 0)
                    return;

                format = 0;

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

                vao = nullptr;
                vao_format = 0;

                format = 0;

                always_clone = false;

                last_model_dynamic_upload = 0;
                last_model_static_upload = 0;
            }

            ComponentMesh::~ComponentMesh()
            {
                releaseVBO();
            }

            void ComponentMesh::clear()
            {
                format = 0;

                pos.clear();
                normals.clear();
                tangent.clear();
                binormal.clear();
                for (int i = 0; i < 8; i++)
                {
                    uv[i].clear();
                    color[i].clear();
                }

                indices.clear();

                // store the structure for vertex skinning
                bones.clear();
                skin_index.clear();
                skin_weights.clear();
            }
            void ComponentMesh::concatenate(Transform *toApply, const ComponentMesh *other, const DefaultEngineShader *shader)
            {
                ITK_ABORT((format != 0 && format != shader->format), "Mesh incompatible format for concatenation.\n");

                format = shader->format;

                // concatenate the vertex data
                auto m = toApply->getMatrix(true);

                uint32_t idx_offset = (uint32_t)pos.size();

                if (format & ITKExtension::Model::CONTAINS_POS)
                {
                    for (const auto &v : other->pos)
                        pos.push_back(MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(v)));
                }
                if (format & ITKExtension::Model::CONTAINS_NORMAL)
                {
                    MathCore::mat3f m_it_3x3 = MathCore::GEN<MathCore::mat3f>::fromMat4(m.inverse_transpose_3x3());
                    for (const auto &v : other->normals)
                        normals.push_back(MathCore::OP<MathCore::vec3f>::normalize(m_it_3x3 * v));

                    if (format & ITKExtension::Model::CONTAINS_TANGENT)
                    {
                        MathCore::mat3f m_3x3 = MathCore::GEN<MathCore::mat3f>::fromMat4(m);

                        size_t idx = other->tangent.size();
                        for (const auto &v : other->tangent)
                            tangent.push_back(MathCore::OP<MathCore::vec3f>::normalize(m_3x3 * v));
                        // fix orthogonallity
                        for (size_t i = idx; i < other->tangent.size() && i < other->normals.size(); i++)
                        {
                            MathCore::vec3f &T = tangent[i];
                            MathCore::vec3f &N = normals[i];
                            T = MathCore::OP<MathCore::vec3f>::normalize(T - MathCore::OP<MathCore::vec3f>::dot(T, N) * N);
                        }

                        if (format & ITKExtension::Model::CONTAINS_BINORMAL)
                        {
                            size_t idx = other->binormal.size();
                            for (size_t i = idx; i < other->tangent.size() && i < other->normals.size(); i++)
                            {
                                MathCore::vec3f &T = tangent[i];
                                MathCore::vec3f &N = normals[i];
                                binormal.push_back(MathCore::OP<MathCore::vec3f>::cross(T, N));
                            }
                        }
                    }
                }

                for (int i = 0; i < 8; i++)
                {
                    if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                        uv[i].insert(uv[i].end(), other->uv[i].begin(), other->uv[i].end());
                    if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                        color[i].insert(color[i].end(), other->color[i].begin(), other->color[i].end());
                }

                for (const auto &v : other->indices)
                    indices.push_back(v + idx_offset);

                // concatenate the structure for vertex skinning
                // bones.insert(bones.end(), other->bones.begin(), other->bones.end());
                if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
                {
                    skin_index.insert(skin_index.end(), other->skin_index.begin(), other->skin_index.end());
                    skin_weights.insert(skin_weights.end(), other->skin_weights.begin(), other->skin_weights.end());
                }
            }

            void ComponentMesh::releaseVBO()
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
                if (vao != nullptr)
                    delete vao;

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

                vao = nullptr;
                vao_format = 0;

                last_model_dynamic_upload = 0;
                last_model_static_upload = 0;
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

            void ComponentMesh::syncVBO(ITKExtension::Model::BitMask model_dynamic_upload, ITKExtension::Model::BitMask model_static_upload, bool index)
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
                uploadVBO(model_dynamic_upload, model_static_upload, index);
            }

            void ComponentMesh::setLayoutPointers(const DefaultEngineShader *shader)
            {
                setLayoutPointers(shader->format);
            }
            void ComponentMesh::setLayoutPointers(ITKExtension::Model::BitMask shaderFormat)
            {
                ComputeFormat();
                if (!format || !shaderFormat)
                    return;

                ITK_ABORT(((format ^ shaderFormat) & shaderFormat), "Shader not compatible with this mesh.\n");

                if (vbo_indexCount > 0)
                {
                    vao->enable();

                    if (vao_format != 0 && vao_format == shaderFormat)
                        return;

                    if (vao_format != 0)
                    {
                        ITKExtension::Model::BitMask aux = vao_format;
                        vao_format = 0;
                        unsetLayoutPointers(aux);
                    }

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

                    vao->disable();
                    unsetLayoutPointers(shaderFormat);

                    vao->enable();

                    vao_format = shaderFormat;
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
                    if (vao_format != 0)
                        vao->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_INT);
                    else
                        vbo_index->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_INT);
                }
                else if (indices.size() > 0)
                {
                    OPENGL_CMD(glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, &indices[0]));
                }
            }

            void ComponentMesh::unsetLayoutPointers(const DefaultEngineShader *shader)
            {
                unsetLayoutPointers(shader->format);
            }

            void ComponentMesh::unsetLayoutPointers(ITKExtension::Model::BitMask shaderFormat)
            {
                ComputeFormat();
                if (!format || !shaderFormat)
                    return;
                if (vbo_indexCount > 0)
                {
                    if (vao_format != 0)
                    {
                        vao->disable();
                        return;
                    }

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
                if (usesVBO())
                    result->syncVBO(this->last_model_dynamic_upload, this->last_model_static_upload);

                return result;
            }
            void ComponentMesh::fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap)
            {
            }

            void ComponentMesh::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer)
            {
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentMesh::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());

                writer.String("format");
                writer.Uint(this->format);

                writer.String("always_clone");
                writer.Bool(this->always_clone);

                writer.String("last_model_dynamic_upload");
                writer.Uint(last_model_dynamic_upload);

                writer.String("last_model_static_upload");
                writer.Uint(last_model_static_upload);

                SerializerUtil::write_vector(writer, "pos", pos);
                SerializerUtil::write_vector(writer, "normals", normals);
                SerializerUtil::write_vector(writer, "tangent", tangent);
                SerializerUtil::write_vector(writer, "binormal", binormal);
                char field_name[64];
                for (int i = 0; i < 8; i++)
                {
                    snprintf(field_name, 64, "uv_%i", i);
                    SerializerUtil::write_vector(writer, field_name, uv[i]);
                    snprintf(field_name, 64, "color_%i", i);
                    SerializerUtil::write_vector(writer, field_name, color[i]);
                }

                SerializerUtil::write_vector(writer, "indices", indices);

                SerializerUtil::write_vector(writer, "bones", bones);
                SerializerUtil::write_vector(writer, "skin_index", skin_index);
                SerializerUtil::write_vector(writer, "skin_weights", skin_weights);

                writer.EndObject();
            }
            void ComponentMesh::Deserialize(rapidjson::Value &_value,
                                            std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                            std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                            ResourceSet &resourceSet)
            {
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentMesh::Type) == 0)
                    return;

                if (!_value.HasMember("format") || !_value["format"].IsUint())
                    return;
                this->format = _value["format"].GetUint();

                if (!_value.HasMember("always_clone") || !_value["always_clone"].IsBool())
                    return;
                this->always_clone = _value["always_clone"].GetBool();

                if (!_value.HasMember("last_model_dynamic_upload") || !_value["last_model_dynamic_upload"].IsUint())
                    return;
                this->last_model_dynamic_upload = _value["last_model_dynamic_upload"].GetUint();

                if (!_value.HasMember("last_model_static_upload") || !_value["last_model_static_upload"].IsUint())
                    return;
                this->last_model_static_upload = _value["last_model_static_upload"].GetUint();

                SerializerUtil::read_vector(_value, "pos", pos);
                SerializerUtil::read_vector(_value, "normals", normals);
                SerializerUtil::read_vector(_value, "tangent", tangent);
                SerializerUtil::read_vector(_value, "binormal", binormal);
                char field_name[64];
                for (int i = 0; i < 8; i++)
                {
                    snprintf(field_name, 64, "uv_%i", i);
                    SerializerUtil::read_vector(_value, field_name, uv[i]);
                    snprintf(field_name, 64, "color_%i", i);
                    SerializerUtil::read_vector(_value, field_name, color[i]);
                }

                SerializerUtil::read_vector(_value, "indices", indices);

                SerializerUtil::read_vector(_value, "bones", bones);
                SerializerUtil::read_vector(_value, "skin_index", skin_index);
                SerializerUtil::read_vector(_value, "skin_weights", skin_weights);

                if (last_model_dynamic_upload != 0 || last_model_static_upload != 0)
                    syncVBO(this->last_model_dynamic_upload, this->last_model_static_upload);
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
                result->syncVBOStatic();
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
                result->syncVBOStatic();
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
                result->syncVBOStatic();
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
                result->syncVBOStatic();
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
                result->syncVBOStatic();
                return result;
            }

        }
    }
}