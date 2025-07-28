#include <appkit-gl-engine/DefaultEngineShader.h>
// #include <appkit-gl-engine/GL/GLRenderState.h>
#include <appkit-gl-engine/util/ShaderPropertyBag.h>
#include <cstring>
#include <cstdio>

namespace AppKit
{
    namespace GLEngine
    {
        DefaultEngineShader::DefaultEngineShader()
        {
            format = 0;
            // format = ITKExtension::Model::CONTAINS_POS | ITKExtension::Model::CONTAINS_UV0;

            snprintf(class_name, 128, "DefaultEngineShader");
        }

        int DefaultEngineShader::queryAttribLocation(const char *aname)
        {
            ITK_ABORT(!format, "shader format cannot be 0\n.");

            int count = 0;

            if (format & ITKExtension::Model::CONTAINS_POS)
            {
                if (strcmp(aname, "aPosition") == 0)
                    return count;
                count++;
            }
            if (format & ITKExtension::Model::CONTAINS_NORMAL)
            {
                if (strcmp(aname, "aNormal") == 0)
                    return count;
                count++;
            }
            if (format & ITKExtension::Model::CONTAINS_TANGENT)
            {
                if (strcmp(aname, "aTangent") == 0)
                    return count;
                count++;
            }
            if (format & ITKExtension::Model::CONTAINS_BINORMAL)
            {
                if (strcmp(aname, "aBinormal") == 0)
                    return count;
                count++;
            }
            char name[16];

            for (int i = 0; i < 8; i++)
            {
                if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                {
                    snprintf(name, 16, "aUV%i", i);
                    if (strcmp(aname, name) == 0)
                        return count;
                    count++;
                }
                if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                {
                    snprintf(name, 16, "aColor%i", i);
                    if (strcmp(aname, name) == 0)
                        return count;
                    count++;
                }
            }

            if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
            {
                if (strcmp(aname, "aSkinIndex") == 0)
                    return count;
                count++;
                if (strcmp(aname, "aSkinWeight") == 0)
                    return count;
                count++;
            }

            return -1;
        }

        int DefaultEngineShader::getUserAttribLocationStart()
        {
            int count = 0;

            // check max index
            if (format & ITKExtension::Model::CONTAINS_POS)
                count++;
            if (format & ITKExtension::Model::CONTAINS_NORMAL)
                count++;
            if (format & ITKExtension::Model::CONTAINS_TANGENT)
                count++;
            if (format & ITKExtension::Model::CONTAINS_BINORMAL)
                count++;
            for (int i = 0; i < 8; i++)
            {
                if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                    count++;
                if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                    count++;
            }
            if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
            {
                count++;
                count++;
            }

            return count;
        }

        void DefaultEngineShader::setupAttribLocation()
        {
            ITK_ABORT(!format, "shader format cannot be 0\n.");

            int count = 0;

            // check max index
            if (format & ITKExtension::Model::CONTAINS_POS)
                count++;
            if (format & ITKExtension::Model::CONTAINS_NORMAL)
                count++;
            if (format & ITKExtension::Model::CONTAINS_TANGENT)
                count++;
            if (format & ITKExtension::Model::CONTAINS_BINORMAL)
                count++;
            for (int i = 0; i < 8; i++)
            {
                if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                    count++;
                if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                    count++;
            }

            if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
            {
                count++;
                count++;
            }

            GLint max;
            OPENGL_CMD(glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max));
            ITK_ABORT((count >= max), "Max vertex attrib reached (max=%i).\n", max);

            count = 0;

            if (format & ITKExtension::Model::CONTAINS_POS)
                bindAttribLocation(count++, "aPosition");
            if (format & ITKExtension::Model::CONTAINS_NORMAL)
                bindAttribLocation(count++, "aNormal");
            if (format & ITKExtension::Model::CONTAINS_TANGENT)
                bindAttribLocation(count++, "aTangent");
            if (format & ITKExtension::Model::CONTAINS_BINORMAL)
                bindAttribLocation(count++, "aBinormal");

            char name[16];

            for (int i = 0; i < 8; i++)
            {
                if (format & (ITKExtension::Model::CONTAINS_UV0 << i))
                {
                    snprintf(name, 16, "aUV%i", i);
                    bindAttribLocation(count++, name);
                }
                if (format & (ITKExtension::Model::CONTAINS_COLOR0 << i))
                {
                    snprintf(name, 16, "aColor%i", i);
                    bindAttribLocation(count++, name);
                }
            }

            if (format & ITKExtension::Model::CONTAINS_VERTEX_WEIGHT_ANY)
            {
                bindAttribLocation(count++, "aSkinIndex");
                bindAttribLocation(count++, "aSkinWeight");
            }
        }

        // void DefaultEngineShader::activateShaderAndSetPropertiesFromBag(
        //     Components::ComponentCamera *camera,
        //     const MathCore::mat4f *mvp,
        //     const Transform *element, // for localToWorld, localToWorld_IT, worldToLocal,
        //     GLRenderState *state,
        //     const Utils::ShaderPropertyBag &bag)
        // {
        // }
        // void DefaultEngineShader::deactivateShader(GLRenderState *state)
        // {
        // }

        Utils::ShaderPropertyBag DefaultEngineShader::createDefaultBag() const
        {
            Utils::ShaderPropertyBag bag;
            return bag;
        }

        void DefaultEngineShader::ActiveShader_And_SetUniformsFromMaterial(
            GLRenderState *state,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material)
        {
        }
        void DefaultEngineShader::setUniformsFromMatrices(
            GLRenderState *state,
            RenderPipeline *renderPipeline,
            Components::ComponentMaterial *material,
            Transform *element,
            Components::ComponentCamera *camera,
            const MathCore::mat4f *mvp,
            const MathCore::mat4f *mv,
            const MathCore::mat4f *mvIT,
            const MathCore::mat4f *mvInv)
        {
        }

    }
}
