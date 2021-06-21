#ifndef GLPointRenderer_H
#define GLPointRenderer_H

#include <vector>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;


class PointRenderer {

    aligned_vector<vec3> triangles;

public:

    aligned_vector<vec3> data;

    PointRenderer(int initialNumberOfVertices) :data(initialNumberOfVertices) {

    }

    void draw(GLShaderColor *shader) {

        const int COORDS_PER_POS = 3;
        const int STRUCTURE_STRIDE_BYTES = sizeof(vec3);

        triangles.clear();
        for(int i=0;i<data.size();i++){
            triangles.push_back( data[i] + vec3(0,0.01f,0) );
            triangles.push_back( data[i] + vec3(-0.01f,-0.01f,0));
            triangles.push_back( data[i] + vec3(0.01f,-0.01f,0));
        }

        OPENGL_CMD(glEnableVertexAttribArray(GLShaderColor::vPosition));
        OPENGL_CMD(glVertexAttribPointer(GLShaderColor::vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES, &triangles[0]));
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, triangles.size()));
        OPENGL_CMD(glDisableVertexAttribArray(GLShaderColor::vPosition));

    }

};


#endif
