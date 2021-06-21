#ifndef ft2__h__
#define ft2__h__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

#include <aribeiro/aribeiro.h>

#include "FT2Glyph.h"

namespace ft2Wrapper {
    

    /// \brief Wrapper class to the FreeType 2 library
    ///
    /// The FreeType is used to render any font format to a bitmap.
    ///
    /// The good thing about freetype is that it loads a lot of font formats.
    ///
    /// True Type Font (TTF), Open Type Font (OTF), etc...
    ///
    /// With this class you can get the glyphs from freetype and<br />
    /// make some processing to construct a font atlas to be used<br />
    /// more efficiently with a rendering library like OpenGL, DirectX or Vulkan.
    /// 
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <ft2-wrapper/ft2-wrapper.h>
    /// using namespace aRibeiro;
    /// using namespace ft2Wrapper;
    ///
    /// ...
    ///
    /// FT2 ft2;
    /// ft2.readFromFile_PX(
    ///     "input_file.ttf", // inputFile
    ///     0, // face to select
    ///     1.2f, // outline thickness
    ///     60,// characterSize w
    ///     60 // characterSize h
    /// );
    ///
    /// // char code to query from freetype2
    /// UTF32* char_array;
    ///
    /// for ( int i=0 ; i < char_count ; i++ ) {
    ///     FT2Glyph *glyph = ft2.generateGlyph(char_array[i]);
    ///     if (glyph == NULL) {
    ///         printf("Glyph not found: %u\n", char_array[i]);
    ///         continue;
    ///     }
    ///
    ///     // here you can use the glyph bitmap information
    ///     ...
    ///
    ///     ft2.releaseGlyph(&glyph);
    /// }
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class FT2{
        
        FT_Library library;
        FT_Face face;
        FT_Stroker stroker;
        
    public:
        FT2();
        virtual ~FT2();
        void clear();
        
        /// \brief Open a font file using pixel size as reference.
        ///
        /// It is possible to use 2 modes to render pixelmap fonts:
        ///
        /// * The Pixel mode, using pixel as reference size;<br />
        /// * The DPI (Dots Per Inch) mode, using the device DPI and set the size as PT.
        ///
        /// The font file could have more than one font type inside the file. Example: normal, bold, italic, etc...
        ///
        /// You need to specify the face index inside the file.
        ///
        /// The last version of this reader supports the font stroker also. You need to specify in pixels units (float).
        /// 
        /// \author Alessandro Ribeiro
        /// \param filename The filename to load
        /// \param face_index The face index (default = 0)
        /// \param outlineThickness The thickness of the bitmap stroke to be generated
        /// \param max_width base pixel square area width reference
        /// \param max_height base pixel square area height reference
        ///
        void readFromFile_PX( const std::string &filename, FT_Int face_index = 0,
                             float outlineThickness = 1.0f,
                             FT_UInt max_width = 60, FT_UInt max_height = 60);
        
        
        ///    windows default DPI = 96
        ///    mac default DPI = 72
        ///    iphone4 - 960-by-640-pixel resolution at 326 ppi
        ///    iphone - 480 x 320 163 pixels per inch
        ///    ipod touch - 480 x 320 160 pixels per inch

        /// \brief Open a font file using DPI and PT size as reference.
        ///
        /// It is possible to use 2 modes to render pixelmap fonts:
        ///
        /// * The Pixel mode, using pixel as reference size;<br />
        /// * The DPI (Dots Per Inch) mode, using the device DPI and set the size as PT.
        ///
        /// The font file could have more than one font type inside the file. Example: normal, bold, italic, etc...
        ///
        /// You need to specify the face index inside the file.
        ///
        /// The last version of this reader supports the font stroker also. You need to specify in pixels units (float).
        ///
        /// There are some values of DPI configuration (found in the internet): <br />
        /// * windows default DPI = 96
        /// * mac default DPI = 72
        /// 
        /// \author Alessandro Ribeiro
        /// \param filename The filename to load
        /// \param face_index The face index (default = 0)
        /// \param outlineThickness The thickness of the bitmap stroke to be generated
        /// \param DPI Device Per Inch metrics
        /// \param fontPT Font size
        ///
        void readFromFile_DPI(const std::string &filename, FT_Int face_index = 0,
                              float outlineThickness = 1.0f,
                              FT_UInt DPI = 96, float fontPT = 9);

        /// \brief Generate one glyph
        ///
        /// This method render two bitmaps (the normal face and the stroke face).
        ///
        /// You can use the stroke bitmap to render the outline.
        ///
        /// After call this method, if it returns any value != NULL then you need to<br />
        /// release it with #releaseGlyph method.
        /// 
        /// \author Alessandro Ribeiro
        /// \param charcode_utf32 UTF32 char code
        /// \return #FT2Glyph
        ///
        FT2Glyph* generateGlyph(FT_ULong charcode_utf32);

        /// \brief Release a glyph generated with #generateGlyph
        ///
        /// \author Alessandro Ribeiro
        /// \param glyph the glyph to release
        ///
        void releaseGlyph(FT2Glyph**glyph);
        
    };
    
}

#endif
