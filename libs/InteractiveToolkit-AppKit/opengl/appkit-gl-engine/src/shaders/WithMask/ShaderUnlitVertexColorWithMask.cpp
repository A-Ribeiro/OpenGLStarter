// #include <appkit-gl-engine/shaders/ShaderUnlitVertexColorWithMask.h>
#include <appkit-gl-engine/shaders/WithMask/ShaderUnlitVertexColorWithMask.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

#include <appkit-gl-engine/components/2d/ComponentRectangle.h>

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>

namespace AppKit
{
    namespace GLEngine
    {

        ShaderUnlitVertexColorWithMask::ShaderUnlitVertexColorWithMask()
        {
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

            uMVP = MathCore::mat4f();
            uColor = MathCore::vec4f(1.0, 1.0, 1.0, 1.0);

            GLRenderState *state = GLRenderState::Instance();
            state->CurrentShader = this;

            setUniform(u_mvp, uMVP);
            setUniform(u_color, uColor);

            mask_query_uniform_locations_and_set_default_values();
            
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
            Utils::ShaderPropertyBag bag = mask_default_bag();

            bag.addProperty("uColor", uColor);
            bag.addProperty("BlendMode", (int)AppKit::GLEngine::BlendModeDisabled);

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

            setMaskFromPropertyBag(materialBag);

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
            setMVP(*mvp);
        }

    }
}
