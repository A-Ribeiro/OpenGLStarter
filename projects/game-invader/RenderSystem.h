#ifndef RenderSystem__H
#define RenderSystem__H

#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include <wchar.h>

#include <glew/glew.h>

#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
using namespace aRibeiro::collision;

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <stdio.h>

#include "GameResources.h"

class RenderSystem {

private:

    // for cache MVP calculation
    mat4 lastmodel, lastprojection, returnedMVP;

public:

    GameResources *gameResources;

    GLShaderTextureColor *shader;
    GLShaderVertexColor *shaderVertexColor;

    TransformStack<mat4> projection;
    TransformStack<mat4> model;

    vec2 dimension,
        dimension_inv;

    RenderSystem(GameResources *gameResources);
    virtual ~RenderSystem();

    void pushProjetionAndConfigureParallel();

    void pushProjetionAndConfigurePerpective(float aspectX);

    void popProjection();

    void setupViewport(int w, int h);

    mat4 &getMVP();

    void drawTexture(GLTexture *texture, const AABB &aabb);

    void drawTexture(GLTexture *texture, GLuint oglPrimitive, const vec3 *vertexBuffer, const vec2 *uvBuffer, int count);

    void drawColor(GLuint oglPrimitive, const vec3 *vertexBuffer, const vec4 *colorBuffer, int count);

    void drawAABB(const AABB &aabb, const vec4 &color);

    void print(const wchar_t *txt, const vec2 &position, const vec4 &color, float alignx = 0.5f, float aligny = 0.5f);

    SSE2_CLASS_NEW_OPERATOR
};


#endif
