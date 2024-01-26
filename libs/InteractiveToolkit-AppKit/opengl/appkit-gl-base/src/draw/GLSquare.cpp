#include <appkit-gl-base/draw/GLSquare.h>

#include <appkit-gl-base/platform/PlatformGL.h>

namespace AppKit
{

    namespace OpenGL
    {

        // short drawOrder[6] = { 0, 1, 2, 0, 2, 3 }; // order to draw vertices

        GLSquare::GLSquare()
        {
        }

        void GLSquare::draw(GLShaderColor *shader)
        {

            const int COORDS_PER_POS = 3;
            const int STRUCTURE_STRIDE_BYTES_POS = COORDS_PER_POS * sizeof(float);

            const int VERTEX_COUNT = 4;

            const float vertexBuffer[12] = {
                -1.0f, 1.0f, 0.0f,  // top left
                -1.0f, -1.0f, 0.0f, // bottom left
                1.0f, -1.0f, 0.0f,  // bottom right
                1.0f, 1.0f, 0.0f};  // top right

            //
            // Set the vertex position attrib array
            //
            OPENGL_CMD(glEnableVertexAttribArray(GLShaderColor::vPosition));
            OPENGL_CMD(glVertexAttribPointer(GLShaderColor::vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES_POS, vertexBuffer));

            //
            // Draw quad
            //
            OPENGL_CMD(glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT));

            //
            // Disable arrays after draw
            //
            OPENGL_CMD(glDisableVertexAttribArray(GLShaderTextureColor::vPosition));
        }

        void GLSquare::draw(GLShaderTextureColor *shader)
        {

            const int COORDS_PER_POS = 3;
            const int STRUCTURE_STRIDE_BYTES_POS = COORDS_PER_POS * sizeof(float);
            const int COORDS_PER_UV = 2;
            const int STRUCTURE_STRIDE_BYTES_UV = COORDS_PER_UV * sizeof(float);

            const int VERTEX_COUNT = 4;

            const float vertexBuffer[12] = {
                -1.0f, 1.0f, 0.0f,  // top left
                -1.0f, -1.0f, 0.0f, // bottom left
                1.0f, -1.0f, 0.0f,  // bottom right
                1.0f, 1.0f, 0.0f};  // top right

            const float uvBuffer[8] = {
                0, 0,  // top left
                0, 1,  // bottom left
                1, 1,  // bottom right
                1, 0}; // top right

            //
            // Set the vertex position attrib array
            //
            OPENGL_CMD(glEnableVertexAttribArray(GLShaderTextureColor::vPosition));
            OPENGL_CMD(glVertexAttribPointer(GLShaderTextureColor::vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES_POS, vertexBuffer));

            //
            // Set the vertex uv attrib array
            //
            OPENGL_CMD(glEnableVertexAttribArray(GLShaderTextureColor::vUV));
            OPENGL_CMD(glVertexAttribPointer(GLShaderTextureColor::vUV, COORDS_PER_UV, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES_UV, uvBuffer));

            //
            // Draw quad
            //
            OPENGL_CMD(glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT));

            //
            // Disable arrays after draw
            //
            OPENGL_CMD(glDisableVertexAttribArray(GLShaderTextureColor::vPosition));
            OPENGL_CMD(glDisableVertexAttribArray(GLShaderTextureColor::vUV));
        }

    }

}