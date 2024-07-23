#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-base/GLTexture.h>

#include <appkit-gl-engine/shaders/UnlitPassShader.h>
#include <appkit-gl-engine/GL/GLRenderState.h>

#include <appkit-gl-engine/Components/ComponentParticleSystem.h>
#include <appkit-gl-engine/shaders/ParticleSystemShaders.h>

#include <appkit-gl-engine/Components/ComponentCamera.h>


namespace AppKit
{
    namespace GLEngine
    {

        class NormalAttributes
        {
        public:
            MathCore::vec3f pos;
            MathCore::vec2f uv;
            MathCore::vec3f color;
            float size;
            float alpha;
        };

        class ParticleSystemRenderer
        {
        public:
            // Debug info
            UnlitPassShader debugLinesShader;
            int debugLinesShader_AttribLocation_Pos;
            std::vector<MathCore::vec3f> lines;

            // Draw elements
            NormalParticleShader normalShader;
            int normalShader_pos;
            int normalShader_uv;
            int normalShader_color;
            int normalShader_size;
            int normalShader_alpha;

            std::vector<NormalAttributes> normalVertex;

            SoftParticleShader softShader;
            int softShader_pos;
            int softShader_uv;
            int softShader_color;
            int softShader_size;
            int softShader_alpha;

            ParticleSystemRenderer();

            void drawDebugPoints(
                std::shared_ptr<Components::ComponentCamera> camera,
                std::shared_ptr<Components::ComponentParticleSystem> particleSystem,
                float size);

            void draw(std::shared_ptr<Components::ComponentCamera> camera,
                      std::shared_ptr<Components::ComponentParticleSystem> particleSystem);

            void drawSoftDepthComponent24(std::shared_ptr<Components::ComponentCamera> camera,
                                          std::shared_ptr<Components::ComponentParticleSystem> particleSystem,
                                          AppKit::OpenGL::GLTexture *depthComponent24);
        };

    }

}
