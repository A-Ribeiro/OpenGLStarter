#ifndef GLFont2_VBO__H
#define GLFont2_VBO__H

#include <opengl-wrapper/GLFont2Builder.h>

namespace openglWrapper {

    /// \brief Draw the font vertex information through a VBO
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLFont2VBO {
    public:
        GLVertexBufferObject vbo;///< current VBO
        size_t count;///< current vertex count in the vbo

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
        void uploadData(const aRibeiro::aligned_vector<GLFont2Builder_VertexAttrib> &vertexAttrib);

        /// \brief Draw this vbo
        ///
        /// \author Alessandro Ribeiro
        /// \param aPosition the shader vertex attribute layout for position
        /// \param aUV the shader vertex attribute layout for uv
        /// \param aColor the shader vertex attribute layout for color
        ///
        void draw(int aPosition, int aUV, int aColor);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
