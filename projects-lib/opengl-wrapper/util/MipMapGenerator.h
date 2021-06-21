#ifndef MIP_MAP_GENERATOR_h
#define MIP_MAP_GENERATOR_h

#include <string>
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLVertexBufferObject.h>
#include <opengl-wrapper/GLShaderTextureColor.h>

namespace openglWrapper {
    
    /// \brief One MipMap level representation
    ///
    /// This structure is used inside the #MipMapGenerator class.
    ///
    /// The minification filter is implemented inside this class.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE MipMapImage{
    public:
        uint8_t* buffer;
        int width;
        int height;
        int channels;

        MipMapImage(int width, int height, int channels);
        void copyBuffer(uint8_t* _buffer);
        uint8_t getPixel(int x, int y, int component)const;
        void setPixel(int x, int y, int component, uint8_t v);
        void copyBufferResample(const MipMapImage& src);
        ~MipMapImage();

        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS ;
    

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
    /// #include <aribeiro/aribeiro.h>
    /// #include <opengl-wrapper/opengl-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace openglWrapper;
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
    class _SSE2_ALIGN_PRE MipMapGenerator {
        
    public:
        std::vector<MipMapImage*> images;///<The MIP array. The position 0 is the largest image.
        
        /// \brief Construct the MipMap
        ///
        /// \param buffer the input image
        /// \param width the width of the input image
        /// \param height the height of the input image
        /// \param channels the number of channels the image has
        /// \author Alessandro Ribeiro
        ///
        MipMapGenerator(uint8_t* buffer,
                        int width,
                        int height,
                        int channels);
        ~MipMapGenerator();
        
        SSE2_CLASS_NEW_OPERATOR
    } _SSE2_ALIGN_POS ;
    
}

#endif

