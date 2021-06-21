#include "ComponentMesh.h"

using namespace aRibeiro;
using namespace GLEngine;
using namespace GLEngine::Components;

namespace GLEngine {

    namespace Components {

        const ComponentType ComponentMesh::Type = "ComponentMesh";


        void ComponentMesh::allocateVBO() {
            if (vbo_index != NULL)
                return;
            if (format & model::CONTAINS_POS)
                vbo_pos = new openglWrapper::GLVertexBufferObject();
            if (format & model::CONTAINS_NORMAL)
                vbo_normals = new openglWrapper::GLVertexBufferObject();
            if (format & model::CONTAINS_TANGENT)
                vbo_tangent = new openglWrapper::GLVertexBufferObject();
            if (format & model::CONTAINS_BINORMAL)
                vbo_binormal = new openglWrapper::GLVertexBufferObject();
            for (int i = 0; i < 8; i++) {
                if (format & (model::CONTAINS_UV0 << i))
                    vbo_uv[i] = new openglWrapper::GLVertexBufferObject();
                if (format & (model::CONTAINS_COLOR0 << i))
                    vbo_color[i] = new openglWrapper::GLVertexBufferObject();
            }
            if (format & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                vbo_skin_index = new openglWrapper::GLVertexBufferObject();
                vbo_skin_weights = new openglWrapper::GLVertexBufferObject();
            }
            vbo_index = new openglWrapper::GLVertexBufferObject();
        }

        void ComponentMesh::uploadVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload) {

            bool _dynamic = (model_dynamic_upload & model::CONTAINS_POS) != 0;
            bool _static = (model_static_upload & model::CONTAINS_POS) != 0;
            bool canSet = _dynamic || _static;
            bool contains_attribute = (format & model::CONTAINS_POS) != 0;
            if (contains_attribute && canSet)
                vbo_pos->uploadData((void*)&pos[0], sizeof(aRibeiro::vec3)*pos.size(), _dynamic);

            _dynamic = (model_dynamic_upload & model::CONTAINS_NORMAL) != 0;
            _static = (model_static_upload & model::CONTAINS_NORMAL) != 0;
            canSet = _dynamic || _static;
            contains_attribute = (format & model::CONTAINS_NORMAL) != 0;
            if (contains_attribute && canSet)
                vbo_normals->uploadData((void*)&normals[0], sizeof(aRibeiro::vec3)*normals.size(), _dynamic);

            _dynamic = (model_dynamic_upload & model::CONTAINS_TANGENT) != 0;
            _static = (model_static_upload & model::CONTAINS_TANGENT) != 0;
            canSet = _dynamic || _static;
            contains_attribute = (format & model::CONTAINS_TANGENT) != 0;
            if (contains_attribute && canSet)
                vbo_tangent->uploadData((void*)&tangent[0], sizeof(aRibeiro::vec3)*tangent.size(), _dynamic);

            _dynamic = (model_dynamic_upload & model::CONTAINS_BINORMAL) != 0;
            _static = (model_static_upload & model::CONTAINS_BINORMAL) != 0;
            canSet = _dynamic || _static;
            contains_attribute = (format & model::CONTAINS_BINORMAL) != 0;
            if (contains_attribute && canSet)
                vbo_binormal->uploadData((void*)&binormal[0], sizeof(aRibeiro::vec3)*binormal.size(), _dynamic);

            uint32_t bit_flag;
            for (int i = 0; i < 8; i++) {
                bit_flag = (model::CONTAINS_UV0 << i);
                _dynamic = (model_dynamic_upload & bit_flag) != 0;
                _static = (model_static_upload & bit_flag) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & bit_flag) != 0;
                if (contains_attribute && canSet)
                    vbo_uv[i]->uploadData((void*)&uv[i][0], sizeof(aRibeiro::vec3)*uv[i].size(), _dynamic);

                bit_flag = (model::CONTAINS_COLOR0 << i);
                _dynamic = (model_dynamic_upload & bit_flag) != 0;
                _static = (model_static_upload & bit_flag) != 0;
                canSet = _dynamic || _static;
                contains_attribute = (format & bit_flag) != 0;
                if (contains_attribute && canSet)
                    vbo_color[i]->uploadData((void*)&color[i][0], sizeof(aRibeiro::vec4)*color[i].size(), _dynamic);
            }

            _dynamic = (model_dynamic_upload & model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
            _static = (model_static_upload & model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
            canSet = _dynamic || _static;
            contains_attribute = (format & model::CONTAINS_VERTEX_WEIGHT_ANY) != 0;
            if (contains_attribute && canSet) {
                vbo_skin_index->uploadData((void*)&skin_index[0], sizeof(aRibeiro::vec4)*skin_index.size(), _dynamic);
                vbo_skin_weights->uploadData((void*)&skin_weights[0], sizeof(aRibeiro::vec4)*skin_weights.size(), _dynamic);
            }

            vbo_indexCount = indices.size();
            vbo_index->uploadIndex((void*)&indices[0], indices.size() * sizeof(unsigned short), false);
        }

        void ComponentMesh::ComputeFormat() {
            if (format != 0)
                return;

            if (pos.size() > 0)
                format |= model::CONTAINS_POS;
            if (normals.size() > 0)
                format |= model::CONTAINS_NORMAL;
            if (tangent.size() > 0)
                format |= model::CONTAINS_TANGENT;
            if (binormal.size() > 0)
                format |= model::CONTAINS_BINORMAL;
            for (int i = 0; i < 8; i++) {
                if (uv[i].size() > 0)
                    format |= model::CONTAINS_UV0 << i;
                if (color[i].size() > 0)
                    format |= model::CONTAINS_COLOR0 << i;
            }

            //skinned mesh, works with any uniform matrix size...
            if (skin_index.size() > 0)
                format |= model::CONTAINS_VERTEX_WEIGHT_ANY;
        }

        ComponentMesh::ComponentMesh() :Component(ComponentMesh::Type) {
            vbo_indexCount = 0;

            vbo_pos = NULL;
            vbo_normals = NULL;
            vbo_tangent = NULL;
            vbo_binormal = NULL;
            for (int i = 0; i < 8; i++) {
                vbo_uv[i] = NULL;
                vbo_color[i] = NULL;
            }

            vbo_skin_index = NULL;
            vbo_skin_weights = NULL;

            vbo_index = NULL;

            format = 0;
        }

        ComponentMesh::~ComponentMesh() {
            aRibeiro::setNullAndDelete(vbo_pos);
            aRibeiro::setNullAndDelete(vbo_normals);
            aRibeiro::setNullAndDelete(vbo_tangent);
            aRibeiro::setNullAndDelete(vbo_binormal);
            for (int i = 0; i < 8; i++) {
                aRibeiro::setNullAndDelete(vbo_uv[i]);
                aRibeiro::setNullAndDelete(vbo_color[i]);
            }

            aRibeiro::setNullAndDelete(vbo_skin_index);
            aRibeiro::setNullAndDelete(vbo_skin_weights);

            aRibeiro::setNullAndDelete(vbo_index);
        }

        void ComponentMesh::syncVBOStatic() {
            ComputeFormat();
            if (pos.size() == 0 || indices.size() == 0) {
                vbo_indexCount = 0;
                return;
            }
            else
                ARIBEIRO_ABORT(!format, "mesh without vertex\n.");
            allocateVBO();
            uploadVBO(0,0xffffffff);
        }

        void ComponentMesh::syncVBODynamic() {
            ComputeFormat();
            if (pos.size() == 0 || indices.size() == 0) {
                vbo_indexCount = 0;
                return;
            }
            else
                ARIBEIRO_ABORT(!format, "mesh without vertex\n.");
            allocateVBO();
            uploadVBO(0xffffffff, 0);
        }

        void ComponentMesh::syncVBO(uint32_t model_dynamic_upload, uint32_t model_static_upload) {
            ComputeFormat();
            if (pos.size() == 0 || indices.size() == 0) {
                vbo_indexCount = 0;
                return;
            }
            else
                ARIBEIRO_ABORT(!format, "mesh without vertex\n.");
            allocateVBO();
            uploadVBO(model_dynamic_upload, model_static_upload);
        }

        void ComponentMesh::setLayoutPointers(const DefaultEngineShader *shader) {
            uint32_t shaderFormat = shader->format;
            ComputeFormat();
            if (!format)
                return;

            ARIBEIRO_ABORT(((format^shaderFormat) & shaderFormat), "Shader not compatible with this mesh.\n");

            if (vbo_indexCount > 0) {
                int count = 0;
                if (shaderFormat & model::CONTAINS_POS)
                    vbo_pos->setLayout(count++, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                if (shaderFormat & model::CONTAINS_NORMAL)
                    vbo_normals->setLayout(count++, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                if (shaderFormat & model::CONTAINS_TANGENT)
                    vbo_tangent->setLayout(count++, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                if (shaderFormat & model::CONTAINS_BINORMAL)
                    vbo_binormal->setLayout(count++, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                for (int i = 0; i < 8; i++) {
                    if (shaderFormat & (model::CONTAINS_UV0 << i))
                        vbo_uv[i]->setLayout(count++, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                    if (shaderFormat & (model::CONTAINS_COLOR0 << i))
                        vbo_color[i]->setLayout(count++, 4, GL_FLOAT, sizeof(aRibeiro::vec4), 0);
                }
                if (shaderFormat & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                    vbo_skin_index->setLayout(count++, 4, GL_FLOAT, sizeof(aRibeiro::vec4), 0);
                    vbo_skin_weights->setLayout(count++, 4, GL_FLOAT, sizeof(aRibeiro::vec4), 0);
                }

                vbo_index->setIndex();

            }
            else if (indices.size() > 0) {
                int count = 0;

                if (shaderFormat & model::CONTAINS_POS) {
                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &pos[0]));
                }
                if (shaderFormat & model::CONTAINS_NORMAL) {
                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &normals[0]));
                }
                if (shaderFormat & model::CONTAINS_TANGENT) {
                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &tangent[0]));
                }
                if (shaderFormat & model::CONTAINS_BINORMAL) {
                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &binormal[0]));
                }
                for (int i = 0; i < 8; i++) {
                    if (shaderFormat & (model::CONTAINS_UV0 << i)) {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &uv[i][0]));
                    }
                    if (shaderFormat & (model::CONTAINS_COLOR0 << i)) {
                        OPENGL_CMD(glEnableVertexAttribArray(count));
                        OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(aRibeiro::vec4), &color[i][0]));
                    }
                }

                //skinned mesh information
                if (shaderFormat & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(aRibeiro::vec4), &skin_index[0]));

                    OPENGL_CMD(glEnableVertexAttribArray(count));
                    OPENGL_CMD(glVertexAttribPointer(count++, 4, GL_FLOAT, false, sizeof(aRibeiro::vec4), &skin_weights[0]));
                }

            }
        }

        void ComponentMesh::draw() {
            ComputeFormat();
            if (!format)
                return;
            if (vbo_indexCount > 0) {
                vbo_index->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_SHORT);
            }
            else if (indices.size() > 0) {
                OPENGL_CMD(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]));
            }
        }

        void ComponentMesh::unsetLayoutPointers(const DefaultEngineShader *shader) {
            uint32_t shaderFormat = shader->format;

            ComputeFormat();
            if (!format)
                return;
            if (vbo_indexCount > 0) {
                int count = 0;

                if (shaderFormat & model::CONTAINS_POS)
                    vbo_pos->unsetLayout(count++);
                if (shaderFormat & model::CONTAINS_NORMAL)
                    vbo_normals->unsetLayout(count++);
                if (shaderFormat & model::CONTAINS_TANGENT)
                    vbo_tangent->unsetLayout(count++);
                if (shaderFormat & model::CONTAINS_BINORMAL)
                    vbo_binormal->unsetLayout(count++);
                for (int i = 0; i < 8; i++) {
                    if (shaderFormat & (model::CONTAINS_UV0 << i))
                        vbo_uv[i]->unsetLayout(count++);
                    if (shaderFormat & (model::CONTAINS_COLOR0 << i))
                        vbo_color[i]->unsetLayout(count++);
                }
                if (shaderFormat & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                    vbo_skin_index->unsetLayout(count++);
                    vbo_skin_weights->unsetLayout(count++);
                }

                vbo_index->unsetIndex();
            }
            else if (indices.size() > 0) {
                int count = 0;

                if (shaderFormat & model::CONTAINS_POS)
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                if (shaderFormat & model::CONTAINS_NORMAL)
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                if (shaderFormat & model::CONTAINS_TANGENT)
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                if (shaderFormat & model::CONTAINS_BINORMAL)
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                for (int i = 0; i < 8; i++) {
                    if (shaderFormat & (model::CONTAINS_UV0 << i))
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                    if (shaderFormat & (model::CONTAINS_COLOR0 << i))
                        OPENGL_CMD(glDisableVertexAttribArray(count++));
                }
                if (shaderFormat & model::CONTAINS_VERTEX_WEIGHT_ANY) {
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                    OPENGL_CMD(glDisableVertexAttribArray(count++));
                }
            }
        }

        //
        // Another constructor
        //
        ComponentMesh* ComponentMesh::createTriangle() {
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

        ComponentMesh* ComponentMesh::createPlaneXZ(float _width, float _height) {
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

        ComponentMesh* ComponentMesh::createPlaneXY(float _width, float _height) {
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

        ComponentMesh* ComponentMesh::createBox(const aRibeiro::vec3 &dimension) {
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

        ComponentMesh* ComponentMesh::createSphere(float radius, int sectorCount, int stackCount) {
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
