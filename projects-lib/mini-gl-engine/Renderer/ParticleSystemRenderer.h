#ifndef __particle_system_renderer__h_
#define __particle_system_renderer__h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>

#include <mini-gl-engine/UnlitPassShader.h>
#include <mini-gl-engine/GLRenderState.h>

#include <mini-gl-engine/ComponentParticleSystem.h>
#include <mini-gl-engine/ParticleSystemShaders.h>

namespace GLEngine {

    class _SSE2_ALIGN_PRE NormalAttributes {
    public:

        aRibeiro::vec3 pos;
        aRibeiro::vec2 uv;
        aRibeiro::vec3 color;
        float size;
        float alpha;

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    class _SSE2_ALIGN_PRE ParticleSystemRenderer {
    public:

        // Debug info
        UnlitPassShader debugLinesShader;
        int debugLinesShader_AttribLocation_Pos;
        aRibeiro::aligned_vector<aRibeiro::vec3> lines;

        // Draw elements
        NormalParticleShader normalShader;
        int normalShader_pos;
        int normalShader_uv;
        int normalShader_color;
        int normalShader_size;
        int normalShader_alpha;

        aRibeiro::aligned_vector<NormalAttributes> normalVertex;

        SoftParticleShader softShader;
        int softShader_pos;
        int softShader_uv;
        int softShader_color;
        int softShader_size;
        int softShader_alpha;
        
        ParticleSystemRenderer();

        void drawDebugPoints( 
            const Components::ComponentCamera* camera,
            const Components::ComponentParticleSystem* particleSystem, 
            float size );

        void draw(const Components::ComponentCamera* camera,
            const Components::ComponentParticleSystem* particleSystem);

        void drawSoftDepthComponent24(const Components::ComponentCamera* camera,
            const Components::ComponentParticleSystem* particleSystem,
            const openglWrapper::GLTexture* depthComponent24);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif