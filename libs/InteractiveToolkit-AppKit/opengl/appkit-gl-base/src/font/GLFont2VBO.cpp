#include <appkit-gl-base/font/GLFont2VBO.h>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {

        GLFont2VBO::GLFont2VBO()
        {
            count = 0;
        }

        void GLFont2VBO::uploadData(const GLFont2Builder *fontBuilder)
        {
            uploadData(fontBuilder->vertexAttrib);
        }

        void GLFont2VBO::uploadData(const std::vector<GLFont2Builder_VertexAttrib> &vertexAttrib)
        {
            // need to be at least 3 vertex in this data to be possible to draw it...
            if (vertexAttrib.size() < 3)
            {
                count = 0;
                return;
            }
            vbo.uploadData(&vertexAttrib[0], sizeof(GLFont2Builder_VertexAttrib) * (int)vertexAttrib.size(), true);
            count = (int)vertexAttrib.size();
        }

        void GLFont2VBO::draw(int aPosition, int aUV, int aColor)
        {
            if (count == 0)
                return;

            vbo.setLayout(aPosition, 3, GL_FLOAT, sizeof(GLFont2Builder_VertexAttrib), 0);
            vbo.setLayout(aUV, 2, GL_FLOAT, sizeof(GLFont2Builder_VertexAttrib), sizeof(MathCore::vec3f));
            vbo.setLayout(aColor, 4, GL_FLOAT, sizeof(GLFont2Builder_VertexAttrib), sizeof(MathCore::vec3f) + sizeof(MathCore::vec2f));

            vbo.drawArrays(GL_TRIANGLES, count);

            vbo.unsetLayout(aPosition);
            vbo.unsetLayout(aUV);
            vbo.unsetLayout(aColor);
        }

    }

}
