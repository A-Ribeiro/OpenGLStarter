#ifndef _component_color_line_h_
#define _component_color_line_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

namespace GLEngine {

    namespace Components {

        class ComponentColorLine: public Component {
            
            openglWrapper::GLVertexBufferObject *vbo_data;
            int vbo_vertexCount;
        public:
            static const ComponentType Type;
            aRibeiro::aligned_vector<aRibeiro::vec3> vertices;
            aRibeiro::vec4 color;
            float width;
            
            ComponentColorLine():Component(ComponentColorLine::Type) {
                width = 1.0f;
                color = aRibeiro::vec4(0,0,0,1);
                vbo_data = NULL;
                vbo_vertexCount = 0;
            }
            
            virtual ~ComponentColorLine() {
                aRibeiro::setNullAndDelete(vbo_data);
            }

            void syncVBOStatic() {
                if (vbo_data == NULL)
                    vbo_data = new openglWrapper::GLVertexBufferObject();

                vbo_data->uploadData((void*)&vertices[0], sizeof(aRibeiro::vec3)*vertices.size());
                vbo_vertexCount = vertices.size();
            }
            
            void syncVBODynamic() {
                if (vbo_data == NULL)
                    vbo_data = new openglWrapper::GLVertexBufferObject();

                vbo_data->uploadData((void*)&vertices[0], sizeof(aRibeiro::vec3)*vertices.size(), true);
                vbo_vertexCount = vertices.size();
            }
            
            virtual void setLayoutPointers(GLint positionLayout) {
                if (vbo_vertexCount > 0)
                    vbo_data->setLayout(positionLayout, 3, GL_FLOAT, sizeof(aRibeiro::vec3), 0);
                else if (vertices.size() > 0) {
                    OPENGL_CMD(glEnableVertexAttribArray(positionLayout));
                    OPENGL_CMD(glVertexAttribPointer(positionLayout, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertices[0]));
                }
            }
            
            virtual void draw() {
                if (vbo_vertexCount > 0)
                    vbo_data->drawArrays(GL_LINES,vbo_vertexCount);
                else if (vertices.size() > 0) {
                    OPENGL_CMD(glDrawArrays(GL_LINES, 0, vertices.size()));
                }
            }
            
            virtual void unsetLayoutPointers(GLint positionLayout) {
                if (vbo_vertexCount > 0)
                    vbo_data->unsetLayout(positionLayout);
                else if (vertices.size() > 0) {
                    OPENGL_CMD(glDisableVertexAttribArray(positionLayout));
                }
            }

        };

    }
}

#endif
