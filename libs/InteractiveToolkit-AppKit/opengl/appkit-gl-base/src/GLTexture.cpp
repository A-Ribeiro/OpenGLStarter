#include <appkit-gl-base/GLTexture.h>

#include <appkit-gl-base/util/MipMapGenerator.h>
#include <appkit-gl-base/platform/PlatformGL.h>

#include <InteractiveToolkit-Extension/image/JPG.h> // JPGHelper
#include <InteractiveToolkit-Extension/image/PNG.h> // PNGHelper

// #include <aRibeiroCore/SetnullptrAndDelete.h>
// #include <aRibeiroCore/geometricOperations.h>

#include <stdlib.h>
#include <stdio.h>

// using namespace aRibeiro;

namespace AppKit
{

    namespace OpenGL
    {

        bool GLTexture::isSRGB() const
        {
            return internal_format == GL_SRGB || internal_format == GL_SRGB_ALPHA;
        }

        GLTexture::GLTexture(GLsizei _width, GLsizei _height, GLint _internal_format)
        {

            mTexture = 0;
            width = 0;
            height = 0;

            is_depth_stencil = false;
            internal_format = 0;
            input_format = GL_RGB;
            input_data_type = GL_UNSIGNED_BYTE;
            input_alignment = 4;
            input_raw_element_size = 1;
            input_component_count = 1;
            max_mip_level = 0; // no mipmap

            OPENGL_CMD(glGenTextures(1, &mTexture));

            active(0);
            // OPENGL_CMD(glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

            setSize(_width, _height, _internal_format);

            deactive(0);
            // GLES20.glBindTexture(GL.GL_TEXTURE_2D, 0);
        }

        bool GLTexture::isInitialized()
        {
            return width > 0 && height > 0;
        }

        void GLTexture::uploadBuffer(const void *buffer)
        {
            ITK_ABORT(!isInitialized(), "You need to set the texture size and format before upload the buffer.");

            // check and active on texture 0
            GLint activeTexture;
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + 0));
            OPENGL_CMD(glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTexture));
            if (activeTexture != mTexture)
                active(0);

            // OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, input_alignment));
            OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, input_alignment));
            OPENGL_CMD(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, input_format, input_data_type, buffer));
            // OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 4));
            OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
        }

        void GLTexture::uploadBufferAlpha8(const void *buffer, int w, int h)
        {
            setSize(w, h, GL_ALPHA);
            uploadBuffer(buffer);
        }

        void GLTexture::uploadBufferRGB_888(const void *buffer, int w, int h, bool force_srgb)
        {
            if (force_srgb)
                setSize(w, h, GL_SRGB);
            else
                setSize(w, h, GL_RGB);
            uploadBuffer(buffer);
        }

        void GLTexture::uploadBufferRGBA_8888(const void *buffer, int w, int h, bool force_srgb)
        {
            if (force_srgb)
                setSize(w, h, GL_SRGB_ALPHA);
            else
                setSize(w, h, GL_RGBA);
            uploadBuffer(buffer);
        }

        //
        // Old way to copy framebuffer to texture... consider to use FBOs to acomplish it better
        //
        // can read rgb(GL_RGB) from framebuffer, or the depth component (GL_DEPTH_COMPONENT24)
        void GLTexture::setSize(int w, int h, GLuint format)
        {

            // check and active on texture 0
            GLint activeTexture;
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + 0));
            OPENGL_CMD(glGetIntegerv(GL_TEXTURE_BINDING_2D, &activeTexture));
            if (activeTexture != mTexture)
                active(0);

            if (w == 0)
                w = width;
            if (h == 0)
                h = height;

            bool power_of_two_w = w && !(w & (w - 1));
            bool power_of_two_h = h && !(h & (h - 1));

            if (!power_of_two_w || !power_of_two_h)
                ITK_ABORT(!GLAD_GL_ARB_texture_non_power_of_two && !GLAD_GL_OES_texture_npot, "ARB_texture_non_power_of_two not supported.");

            if (format == 0xffffffff)
                format = this->internal_format;

            if (w == (int)width && h == (int)height && format == internal_format)
                return;

            if (w == 0 || h == 0 || format == 0xffffffff)
                return;

            width = w;
            height = h;
            internal_format = format;
            is_depth_stencil = false;

            if (internal_format == GL_RGB)
            {
                input_format = GL_RGB;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 1;
                input_raw_element_size = sizeof(uint8_t) * 3;
                input_component_count = 3;
            }
            else if (internal_format == GL_RGBA)
            {
                input_format = GL_RGBA;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 4;
                input_raw_element_size = sizeof(uint8_t) * 4;
                input_component_count = 4;
            }
            else if (internal_format == GL_DEPTH_COMPONENT16 || internal_format == GL_DEPTH_COMPONENT24 || internal_format == GL_DEPTH_COMPONENT32)
            {
                input_format = GL_DEPTH_COMPONENT;
                input_data_type = GL_FLOAT;
                input_alignment = 4;
                input_raw_element_size = sizeof(float) * 1;
                input_component_count = 1;
                // depth buffer force to nearest filtering...
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            }
            else if (internal_format == GL_STENCIL_INDEX8)
            {
                input_format = GL_STENCIL_INDEX;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 1;
                input_raw_element_size = sizeof(uint8_t) * 1;
                input_component_count = 1;
                // index buffer force to nearest filtering...
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            }
            else if (internal_format == GL_DEPTH24_STENCIL8)
            {
                input_format = GL_DEPTH_STENCIL;
                input_data_type = GL_UNSIGNED_INT_24_8;
                input_alignment = 4;
                input_raw_element_size = sizeof(uint8_t) * 4;
                input_component_count = 1;
                // depth buffer force to nearest filtering...
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                is_depth_stencil = true;
            }
            else if (internal_format == GL_RGBA16F || internal_format == GL_RGBA32F)
            {
                input_format = GL_RGBA;
                input_data_type = GL_FLOAT;
                input_alignment = 4;
                input_raw_element_size = sizeof(float) * 4;
                input_component_count = 4;
            }
            else if (internal_format == GL_RGB16F || internal_format == GL_RGB32F)
            {
                input_format = GL_RGB;
                input_data_type = GL_FLOAT;
                input_alignment = 4;
                input_raw_element_size = sizeof(float) * 3;
                input_component_count = 3;
            }
            else if (internal_format == GL_R16F || internal_format == GL_R32F)
            {
                input_format = GL_R;
                input_data_type = GL_FLOAT;
                input_alignment = 4;
                input_raw_element_size = sizeof(float) * 1;
                input_component_count = 1;
            }
            else if (internal_format == GL_SRGB)
            {
                input_format = GL_RGB;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 1;
                input_raw_element_size = sizeof(uint8_t) * 3;
                input_component_count = 3;
            }
            else if (internal_format == GL_SRGB_ALPHA)
            {
                input_format = GL_RGBA;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 4;
                input_raw_element_size = sizeof(uint8_t) * 4;
                input_component_count = 4;
            }
            else if (internal_format == GL_R)
            {
                input_format = GL_R;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 1;
                input_raw_element_size = sizeof(uint8_t) * 1;
                input_component_count = 1;
            }
            else if (internal_format == GL_ALPHA)
            {
                input_format = GL_ALPHA;
                input_data_type = GL_UNSIGNED_BYTE;
                input_alignment = 1;
                input_raw_element_size = sizeof(uint8_t) * 1;
                input_component_count = 1;
            }
            else
            {
                ITK_ABORT(true, "Texture wrong format...");
            }
            ITK_ABORT((input_data_type == GL_FLOAT && !GLAD_GL_ARB_texture_float), "GL_ARB_texture_float not found");

            ITK_ABORT(((internal_format == GL_SRGB || internal_format == GL_SRGB_ALPHA) && !GLAD_GL_EXT_texture_sRGB), "Graphics driver does not support texture sRGB.");
            ITK_ABORT(input_format == GL_STENCIL_INDEX && !GLAD_GL_VERSION_4_4, "VERSION_4_4 not supported.");
            ITK_ABORT((input_format == GL_DEPTH_COMPONENT || input_format == GL_DEPTH_STENCIL) && !GLAD_GL_ARB_depth_texture, "ARB_depth_texture not supported.");

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, input_format, input_data_type, nullptr));
        }

        /*
        //copy the color buffer, or depth buffer, using the texture current size
        void GLTexture::copyFrameBuffer() {
            //OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));
            OPENGL_CMD(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height));
            //OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, 0));
        }
        */

        void GLTexture::active(int id) const
        {
#if !defined(GLAD_GLES2)
            if (id != 0)
                ITK_ABORT(!GLAD_GL_ARB_multitexture, "ARB_multitexture not supported.");
#endif

            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ITK_RPI)
            // OPENGL_CMD(glEnable(GL_TEXTURE_2D));
#endif
            OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, mTexture));
        }

        void GLTexture::deactive(int id) const
        {
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ITK_RPI)
            // OPENGL_CMD(glDisable(GL_TEXTURE_2D));
#endif
            OPENGL_CMD(glBindTexture(GL_TEXTURE_2D, 0));
        }

        GLTexture::~GLTexture()
        {
            if (mTexture != 0 && glIsTexture(mTexture))
                OPENGL_CMD(glDeleteTextures(1, &mTexture));
            mTexture = 0;
        }

        GLTexture *GLTexture::loadFromFile(const char *filename, bool invertY, bool sRGB)
        {
            int w, h, channels, depth;

            bool isPNG = ITKExtension::Image::PNG::isPNGFilename(filename);
            bool isJPG = ITKExtension::Image::JPG::isJPGFilename(filename);

            void (*closeFnc)(char *&) = nullptr;

            char *buffer = nullptr;
            if (isPNG)
            {
                buffer = ITKExtension::Image::PNG::readPNG(filename, &w, &h, &channels, &depth, invertY);
                closeFnc = &ITKExtension::Image::PNG::closePNG;
            }
            else if (isJPG)
            {
                buffer = ITKExtension::Image::JPG::readJPG(filename, &w, &h, &channels, &depth, invertY);
                closeFnc = &ITKExtension::Image::JPG::closeJPG;
            }

            ITK_ABORT(
                buffer == nullptr,
                "error to load: %s\n", filename);

            // if (buffer == nullptr) {
            //     fprintf( stderr, "error to load: %s\n", filename);
            //     exit(-1);
            // }

            int maxResolution = 8192;
#ifdef ITK_RPI
            maxResolution = 1024;
#endif

            if (channels == 1 && depth == 8)
            {
                GLTexture *result = new GLTexture();

                if (w > maxResolution || h > maxResolution)
                {
                    MipMapGenerator *generator = new MipMapGenerator((uint8_t *)buffer, w, h, 1);

                    for (size_t i = 0; i < generator->images.size(); i++)
                    {
                        MipMapImage *image = generator->images[i];
                        if (image->width <= maxResolution && image->height <= maxResolution)
                        {
                            result->uploadBufferAlpha8((const void *)image->buffer, image->width, image->height);
                            break;
                        }
                    }
                    delete generator;
                }
                else
                    result->uploadBufferAlpha8(buffer, w, h);
                // setnullptrAndDelete(buffer);
                closeFnc(buffer);
                return result;
            }
            else if (channels == 3 && depth == 8)
            {
                GLTexture *result = new GLTexture();

                if (w > maxResolution || h > maxResolution)
                {
                    MipMapGenerator *generator = new MipMapGenerator((uint8_t *)buffer, w, h, 3);

                    for (size_t i = 0; i < generator->images.size(); i++)
                    {
                        MipMapImage *image = generator->images[i];
                        if (image->width <= maxResolution && image->height <= maxResolution)
                        {
                            result->uploadBufferRGB_888((const void *)image->buffer, image->width, image->height, sRGB);
                            break;
                        }
                    }
                    delete generator;
                }
                else
                    result->uploadBufferRGB_888(buffer, w, h, sRGB);

                // setnullptrAndDelete(buffer);
                closeFnc(buffer);
                return result;
            }
            else if (channels == 4 && depth == 8)
            {
                GLTexture *result = new GLTexture();

                if (w > maxResolution || h > maxResolution)
                {
                    MipMapGenerator *generator = new MipMapGenerator((uint8_t *)buffer, w, h, 4);

                    for (size_t i = 0; i < generator->images.size(); i++)
                    {
                        MipMapImage *image = generator->images[i];
                        if (image->width <= maxResolution && image->height <= maxResolution)
                        {
                            result->uploadBufferRGBA_8888((const void *)image->buffer, image->width, image->height, sRGB);
                            break;
                        }
                    }
                    delete generator;
                }
                else
                    result->uploadBufferRGBA_8888(buffer, w, h, sRGB);
                // setnullptrAndDelete(buffer);
                closeFnc(buffer);
                return result;
            }

            closeFnc(buffer);
            // setnullptrAndDeleteArray(buffer);

            ITK_ABORT(
                true,
                "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);

            // fprintf(stderr, "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);
            // exit(-1);

            return nullptr;
        }

        TextureBuffer GLTexture::download(TextureBuffer *output)
        {

            ITK_ABORT(!GLAD_GL_ARB_framebuffer_object, "the download texture needs GL_ARB_framebuffer_object extension...");

            TextureBuffer result;

            if (output != nullptr)
            {
                if (output->data_size_bytes == width * height * input_raw_element_size)
                {
                    result = *output;
                    result.input_data_type = input_data_type;
                }
                else
                {
                    output->dispose();
                    result = TextureBuffer(input_data_type, width * height * input_raw_element_size);
                }
            }
            else
                result = TextureBuffer(input_data_type, width * height * input_raw_element_size);

            GLuint fbo;
            OPENGL_CMD(glGenFramebuffers(1, &fbo));
            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
            OPENGL_CMD(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexture, 0));

            OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, input_alignment));
            OPENGL_CMD(glReadPixels(0, 0, width, height, input_format, input_data_type, result.data));
            OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 4));

            OPENGL_CMD(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            OPENGL_CMD(glDeleteFramebuffers(1, &fbo));

            result.width = width;
            result.height = height;
            result.input_component_count = input_component_count;
            result.input_raw_element_size = input_raw_element_size;

            if (output != nullptr)
                *output = result;

            return result;
        }

        void GLTexture::writeToPNG(const char *filename, bool invertY)
        {
            ITK_ABORT(input_data_type != GL_UNSIGNED_BYTE, "writeToPNG only works with GL_UNSIGNED_BYTE data type.");

            TextureBuffer buffer = download(nullptr);
            ITKExtension::Image::PNG::writePNG(filename, width, height, input_component_count, (char *)buffer.data, !invertY);
            buffer.dispose();
        }

        void GLTexture::generateMipMap()
        {

#if !defined(GLAD_GLES2)
            ITK_ABORT(!GLAD_GL_SGIS_generate_mipmap, "SGIS_generate_mipmap not supported.");
#endif

            active(0);
            OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_2D));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

            max_mip_level = 0;

            int w = width;
            int h = height;

            while (w != 1 && h != 1)
            {
                w >>= 1;
                h >>= 1;
                if (w == 0)
                    w = 1;
                if (h == 0)
                    h = 1;
                max_mip_level++;
            }

            deactive(0);
        }

        void GLTexture::getMipResolution(int *mip, int *width, int *height)
        {

            if (max_mip_level < *mip)
                *mip = max_mip_level;

            int w = this->width >> (*mip);
            int h = this->height >> (*mip);

            if (w == 0)
                w = 1;
            if (h == 0)
                h = 1;

            *width = w;
            *height = h;
        }

        // Nvidia has 16.0 max
        void GLTexture::setAnisioLevel(float level)
        {
            if (GLAD_GL_EXT_texture_filter_anisotropic)
            {
                float aniso = 0.0f;
                OPENGL_CMD(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso));
                level = MathCore::OP<float>::clamp(level, 1.0f, aniso);
                OPENGL_CMD(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, level));
                printf("Anisotropic texture set to: %f\n", aniso);
            }
        }

        void GLTexture::setAsShadowMapFiltering(bool enable, GLint compare_as)
        {
#ifndef ITK_RPI
            active(0);
            if (enable)
            {
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE));
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, compare_as));
            }
            else
                OPENGL_CMD(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE));
            deactive(0);
#else
            printf("setAsShadowMapFiltering not implemented on RPI.\n");
#endif
        }

    }

}