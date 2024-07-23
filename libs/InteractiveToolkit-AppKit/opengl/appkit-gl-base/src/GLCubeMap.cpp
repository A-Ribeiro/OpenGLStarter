// #include <glew/glew.h>

#include <appkit-gl-base/GLCubeMap.h>

// #include <appkit-gl-engine/mini-gl-engine.h>

#include <InteractiveToolkit/Platform/Time.h>       // PlatformTime
#include <InteractiveToolkit-Extension/image/JPG.h> // JPGHelper
#include <InteractiveToolkit-Extension/image/PNG.h> // PNGHelper

#include <appkit-gl-base/platform/PlatformGL.h>

#include <appkit-gl-base/util/MipMapGenerator.h>

namespace AppKit
{

    namespace OpenGL
    {

        void GLCubeMap::copySquareAndUpload(int x, int y, int w, int h,
                                            int channels,
                                            char *src, int src_stridex,
                                            char *dst,
                                            GLuint target,
                                            bool force_srgb)
        {

            for (int _y = 0; _y < h; _y++)
            {
                memcpy(&dst[_y * w * channels],
                       &src[(y + _y) * src_stridex + x * channels],
                       w * channels);
            }

            if (channels == 1)
            {
                uploadBufferAlpha8(target, dst, w, h);
            }
            else if (channels == 3)
            {
                uploadBufferRGB_888(target, dst, w, h, force_srgb);
            }
            else if (channels == 4)
            {
                uploadBufferRGBA_8888(target, dst, w, h, force_srgb);
            }
        }

        bool GLCubeMap::isSRGB() const
        {
            return internal_format == GL_SRGB || internal_format == GL_SRGB_ALPHA;
        }

        GLCubeMap::GLCubeMap(GLsizei _width, GLsizei _height, GLint _internal_format, int _maxResolution)
        {

            maxResolution = _maxResolution;
#if ITK_RPI
            // tested on Raspbery Pi 3
            if (maxResolution > 512)
                maxResolution = 512;
#else
            // tested on Intel HD3000
            if (maxResolution > 1024)
                maxResolution = 1024;
#endif

            mCube = 0;
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

            printf("Creating a cubemap with the resolution: %i\n", maxResolution);
#if !defined(GLAD_GLES2)
            ITK_ABORT(!GLAD_GL_ARB_texture_cube_map, "Error, this hardware does not support ARB_texture_cube_map...\n");
#endif
            OPENGL_CMD(glGenTextures(1, &mCube));

            active(0);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            setSize(_width, _height, _internal_format);

            deactive(0);
        }

        bool GLCubeMap::isInitialized()
        {
            return width > 0 && height > 0;
        }

        void GLCubeMap::uploadBuffer(GLuint cube_target, const void *buffer)
        {
            ITK_ABORT(!isInitialized(), "You need to set the texture size and format before upload the buffer.");

            // check and active on texture 0
            GLint activeTexture;
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + 0));
            OPENGL_CMD(glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &activeTexture));
            if (activeTexture != mCube)
                active(0);

            // OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, input_alignment));
            OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, input_alignment));
            OPENGL_CMD(glTexSubImage2D(cube_target, 0, 0, 0, width, height, input_format, input_data_type, buffer));
            // OPENGL_CMD(glPixelStorei(GL_PACK_ALIGNMENT, 4));
            OPENGL_CMD(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
        }

        void GLCubeMap::active(int id) const
        {
#if !defined(GLAD_GLES2)
            if (id != 0)
                ITK_ABORT(!GLAD_GL_ARB_multitexture, "ARB_multitexture not supported.");
#endif
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ITK_RPI)
            // OPENGL_CMD(glEnable(GL_TEXTURE_2D));
#endif
            OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, mCube));
        }

        void GLCubeMap::deactive(int id) const
        {
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + id));
#if !defined(ITK_RPI)
            // OPENGL_CMD(glDisable(GL_TEXTURE_2D));
#endif
            OPENGL_CMD(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        }

        GLCubeMap::~GLCubeMap()
        {
            if (mCube != 0 && glIsTexture(mCube))
                OPENGL_CMD(glDeleteTextures(1, &mCube));
            mCube = 0;
        }

        void GLCubeMap::uploadBufferAlpha8(GLuint cube_target, const void *buffer_, int w_, int h_)
        {
            // mip reduction / color estimation
            const void *buffer = buffer_;
            int w = w_;
            int h = h_;

            MipMapGenerator *generator = nullptr;
            {
                Platform::Time timer;

                printf("Generating mipmap on CPU...\n");
                timer.update();
                generator = new MipMapGenerator((uint8_t *)buffer_, w_, h_, 1);
                for (size_t i = 0; i < generator->images.size(); i++)
                {
                    MipMapImage *image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution)
                    {
                        buffer = (const void *)image->buffer;
                        w = image->width;
                        h = image->height;
                        break;
                    }
                }
                timer.update();
                printf("Time: %f\n", timer.deltaTime);

                MathCore::vec3f estimation = MathCore::vec3f(
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 0)) /
                                             255.0f;

                // texture to Gamma...
                estimation.x = pow(estimation.x, 2.2f);
                estimation.y = pow(estimation.y, 2.2f);
                estimation.z = pow(estimation.z, 2.2f);

                estimation /= 6.0f; // 6 sides

                colorEstimation += estimation;
            }

            setSize(w, h, GL_ALPHA);
            uploadBuffer(cube_target, buffer);

            delete generator;
        }

        void GLCubeMap::uploadBufferRGB_888(GLuint cube_target, const void *buffer_, int w_, int h_, bool force_srgb)
        {

            const void *buffer = buffer_;
            int w = w_;
            int h = h_;

            MipMapGenerator *generator = nullptr;
            {
                Platform::Time timer;

                printf("Generating mipmap on CPU...\n");
                timer.update();
                generator = new MipMapGenerator((uint8_t *)buffer_, w_, h_, 3);
                for (size_t i = 0; i < generator->images.size(); i++)
                {
                    MipMapImage *image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution)
                    {
                        buffer = (const void *)image->buffer;
                        w = image->width;
                        h = image->height;
                        break;
                    }
                }
                timer.update();
                printf("Time: %f\n", timer.deltaTime);

                MathCore::vec3f estimation = MathCore::vec3f(
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 0),
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 1),
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 2)) /
                                             255.0f;

                // texture to Gamma...
                estimation.x = pow(estimation.x, 2.2f);
                estimation.y = pow(estimation.y, 2.2f);
                estimation.z = pow(estimation.z, 2.2f);

                estimation /= 6.0f; // 6 sides

                colorEstimation += estimation;
            }

            if (force_srgb)
                setSize(w, h, GL_SRGB);
            else
                setSize(w, h, GL_RGB);
            uploadBuffer(cube_target, buffer);

            delete generator;
        }

        void GLCubeMap::uploadBufferRGBA_8888(GLuint cube_target, const void *buffer_, int w_, int h_, bool force_srgb)
        {

            const void *buffer = buffer_;
            int w = w_;
            int h = h_;

            MipMapGenerator *generator = nullptr;
            {
                Platform::Time timer;

                printf("Generating mipmap on CPU...\n");
                timer.update();
                generator = new MipMapGenerator((uint8_t *)buffer_, w_, h_, 4);
                for (size_t i = 0; i < generator->images.size(); i++)
                {
                    MipMapImage *image = generator->images[i];
                    if (image->width <= maxResolution && image->height <= maxResolution)
                    {
                        buffer = (const void *)image->buffer;
                        w = image->width;
                        h = image->height;
                        break;
                    }
                }
                timer.update();
                printf("Time: %f\n", timer.deltaTime);

                float alpha = (float)generator->images[generator->images.size() - 1]->getPixel(0, 0, 3) / 255.0f;

                MathCore::vec3f estimation = MathCore::vec3f(
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 0),
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 1),
                                                 generator->images[generator->images.size() - 1]->getPixel(0, 0, 2)) /
                                             255.0f;

                // texture to Gamma...
                estimation.x = pow(estimation.x, 2.2f);
                estimation.y = pow(estimation.y, 2.2f);
                estimation.z = pow(estimation.z, 2.2f);

                estimation /= 6.0f; // 6 sides

                estimation *= alpha;

                colorEstimation += estimation;
            }

            if (force_srgb)
                setSize(w, h, GL_SRGB_ALPHA);
            else
                setSize(w, h, GL_RGBA);
            uploadBuffer(cube_target, buffer);

            delete generator;
        }

        void GLCubeMap::setSize(int w, int h, GLuint format)
        {

            // check and active on texture 0
            GLint activeTexture;
            OPENGL_CMD(glActiveTexture(GL_TEXTURE0 + 0));
            OPENGL_CMD(glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &activeTexture));
            if (activeTexture != mCube)
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

            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
            OPENGL_CMD(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internal_format, width, height, 0, input_format, input_data_type, 0));
        }

        void GLCubeMap::generateMipMap()
        {

#if !defined(GLAD_GLES2)
            ITK_ABORT(!GLAD_GL_SGIS_generate_mipmap, "SGIS_generate_mipmap not supported.");
#endif

            active(0);
            OPENGL_CMD(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            OPENGL_CMD(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

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

        void GLCubeMap::getMipResolution(int *mip, int *width, int *height)
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

        void GLCubeMap::loadFromFile_SingleSide(GLuint cube_target, const char *filename, bool invertY, bool force_srgb)
        {
            int w, h, channels, depth;

            char *buffer = nullptr;

            bool isPNG = ITKExtension::Image::PNG::isPNGFilename(filename);
            bool isJPG = ITKExtension::Image::JPG::isJPGFilename(filename);

            void (*closeFnc)(char *&) = nullptr;

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

            GLint max_tex_size;
            OPENGL_CMD(glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &max_tex_size));

            ITK_ABORT((w > max_tex_size || h > max_tex_size),
                      "Max cube texture size (%i) overloading. File: %s\n", max_tex_size, filename);

            if (channels == 1 && depth == 8)
            {
                uploadBufferAlpha8(cube_target, buffer, w, h);
                closeFnc(buffer);
                return;
            }
            else if (channels == 3 && depth == 8)
            {
                uploadBufferRGB_888(cube_target, buffer, w, h, force_srgb);
                closeFnc(buffer);
                return;
            }
            else if (channels == 4 && depth == 8)
            {
                uploadBufferRGBA_8888(cube_target, buffer, w, h, force_srgb);
                closeFnc(buffer);
                return;
            }

            closeFnc(buffer);
            ITK_ABORT(
                true,
                "invalid image format: %d channels %d depth. Error to load: %s\n", channels, depth, filename);
            return;
        }

        void GLCubeMap::loadFromFile(const std::string &_negz, const std::string &_posz,
                                     const std::string &_negx, const std::string &_posx,
                                     const std::string &_negy, const std::string &_posy,
                                     bool force_srgb, bool LeftHanded)
        {

            colorEstimation = MathCore::vec3f(0);

            if (LeftHanded)
            {
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, _posx.c_str(), false, force_srgb);
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _negx.c_str(), false, force_srgb);
            }
            else
            {
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X, _negx.c_str(), false, force_srgb);
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, _posx.c_str(), false, force_srgb);
            }

            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, _posy.c_str(), false, force_srgb);
            loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, _negy.c_str(), false, force_srgb);

            // Left Handed load
            if (LeftHanded)
            {
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _posz.c_str(), false, force_srgb);
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _negz.c_str(), false, force_srgb);
            }
            else
            {
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, _negz.c_str(), false, force_srgb);
                loadFromFile_SingleSide(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, _posz.c_str(), false, force_srgb);
            }

            // Gamma to texture
            // colorEstimation = colorEstimation / (colorEstimation + MathCore::vec3f(1));
            colorEstimation.x = pow(colorEstimation.x, 1.0f / 2.2f);
            colorEstimation.y = pow(colorEstimation.y, 1.0f / 2.2f);
            colorEstimation.z = pow(colorEstimation.z, 1.0f / 2.2f);

            generateMipMap();
        }

        void GLCubeMap::loadFromSingleFile(std::string single_file,
                                           bool force_srgb, bool LeftHanded)
        {

            colorEstimation = MathCore::vec3f(0);

            int w, h, channels, depth;

            bool isPNG = ITKExtension::Image::PNG::isPNGFilename(single_file.c_str());
            bool isJPG = ITKExtension::Image::JPG::isJPGFilename(single_file.c_str());

            char *buffer = nullptr;

            if (isPNG)
                buffer = ITKExtension::Image::PNG::readPNG(single_file.c_str(), &w, &h, &channels, &depth, false);
            else if (isJPG)
                buffer = ITKExtension::Image::JPG::readJPG(single_file.c_str(), &w, &h, &channels, &depth, false);

            ITK_ABORT(
                buffer == nullptr,
                "error to load: %s\n", single_file.c_str());

            ITK_ABORT((depth != 8), "Bit depth (%i) not supported.\n", depth);

            bool power_of_two_w = w && !(w & (w - 1));
            bool power_of_two_h = h && !(h & (h - 1));

            if (!power_of_two_w || !power_of_two_h)
            {
                ITK_ABORT(true, "non Power Of Two cubemap: %s\n", single_file.c_str());
            }
            // ITK_ABORT(!GLAD_GL_ARB_texture_non_power_of_two,"ARB_texture_non_power_of_two not supported.");

            GLuint target_left, target_right, target_top, target_bottom, target_front, target_back;

            if (LeftHanded)
            {
                target_left = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
                target_right = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            }
            else
            {
                target_left = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
                target_right = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            }

            target_top = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            target_bottom = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

            if (LeftHanded)
            {
                target_front = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
                target_back = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
            }
            else
            {
                target_front = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
                target_back = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            }

            // load each buffer...
            int internal_w = w / 4;
            int internal_h = h / 3;

            char *internal_buffer = new char[internal_w * internal_h * channels];

            // left
            copySquareAndUpload(0, internal_h, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_left,
                                force_srgb);

            // right
            copySquareAndUpload(internal_w * 2, internal_h, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_right,
                                force_srgb);

            // front
            copySquareAndUpload(internal_w, internal_h, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_front,
                                force_srgb);

            // back
            copySquareAndUpload(internal_w * 3, internal_h, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_back,
                                force_srgb);

            // top
            copySquareAndUpload(internal_w, 0, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_top,
                                force_srgb);

            // down
            copySquareAndUpload(internal_w, internal_h * 2, internal_w, internal_h,
                                channels,
                                buffer, w * channels,
                                internal_buffer,
                                target_bottom,
                                force_srgb);

            delete[] internal_buffer;

            if (isPNG)
                ITKExtension::Image::PNG::closePNG(buffer);
            else if (isJPG)
                ITKExtension::Image::JPG::closeJPG(buffer);

            // Gamma to texture
            // colorEstimation = colorEstimation / (colorEstimation + MathCore::vec3f(1));
            colorEstimation.x = pow(colorEstimation.x, 1.0f / 2.2f);
            colorEstimation.y = pow(colorEstimation.y, 1.0f / 2.2f);
            colorEstimation.z = pow(colorEstimation.z, 1.0f / 2.2f);

            generateMipMap();
        }

    }

}