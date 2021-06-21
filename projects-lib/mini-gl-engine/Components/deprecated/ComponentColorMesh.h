#ifndef _component_color_mesh_h_
#define _component_color_mesh_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

namespace GLEngine {

    namespace Components {


        void setTriangle(std::vector<unsigned short> *indices,
                        aRibeiro::aligned_vector<aRibeiro::vec3> *vertices,
                        aRibeiro::aligned_vector<aRibeiro::vec3> *uv = NULL,
                        aRibeiro::aligned_vector<aRibeiro::vec3> *normals = NULL,
                        aRibeiro::aligned_vector<aRibeiro::vec3> *tangents = NULL,
                        aRibeiro::aligned_vector<aRibeiro::vec3> *binormals = NULL);

        void setPlane(bool _xy,
                     bool _xz,
                    float _width, float _height,
                    std::vector<unsigned short> *indices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *vertices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *uv = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *normals = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *tangents = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *binormals = NULL);

        void setBox(const aRibeiro::vec3 &dimension,
                    std::vector<unsigned short> *indices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *vertices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *uv = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *normals = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *tangents = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *binormals = NULL);

        void setSphere(float radius, int sectorCount, int stackCount,
                    std::vector<unsigned short> *indices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *vertices,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *uv = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *normals = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *tangents = NULL,
                    aRibeiro::aligned_vector<aRibeiro::vec3> *binormals = NULL);

        class ComponentColorMesh: public Component {

            openglWrapper::GLVertexBufferObject *vbo_data;
            openglWrapper::GLVertexBufferObject *vbo_index;
            int vbo_indexCount;

        public:
            
            static const ComponentType Type;

            aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
            std::vector<unsigned short> indices;
            
            aRibeiro::vec4 color;

            ComponentColorMesh():Component(ComponentColorMesh::Type) {
                vbo_indexCount = 0;
                vbo_data = NULL;
                vbo_index = NULL;
            }
    
            ComponentColorMesh(aRibeiro::aligned_vector<aRibeiro::vec3> &vertices,
                                const std::vector<unsigned short> &indices,
                                const aRibeiro::vec4 &color):Component(ComponentColorMesh::Type){

                this->vertices = vertices;
                this->indices = indices;

                this->color = color;

                vbo_indexCount = 0;
                vbo_data = NULL;
                vbo_index = NULL;
                
            }

            void syncVBOStatic() {

                if (vbo_data == NULL){
                    vbo_data = new openglWrapper::GLVertexBufferObject();
                    vbo_index = new openglWrapper::GLVertexBufferObject();
                }

                vbo_indexCount = indices.size();
                vbo_data->uploadData((void*)&vertices[0], sizeof(aRibeiro::vec3)*vertices.size(), false);
                vbo_index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short), false);
            }
            
            void syncVBODynamic() {
                
                if (vbo_data == NULL){
                    vbo_data = new openglWrapper::GLVertexBufferObject();
                    vbo_index = new openglWrapper::GLVertexBufferObject();
                }

                vbo_indexCount = indices.size();
                vbo_data->uploadData((void*)&vertices[0], sizeof(aRibeiro::vec3)*vertices.size(), true);
                vbo_index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short), true);
            }
            
            ~ComponentColorMesh(){
                aRibeiro::setNullAndDelete(vbo_data);
                aRibeiro::setNullAndDelete(vbo_index);
            }
            
            void setLayoutPointers(GLint positionLayout) {

                if (vbo_indexCount > 0) {
                    vbo_data->setLayout(positionLayout, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                    vbo_index->setIndex();
                } else if ( indices.size() > 0){
                    OPENGL_CMD(glEnableVertexAttribArray(positionLayout));
                    OPENGL_CMD(glVertexAttribPointer(positionLayout, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertices[0]));
                }
            }
            
            void draw() {
                if (vbo_indexCount > 0) {
                    vbo_index->drawIndex(GL_TRIANGLES, vbo_indexCount, GL_UNSIGNED_SHORT);
                } else if ( indices.size() > 0){
                    OPENGL_CMD(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]));
                }
            }
            
            void unsetLayoutPointers(GLint positionLayout) {
                if (vbo_indexCount > 0) {
                    vbo_data->unsetLayout(positionLayout);
                    vbo_index->unsetIndex();
                } else if ( indices.size() > 0){
                    OPENGL_CMD(glDisableVertexAttribArray(positionLayout));
                }
            }
            
            //
            // Another constructor
            //
            static ComponentColorMesh* createTriangle(const aRibeiro::vec4 &color){
                aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
                std::vector<unsigned short> indices;
                setTriangle(&indices, &vertices);
                return new ComponentColorMesh(vertices, indices, color);
            }
            
            static ComponentColorMesh* createPlane(const aRibeiro::vec4 &color, const aRibeiro::vec3 &dimension){
                aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
                std::vector<unsigned short> indices;
                setPlane(false, true, dimension.x,dimension.z,&indices, &vertices);
                return new ComponentColorMesh(vertices, indices, color);
            }
            
            static ComponentColorMesh* createBox(const aRibeiro::vec4 &color, const aRibeiro::vec3 &dimension){
                aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
                std::vector<unsigned short> indices;
                setBox(dimension,&indices, &vertices);
                return new ComponentColorMesh(vertices, indices, color);
            }
            
            static ComponentColorMesh* createSphere(const aRibeiro::vec4 &color, float radius, int sectorCount, int stackCount){
                aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
                std::vector<unsigned short> indices;
                setSphere(radius, sectorCount, stackCount, &indices, &vertices);
                return new ComponentColorMesh(vertices, indices, color);
            }
            
        };

    }
}

#endif
