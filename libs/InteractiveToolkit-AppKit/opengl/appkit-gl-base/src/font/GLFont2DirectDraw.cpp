#include <appkit-gl-base/font/GLFont2DirectDraw.h>

// #include <appkit-gl-base/opengl-wrapper.h>

namespace AppKit
{

    namespace OpenGL
    {

        void GLFont2DirectDraw::draw(const GLFont2Builder *fontBuilder, int aPosition, int aUV, int aColor)
        {
            if (fontBuilder->vertexAttrib.size() == 0)
                return;

            OPENGL_CMD(glEnableVertexAttribArray(aPosition));
            OPENGL_CMD(glVertexAttribPointer(aPosition, 3, GL_FLOAT, false, sizeof(GLFont2Builder_VertexAttrib), &fontBuilder->vertexAttrib[0].pos));

            OPENGL_CMD(glEnableVertexAttribArray(aUV));
            OPENGL_CMD(glVertexAttribPointer(aUV, 2, GL_FLOAT, false, sizeof(GLFont2Builder_VertexAttrib), &fontBuilder->vertexAttrib[0].uv));

            OPENGL_CMD(glEnableVertexAttribArray(aColor));
            OPENGL_CMD(glVertexAttribPointer(aColor, 4, GL_FLOAT, false, sizeof(GLFont2Builder_VertexAttrib), &fontBuilder->vertexAttrib[0].color));

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, (GLsizei)fontBuilder->vertexAttrib.size()));

            OPENGL_CMD(glDisableVertexAttribArray(aPosition));
            OPENGL_CMD(glDisableVertexAttribArray(aUV));
            OPENGL_CMD(glDisableVertexAttribArray(aColor));
        }
    }

}
