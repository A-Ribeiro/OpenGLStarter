#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
// #include <glew/glew.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Handle OpenGL VAO (Vertex Array Objects)
        ///
        /// This extensions wrap the VBO layout setup into one OpenGL command.
        ///
        /// The VAO lacks support on Raspberry PI 3.
        ///
        /// Example without index buffer:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace AppKit::OpenGL;
        ///
        /// struct VertexAttrib {
        ///     vec3 position;
        ///     vec2 uv;
        /// };
        /// std::vector<VertexAttrib> vertices;
        ///
        /// GLVertexBufferObject *data = new GLVertexBufferObject();
        ///
        /// data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());
        ///
        /// GLVertexArrayObject *vao = new GLVertexArrayObject();
        ///
        /// // setup VAO code
        /// vao->enable();
        /// data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
        /// data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
        /// vao->disable();
        /// data->unsetLayout(shader_position_attrib);
        /// data->unsetLayout(shader_uv_attrib);
        ///
        /// // Drawing code
        /// vao->enable();
        /// vao->drawArrays(GL_TRIANGLES, vertices.size());
        /// vao->disable();
        /// \endcode
        ///
        /// Example with index buffer:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace AppKit::OpenGL;
        ///
        /// struct VertexAttrib {
        ///     vec3 position;
        ///     vec2 uv;
        /// };
        /// std::vector<VertexAttrib> vertices;
        /// std::vector<unsigned short> indices;
        ///
        /// GLVertexBufferObject *data = new GLVertexBufferObject();
        /// GLVertexBufferObject *index = new GLVertexBufferObject();
        ///
        /// data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size());
        /// index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short));
        ///
        /// GLVertexArrayObject *vao = new GLVertexArrayObject();
        ///
        /// // setup VAO code
        /// vao->enable();
        /// data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
        /// data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
        /// index->setIndex();
        /// vao->disable();
        /// data->unsetLayout(shader_position_attrib);
        /// data->unsetLayout(shader_uv_attrib);
        /// index->unsetIndex();
        ///
        /// // Drawing code
        /// vao->enable();
        /// vao->drawIndex(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT);
        /// vao->disable();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLVertexArrayObject
        {

            GLuint mVAO;

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            GLVertexArrayObject(const GLVertexArrayObject &v) = delete;
            GLVertexArrayObject& operator=(const GLVertexArrayObject &v) = delete;

            GLVertexArrayObject();

            ~GLVertexArrayObject();

            /// \brief Active this VAO to OpenGL state
            ///
            /// \author Alessandro Ribeiro
            ///
            void enable() const;

            /// \brief Deactive this VAO from OpenGL state
            ///
            /// \author Alessandro Ribeiro
            ///
            static void disable();

            /// \brief Draw VAO using it as an index buffer
            ///
            /// Example:
            ///
            /// \code
            /// vao->drawIndex(GL_TRIANGLES, count, GL_UNSIGNED_SHORT);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param primitive OpenGL primitive: GL_TRIANGLES, etc...
            /// \param count Number of vertex index inside the buffer
            /// \param type The type in the VBO. Example: GL_UNSIGNED_SHORT
            /// \param offset Number of indices to skip from the beggining of the buffer
            ///
            static void drawIndex(GLint primitive, int count, GLint type, int offset = 0);

            /// \brief Draw VAO using it as a vertex buffer (without index buffer)
            ///
            /// Example:
            ///
            /// \code
            /// vao->drawArrays(GL_TRIANGLES, count);
            /// \endcode
            /// \author Alessandro Ribeiro
            /// \param primitive OpenGL primitive. Example: GL_TRIANGLES
            /// \param count Number of vertex inside the buffer
            /// \param offset Number of vertex to skip from beggining of the buffer
            ///
            static void drawArrays(GLint primitive, int count, int offset = 0);
        };

    }

}