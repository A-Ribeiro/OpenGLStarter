#ifndef GL_SHADER_CUBEMAP_MIRROR_h_
#define GL_SHADER_CUBEMAP_MIRROR_h_

#include <opengl-wrapper/GLShader.h>

namespace openglWrapper {

    /// \brief Shader that draws the reflected texture inside a cubemap.
    ///
    /// \author Alessandro Ribeiro
    ///
    class GLShaderCubeMapMirror : public GLShader {

    protected:

        void setupAttribLocation() {
            bindAttribLocation(GLShaderCubeMapMirror::vPosition, "vPosition");
            bindAttribLocation(GLShaderCubeMapMirror::vNormal, "vNormal");
        }

        //
        // uniform locations
        //
        int mvp;
        int localToWorld;
        int localToWorld_it;
        int worldCameraPos;
        int cubeTexture;

    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;///< vertex atribute layout position
        static const int vNormal = 1;///< vertex atribute layout position

        GLShaderCubeMapMirror():GLShader(){
            const char vertexShaderCode[] = {
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
                "}" };

            const char fragmentShaderCode[] = {
                //"precision mediump float;"
                //"varying vec3 worldPos;"
                "varying vec3 worldNormal;"
                "varying vec3 worldI;"

                "uniform samplerCube cubeTexture;"

                "void main() {"
                //"  vec3 I = normalize(worldPos - worldCameraPos);"
                "  vec3 I = normalize(worldI);"
                "  vec3 R = reflect(I, normalize(worldNormal));"
                "  vec3 texel = textureCube(cubeTexture, R).rgb;"
                "  gl_FragColor = vec4(texel,1.0);"
                "}" };

            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);

            mvp = getUniformLocation("mvp");
            localToWorld = getUniformLocation("localToWorld");
            localToWorld_it = getUniformLocation("localToWorld_it");
            worldCameraPos = getUniformLocation("worldCameraPos");
            cubeTexture = getUniformLocation("cubeTexture");
        }

        void setMVP(const aRibeiro::mat4 & m){
            setUniform(mvp, m);
        }
        void setLocalToWorld(const aRibeiro::mat4 & m){
            setUniform(localToWorld, m);
        }
        void setLocalToWorld_it(const aRibeiro::mat4 & m){
            setUniform(localToWorld_it, m);
        }
        void setWorldCameraPos(const aRibeiro::vec3 & v){
            setUniform(worldCameraPos, v);
        }

        void setCubeTexture(int texunit){
            setUniform(cubeTexture, texunit);
        }

        virtual ~GLShaderCubeMapMirror(){

        }

    };

}

#endif

