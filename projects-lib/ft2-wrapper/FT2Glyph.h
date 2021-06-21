#ifndef ft2_glyph__h__
#define ft2_glyph__h__

#include <aribeiro/aribeiro.h>
#include "FT2Rect.h"

namespace ft2Wrapper {
    
    /// \brief FT2 wrapper glyph representation
    ///
    /// It is possible to use this class as an auxiliary structure with the glyph information.
    ///
    /// It holds the font metrics and the bitmap information in grayscale and RGBA format.
    ///
    /// \author Alessandro Ribeiro
    ///
    class FT2Glyph{
        
        //private copy constructores, to avoid copy...
        FT2Glyph(const FT2Glyph& v);
        void operator=(const FT2Glyph& v);
        
    public:
        
        uint32_t charcode;///< UTF32 character code
        
        float advancex;///< Horizontal advance in pixels units
        float advancey;///< Vertical advance in pixels units (rarely used)
        
        FT2Rect normalRect;///< Normal face rectangle reference (top, left, width, height)
        uint8_t *bitmapGrayNormal;///< RAW grayscale font pixels (size = width*height)
        uint8_t *bitmapRGBANormal;///< RAW RGBA font pixels (size = width*height*4)
        
        
        FT2Rect strokeRect;///< Stroke(outline) face rectangle reference (top, left, width, height)
        uint8_t *bitmapGrayStroke;///< RAW grayscale font pixels (size = width*height)
        uint8_t *bitmapRGBAStroke;///< RAW RGBA font pixels (size = width*height*4)
        
        /// \brief Copy the normal face buffer from FreeType2 to this glyph representation
        ///
        /// The source buffer (from FreeType2) need to have the size = width*height.
        ///
        /// It uses the normalRect to know the bitmap size.
        ///
        /// It will generate the RGBA representation of the input buffer.
        ///
        /// \author Alessandro Ribeiro
        /// \param src The FreeType2 source raw normal bitmap buffer
        /// \param invert Should to invert the y coord of the input buffer.
        ///
        void copyNormalBuffer(uint8_t *src, bool invert);

        /// \brief Copy the stroke(outline) face buffer from FreeType2 to this glyph representation
        ///
        /// The source buffer (from FreeType2) need to have the size = width*height.
        ///
        /// It uses the strokeRect to know the bitmap size.
        ///
        /// It will generate the RGBA representation of the input buffer.
        ///
        /// \author Alessandro Ribeiro
        /// \param src The FreeType2 source raw stroke bitmap buffer
        /// \param invert Should to invert the y coord of the input buffer.
        ///
        void copyStrokeBuffer(uint8_t *src, bool invert);

        /// \brief Save the grayscale normal buffer to a PNG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename to save
        ///
        void saveNormalGrayPNG(const std::string& filename);

        /// \brief Save the grayscale stroke(outline) buffer to a PNG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename to save
        ///
        void saveStrokeGrayPNG(const std::string& filename);

        /// \brief Save the RGBA normal buffer to a PNG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename to save
        ///
        void saveNormalRGBAPNG(const std::string& filename);

        /// \brief Save the RGBA stroke(outline) buffer to a PNG file
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The filename to save
        ///
        void saveStrokeRGBAPNG(const std::string& filename);
        
        FT2Glyph();
        virtual ~FT2Glyph();
    };
    
}

#endif
