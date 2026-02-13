#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>

// #include <glew/glew.h>
// #include <appkit-gl-base/platform/PlatformGL.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Handle OpenGL VBO (Vertex Buffer Objects)
        ///
        /// It is possible to upload any kind of data to a VBO.
        ///
        /// The way the vertex shader will read it, depends on the vertex attribute layout setup.
        ///
        /// Example without index buffer:
        ///
        /// \code
        /// #include <appkit-gl-base/opengl-wrapper.h>
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
        /// // drawing code
        /// data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
        /// data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
        ///
        /// data->drawArrays(GL_TRIANGLES, vertices.size());
        ///
        /// data->unsetLayout(shader_position_attrib);
        /// data->unsetLayout(shader_uv_attrib);
        /// \endcode
        ///
        /// Example with index buffer:
        ///
        /// \code
        /// #include <appkit-gl-base/opengl-wrapper.h>
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
        /// // drawing code
        /// data->setLayout(shader_position_attrib, 3, GL_FLOAT, sizeof(VertexAttrib), 0);
        /// data->setLayout(shader_uv_attrib, 2, GL_FLOAT, sizeof(VertexAttrib), sizeof(vec3) );
        /// index->setIndex();
        ///
        /// index->drawIndex(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT);
        ///
        /// data->unsetLayout(shader_position_attrib);
        /// data->unsetLayout(shader_uv_attrib);
        /// index->unsetIndex();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLVertexBufferObject
        {
            GLuint mVBO;
            bool mIsLastUploadDynamic;

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            GLVertexBufferObject(const GLVertexBufferObject &v) = delete;
            GLVertexBufferObject& operator=(const GLVertexBufferObject &v) = delete;

            GLVertexBufferObject();

            ~GLVertexBufferObject();

            bool isLastUploadDynamic();

            /// \brief Upload binary data to this VBO as vertex attributes.
            ///
            /// Example:
            ///
            /// \code
                /// #include <appkit-gl-base/opengl-wrapper.h>
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
            /// data->uploadData((void*)&vertices[0].position, sizeof(VertexAttrib)*vertices.size(), false);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param data data to upload
            /// \param sizeBytes size in bytes
            /// \param dynamic if true then the VBO will be created with GL_DYNAMIC_DRAW parameter.
            ///
            void uploadData(const void *data, int sizeBytes, bool dynamic = false);

            /// \brief Upload binary data to this VBO as vertex index.
            ///
            /// Example:
            ///
            /// \code
                /// #include <appkit-gl-base/opengl-wrapper.h>
                /// using namespace AppKit::OpenGL;
            ///
            /// std::vector<unsigned short> indices;
            ///
            /// GLVertexBufferObject *index = new GLVertexBufferObject();
            ///
            /// index->uploadIndex((void*)&indices[0], indices.size()*sizeof(unsigned short));
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param data data to upload
            /// \param sizeBytes size in bytes
            /// \param dynamic if true then the VBO will be created with GL_DYNAMIC_DRAW parameter.
            ///
            void uploadIndex(const void *data, int sizeBytes, bool dynamic = false);

            /// \brief Active this VBO to be used as index buffer
            ///
            /// \author Alessandro Ribeiro
            ///
            void setIndex() const;

            /// \brief Draw VBO using it as an index buffer
            ///
            /// Example:
            ///
            /// \code
            /// vbo->drawIndex(GL_TRIANGLES, count, GL_UNSIGNED_SHORT);
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param primitive OpenGL primitive: GL_TRIANGLES, etc...
            /// \param count Number of vertex index inside the buffer
            /// \param type The type in the VBO. Example: GL_UNSIGNED_SHORT
            /// \param offset Number of indices to skip from the beggining of the buffer
            ///
            static void drawIndex(GLint primitive, int count, GLint type, int offset = 0);

            /// \brief Deactive the current VBO ( used as index buffer )
            ///
            /// \author Alessandro Ribeiro
            ///
            static void unsetIndex();

            /// \brief Set one vertex attribute from the already uploaded vertex VBO.
            ///
            /// Example:
            ///
            /// \code
            /// struct GLFont2Builder_VertexAttrib {
            ///     vec3 pos;
            ///     vec2 uv;
            ///     vec4 color;
            ///                 /// };
            ///
            /// ...
            /// vbo->setLayout(shader_atribute_color, 4, GL_FLOAT, sizeof(GLFont2Builder_VertexAttrib), sizeof(vec3) + sizeof(vec2));
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            /// \param layoutIndex The layout index used as vertex attribute index in the shader
            /// \param sizeCount Number of components in the structure (vec2=2, vec3=3, ...).
            /// \param type The OpenGL data type. Example: GL_FLOAT
            /// \param strideBytes The size of the structure you are using. Example: sizeof(GLFont2Builder_VertexAttrib)
            /// \param offset The bytes offset inside the main structure. Example: sizeof(vec3) + sizeof(vec2)
            ///
            void setLayout(int layoutIndex, int sizeCount, int type, int strideBytes, int offset);

            /// \brief Deactive the current VBO layout
            ///
            /// \author Alessandro Ribeiro
            /// \param layoutIndex The layout index used as vertex attribute index in the shader
            ///
            static void unsetLayout(int layoutIndex);

            /// \brief Draw VBO using it as a vertex buffer (without index buffer)
            ///
            /// Example:
            ///
            /// \code
            /// vbo->drawArrays(GL_TRIANGLES, count);
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
