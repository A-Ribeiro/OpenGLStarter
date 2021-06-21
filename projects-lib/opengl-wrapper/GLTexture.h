#ifndef GL_TEXTURE_h
#define GL_TEXTURE_h

#include <glew/glew.h> // extensions here

namespace openglWrapper {

    /// \brief Handle OpenGL 2D texture objects
    ///
    /// The common case is to load PNG files to use as texture.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <opengl-wrapper/opengl-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace openglWrapper;
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
    class GLTexture {

    private:

        static int originalPackAlignment;
        static int originalUnpackAlignment;

    public:
        GLuint mTexture;///< OpenGL texture object
        int width;///<the texture width
        int height;///<the texture height
        bool sRGB;

        GLTexture(bool _sRGB = false);

        /// \brief Check if the texture is loaded (with a width and height)
        ///
        /// \author Alessandro Ribeiro
        ///
        bool isInitialized();

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
        void uploadBufferAlpha8(const void* buffer, int w, int h);

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
        void uploadBufferRGB_888(const void* buffer, int w, int h);

        /// \brief Upload RGBA32 bits Texture
        ///
        /// This method handle 4 channels, 8 bit depth per channel (uint8_t, unsigned char).
        ///
        /// \author Alessandro Ribeiro
        /// \param buffer a pointer to the texture image
        /// \param w texture width
        /// \param h texture height
        ///
        void uploadBufferRGBA_8888(const void* buffer, int w, int h);

        /// \brief Modify the size of the texture
        ///
        /// You can change the texture format when resizing.
        ///
        /// \author Alessandro Ribeiro
        /// \param w texture width
        /// \param h texture height
        /// \param format GL_RGB, GL_RGBA, GL_ALPHA, etc...
        ///
        void setSize(int w, int h, GLuint format = GL_RGB);

        /// \brief Copy the current framebuffer to this texture
        ///
        /// You need to set the texture size before use this method.
        ///
        /// \author Alessandro Ribeiro
        ///
        void copyFrameBuffer();

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
        static void deactive(int id);

        virtual ~GLTexture();

        /// \brief Load a texture from a PNG or JPEG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The file to be loaded
        /// \param invertY if true then will invert the y-axis from texture
        ///
        static GLTexture *loadFromFile(const char* filename, bool invertY = false, bool sRGB = false);
        

        /// \brief Write this texture to a PNG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The file to be writen
        /// \param invertY if true then will invert the y-axis from texture
        ///
        void writeToPNG(const char* filename, bool invertY = false);

        /// \brief Generate mipmap from the current texture.
        ///
        /// Uses the SGIS_generate_mipmap extension when available.
        ///
        /// \author Alessandro Ribeiro
        ///
        void generateMipMap();

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
        void setAsShadowMapFiltering(bool enable = true);
    };

}

#endif
