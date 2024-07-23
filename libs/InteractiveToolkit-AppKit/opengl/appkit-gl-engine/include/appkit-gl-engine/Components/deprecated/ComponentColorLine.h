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

            class ComponentColorLine : public Component
            {

                AppKit::OpenGL::GLVertexBufferObject *vbo_data;
                int vbo_vertexCount;

            public:
                static const ComponentType Type;
                std::vector<MathCore::vec3f> vertices;
                MathCore::vec4f color;
                float width;

                ComponentColorLine() : Component(ComponentColorLine::Type)
                {
                    width = 1.0f;
                    color = MathCore::vec4f(0, 0, 0, 1);
                    vbo_data = nullptr;
                    vbo_vertexCount = 0;
                }

                ~ComponentColorLine()
                {
                    if (vbo_data != nullptr){
                        delete vbo_data;
                        vbo_data = nullptr;
                    }
                }

                void syncVBOStatic()
                {
                    if (vertices.size() == 0)
                        return;
                    if (vbo_data == nullptr)
                        vbo_data = new AppKit::OpenGL::GLVertexBufferObject();

                    vbo_data->uploadData((void *)&vertices[0], sizeof(MathCore::vec3f) * (int)vertices.size());
                    vbo_vertexCount = (int)vertices.size();
                }

                void syncVBODynamic()
                {
                    if (vertices.size() == 0)
                        return;
                    if (vbo_data == nullptr)
                        vbo_data = new AppKit::OpenGL::GLVertexBufferObject();

                    vbo_data->uploadData((void *)&vertices[0], sizeof(MathCore::vec3f) * (int)vertices.size(), true);
                    vbo_vertexCount = (int)vertices.size();
                }

                virtual void setLayoutPointers(GLint positionLayout)
                {
                    if (vbo_vertexCount > 0)
                        vbo_data->setLayout(positionLayout, 3, GL_FLOAT, sizeof(MathCore::vec3f), 0);
                    else if (vertices.size() > 0)
                    {
                        OPENGL_CMD(glEnableVertexAttribArray(positionLayout));
                        OPENGL_CMD(glVertexAttribPointer(positionLayout, 3, GL_FLOAT, false, (GLsizei)sizeof(MathCore::vec3f), &vertices[0]));
                    }
                }

                virtual void draw()
                {
                    if (vbo_vertexCount > 0)
                        vbo_data->drawArrays(GL_LINES, vbo_vertexCount);
                    else if (vertices.size() > 0)
                    {
                        OPENGL_CMD(glDrawArrays(GL_LINES, 0, (GLsizei)vertices.size()));
                    }
                }

                virtual void unsetLayoutPointers(GLint positionLayout)
                {
                    if (vbo_vertexCount > 0)
                        vbo_data->unsetLayout(positionLayout);
                    else if (vertices.size() > 0)
                    {
                        OPENGL_CMD(glDisableVertexAttribArray(positionLayout));
                    }
                }
            };

        }
    }

}