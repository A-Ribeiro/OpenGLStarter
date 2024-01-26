#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <string>
// #include <aRibeiroCore/aRibeiroCore.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/shaders/GLShaderTextureColor.h>
#include <appkit-gl-base/VirtualTexture.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief Handle OpenGL CubeMap texture objects
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace AppKit::OpenGL;
        ///
        /// GLCubeMap *texture = new GLCubeMap();
        /// texture->loadFromFile(
        ///     "_negz.jpg","_posz.jpg",
        ///     "_negx.jpg","_posx.jpg",
        ///     "_negy.jpg","_posy.jpg" );
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
        class GLCubeMap : public VirtualTexture
        {

            void copySquareAndUpload(int x, int y, int w, int h,
                                     int channels,
                                     char *src, int src_stridex,
                                     char *dst,
                                     GLuint target,
                                     bool force_srgb);

        public:
            GLuint mCube;                    ///< the opengl texture ID
            int maxResolution;               ///< Contains the max resolution set to this cubemap
            MathCore::vec3f colorEstimation; ///< Contains the cubemap color estimation
            int width;                       ///< the texture width
            int height;                      ///< the texture height
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

            /// \brief Construct a cubemap
            ///
            /// The cubemap uses a lot of memory, to run in systems with less memory,
            /// need to limit the texture size.
            ///
            /// In the RaspberryPi 3 the max limit is hard set to 512.
            ///
            /// In the IntelHD Graphics 3000, the max limit is hard set to 1024.
            ///
            /// \author Alessandro Ribeiro
            ///
            GLCubeMap(GLsizei width = 0, GLsizei height = 0, GLint internal_format = GL_RGB, int _maxResolution = 1024);

            bool isInitialized();

            void uploadBuffer(GLuint cube_target, const void *buffer);

            /// \brief Active this texture to any OpenGL texture unit
            ///
            /// \author Alessandro Ribeiro
            /// \param id the texture unit you want to activate
            ///
            void active(int texUnit) const;

            /// \brief Disable any OpenGL texture unit
            ///
            /// \author Alessandro Ribeiro
            /// \param id the texture unit you want to deactivate
            ///
            void deactive(int texUnit) const;

            /// \brief Upload Alpha/Grayscale Texture
            ///
            /// You can access the texture value from the alpha channel inside the shader.
            ///
            /// The bit depth is 8 bits (uint8_t, unsigned char).
            ///
            /// The method loads unaligned data from the host memory (could be slower).
            ///
            /// \author Alessandro Ribeiro
            /// \param cube_target the cube texture target (GL_TEXTURE_CUBE_MAP_POSITIVE_X,...)
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferAlpha8(GLuint cube_target, const void *buffer, int w, int h);

            /// \brief Upload RGB24 bits Texture
            ///
            /// This method handle 3 channels, 8 bit depth per channel (uint8_t, unsigned char).
            ///
            /// The method loads unaligned data from the host memory (could be slower).
            ///
            /// \author Alessandro Ribeiro
            /// \param cube_target the cube texture target (GL_TEXTURE_CUBE_MAP_POSITIVE_X,...)
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferRGB_888(GLuint cube_target, const void *buffer, int w, int h, bool force_srgb);

            /// \brief Upload RGBA32 bits Texture
            ///
            /// This method handle 4 channels, 8 bit depth per channel (uint8_t, unsigned char).
            ///
            /// \author Alessandro Ribeiro
            /// \param cube_target the cube texture target (GL_TEXTURE_CUBE_MAP_POSITIVE_X,...)
            /// \param buffer a pointer to the texture image
            /// \param w texture width
            /// \param h texture height
            ///
            void uploadBufferRGBA_8888(GLuint cube_target, const void *buffer, int w, int h, bool force_srgb);

            // RPI only render to RGBA framebuffer...
            void setSize(int w, int h, GLuint internal_format = 0xffffffff); // int w, int h, GLuint format = GL_RGBA

            void generateMipMap();

            void getMipResolution(int *mip, int *width, int *height);

            /// \brief Load a texture from a PNG or JPEG file
            ///
            /// \author Alessandro Ribeiro
            /// \param cube_target the cube texture target (GL_TEXTURE_CUBE_MAP_POSITIVE_X,...)
            /// \param filename The file to be loaded
            /// \param invertY if true then will invert the y-axis from texture
            ///
            void loadFromFile_SingleSide(GLuint cube_target, const char *filename, bool invertY, bool force_srgb);

            /// \brief Load a complete set of textures to this cubemap
            ///
            /// \author Alessandro Ribeiro
            /// \param _negz The file to be loaded in the negative z coord
            /// \param _posz The file to be loaded in the positive z coord
            /// \param _negx The file to be loaded in the negative x coord
            /// \param _posx The file to be loaded in the positive x coord
            /// \param _negy The file to be loaded in the negative y coord
            /// \param _posy The file to be loaded in the positive y coord
            /// \param LeftHanded if true then will load the texture set in left handed coord system
            ///
            void loadFromFile(const std::string &_negz, const std::string &_posz,
                              const std::string &_negx, const std::string &_posx,
                              const std::string &_negy, const std::string &_posy,
                              bool force_srgb, bool LeftHanded);

            /// \brief Load a texture composed by 6 internal images
            ///
            /// The texture need to be power of two.
            ///
            /// The interior is organized like a cross.
            ///
            /// If the resolution of the internal image is 128x128,
            /// so the total image size will be 128*4 x 128*3.
            ///
            /// \author Alessandro Ribeiro
            /// \param single_file Single image cubemap.
            /// \param LeftHanded if true then will load the texture set in left handed coord system
            ///
            void loadFromSingleFile(std::string single_file,
                                    bool force_srgb, bool LeftHanded);

            ~GLCubeMap();

                    } ;

    }

}
