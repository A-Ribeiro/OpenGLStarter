#ifndef Sphere__H
#define Sphere__H

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
using namespace aRibeiro;
using namespace openglWrapper;
#include <glew/glew.h>


struct SphereVertexAttrib {
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
};


class SphereModel {
public:
    aligned_vector<SphereVertexAttrib> vertices;
    std::vector<unsigned short> indices;
    
    int sectorCount;
    int stackCount;
    float radius;
    
    SphereModel(int sectorCount, int stackCount, float radius) {
        
        this->sectorCount = sectorCount;
        this->stackCount = stackCount;
        this->radius = radius;
        
        
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
                
                // normalized vertex normal (nx, ny, nz)
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                
                // vertex tex coord (s, t) range between [0, 1]
                s = (float)j / sectorCount;
                t = (float)i / stackCount;
                
                vec3 p = vec3(x,z,-y);
                vec3 normal = vec3(nx,nz,-ny);
                vec3 tangent = normalize( cross(vec3(0,1,0),normal) );
                vec3 binormal = normalize( cross(normal, tangent) );
                SphereVertexAttrib va = {
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
    }
    
    void setLayoutPointers(GLint positionLayout, GLint uvLayout) {
        OPENGL_CMD(glEnableVertexAttribArray(positionLayout));
        OPENGL_CMD(glVertexAttribPointer(positionLayout, 3, GL_FLOAT, false, sizeof(SphereVertexAttrib), &vertices[0].position));
        
        OPENGL_CMD(glEnableVertexAttribArray(uvLayout));
        OPENGL_CMD(glVertexAttribPointer(uvLayout, 2, GL_FLOAT, false, sizeof(SphereVertexAttrib), &vertices[0].uv));
    }
    
    void draw() {
        OPENGL_CMD(glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, &indices[0]));
    }
    
    void unsetLayoutPointers(GLint positionLayout, GLint uvLayout) {
        OPENGL_CMD(glDisableVertexAttribArray(positionLayout));
        OPENGL_CMD(glDisableVertexAttribArray(uvLayout));
    }
};


class SphereModelVBO {
    
    //private copy constructores, to avoid copy...
    SphereModelVBO(const SphereModelVBO& v){}
    void operator=(const SphereModelVBO& v){}
    
public:
    GLVertexBufferObject *data;
    GLVertexBufferObject *index;
    int vertexCount;
    int indexCount;
    
    SphereModelVBO(const SphereModel * sphere) {
        
        vertexCount = sphere->vertices.size();
        indexCount = sphere->indices.size();
        
        data = new GLVertexBufferObject();
        index = new GLVertexBufferObject();
        
        data->uploadData((void*)&sphere->vertices[0].position, sizeof(SphereVertexAttrib)*sphere->vertices.size());
        
        index->uploadIndex((void*)&sphere->indices[0], sphere->indices.size()*sizeof(unsigned short));
        
    }
    
    void setLayoutPointers(GLint positionLayout, GLint uvLayout) {
        data->setLayout(positionLayout, 3, GL_FLOAT, sizeof(SphereVertexAttrib), 0);
        data->setLayout(uvLayout, 2, GL_FLOAT, sizeof(SphereVertexAttrib),
                        sizeof(vec3)
                        //offsetof(SphereVertexAttrib, uv)
                        );
        index->setIndex();
    }
    
    void draw() {
        index->drawIndex(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT);
    }
    
    void unsetLayoutPointers(GLint positionLayout, GLint uvLayout) {
        data->unsetLayout(positionLayout);
        data->unsetLayout(uvLayout);
        index->unsetIndex();
    }
    
    virtual ~SphereModelVBO() {
        setNullAndDelete(data);
        setNullAndDelete(index);
    }
    
};

class SphereModelVAO {
    
    //private copy constructores, to avoid copy...
    SphereModelVAO(const SphereModelVAO& v){}
    void operator=(const SphereModelVAO& v){}
    
    //SphereModelVBO *srcVBO;
    
    int indexCount;
    
public:
    
    GLVertexArrayObject *vao;
    
    SphereModelVAO(SphereModelVBO *vbo, GLint positionLayout, GLint uvLayout) {
        //srcVBO = vbo;
        
        vao = new GLVertexArrayObject();
        vao->enable();
        vbo->setLayoutPointers(positionLayout, uvLayout);
        vao->disable();
        vbo->unsetLayoutPointers(positionLayout, uvLayout);
        
        indexCount = vbo->indexCount;
        
    }
    
    void enable() {
        vao->enable();
    }
    
    void draw() {
        vao->drawIndex(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT);
    }
    
    void disable() {
        vao->disable();
    }
    
    virtual ~SphereModelVAO() {
        setNullAndDelete(vao);
    }
    
};


#endif
