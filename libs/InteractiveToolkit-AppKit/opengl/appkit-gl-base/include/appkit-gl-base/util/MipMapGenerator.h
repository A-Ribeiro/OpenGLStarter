#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <string>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLVertexBufferObject.h>
#include <appkit-gl-base/shaders/GLShaderTextureColor.h>

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief One MipMap level representation
        ///
        /// This structure is used inside the #MipMapGenerator class.
        ///
        /// The minification filter is implemented inside this class.
        ///
        /// \author Alessandro Ribeiro
        ///
        class MipMapImage
        {
        public:
            uint8_t *buffer;
            int width;
            int height;
            int channels;

            MipMapImage(int width, int height, int channels);
            void copyBuffer(uint8_t *_buffer);
            uint8_t getPixel(int x, int y, int component) const;
            void setPixel(int x, int y, int component, uint8_t v);
            void copyBufferResample(const MipMapImage &src);
            ~MipMapImage();
        };

        /// \brief Software MIPMAP generator
        ///
        /// The input image needs to be power of two.
        ///
        /// It generates all MIP levels from that image.
        ///
        /// The filtering is done by sample 4 coords from the higher MIP to the next lower level.
        ///
        /// The MIPMAP algorithm finish when the image of 1x1 pixel is reached.
        ///
        /// Example:
        ///
        /// \code
        /// #include <appkit-gl-base/opengl-wrapper.h>
        /// using namespace AppKit::OpenGL;
        ///
        /// ...
        ///
        /// MipMapGenerator *mipMapGenerator;
        ///
        /// uint8_t* imageGrayScale;
        /// int width;
        /// int height;
        ///
        /// mipMapGenerator = new MipMapGenerator(imageGrayScale,width,height,1);
        ///
        /// //use the mipMapGenerator->images[...] to access the desired MIP level.
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        class MipMapGenerator
        {

        public:
            std::vector<MipMapImage *> images; ///< The MIP array. The position 0 is the largest image.

            /// \brief Construct the MipMap
            ///
            /// \param buffer the input image
            /// \param width the width of the input image
            /// \param height the height of the input image
            /// \param channels the number of channels the image has
            /// \author Alessandro Ribeiro
            ///
            MipMapGenerator(uint8_t *buffer,
                            int width,
                            int height,
                            int channels);
            ~MipMapGenerator();
        };

    }

}