// #include <appkit-gl-engine/shaders/ShaderUnlitVertexColorWithMask.h>
#include "./ShaderUnlitVertexColorWithMask.h"
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

#include "../components/ui/ComponentRectangle.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

#include "./MaskCommon.h"

namespace AppKit
{
    namespace GLEngine
    {

        ShaderUnlitVertexColorWithMask::ShaderUnlitVertexColorWithMask()
        {
            componentRectangle = nullptr;
            componentRectangle_transform = nullptr;

            format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_COLOR0;

            const char vertexShaderCode[] = {
                SHADER_HEADER_120
                "attribute vec4 aPosition;\n"
                "attribute vec4 aColor0;\n"
                "uniform mat4 uMVP;\n"
                "varying vec4 color;\n"
                "void main() {\n"
                "  color = aColor0;\n"
                "  gl_Position = uMVP * aPosition;\n"
                "}"};

            const char fragmentShaderCode[] = {
                SHADER_HEADER_120
                "varying vec4 color;\n"
                "uniform vec4 uColor;\n"

                MASKSHADER_FRAGMENT_UNIFORM

                MASKSHADER_COMPUTE_MASK_FUNCTION

                "void main() {\n"
                "  float mask = compute_mask();\n"
                "  if (mask <= 0.0)\n"
                "    discard;\n"
                "  vec4 result = color * uColor;\n"
                "  result.a *= mask;\n"
                "  gl_FragColor = result;\n"
                "}"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            DefaultEngineShader::setupAttribLocation();
            link(__FILE__, __LINE__);

            u_mvp = getUniformLocation("uMVP");
            u_color = getUniformLocation("uColor");

            u_transform_to_mask = getUniformLocation("uTransformToMask");
            u_mask_corner = getUniformLocation("uMask_corner");
            u_mask_radius = getUniformLocation("uMask_radius");

            uMVP = MathCore::mat4f();
            uColor = MathCore::vec4f(1.0, 1.0, 1.0, 1.0);

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = this;

            setUniform(u_mvp, uMVP);
            setUniform(u_color, uColor);

            setUniform(u_transform_to_mask, uTransformToMask);
            setUniform(u_mask_radius, uMask_radius);
            // array uniform upload
            if (u_mask_corner >= 0)
                OPENGL_CMD(glUniform2fv(u_mask_corner, (GLsizei)4, uMask_corner[0].array));

            state->CurrentShader = nullptr;
        }

        void ShaderUnlitVertexColorWithMask::setMVP(const MathCore::mat4f &mvp)
        {
            if (uMVP != mvp)
            {
                uMVP = mvp;
                setUniform(u_mvp, uMVP);
            }
        }
        void ShaderUnlitVertexColorWithMask::setColor(const MathCore::vec4f &color)
        {
            if (uColor != color)
            {
                uColor = color;
                setUniform(u_color, uColor);
            }
        }

        Utils::ShaderPropertyBag ShaderUnlitVertexColorWithMask::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("uColor", uColor);
            bag.addProperty("BlendMode", (int)AppKit::GLEngine::BlendModeDisabled);

            bag.addProperty("ComponentRectangle", std::weak_ptr<Component>());

            return bag;
        }

        void ShaderUnlitVertexColorWithMask::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
            const auto &materialBag = material->property_bag;
            state->CurrentShader = this;

            state->BlendMode = (AppKit::GLEngine::BlendModeType)materialBag.getProperty<int>("BlendMode");
            setColor(materialBag.getProperty<MathCore::vec4f>("uColor"));

            auto componentRectangleRaw = materialBag.getProperty<std::weak_ptr<Component>>("ComponentRectangle").lock();
            if (componentRectangleRaw != nullptr)
            {
                componentRectangle = (Components::ComponentRectangle *)(componentRectangleRaw.get());
                componentRectangle_transform = componentRectangle->getTransform().get();

                if (uMask_radius != componentRectangle->mask_radius)
                {
                    uMask_radius = componentRectangle->mask_radius;
                    setUniform(u_mask_radius, uMask_radius);
                }

                bool needs_set = false;
                for (int i = 0; i < 4; i++)
                {
                    if (uMask_corner[i] != componentRectangle->mask_corner[i])
                    {
                        uMask_corner[i] = componentRectangle->mask_corner[i];
                        needs_set = true;
                    }
                }

                // array uniform upload
                if (u_mask_corner >= 0 && needs_set)
                    OPENGL_CMD(glUniform2fv(u_mask_corner, (GLsizei)4, uMask_corner[0].array));
            }
            else
            {
                componentRectangle = nullptr;
                componentRectangle_transform = nullptr;
            }

            state->clearTextureUnitActivationArray();
        }
        void ShaderUnlitVertexColorWithMask::setUniformsFromMatrices(
            GLRenderState *state,
            ResourceMap *resourceMap,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {
            // needs the camera that renders the mask, to inverse project correctly
            if (componentRectangle_transform != nullptr)
            {
                MathCore::mat4f *mvp;
                MathCore::mat4f *mv;
                MathCore::mat4f *mvIT;
                MathCore::mat4f *mvInv;
                componentRectangle_transform->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                                                  &mvp, &mv, &mvIT, &mvInv);
                MathCore::vec3f scale = 2.0f / MathCore::vec3f(camera->viewport.w, camera->viewport.h, 2.0f);
                MathCore::mat4f transform_mask = mvp->inverse() *
                                                 MathCore::GEN<MathCore::mat4f>::translateHomogeneous(-1.0f, -1.0f, 0.0f) *
                                                 MathCore::GEN<MathCore::mat4f>::scaleHomogeneous(scale);

                if (uTransformToMask != transform_mask)
                {
                    uTransformToMask = transform_mask;
                    setUniform(u_transform_to_mask, uTransformToMask);
                }
            }

            setMVP(*mvp);
        }

    }
}
