#include "RenderSystem.h"

//#include "../ShaderManager.h"

#include <string.h>

RenderSystem *RenderSystem::getSingleton() {

    static RenderSystem result;
    return &result;

}

RenderSystem::RenderSystem() {


    glClearColor(0, 0, 0, 1);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);


    glEnable(GL_DEPTH_TEST);
    //glDisable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);
    glClearDepth(1.0);

    //glDepthMask(GL_TRUE);
    glDepthMask(GL_TRUE);//depth writing enabled

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);//use alpha blend in separate channels
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GREATER, 1.0 / 255.0);
#ifndef ARIBEIRO_RPI
    glDisable(GL_ALPHA_TEST);

    //glEnable(GL_LINE_SMOOTH);
    //glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

    //glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
#endif

    glLineWidth(2.0);
    //glLineWidth(1);

    //wireframe on
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //wireframe off
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //glPolygonMode(GL_BACK, GL_LINE);


    shader = new GLShaderTextureColor();
    shaderVertexColor = new GLShaderVertexColor();


    projection.top = mat4::IdentityMatrix;
    model.top = mat4::IdentityMatrix;

    color = vec4(1.0f);

    //shaderManager = new ShaderManager(this);
}

RenderSystem::~RenderSystem() {
}

void RenderSystem::releaseGLResources() {

    //setNullAndDelete(shaderManager);

    setNullAndDelete(shader);
    setNullAndDelete(shaderVertexColor);
}

/*
void RenderSystem::pushProjetionAndConfigureParallel() {
    projection.push();
    projection.top = translate(-1.0f, -1.0f, 0.0f) * scale(2.0f / dimension.x, 2.0f / dimension.y, 0.0f);
    model.push();
    model.top = mat4::IdentityMatrix;
}

void RenderSystem::pushProjetionAndConfigurePerpective(float aspectX) {
    projection.push();
    projection.top = projection_perspective(60.0f, aspectX, 0.001f, 1000.0f);
    model.push();
    model.top = mat4::IdentityMatrix;
}

void RenderSystem::popProjection()
{
    projection.pop();
    model.pop();
}

void RenderSystem::setupViewport(int w, int h) {

    if (projection.size() > 0)
        printf("Error, stack is not root...\n");

    glViewport(0, 0, w, h);

    projection.top = projection_perspective(60.0f, (double)w / (double)h, 0.001f, 1000.0f);
    model.top = mat4::IdentityMatrix;

    dimension = vec2(w, h);
    dimension_inv.x = 2.0f / (float)w;
    dimension_inv.y = 2.0f / (float)h;

}
*/

mat4 &RenderSystem::getMVP() {
    //mat4 lastmodel, lastprojection, returnedMVP;
    //return projection.top * model.top;

    //
    // computes the MVP if the model and projection matrix are different...
    //
    if ((memcmp(projection.top.array, lastprojection.array, sizeof(mat4)) != 0) ||
        (memcmp(model.top.array, lastmodel.array, sizeof(mat4)) != 0)) {

        returnedMVP = projection.top * model.top;

        lastprojection = projection.top;
        lastmodel = model.top;

        //modelIT = transpose(inv_faster(model.top));
        modelIT = transpose(inv(model.top));
        //modelIT = inv_faster(transpose(model.top));
    }

    return returnedMVP;
}

mat4 &RenderSystem::getModelIT() {
    return modelIT;
}

void RenderSystem::drawAABB_2D(const AABB &aabb, const vec4 &color) {

    vec3 pos[] = {
        vec3(aabb.min_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.max_box.y, 0.0f),

        vec3(aabb.min_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.max_box.y, 0.0f),
        vec3(aabb.min_box.x, aabb.max_box.y, 0.0f)
    };

    vec4 vcolor[] = {
        color,
        color,
        color,
        color,
        color,
        color
    };

    drawColor(GL_TRIANGLES, pos, vcolor, 6);
}


void RenderSystem::drawAABB_Lines(const AABB &aabb, const vec4 &color) {
    vec3 pos[] = {
        vec3(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z),
        vec3(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z),
        vec3(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z), vec3(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z),
        vec3(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z), vec3(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z),

        vec3(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z),
        vec3(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z), vec3(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z),
        vec3(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z), vec3(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z),
        vec3(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z),

        vec3(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z),
        vec3(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z), vec3(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z),
        vec3(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z), vec3(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z),
        vec3(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z), vec3(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z)
    };

    vec4 vcolor[] = {
        color,color,color,color,color,color,color,color,
        color,color,color,color,color,color,color,color,
        color,color,color,color,color,color,color,color
    };

    drawColor(GL_LINES, pos, vcolor, 24);
}

void RenderSystem::drawAABB_Cube(const AABB &aabb, const vec4 &color) {

    exit(-1);

/*
    vec3 pos[] = {
        vec3(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z), vec3(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z),
        vec3(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z), vec3(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z),

        vec3(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z), vec3(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z),
        vec3(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z), vec3(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z),

        vec3(aabb.min_box.x, aabb.min_box.y, aabb.min_box.z), vec3(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z),
        vec3(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z), vec3(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z),

        vec3(aabb.min_box.x, aabb.max_box.y, aabb.min_box.z), vec3(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z),
        vec3(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z), vec3(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z),

        vec3(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z), vec3(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z),
        vec3(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z), vec3(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z),

        vec3(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z), vec3(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z),
        vec3(aabb.min_box.x, aabb.max_box.y, aabb.min_box.z), vec3(aabb.min_box.x, aabb.min_box.y, aabb.min_box.z)
    };

    vec4 vcolor[] = {
        color,color,color,color,color,color,color,color,
        color,color,color,color,color,color,color,color,
        color,color,color,color,color,color,color,color
    };

    drawColor(GL_QUADS, pos, vcolor, 24);
    */
}

void RenderSystem::drawTexture_center(GLTexture *texture, const vec2 &p) {

    vec2 center = vec2((float)texture->width, (float)texture->height) * 0.5f;

    vec3 vpos[] = {
        vec3(-center.x + p.x, -center.y + p.y, 0.0f),
        vec3(center.x + p.x, -center.y + p.y, 0.0f),
        vec3(center.x + p.x,  center.y + p.y, 0.0f),

        vec3(-center.x + p.x, -center.y + p.y, 0.0f),
        vec3(center.x + p.x,  center.y + p.y, 0.0f),
        vec3(-center.x + p.x,  center.y + p.y, 0.0f)
    };

    const vec2 vuv[] = {
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 1),
        vec2(1, 0),
        vec2(0, 0)
    };

    drawTexture(texture, GL_TRIANGLES, vpos, vuv, 6);

}

void RenderSystem::drawTexture(GLTexture *texture, const AABB &aabb) {

    vec3 vpos[] = {

        vec3(aabb.min_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.max_box.y, 0.0f),

        vec3(aabb.min_box.x, aabb.min_box.y, 0.0f),
        vec3(aabb.max_box.x, aabb.max_box.y, 0.0f),
        vec3(aabb.min_box.x, aabb.max_box.y, 0.0f)
    };

    vec2 vuv[] = {
        vec2(0, 1),
        vec2(1, 1),
        vec2(1, 0),

        vec2(0, 1),
        vec2(1, 0),
        vec2(0, 0)
    };

    drawTexture(texture, GL_TRIANGLES, vpos, vuv, 6);
}

void RenderSystem::drawTexture(GLTexture *texture, const vec4 &color, GLuint oglPrimitive, const vec3 *vertexBuffer, const vec2 *uvBuffer, int count) {
    shader->enable();

    shader->setMatrix(getMVP());
    shader->setColor(color);

    texture->active(0);
    shader->setTexture(0);

    //
    // Set the vertex position attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shader->vPosition));

    OPENGL_CMD(glVertexAttribPointer(shader->vPosition, 3, GL_FLOAT, false, sizeof(vec3), &vertexBuffer[0]));

    //
    // Set the vertex uv attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shader->vUV));
    OPENGL_CMD(glVertexAttribPointer(shader->vUV, 2, GL_FLOAT, false, sizeof(vec2), &uvBuffer[0]));

    //
    // Draw quad
    //
    OPENGL_CMD(glDrawArrays(oglPrimitive, 0, count));

    //
    // Disable arrays after draw
    //
    OPENGL_CMD(glDisableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glDisableVertexAttribArray(shader->vUV));
}

void RenderSystem::drawSphere(GLTexture *texture, int sectorCount, int stackCount, float radius) {

    //std::vector<vec3> lines;
    //std::vector<vec4> colors;

    aligned_vector<VertexAttrib> vertices;
    std::vector<unsigned short> indices;

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            //vertices.push_back(x);
            //vertices.push_back(y);
            //vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            //normals.push_back(nx);
            //normals.push_back(ny);
            //normals.push_back(nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            //texCoords.push_back(s);
            //texCoords.push_back(t);

            vec3 p = vec3(x,z,-y);
            vec3 normal = vec3(nx,nz,-ny);

            vec3 tangent = normalize( cross(vec3(0,1,0),normal) );
            vec3 binormal = normalize( cross(normal, tangent) );

            /*
            //draw normal
            lines.push_back(p);
            lines.push_back(p+normal);
            colors.push_back(vec4(0.5,0.5,1.0,1.0));
            colors.push_back(vec4(0.5,0.5,1.0,1.0));
            //draw tangent
            lines.push_back(p);
            lines.push_back(p+tangent);
            colors.push_back(vec4(0.5,1.0,0.5,1.0));
            colors.push_back(vec4(0.5,1.0,0.5,1.0));
            */


            VertexAttrib va = {
                p,// position;
                vec2(s,t),// uv;
                normal,// normal;
                tangent,// tangent;
                binormal//vec3(0),// binormal;
            };

            vertices.push_back(va);
        }
    }

    int k1, k2;
    int i = 0;

    //
    // Fix tangent and binormal from poles
    //
    k1 = i * (sectorCount + 1);     // beginning of current stack
    k2 = k1 + sectorCount + 1;      // beginning of next stack

    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
    {
        // k1+1 => k2 => k2+1
        int index1 = k1 + 1;
        int index2 = k2;
        int index3 = k2 + 1;

        vertices[index1].tangent = normalize( vertices[index2].tangent + vertices[index3].tangent );
        vertices[index1].binormal = normalize( cross( vertices[index1].normal, vertices[index1].tangent ) );
        /*
        //draw tangent
        lines.push_back(vertices[index1].position);
        lines.push_back(vertices[index1].position+vertices[index1].tangent);
        colors.push_back(vec4(1.0,0.5,0.5,1.0));
        colors.push_back(vec4(1.0,0.5,0.5,1.0));
        */

    }

    i = (stackCount - 1);
    k1 = i * (sectorCount + 1);     // beginning of current stack
    k2 = k1 + sectorCount + 1;      // beginning of next stack

    for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
    {
        // k1+1 => k2 => k2+1
        int index1 = k1;
        int index2 = k2;
        int index3 = k1 + 1;

        vertices[index2].tangent = normalize( vertices[index1].tangent + vertices[index3].tangent );
        vertices[index2].binormal = normalize( cross( vertices[index2].normal, vertices[index2].tangent ) );

        /*
        //draw tangent
        lines.push_back(vertices[index2].position);
        lines.push_back(vertices[index2].position+vertices[index2].tangent);
        colors.push_back(vec4(1.0,0.5,0.5,1.0));
        colors.push_back(vec4(1.0,0.5,0.5,1.0));
        */
    }


    for (i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    //
    // OpenGL Draw
    //

    shader->enable();
    shader->setMatrix(getMVP());
    texture->active(0);
    shader->setTexture(0);
    shader->setColor(color);

    OPENGL_CMD(glEnableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glVertexAttribPointer(shader->vPosition, 3, GL_FLOAT, false, sizeof(VertexAttrib), &vertices[0].position));

    OPENGL_CMD(glEnableVertexAttribArray(shader->vUV));
    OPENGL_CMD(glVertexAttribPointer(shader->vUV, 2, GL_FLOAT, false, sizeof(VertexAttrib), &vertices[0].uv));

    //GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
    OPENGL_CMD(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]));

    //OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));

    OPENGL_CMD(glDisableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glDisableVertexAttribArray(shader->vUV));

    //draw lines
    //drawColor(GL_LINES, &lines[0], &colors[0], lines.size());


}

void RenderSystem::drawTexture(GLTexture *texture, GLuint oglPrimitive, const vec3 *vertexBuffer, const vec2 *uvBuffer, int count) {

    shader->enable();

    shader->setMatrix(getMVP());

    texture->active(0);
    shader->setTexture(0);

    shader->setColor(color);

    //
    // Set the vertex position attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shader->vPosition));

    OPENGL_CMD(glVertexAttribPointer(shader->vPosition, 3, GL_FLOAT, false, sizeof(vec3), &vertexBuffer[0]));

    //
    // Set the vertex uv attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shader->vUV));
    OPENGL_CMD(glVertexAttribPointer(shader->vUV, 2, GL_FLOAT, false, sizeof(vec2), &uvBuffer[0]));

    //
    // Draw quad
    //
    OPENGL_CMD(glDrawArrays(oglPrimitive, 0, count));

    //
    // Disable arrays after draw
    //
    OPENGL_CMD(glDisableVertexAttribArray(shader->vPosition));
    OPENGL_CMD(glDisableVertexAttribArray(shader->vUV));


}


void RenderSystem::drawColor(GLuint oglPrimitive, const vec3 *vertexBuffer, const vec4 *colorBuffer, int count) {


    shaderVertexColor->enable();

    shaderVertexColor->setMatrix(getMVP());

    //
    // Set the vertex position attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shaderVertexColor->vPosition));
    OPENGL_CMD(glVertexAttribPointer(shaderVertexColor->vPosition, 3, GL_FLOAT, false, sizeof(vec3), &vertexBuffer[0]));

    //
    // Set the vertex color attrib array
    //
    OPENGL_CMD(glEnableVertexAttribArray(shaderVertexColor->vColor));
    OPENGL_CMD(glVertexAttribPointer(shaderVertexColor->vColor, 4, GL_FLOAT, false, sizeof(vec4), &colorBuffer[0]));

    //
    // Draw quad
    //
    OPENGL_CMD(glDrawArrays(oglPrimitive, 0, count));

    //
    // Disable arrays after draw
    //
    OPENGL_CMD(glDisableVertexAttribArray(shaderVertexColor->vPosition));
    OPENGL_CMD(glDisableVertexAttribArray(shaderVertexColor->vColor));



}


void RenderSystem::print(GLFont *font, const wchar_t *txt, const vec2 &position, const vec4 &color, float alignx, float aligny, float size) {

    AABB aabb = font->computeBounds(txt);

    vec3 center = vec3(0);

    center.x = -aabb.max_box.x + lerp(0.0f, aabb.max_box.x - aabb.min_box.x, 1.0f - alignx);
    center.y = -aabb.max_box.y + lerp(0.0f, aabb.max_box.y - aabb.min_box.y, 1.0f - aligny);


    GLShaderFont *shaderfont = font->getShader();
    shaderfont->enable();
    shaderfont->setMatrix(
        getMVP() *
        translate(vec3(position, 0.0f)) *
        scale(vec3(size)) *
        translate(center)
    );

    font->print(color, txt);

}

void RenderSystem::print(GLFont *font, const char *txt, const vec2 &position, const vec4 &color, float alignx, float aligny, float size) {
    std::string text(txt);
    std::wstring wText(text.begin(), text.end());

    print(font, wText.c_str(), position, color, alignx, aligny, size);

}


void RenderSystem::print(GLFont *font, const wchar_t *txt, const vec2 &position, const vec4 &color) {

    GLShaderFont *shaderfont = font->getShader();
    shaderfont->enable();
    shaderfont->setMatrix(
        getMVP()
    );

    font->print(color, position, txt);

}

void RenderSystem::print(GLFont *font, const char *txt, const vec2 &position, const vec4 &color) {

    std::string text(txt);
    std::wstring wText(text.begin(), text.end());

    print(font, wText.c_str(), position, color);
}


void RenderSystem::print(GLFont *font, const wchar_t *txt, const vec2 &position, const vec4 &color, float size) {

    GLShaderFont *shaderfont = font->getShader();
    shaderfont->enable();
    shaderfont->setMatrix(
        getMVP() *
        translate(vec3(position, 0.0f)) *
        scale(vec3(size))
    );

    font->print(color, txt);
}

void RenderSystem::print(GLFont *font, const char *txt, const vec2 &position, const vec4 &color, float size) {
    std::string text(txt);
    std::wstring wText(text.begin(), text.end());

    print(font, wText.c_str(), position, color, size);
}
