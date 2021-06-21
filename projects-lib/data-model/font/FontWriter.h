#ifndef fontwriter__h__
#define fontwriter__h__

#include <aribeiro/aribeiro.h>
#include <vector>
#include <map>

#include <data-model/Atlas.h>
#include "FontWriterGlyph.h"

namespace DataModel {

    /// \brief Save a font character set using the basof2 or asbgt2 format.
    ///
    /// The basof2 (Binary ASilva OpenGL Font, version 2.0) or<br />
    /// asbgt2 (ASilva Binary Glyph Table, version 2.0)<br />
    /// file format exports 2 glyphs information (normal and stroke).
    ///
    /// This class handle the correct writing pattern of glyph table and pixelmap (grayscale).
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// #include <data-model/data-model.h>
    /// using namespace aRibeiro;
    /// using namespace DataModel;
    ///
    /// Atlas atlas = ...;
    ///
    /// ...
    ///
    /// FontWriter fontWriter;
    ///
    /// float characterSize;
    /// float spaceWidth;
    /// float newLineHeight;
    /// 
    /// fontWriter.initFromAtlas(&atlas, characterSize, spaceWidth, newLineHeight);
    ///
    /// for (int i=0;i<utf32data->count();i++){
    ///     AtlasElement* atlasElementFace = ...
    ///     AtlasElement* atlasElementStroke = ...
    ///
    ///     fontWriter.setCharacter(...);
    /// }
    ///
    /// atlas.organizePositions(false);
    ///
    /// fontWriter.save( "file.basof2" );
    /// fontWriter.saveGlyphTable( "file.asbgt2" );
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class FontWriter {
        
        //private copy constructores, to avoid copy...
        FontWriter(const FontWriter& v);
        void operator=(const FontWriter& v);

    public:
        Atlas *atlas;///< reference to sprite #Atlas of this font

        std::map<uint32_t, FontWriterGlyph*> glyphmap;///< the glyphmap (char code to FontWriterGlyph mapping)

        float space_width;///< white space horizontal increment
        float new_line_height;///< new line height
        float max_square_size_px;///< font reference matrix size
        
        FontWriter();

        virtual ~FontWriter();

        /// \brief Setup this FontWriter to work with an #Atlas instance
        ///
        /// The font writer manages the mapping of a character to a sprite inside this atlas.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// Atlas atlas = ...;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// float characterSize;
        /// float spaceWidth;
        /// float newLineHeight;
        /// 
        /// fontWriter.initFromAtlas(&atlas, characterSize, spaceWidth, newLineHeight);
        ///
        /// ...
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void initFromAtlas(Atlas *_atlas, float _max_square_size_px, float _space_width, float _new_line_height);

        /// \brief Setup one character inside this font character set.
        ///
        /// Cannot insert the same character twice.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// ...
        ///
        /// for (int i=0;i<utf32data->count();i++){
        ///     ...
        ///     AtlasElement* atlasElementFace = ...
        ///     AtlasElement* atlasElementStroke = ...
        ///
        ///     fontWriter.setCharacter(
        ///         char_code[i],
        ///         advancex,
        ///         face_top,face_left,atlasElementFace,
        ///         stroke_top,stroke_left,atlasElementStroke
        ///     );
        /// }
        ///
        /// ...
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param charcode UTF32 character code
        /// \param advancex Horizontal character advance
        /// \param ftop Character face origin top
        /// \param fleft Character face origin left
        /// \param atlasElementFace Character normal face reference
        /// \param stop Character stroke origin top
        /// \param sleft Character stroke origin left
        /// \param atlasElementStroke Character stroke face reference
        ///
        void setCharacter(uint32_t charcode,
            float advancex,
            uint16_t ftop,
            uint16_t fleft,
            AtlasElement* atlasElementFace,
            uint16_t stop,
            uint16_t sleft,
            AtlasElement* atlasElementStroke);

        /// \brief Save basof2 file
        ///
        /// The basof2 (Binary ASilva OpenGL Font, version 2.0) or<br />
        /// asbgt2 (ASilva Binary Glyph Table, version 2.0)<br />
        /// file format exports 2 glyphs information (normal and stroke).
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// ...
        ///
        /// fontWriter.save( "file.basof2" );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param filename basof2 filename to save
        ///
        void save(const char* filename);

        /// \brief Save asbgt2 file
        ///
        /// The basof2 (Binary ASilva OpenGL Font, version 2.0) or<br />
        /// asbgt2 (ASilva Binary Glyph Table, version 2.0)<br />
        /// file format exports 2 glyphs information (normal and stroke).
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// ...
        ///
        /// fontWriter.saveGlyphTable( "file.asbgt2" );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param filename basof2 filename to save
        ///
        void saveGlyphTable(const char* filename);

        /// \brief Write the UTF32 character, double glyph map table
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// ...
        ///
        /// BinaryWriter writer;
        /// writer.writeToFile("file.output");
        ///
        /// fontWriter.writeGlyphTable(&writer);
        ///
        /// writer.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param writer The #aRibeiro::BinaryWriter instance
        ///
        void writeGlyphTable(aRibeiro::BinaryWriter *writer);


        /// \brief Write the bitmap compressed using PNG format
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// ...
        ///
        /// FontWriter fontWriter;
        ///
        /// ...
        ///
        /// BinaryWriter writer;
        /// writer.writeToFile("file.output");
        ///
        /// fontWriter.writeBitmap(&writer);
        ///
        /// writer.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param writer The #aRibeiro::BinaryWriter instance
        ///
        void writeBitmap(aRibeiro::BinaryWriter *writer);
    };

}

#endif
