
#ifndef commonDefinition_h

#define commonDefinition_h


#include "common/RenderSystem.h"

void drawCube() {

    RenderSystem *render = RenderSystem::getSingleton();
    ShaderManager *shaderManager = render->shaderManager;

    /*
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    */
    static VertexAttrib buffer[] = {

        // frente - OK
        {vec3(-1,-1,1),vec2(0,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1,-1,1),vec2(1,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,1),vec2(1,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},

        {vec3(-1,-1,1),vec2(0,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,1),vec2(1,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(-1, 1,1),vec2(0,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},

        // tras - OK
        {vec3(1,-1,-1),vec2(0,1),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},
        {vec3(-1, -1,-1),vec2(1,1),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},
        {vec3(-1, 1,-1),vec2(1,0),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},

        {vec3(1,-1,-1),vec2(0,1),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},
        {vec3(-1, 1,-1),vec2(1,0),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},
        {vec3(1,1,-1),vec2(0,0),vec3(0,0,-1),vec3(-1,0,0),vec3(0,1,0)},

        //direita - OK
        {vec3(1, -1, 1),vec2(0,1),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},
        {vec3(1, -1, -1),vec2(1,1),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},
        {vec3(1, 1, -1),vec2(1,0),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},

        {vec3(1, -1, 1),vec2(0,1),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},
        {vec3(1, 1, -1),vec2(1,0),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},
        {vec3(1, 1, 1),vec2(0,0),vec3(1,0,0),vec3(0,0,-1),vec3(0,-1,0)},

        //esquerda ok
        {vec3(-1, -1, -1),vec2(0,1),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},
        {vec3(-1, -1, 1),vec2(1,1),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},
        {vec3(-1, 1, 1),vec2(1,0),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},

        {vec3(-1, -1, -1),vec2(0,1),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},
        {vec3(-1, 1, 1),vec2(1,0),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},
        {vec3(-1, 1,-1),vec2(0,0),vec3(-1,0,0),vec3(0,0,1),vec3(0,1,0)},

        //baixo
        {vec3(-1, -1, -1),vec2(0,1),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},
        {vec3(1, -1,-1),vec2(1,1),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},
        {vec3(1, -1, 1),vec2(1,0),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},

        {vec3(-1, -1, -1),vec2(0,1),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},
        {vec3(1, -1, 1),vec2(1,0),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},
        {vec3(-1, -1, 1),vec2(0,0),vec3(0,-1,0),vec3(1,0,0),vec3(0,0,1)},

        //cima
        {vec3(-1, 1, 1),vec2(0,1),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)},
        {vec3(1, 1, 1),vec2(1,1),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)},
        {vec3(1, 1,-1),vec2(1,0),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)},

        {vec3(-1, 1, 1),vec2(0,1),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)},
        {vec3(1, 1,-1),vec2(1,0),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)},
        {vec3(-1, 1, -1),vec2(0,0),vec3(0,1,0),vec3(1,0,0),vec3(0,0,-1)}

    };

    static bool runonce = true;

    if (runonce) {
        //fix tangent
        for (int i = 0; i < 36; i += 6)
        {
            vec3 t = normalize(buffer[i + 1].position - buffer[i + 0].position);
            //vec3 t = buffer[i + 0].tangent;
            vec3 n = buffer[i + 0].normal;
            //vec3 b = normalize(buffer[i + 0].position - buffer[i + 3].position);
            vec3 b = cross(n,t);

            ARIBEIRO_ABORT(
                (distance(buffer[i + 0].normal, n) > 0.002f),
                "Error with TBN computation\n");

            //if (distance(buffer[i + 0].normal, n) > 0.002f) {
            //    printf("Error with TBN computation(%s)...\n", __FILE__);
            //    exit(-1);
            //}

            for (int j = 0; j < 6; j++) {
                buffer[i + j].normal = n;
                buffer[i + j].tangent = t;
                buffer[i + j].binormal = b;
            }
        }

        runonce = false;
    }



    shaderManager->setupShaderParametersAndDraw(GL_TRIANGLES, buffer, 36);

}


void drawXYplane(int div) {

    RenderSystem *render = RenderSystem::getSingleton();
    ShaderManager *shaderManager = render->shaderManager;

    static aligned_vector<VertexAttrib> buffer;
    buffer.clear();

    const vec3 normal(0, 0, 1);
    const vec3 tangent(1, 0, 0);
    const vec3 binormal(0, 1, 0);

    static vec2 uv;
    static vec3 pos;

    float fator = 1.0f / (float)div;
    for (int j = 0; j < div; j++) {
        for (int i = 0; i < div; i++) {

            float iF = (float)i;
            float jF = (float)j;

            uv = vec2(jF*fator, iF*fator);
            pos = vec3(-1.0f + fator + -fator + jF * fator * 2.0f, -1.0f + fator + -fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }

            uv = vec2((jF + 1.0f)*fator, (iF)*fator);
            pos = vec3(-1.0f +fator + fator + jF * fator * 2.0f, -1.0f + fator + -fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }

            uv = vec2((jF + 1.0f)*fator, (iF + 1.0f)*fator);
            pos = vec3(-1.0f +fator + fator + jF * fator * 2.0f, -1.0f + fator + fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }




            uv = vec2(jF*fator, iF*fator);
            pos = vec3(-1.0f + fator + -fator + jF * fator * 2.0f, -1.0f + fator + -fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }


            uv = vec2((jF + 1.0f)*fator, (iF + 1.0f)*fator);
            pos = vec3(-1.0f +fator + fator + jF * fator * 2.0f, -1.0f + fator + fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }

            uv = vec2((jF)*fator, (iF + 1.0f)*fator);
            pos = vec3(-1.0f + fator + -fator + jF * fator * 2.0f , -1.0f + fator + fator + iF * fator * 2.0f, 0);

            {
                uv.y = 1.0f - uv.y;
                VertexAttrib vattrib = { pos,uv,normal,tangent,binormal };
                buffer.push_back(vattrib);
            }
        }
    }

    shaderManager->setupShaderParametersAndDraw(GL_TRIANGLES, &buffer[0], buffer.size());

}


void drawXYplane() {
    RenderSystem *render = RenderSystem::getSingleton();
    ShaderManager *shaderManager = render->shaderManager;

    /*
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    */
    const VertexAttrib buffer[] = {
        {vec3(-1,-1,0),vec2(0,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1,-1,0),vec2(1,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,0),vec2(1,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},


        {vec3(-1,-1,0),vec2(0,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,0),vec2(1,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(-1, 1,0),vec2(0,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)}
    };

    shaderManager->setupShaderParametersAndDraw(GL_TRIANGLES, buffer, 6);

}

void drawNormalizedMappedQuad() {

    RenderSystem *render = RenderSystem::getSingleton();
    ShaderManager *shaderManager = render->shaderManager;

    /*
    vec3 position;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    */
    const VertexAttrib buffer[] = {
        {vec3(-1,-1,0),vec2(0,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1,-1,0),vec2(1,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,0),vec2(1,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},

        {vec3(-1,-1,0),vec2(0,0),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(1, 1,0),vec2(1,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)},
        {vec3(-1, 1,0),vec2(0,1),vec3(0,0,1),vec3(1,0,0),vec3(0,1,0)}
    };

    shaderManager->setupShaderParametersAndDraw(GL_TRIANGLES, buffer, 6);

}

#endif
