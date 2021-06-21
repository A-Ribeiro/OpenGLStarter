
#ifndef CubeMapHelper__H_
#define CubeMapHelper__H_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>

#include <mini-gl-engine/GLRenderState.h>

namespace GLEngine {


    class ShaderCopyCubeMap : public openglWrapper::GLShader {

    protected:

        void setupAttribLocation() {
            bindAttribLocation(ShaderCopyCubeMap::vPosition, "vPosition");
            bindAttribLocation(ShaderCopyCubeMap::vUV, "vUV");
        }

    public:
        //
        // attrib locations
        //
        static const int vPosition = 0;
        static const int vUV = 1;

        //
        // uniform locations
        //
        int cubeTexture;

        ShaderCopyCubeMap() :GLShader() {
            const char vertexShaderCode[] = {
                "attribute vec4 vPosition;"
                "attribute vec3 vUV;"
                "varying vec3 cubeAccess;"
                "void main() {"
                "  cubeAccess = vUV;"
                "  gl_Position = vPosition;"
                "}" };

            const char fragmentShaderCode[] = {
                //"precision mediump float;"
                "varying vec3 cubeAccess;"
                "uniform samplerCube cubeTexture;"
                "void main() {"
                "  vec4 result = textureCube(cubeTexture, cubeAccess);"
                "  gl_FragColor = result;"
                "}" };

            LoadShaderProgram(vertexShaderCode, fragmentShaderCode, __FILE__, __LINE__);
            cubeTexture = getUniformLocation("cubeTexture");
        }

        void setCubeTexture(int texunit) {
            setUniform(cubeTexture, texunit);
        }

        virtual ~ShaderCopyCubeMap() {

        }
    };



    class _SSE2_ALIGN_PRE CubeMapHelper {
    public:
        openglWrapper::GLDynamicFBO dinamicFBO_1x1;
        openglWrapper::GLRenderBuffer depthBuffer;
        ShaderCopyCubeMap shaderCopyCubeMap;

        CubeMapHelper() ;
        void copyCubeMap(int resolution, openglWrapper::GLCubeMap *inputcubemap, openglWrapper::GLCubeMap *targetcubemap);

        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS;

}

#endif
