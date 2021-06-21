#include "GLTexture.h"

#include <opengl-wrapper/MipMapGenerator.h>
#include <opengl-wrapper/PlatformGL.h>
#include <aribeiro/PNGHelper.h>
#include <aribeiro/JPGHelper.h>
#include <aribeiro/SetNullAndDelete.h>
#include <aribeiro/geometricOperations.h>
#include <stdlib.h>
#include <stdio.h>

using namespace aRibeiro;

namespace openglWrapper {

    int GLTexture::originalPackAlignment = -1;
    int GLTexture::originalUnpackAlignment = -1;

    GLTexture::GLTexture(bool _sRGB) {

        //initialize static pack and unpack alignment
        if (originalPackAlignment == -1) {
            OPENGL_CMD(glGetIntegerv(GL_PACK_ALIGNMENT, &originalPackAlignment));
            OPENGL_CMD(glGetIntegerv(GL_UNPACK_ALIGNMENT, &originalUnpackAlignment));
        }

        mTexture = 0;
        width = 0;
        height = 0;
        sRGB = _sRGB;

        OPENGL_CMD(glGenTextures(1, &mTexture));
        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));

        //active(0);
        //OPENGL_CMD(glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));

        // Set filtering
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        //deactive(0);

        //GLES20.glBindTexture(GL.GL_TEXTURE_2D, 0);
    }

    bool GLTexture::isInitialized() {
        return width > 0 && height > 0;
    }

    void GLTexture::uploadBufferAlpha8(const void* buffer, int w, int h) {

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        //active(0);

        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        if (w != width || h != height) {
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, buffer));
            width = w;
            height = h;
        }
        else
            OPENGL_CMD(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

        //deactive(0);
    }

    void GLTexture::uploadBufferRGB_888(const void* buffer, int w, int h) {

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        if (w != width || h != height) {

            if (sRGB) {
                ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
                OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer));
            }
            else
                OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer));
            width = w;
            height = h;
        }
        else
            OPENGL_CMD(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

    }

    void GLTexture::uploadBufferRGBA_8888(const void* buffer, int w, int h) {

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));

        //set pack data alignment to 1 byte read from memory and 1 byte to write to gpu memory...
        //OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        //OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        if (w != width || h != height) {

            if (sRGB) {
                ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
                OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
            }
            else
                OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
            width = w;
            height = h;
        }
        else
            OPENGL_CMD(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, buffer));

        //back the pack alignment for texture operations
        //OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, originalPackAlignment));
        //OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, originalUnpackAlignment));

    }

    //
    // Old way to copy framebuffer to texture... consider to use FBOs to acomplish it better
    //
    // can read rgb(GL_RGB) from framebuffer, or the depth component (GL_DEPTH_COMPONENT24)
    void GLTexture::setSize(int w, int h, GLuint format) {

        bool power_of_two_w = w && !(w & (w - 1));
        bool power_of_two_h = h && !(h & (h - 1));

        if (!power_of_two_w || !power_of_two_h)
            ARIBEIRO_ABORT(!GLEW_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));

        if (format == GL_DEPTH_COMPONENT16 ||
            format == GL_DEPTH_COMPONENT24
            #ifndef ARIBEIRO_RPI
            || format == GL_DEPTH_COMPONENT32
            #endif
            ) {

            ARIBEIRO_ABORT(!GLEW_ARB_depth_texture,"ARB_depth_texture not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

            //depth buffer force to nearest filtering...
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        }
        else if (format == GL_STENCIL_INDEX8) {

            ARIBEIRO_ABORT(!GLEW_VERSION_4_4, "VERSION_4_4 not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, 0));

            //depth buffer force to nearest filtering...
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        }
        else if (format == GL_DEPTH24_STENCIL8 ) {

            ARIBEIRO_ABORT(!GLEW_ARB_depth_texture,"ARB_depth_texture not supported.");

            //
            // requires extension: GLEW_EXT_packed_depth_stencil or GLEW_NV_packed_depth_stencil
            //
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0));

            //depth buffer force to nearest filtering...
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        }
        #ifndef ARIBEIRO_RPI
        else if (format == GL_RGBA16F ||
            format == GL_RGBA32F) {

            ARIBEIRO_ABORT(!GLEW_ARB_texture_float,"ARB_texture_float not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_RGBA, GL_FLOAT, 0));
        }
        else if (format == GL_RGB16F ||
            format == GL_RGB32F) {

            ARIBEIRO_ABORT(!GLEW_ARB_texture_float,"ARB_texture_float not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_RGB, GL_FLOAT, 0));
        }
        else if (format == GL_R16F ||
            format == GL_R32F) {

            ARIBEIRO_ABORT(!GLEW_ARB_texture_float,"ARB_texture_float not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, GL_R, GL_FLOAT, 0));
        }
        #endif
        else {

            GLuint internalFormat = format;
            if (sRGB) {
                ARIBEIRO_ABORT(!GLEW_EXT_texture_sRGB, "Graphics driver does not support texture sRGB\n.");
                if (format == GL_RGB)
                    internalFormat = GL_SRGB;
                if (format == GL_RGBA)
                    internalFormat = GL_SRGB_ALPHA;
            }

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, 0));
        }



        width = w;
        height = h;
    }

    //copy the color buffer, or depth buffer, using the texture current size
    void GLTexture::copyFrameBuffer() {
        //OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));
        OPENGL_CMD(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height));
        //OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void GLTexture::active(int id) const {

        if (id != 0)
            ARIBEIRO_ABORT(!GLEW_ARB_multitexture,"ARB_multitexture not supported.");

        OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
        #if !defined(ARIBEIRO_RPI)
            //OPENGL_CMD(glEnable(GL_TEXTURE_2D));
        #endif
        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));
    }

    void GLTexture::deactive(int id) {
        OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
        #if !defined(ARIBEIRO_RPI)
            //OPENGL_CMD(glDisable(GL_TEXTURE_2D));
        #endif
        OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, 0));
    }

    GLTexture::~GLTexture()
    {
        if (mTexture != 0 && glIsTexture(mTexture))
            OPENGL_CMD(glDeleteTextures(1, &mTexture));
        mTexture = 0;
    }

    GLTexture *GLTexture::loadFromFile(const char* filename, bool invertY, bool sRGB) {
        int w, h, channels, depth;

        bool isPNG = aRibeiro::PNGHelper::isPNGFilename(filename);
        bool isJPG = aRibeiro::JPGHelper::isJPGFilename(filename);

        void (*closeFnc)(char *&) = NULL;

        char* buffer = NULL;
        if (isPNG){
            buffer = PNGHelper::readPNG(filename, &w, &h, &channels, &depth, invertY);
            closeFnc = &PNGHelper::closePNG;
        } else if (isJPG) {
            buffer = JPGHelper::readJPG(filename, &w, &h, &channels, &depth, invertY);
            closeFnc = &JPGHelper::closeJPG;
        }

        ARIBEIRO_ABORT(
            buffer == NULL,
            "error to load: %s\n", filename);

        //if (buffer == NULL) {
        //    fprintf( stderr, "error to load: %s\n", filename);
        //    exit(-1);
        //}

        int maxResolution = 8192;
        #ifdef ARIBEIRO_RPI
            maxResolution = 1024;
        #endif

        if (channels == 1 && depth == 8) {
            GLTexture *result = new GLTexture(sRGB);

            if (w > maxResolution || h > maxResolution) {
                MipMapGenerator *generator = new MipMapGenerator((uint8_t*)buffer, w, h,1);

                for(size_t i=0;i<generator->images.size();i++){
                    MipMapImage* image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution ){
                        result->uploadBufferAlpha8((const void *)image->buffer, image->width, image->height);
                        break;
                    }
                }
                delete generator;
            } else
                result->uploadBufferAlpha8(buffer, w, h);
            //setNullAndDelete(buffer);
            closeFnc(buffer);
            return result;
        } else if (channels == 3 && depth == 8) {
            GLTexture *result = new GLTexture(sRGB);

            if (w > maxResolution || h > maxResolution) {
                MipMapGenerator *generator = new MipMapGenerator((uint8_t*)buffer, w, h,3);

                for(size_t i=0;i<generator->images.size();i++){
                    MipMapImage* image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution ){
                        result->uploadBufferRGB_888((const void *)image->buffer, image->width, image->height);
                        break;
                    }
                }
                delete generator;
            } else
                result->uploadBufferRGB_888(buffer, w, h);

            //setNullAndDelete(buffer);
            closeFnc(buffer);
            return result;
        } else if(channels == 4 && depth == 8) {
            GLTexture *result = new GLTexture(sRGB);

            if (w > maxResolution || h > maxResolution) {
                MipMapGenerator *generator = new MipMapGenerator((uint8_t*)buffer, w, h,4);

                for(size_t i=0;i<generator->images.size();i++){
                    MipMapImage* image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution ){
                        result->uploadBufferRGBA_8888((const void *)image->buffer, image->width, image->height);
                        break;
                    }
                }
                delete generator;
            } else
                result->uploadBufferRGBA_8888(buffer, w, h);
            //setNullAndDelete(buffer);
            closeFnc(buffer);
            return result;
        }

        closeFnc(buffer);
        //setNullAndDeleteArray(buffer);

        ARIBEIRO_ABORT(
            true,
            "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);

        //fprintf(stderr, "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);
        //exit(-1);

        return NULL;
    }

    void GLTexture::writeToPNG(const char* filename, bool invertY) {

        #ifdef ARIBEIRO_RPI

        //
        // OpenGLES needs to create a framebuffer to copy the pixels
        //
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0);

        char *buff = new char[width*height*4];
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buff);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &fbo);

        PNGHelper::writePNG(filename, width, height,4,buff, !invertY);
        delete[] buff;

        #else

            active(0);
            char *buff = new char[width*height*4];
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff);
            PNGHelper::writePNG(filename, width, height,4,buff, !invertY);
            delete[] buff;
            deactive(0);
        #endif

    }

    void GLTexture::generateMipMap() {

        ARIBEIRO_ABORT(!GLEW_SGIS_generate_mipmap,"SGIS_generate_mipmap not supported.");

        active(0);
        OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_2D));
        OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        //OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        deactive(0);
    }

    //Nvidia has 16.0 max
    void GLTexture::setAnisioLevel(float level) {
        if (GLEW_EXT_texture_filter_anisotropic) {
            float aniso = 0.0f;
            OPENGL_CMD(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso));
            level = clamp(level, 0.0f, aniso);
            OPENGL_CMD(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level));
            printf("Anisotropic texture set to: %f\n", aniso);
        }
    }

    void GLTexture::setAsShadowMapFiltering(bool enable) {
        #ifndef ARIBEIRO_RPI
            active(0);
            if (enable) {
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL));
            }
            else
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
            deactive(0);
        #else
            printf("setAsShadowMapFiltering not implemented on RPI.\n");
        #endif
    }

}
