#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <appkit-gl-base/GLShader.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Shader that draws the reflected texture inside a cubemap.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLShaderCubeMapMirror : public GLShader
        {

        protected:
            //
            // uniform locations
            //
            int mvp;
            int localToWorld;
            int localToWorld_it;
            int worldCameraPos;
            int cubeTexture;

        public:
            static const AppKit::OpenGL::ShaderType Type;
            //
            // attrib locations
            //
            static const int vPosition = 0; ///< vertex atribute layout position
            static const int vNormal = 1;   ///< vertex atribute layout position

            GLShaderCubeMapMirror() : GLShader(GLShaderCubeMapMirror::Type)
            {
                const char vertexShaderCode[] = {
#if !defined(GLAD_GLES2)
                    "#version 120\n"
#endif
                    "attribute vec4 vPosition;"
                    "attribute vec3 vNormal;"

                    //"varying vec3 worldPos;"
                    "varying vec3 worldNormal;"
                    "varying vec3 worldI;"

                    "uniform mat4 mvp;"
                    "uniform mat4 localToWorld;"
                    "uniform mat4 localToWorld_it;"
                    "uniform vec3 worldCameraPos;"

                    "void main() {"
                    "  vec3 worldPos = (localToWorld * vPosition).xyz;"
                    "  worldNormal = ( localToWorld_it * vec4( vNormal, 0.0 ) ).xyz;"
                    "  worldI =  worldPos - worldCameraPos;"
                    "  gl_Position = mvp * vPosition;"
                    "}"};

                const char fragmentShaderCode[] = {
                //"precision mediump float;"
                //"varying vec3 worldPos;"
#if !defined(GLAD_GLES2)
                    "#version 120\n"
#endif
                    "varying vec3 worldNormal;"
                    "varying vec3 worldI;"

                    "uniform samplerCube cubeTexture;"

                    "void main() {"
                    //"  vec3 I = normalize(worldPos - worldCameraPos);"
                    "  vec3 I = normalize(worldI);"
                    "  vec3 R = reflect(I, normalize(worldNormal));"
                    "  vec3 texel = textureCube(cubeTexture, R).rgb;"
                    "  gl_FragColor = vec4(texel,1.0);"
                    "}"};

                compile(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
                bindAttribLocation(GLShaderCubeMapMirror::vPosition, "vPosition");
                bindAttribLocation(GLShaderCubeMapMirror::vNormal, "vNormal");
                link(__FILE__, __LINE__);

                mvp = getUniformLocation("mvp");
                localToWorld = getUniformLocation("localToWorld");
                localToWorld_it = getUniformLocation("localToWorld_it");
                worldCameraPos = getUniformLocation("worldCameraPos");
                cubeTexture = getUniformLocation("cubeTexture");
            }

            void setMVP(const MathCore::mat4f &m)
            {
                setUniform(mvp, m);
            }
            void setLocalToWorld(const MathCore::mat4f &m)
            {
                setUniform(localToWorld, m);
            }
            void setLocalToWorld_it(const MathCore::mat4f &m)
            {
                setUniform(localToWorld_it, m);
            }
            void setWorldCameraPos(const MathCore::vec3f &v)
            {
                setUniform(worldCameraPos, v);
            }

            void setCubeTexture(int texunit)
            {
                setUniform(cubeTexture, texunit);
            }

            ~GLShaderCubeMapMirror()
            {
            }
        };

    }

}