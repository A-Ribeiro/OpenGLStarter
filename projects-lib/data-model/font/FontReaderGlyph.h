#ifndef fontreader_glyph_h__
#define fontreader_glyph_h__

#include <aribeiro/aribeiro.h>

#include "FontReaderBitmapRef.h"

namespace DataModel {

    /// \brief Store a double glyph render information for reading
    ///
    /// The basof2 (Binary ASilva OpenGL Font, version 2.0) or<br />
    /// asbgt2 (ASilva Binary Glyph Table, version 2.0)<br />
    /// file format exports 2 glyphs information.
    ///
    /// By that way it is possible to render the font face in a normal<br />
    /// way and render the stroke of that same font.
    ///
    /// This class has 2 glyphs information: The Normal Face and the Stroke of the Face.
    ///
    /// \author Alessandro Ribeiro
    ///
    struct FontReaderGlyph {
        uint32_t charcode;///< UTF32 character code
        float advancex;///< column advance of this double glyph
        FontReaderBitmapRef face;///<the normal glyph face
        FontReaderBitmapRef stroke;///<the stroke glyph of this face

        /// \brief Read this double glyph metrics from a #aRibeiro::BinaryReader
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// FontReaderGlyph doubleGlyph = ...;
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// doubleGlyph.read(&reader);
        ///
        /// reader.close();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param reader The #aRibeiro::BinaryReader instance
        ///
        void read(aRibeiro::BinaryReader * reader);
    };

}

#endif
