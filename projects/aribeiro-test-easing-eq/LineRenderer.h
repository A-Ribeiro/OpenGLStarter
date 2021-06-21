#ifndef GLLineRenderer_H
#define GLLineRenderer_H

#include <vector>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>

using namespace aRibeiro;
using namespace openglWrapper;


class LineRenderer {
    
public:
    aligned_vector<vec3> data;

    LineRenderer(int initialNumberOfVertices):data(initialNumberOfVertices) {

    }

    void drawLoop(GLShaderColor *shader) {

        const int COORDS_PER_POS = 3;
        const int STRUCTURE_STRIDE_BYTES = sizeof(vec3);

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderColor::vPosition));
        OPENGL_CMD(glVertexAttribPointer(GLShaderColor::vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES, &data[0]));
        OPENGL_CMD(glDrawArrays(GL_LINE_LOOP, 0, data.size()));
        OPENGL_CMD(glDisableVertexAttribArray(GLShaderColor::vPosition));

    }

    void draw(GLShaderColor *shader) {

        const int COORDS_PER_POS = 3;
        const int STRUCTURE_STRIDE_BYTES = sizeof(vec3);

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderColor::vPosition));
        OPENGL_CMD(glVertexAttribPointer(GLShaderColor::vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES, &data[0]));
        OPENGL_CMD(glDrawArrays(GL_LINE_STRIP, 0, data.size()));
        OPENGL_CMD(glDisableVertexAttribArray(GLShaderColor::vPosition));

    }

};


#endif

