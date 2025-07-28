
#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>
#include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        class SpriteShader : public DefaultEngineShader
        {
        private:
            int u_mvp;
            int u_color;
            int u_texture;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;
            int uTexture;

            // std::shared_ptr<OpenGL::VirtualTexture> texture_activated;
    
            ResourceMap *resourceMap;
        public:
            SpriteShader(ResourceMap *resourceMap);

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);
            void setTexture(int texunit);

            // void activateShaderAndSetPropertiesFromBag(
            //     Components::ComponentCamera *camera,
            //     const MathCore::mat4f *mvp,
            //     const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal,
            //     GLRenderState *state,
            //     const Utils::ShaderPropertyBag &bag) override;

            // void deactivateShader(GLRenderState *state) override;

            Utils::ShaderPropertyBag createDefaultBag() const override;


            void ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material)override;
            void setUniformsFromMatrices(
                GLRenderState *state,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv)override;
        };
    }
}
