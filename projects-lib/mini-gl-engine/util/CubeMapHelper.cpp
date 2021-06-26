#include "CubeMapHelper.h"

namespace GLEngine {

    //openglWrapper::GLDynamicFBO dinamicFBO_1x1;
    //ShaderCopyCubeMap shaderCopyCubeMap;

    CubeMapHelper::CubeMapHelper() {
        
        printf("setup renderbuffer.\n");
        
        depthBuffer.setSize(16,16);
        
        dinamicFBO_1x1.enable();
        dinamicFBO_1x1.setDepthRenderBufferAttachment(&depthBuffer);
        dinamicFBO_1x1.disable();
    }

    void CubeMapHelper::copyCubeMap(int resolution, openglWrapper::GLCubeMap *inputcubemap, openglWrapper::GLCubeMap *targetcubemap) {

        GLRenderState *state = GLRenderState::Instance();

        //save state
        BlendModeType blend = state->BlendMode;
        DepthTestType depth = state->DepthTest;
        bool depthwrite = state->DepthWrite;
        iRect viewport = state->Viewport;

        depthBuffer.setSize(resolution,resolution);
        // RPI 2.0 only supports render to RGBA buffers...
        targetcubemap->setSizeAndFormat(resolution, resolution, GL_RGB);

        state->CurrentFramebufferObject = NULL;

        dinamicFBO_1x1.enable();

        state->BlendMode = BlendModeDisabled;
        state->DepthTest = DepthTestDisabled;
        state->DepthWrite = false;
        state->Viewport = iRect(resolution, resolution);

        //draw target
        state->CurrentShader = &shaderCopyCubeMap;
        shaderCopyCubeMap.setCubeTexture(0);

        inputcubemap->active(0);

        const aRibeiro::vec3 vertex[] = {
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),

            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),

            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),

            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),

            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),

            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(-1,1,0),aRibeiro::vec3(1,1,0),
            aRibeiro::vec3(-1,-1,0),aRibeiro::vec3(1,1,0),aRibeiro::vec3(1,-1,0),
        };

        const float _min = (0.5f / (1024.0f / 2.0f) - 1.0f);
        const float _max = (1023.5f / (1024.0f / 2.0f) - 1.0f);
        const aRibeiro::vec3 uv[] = {
            //_posx
            aRibeiro::vec3(1,_max,_max), aRibeiro::vec3(1,_min,_max), aRibeiro::vec3(1,_min,_min),
            aRibeiro::vec3(1,_max,_max), aRibeiro::vec3(1,_min,_min), aRibeiro::vec3(1,_max,_min),

            //_negx
            aRibeiro::vec3(-1,_max,_min), aRibeiro::vec3(-1,_min,_min), aRibeiro::vec3(-1,_min,_max),
            aRibeiro::vec3(-1,_max,_min), aRibeiro::vec3(-1,_min,_max), aRibeiro::vec3(-1,_max,_max),

            //_posy
            aRibeiro::vec3(_min,1,_min), aRibeiro::vec3(_min,1,_max), aRibeiro::vec3(_max,1,_max),
            aRibeiro::vec3(_min,1,_min), aRibeiro::vec3(_max,1,_max), aRibeiro::vec3(_max,1,_min),

            //_negy
            aRibeiro::vec3(_min,-1,_max), aRibeiro::vec3(_min,-1,_min), aRibeiro::vec3(_max,-1,_min),
            aRibeiro::vec3(_min,-1,_max), aRibeiro::vec3(_max,-1,_min), aRibeiro::vec3(_max,-1,_max),

            //_posz
            aRibeiro::vec3(_min,_max,1), aRibeiro::vec3(_min,_min,1), aRibeiro::vec3(_max,_min,1),
            aRibeiro::vec3(_min,_max,1), aRibeiro::vec3(_max,_min,1), aRibeiro::vec3(_max,_max,1),

            //_negz
            aRibeiro::vec3(_max,_max,-1), aRibeiro::vec3(_max,_min,-1), aRibeiro::vec3(_min,_min,-1),
            aRibeiro::vec3(_max,_max,-1), aRibeiro::vec3(_min,_min,-1), aRibeiro::vec3(_min,_max,-1)
        };

        OPENGL_CMD(glEnableVertexAttribArray(shaderCopyCubeMap.vPosition));
        OPENGL_CMD(glVertexAttribPointer(shaderCopyCubeMap.vPosition, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &vertex[0]));

        OPENGL_CMD(glEnableVertexAttribArray(shaderCopyCubeMap.vUV));
        OPENGL_CMD(glVertexAttribPointer(shaderCopyCubeMap.vUV, 3, GL_FLOAT, false, sizeof(aRibeiro::vec3), &uv[0]));

        /*
        static openglWrapper::GLTexture *tex = NULL;
        if (tex == NULL)
            tex = new openglWrapper::GLTexture();
        tex->setSize(resolution,resolution);
        dinamicFBO_1x1.setColorAttachment(tex, 0);
        */

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 0, 6));

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 6, 6));

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 12, 6));

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 18, 6));

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 24, 6));

        dinamicFBO_1x1.setColorAttachmentCube(targetcubemap, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0);
        OPENGL_CMD(glDrawArrays(GL_TRIANGLES, 30, 6));

        OPENGL_CMD(glDisableVertexAttribArray(shaderCopyCubeMap.vPosition));
        OPENGL_CMD(glDisableVertexAttribArray(shaderCopyCubeMap.vUV));


        dinamicFBO_1x1.setColorAttachmentCube(NULL, 0, 0);

        dinamicFBO_1x1.disable();

        //restore state
        state->BlendMode = blend;
        state->DepthTest = depth;
        state->DepthWrite = depthwrite;
        state->Viewport = viewport;

        inputcubemap->deactive(0);

    }

}
