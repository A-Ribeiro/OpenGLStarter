#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
// #include <glew/glew.h> // extensions here
//#include <aRibeiroCore/common.h> // std types
//#include <aRibeiroCore/SetNullAndDelete.h>

#include <appkit-gl-base/VirtualTexture.h>

namespace AppKit
{

    namespace OpenGL
    {

        struct TextureBuffer
        {

            GLenum input_data_type; ///< GL_UNSIGNED_BYTE, GL_FLOAT, GL_UNSIGNED_INT_24_8
            int width;
            int height;
            int input_component_count;
            int input_raw_element_size; ///< sizeof(uint8_t)*3
            uint8_t *data;
            uint32_t data_size_bytes;

            TextureBuffer(GLenum _input_data_type = 0xffffffff, uint32_t size_bytes = 0)
            {
                data_size_bytes = size_bytes;
                input_data_type = _input_data_type;
                if (size_bytes == 0)
                    data = NULL;
                else
                    data = new uint8_t[size_bytes];
            }

            void dispose()
            {
                if (data != NULL) {
                    delete [] data;
                    data = NULL;
                }
            }
        };

        /// \brief Handle OpenGL 2D texture objects
        ///
        /// The common case is to load PNG files to use as texture.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace AppKit::OpenGL;
        ///
        /// GLTexture *texture = GLTexture::loadFromFile("texture.png");
        ///
        /// ...
        ///
        /// // active this texture in the OpenGL texture unit 0
        /// texture->active(0);
        ///
        /// // OpenGL Drawing Code
        /// ...
        ///
        /// // disable texture unit 0
        /// GLTexture::deactive(0);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLTexture : public VirtualTexture
        {

        public:
            GLuint mTexture; ///< OpenGL texture object
            GLsizei width;   ///< the texture width
            GLsizei height;  ///< the texture height
            // bool sRGB;

            // new variables to describe a texture
            bool is_depth_stencil;
            GLint internal_format;      ///< GL_SRGB, GL_DEPTH_COMPONENT24, GL_RGBA, ...
            GLenum input_format;        ///< GL_RGB, GL_RGBA, GL_DEPTH_COMPONENT, GL_ALPHA, ...
            GLenum input_data_type;     ///< GL_UNSIGNED_BYTE, GL_FLOAT, GL_UNSIGNED_INT_24_8
            GLint input_alignment;      ///< 1, 2, 4
            int input_raw_element_size; ///< sizeof(uint8_t)*3
            int input_component_count;  ///< 3
            GLint max_mip_level;        ///< max mip level

            bool isSRGB() const;

            GLTexture(GLsizei width = 0, GLsizei height = 0, GLint internal_format = GL_RGB);

            /// \brief Check if the texture is loaded (with a width and height)
            ///
            /// \author Alessandro Ribeiro
            ///
            bool isInitialized();

            void uploadBuffer(const void *buffer);

            /// \brief Upload Alpha/Grayscale Texture
            ///
            /// You can access the texture value from the alpha channel inside the shader.
            ///
            /// The bit depth is 8 bits (uint8_t, unsigned char).
            ///
            /// The method loads unaligned data from the host memory (could be slower).
            ///
            /// \author Alessandro Ribeiro
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferAlpha8(const void *buffer, int w, int h);

            /// \brief Upload RGB24 bits Texture
            ///
            /// This method handle 3 channels, 8 bit depth per channel (uint8_t, unsigned char).
            ///
            /// The method loads unaligned data from the host memory (could be slower).
            ///
            /// \author Alessandro Ribeiro
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferRGB_888(const void *buffer, int w, int h, bool force_srgb);

            /// \brief Upload RGBA32 bits Texture
            ///
            /// This method handle 4 channels, 8 bit depth per channel (uint8_t, unsigned char).
            ///
            /// \author Alessandro Ribeiro
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferRGBA_8888(const void *buffer, int w, int h, bool force_srgb);

            /// \brief Modify the size of the texture
            ///
            /// You can change the texture format when resizing.
            ///
            /// \author Alessandro Ribeiro
            /// \param w texture width
            /// \param h texture height
            /// \param internal_format GL_RGB, GL_RGBA, GL_ALPHA, etc...
            ///
            void setSize(int w, int h, GLuint internal_format = 0xffffffff);

            /// \brief Copy the current framebuffer to this texture
            ///
            /// You need to set the texture size before use this method.
            ///
            /// \author Alessandro Ribeiro
            ///
            // void copyFrameBuffer();

            /// \brief Active this texture to any OpenGL texture unit
            ///
            /// \author Alessandro Ribeiro
            /// \param id the texture unit you want to activate
            ///
            void active(int id) const;

            /// \brief Disable any OpenGL texture unit
            ///
            /// \author Alessandro Ribeiro
            /// \param id the texture unit you want to deactivate
            ///
            void deactive(int id) const;

            ~GLTexture();

            /// \brief Load a texture from a PNG or JPEG file
            ///
            /// \author Alessandro Ribeiro
            /// \param filename The file to be loaded
            /// \param invertY if true then will invert the y-axis from texture
            ///
            static GLTexture *loadFromFile(const char *filename, bool invertY = false, bool sRGB = false);

            TextureBuffer download(TextureBuffer *output);

            /// \brief Write this texture to a PNG file
            ///
            /// \author Alessandro Ribeiro
            /// \param filename The file to be writen
            /// \param invertY if true then will invert the y-axis from texture
            ///
            void writeToPNG(const char *filename, bool invertY = false);

            /// \brief Generate mipmap from the current texture.
            ///
            /// Uses the SGIS_generate_mipmap extension when available.
            ///
            /// \author Alessandro Ribeiro
            ///
            void generateMipMap();

            void getMipResolution(int *mip, int *width, int *height);

            /// \brief Set the anisiotropic filtering level
            ///
            /// Nvidia has 16.0f max.
            ///
            /// \author Alessandro Ribeiro
            /// \param level the anisiotropic filtering level to set
            ///
            void setAnisioLevel(float level);

            /// \brief Set the current texture to be used as shadow input
            ///
            /// Need depth buffer as float as extension.
            ///
            /// This method will enable texture fetch comparison from the shader.
            ///
            /// \author Alessandro Ribeiro
            /// \param enable enable or disable depth comparison
            ///
            void setAsShadowMapFiltering(bool enable = true, GLint compare_as = GL_LEQUAL);
        };

    }

}
