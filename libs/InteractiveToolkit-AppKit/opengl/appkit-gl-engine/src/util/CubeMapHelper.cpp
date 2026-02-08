#include <appkit-gl-engine/util/CubeMapHelper.h>
#include <appkit-gl-base/GLCubeMap.h>

#include <appkit-gl-engine/shaders/BasicShadersDefinitions.inl>

namespace AppKit
{
    namespace GLEngine
    {

        const AppKit::OpenGL::ShaderType ShaderCopyCubeMap::Type = "ShaderCopyCubeMap";
        const AppKit::OpenGL::ShaderType ShaderRender1x1CubeMap::Type = "ShaderRender1x1CubeMap";

        ShaderRender1x1CubeMap::ShaderRender1x1CubeMap() : GLShader(ShaderRender1x1CubeMap::Type)
        {
            const char vertexShaderCode[] = {
                "attribute vec4 vPosition;\n"
                //"attribute vec2 vUV;\n"
                "varying vec2 uv;\n"
                "void main() {\n"
                "  uv = vPosition.xy*0.5 + vec2(0.5);\n"
                "  gl_Position = vPosition;\n"
                "}\n"};

            const char fragmentShaderCode[] = {
                //"precision mediump float;"
                "varying vec2 uv;\n"
                "uniform samplerCube cubeTexture;\n"
                // "vec3 baricentricCoordvec2(vec2 a, vec2 b, vec2 c, vec2 p){\n"
                // "  vec3 bc = vec3(c-b,0);\n"
                // "  vec3 ba = vec3(a-b,0);\n"
                // "  float areaTriangle_inv = 1.0/cross(bc,ba).z;\n"
                // "  vec3 bp = vec3(p-b,0);\n"
                // "  vec3 uvw;\n"
                // "  uvw.x = cross(bc,bp).z;\n"
                // "  uvw.z = cross(bp,ba).z;\n"
                // "  uvw.xz = uvw.xz * areaTriangle_inv;\n"
                // "  uvw.y = 1.0 - uvw.x - uvw.z;\n"
                // "  return uvw;\n"
                // "}\n"
                // "vec3 sampleEnvironmentCubeBaryLerp(vec3 normal){\n"
                // "  vec3 sign = sign(normal);\n"
                // "  vec3 signX = vec3(sign.x,0,0);\n"
                // "  vec3 signY = vec3(0,sign.y,0);\n"
                // "  vec3 signZ = vec3(0,0,sign.z);\n"
                // "  mat3 tex = mat3(\n"
                // "    textureCube(cubeTexture, signX).rgb,\n"
                // "    textureCube(cubeTexture, signY).rgb,\n"
                // "    textureCube(cubeTexture, signZ).rgb\n"
                // "  );\n"
                // "  vec2 signXxZz = step( sign.xz, vec2(-0.5) );\n"
                // "  signXxZz = signXxZz * 3.14159265359;\n"
                // "  vec2 polarSignX = vec2(1.57079637051,signXxZz.x );\n"
                // "  vec2 polarSignY = vec2(1.57079637051);\n"
                // "  vec2 polarSignZ = vec2(signXxZz.y,1.57079637051);\n"
                // "  vec2 polarNormal = acos(normal.zx);\n"
                // "  vec3 bariCentricCoord = baricentricCoordvec2(\n"
                // "    polarSignX,polarSignY,polarSignZ,polarNormal\n"
                // "  );\n"
                // "  return tex * bariCentricCoord;\n"
                // "}\n"
                "" TRIGONOMETRIC_CONSTANTS
                "" TRIGONOMETRIC_LOW_RES_ACOS_ASIN_GENTYPE(vec2) //
                "" CROSS_VEC2
                "" BARYCENTRIC_VEC2
                "" Shader_sampleEnvironmentCubeBaryLerp
                "" Shader_Spherical_st2normal
                "void main() {\n"
                "  vec2 _uv = uv;\n"
                "  _uv.y = 1.0 - abs( (_uv.y - 0.5 ) * 2.0 );\n"
                "  vec3 result = sampleEnvironmentCubeBaryLerp(cubeTexture, st2normal(_uv) );\n"
                "  gl_FragColor = vec4(result, 1.0);\n"
                "}\n"};

            compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            bindAttribLocation(ShaderCopyCubeMap::vPosition, "vPosition");
            bindAttribLocation(ShaderCopyCubeMap::vUV, "vUV");
            link(__FILE__, __LINE__);

            cubeTexture = getUniformLocation("cubeTexture");
        }

        void ShaderRender1x1CubeMap::setCubeTexture(int texunit)
        {
            setUniform(cubeTexture, texunit);
        }

        ShaderRender1x1CubeMap::~ShaderRender1x1CubeMap()
        {
        }

        // AppKit::OpenGL::GLDynamicFBO dinamicFBO_1x1;
        // ShaderCopyCubeMap shaderCopyCubeMap;

        CubeMapHelper::CubeMapHelper()
        {

            printf("[CubeMapHelper] setup renderbuffer.\n");

            // depthBuffer.setSize(16,16);

            targetFBO.enable();
            // dinamicFBO_1x1.setDepthRenderBufferAttachment(&depthBuffer);
            targetFBO.setDrawBufferCount(1);
            // dinamicFBO_1x1.checkAttachment();
            targetFBO.disable();

            sourceFBO.enable();
            sourceFBO.setDrawBufferCount(1);
            sourceFBO.disable();
        }

        /*
            void CubeMapHelper::copyCubeMap(int resolution, AppKit::OpenGL::GLCubeMap *inputcubemap, AppKit::OpenGL::GLCubeMap *targetcubemap) {

                GLRenderState *state = GLRenderState::Instance();

                //save state
                BlendModeType blend = state->BlendMode;
                DepthTestType depth = state->DepthTest;
                bool depthwrite = state->DepthWrite;
                iRect viewport = state->Viewport;

                //depthBuffer.setSize(resolution,resolution);
                targetFBO.setSize(resolution,resolution);
                // RPI 2.0 only supports render to RGBA buffers...
                targetcubemap->setSize(resolution, resolution, GL_RGB);

                state->CurrentFramebufferObject = nullptr;

                targetFBO.enable();

                state->BlendMode = BlendModeDisabled;
                state->DepthTest = DepthTestDisabled;
                state->DepthWrite = false;
                state->Viewport = iRect(resolution, resolution);

                //draw target
                state->CurrentShader = &shaderCopyCubeMap;
                shaderCopyCubeMap.setCubeTexture(0);

                inputcubemap->active(0);

                const MathCore::vec3f vertex[] = {
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),

                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),

                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),

                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),

                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),

                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(-1,1,0),MathCore::vec3f(1,1,0),
                    MathCore::vec3f(-1,-1,0),MathCore::vec3f(1,1,0),MathCore::vec3f(1,-1,0),
                };

                const float _min = (0.5f / (1024.0f / 2.0f) - 1.0f);
                const float _max = (1023.5f / (1024.0f / 2.0f) - 1.0f);
                const MathCore::vec3f uv[] = {
                    //_posx
                    MathCore::vec3f(1,_max,_max), MathCore::vec3f(1,_min,_max), MathCore::vec3f(1,_min,_min),
                    MathCore::vec3f(1,_max,_max), MathCore::vec3f(1,_min,_min), MathCore::vec3f(1,_max,_min),

                    //_negx
                    MathCore::vec3f(-1,_max,_min), MathCore::vec3f(-1,_min,_min), MathCore::vec3f(-1,_min,_max),
                    MathCore::vec3f(-1,_max,_min), MathCore::vec3f(-1,_min,_max), MathCore::vec3f(-1,_max,_max),

                    //_posy
                    MathCore::vec3f(_min,1,_min), MathCore::vec3f(_min,1,_max), MathCore::vec3f(_max,1,_max),
                    MathCore::vec3f(_min,1,_min), MathCore::vec3f(_max,1,_max), MathCore::vec3f(_max,1,_min),

                    //_negy
                    MathCore::vec3f(_min,-1,_max), MathCore::vec3f(_min,-1,_min), MathCore::vec3f(_max,-1,_min),
                    MathCore::vec3f(_min,-1,_max), MathCore::vec3f(_max,-1,_min), MathCore::vec3f(_max,-1,_max),

                    //_posz
                    MathCore::vec3f(_min,_max,1), MathCore::vec3f(_min,_min,1), MathCore::vec3f(_max,_min,1),
                    MathCore::vec3f(_min,_max,1), MathCore::vec3f(_max,_min,1), MathCore::vec3f(_max,_max,1),

                    //_negz
                    MathCore::vec3f(_max,_max,-1), MathCore::vec3f(_max,_min,-1), MathCore::vec3f(_min,_min,-1),
                    MathCore::vec3f(_max,_max,-1), MathCore::vec3f(_min,_min,-1), MathCore::vec3f(_min,_max,-1)
                };

                OPENGL_CMD(glEnableVertexAttribArray(shaderCopyCubeMap.vPosition));
                OPENGL_CMD(glVertexAttribPointer(shaderCopyCubeMap.vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

                OPENGL_CMD(glEnableVertexAttribArray(shaderCopyCubeMap.vUV));
                OPENGL_CMD(glVertexAttribPointer(shaderCopyCubeMap.vUV, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &uv[0]));

                //static AppKit::OpenGL::GLTexture *tex = nullptr;
                //if (tex == nullptr)
                //    tex = new AppKit::OpenGL::GLTexture();
                //tex->setSize(resolution,resolution);
                //dinamicFBO_1x1.setColorAttachment(tex, 0);

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 24, 6));

                targetFBO.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0);
                OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 30, 6));

                OPENGL_CMD(glDisableVertexAttribArray(shaderCopyCubeMap.vPosition));
                OPENGL_CMD(glDisableVertexAttribArray(shaderCopyCubeMap.vUV));


                targetFBO.setColorAttachmentCube(nullptr, 0, 0);

                targetFBO.disable();

                //restore state
                state->BlendMode = blend;
                state->DepthTest = depth;
                state->DepthWrite = depthwrite;
                state->Viewport = viewport;

                inputcubemap->deactive(0);

            }

            */

        void CubeMapHelper::copyCubeMapEnhanced(AppKit::OpenGL::GLCubeMap *inputcubemap, int inputMip, AppKit::OpenGL::GLCubeMap *targetcubemap, int outputMip)
        {

            const GLint cubemap_targets[] = {
                GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

            for (int i = 0; i < 6; i++)
            {
                sourceFBO.enable();
                sourceFBO.setColorAttachmentCube(inputcubemap, cubemap_targets[i], 0, inputMip);
                targetFBO.enable();
                targetFBO.setColorAttachmentCube(targetcubemap, cubemap_targets[i], 0, outputMip);
                targetFBO.blitFrom(&sourceFBO, 0, 0, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            }

            targetFBO.disable();
        }

        void CubeMapHelper::render1x1CubeIntoSphereTexture(AppKit::OpenGL::GLCubeMap *inputcubemap, AppKit::OpenGL::GLTexture *targetTexture, int width, int height)
        {
            GLRenderState *state = GLRenderState::Instance();

            // save state
            BlendModeType blend = state->BlendMode;
            DepthTestType depth = state->DepthTest;
            bool depthwrite = state->DepthWrite;
            iRect viewport = state->Viewport;

            sphereFBO.enable();

            sphereFBO.setColorAttachment(nullptr, 0);
            targetTexture->setSize(width, height, inputcubemap->internal_format);
            sphereFBO.setSize(width, height);
            sphereFBO.setColorAttachment(targetTexture, 0);

            inputcubemap->active(0);
            shaderRender1x1CubeMap.enable();
            shaderRender1x1CubeMap.setCubeTexture(0);

            // set state
            state->BlendMode = BlendModeDisabled;
            state->DepthTest = DepthTestDisabled;
            state->DepthWrite = false;
            state->Viewport = iRect(width, height);

            // render code
            const MathCore::vec3f vertex[] = {
                MathCore::vec3f(-1, -1, 0), MathCore::vec3f(1, 1, 0), MathCore::vec3f(-1, 1, 0),
                MathCore::vec3f(-1, -1, 0), MathCore::vec3f(1, -1, 0), MathCore::vec3f(1, 1, 0)};

            OPENGL_CMD(glEnableVertexAttribArray(ShaderRender1x1CubeMap::vPosition));
            OPENGL_CMD(glVertexAttribPointer(ShaderRender1x1CubeMap::vPosition, 3, GL_FLOAT, false, sizeof(MathCore::vec3f), &vertex[0]));

            if (targetTexture->isSRGB())
                glEnable(GL_FRAMEBUFFER_SRGB);

            OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

            if (targetTexture->isSRGB())
                glDisable(GL_FRAMEBUFFER_SRGB);

            OPENGL_CMD(glDisableVertexAttribArray(ShaderRender1x1CubeMap::vPosition));

            inputcubemap->deactive(0);
            sphereFBO.setColorAttachment(nullptr, 0);
            sphereFBO.disable();

            // restore state
            state->BlendMode = blend;
            state->DepthTest = depth;
            state->DepthWrite = depthwrite;
            state->Viewport = viewport;
        }

    }
}