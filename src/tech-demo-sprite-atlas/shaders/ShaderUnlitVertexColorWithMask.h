#pragma once

#include <appkit-gl-engine/DefaultEngineShader.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {
            class ComponentRectangle;
        }

        class ShaderUnlitVertexColorWithMask : public DefaultEngineShader
        {
            int u_mvp;
            int u_color;

            int u_transform_to_mask;
            int u_mask_corner;
            int u_mask_radius;

            MathCore::mat4f uMVP;
            MathCore::vec4f uColor;

            MathCore::mat4f uTransformToMask;
            MathCore::vec2<float, MathCore::SIMD_TYPE::NONE> uMask_corner[4];
            MathCore::vec4f uMask_radius;

            Components::ComponentRectangle *componentRectangle;
            Transform *componentRectangle_transform;

        public:
            friend class AppKit::GLEngine::Components::ComponentRectangle;

            ShaderUnlitVertexColorWithMask();

            void setMVP(const MathCore::mat4f &mvp);
            void setColor(const MathCore::vec4f &color);

            Utils::ShaderPropertyBag createDefaultBag() const override;

            void ActiveShader_And_SetUniformsFromMaterial(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material) override;
            void setUniformsFromMatrices(
                GLRenderState *state,
                ResourceMap *resourceMap,
                RenderPipeline *renderPipeline,
                Components::ComponentMaterial *material,
                Transform *element,
                Components::ComponentCamera *camera,
                const MathCore::mat4f *mvp,
                const MathCore::mat4f *mv,
                const MathCore::mat4f *mvIT,
                const MathCore::mat4f *mvInv) override;
        };

    }
}
