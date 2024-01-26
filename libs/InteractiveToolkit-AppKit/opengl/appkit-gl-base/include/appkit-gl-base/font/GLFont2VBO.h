#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/font/GLFont2Builder.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Draw the font vertex information through a VBO
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFont2VBO
        {
        public:
            GLVertexBufferObject vbo; ///< current VBO
            size_t count;             ///< current vertex count in the vbo

            GLFont2VBO();

            /// \brief Upload the font builder to the vbo
            ///
            /// \author Alessandro Ribeiro
            /// \param fontBuilder the current build to draw
            ///
            void uploadData(const GLFont2Builder *fontBuilder);

            /// \brief Upload the vertex list to the vbo
            ///
            /// \author Alessandro Ribeiro
            /// \param vertexAttrib the vertex list to draw
            ///
            void uploadData(const std::vector<GLFont2Builder_VertexAttrib> &vertexAttrib);

            /// \brief Draw this vbo
            ///
            /// \author Alessandro Ribeiro
            /// \param aPosition the shader vertex attribute layout for position
            /// \param aUV the shader vertex attribute layout for uv
            /// \param aColor the shader vertex attribute layout for color
            ///
            void draw(int aPosition, int aUV, int aColor);
        };

    }

}