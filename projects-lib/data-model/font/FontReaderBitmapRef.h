#ifndef fontreader_bitmap_ref_h__
#define fontreader_bitmap_ref_h__

#include <aribeiro/aribeiro.h>

#include <data-model/AtlasRect.h>

namespace DataModel {

    /// \brief Store a glyph render information
    ///
    /// Each glyph needs a starting point reference (top,left) and <br />
    /// the sprite rectangle inside the atlas (bitmapBounds).
    ///
    /// With this information you can render non-monospace character types.
    ///
    /// \author Alessandro Ribeiro
    ///
    struct FontReaderBitmapRef {
        int16_t top;///< glyph origin top (y coord).
        int16_t left;///< glyph origin left (x coord).
        AtlasRect bitmapBounds;///< sprite rectangle inside the atlas.

        /// \brief Read this glyph metrics from a #aRibeiro::BinaryReader
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// FontReaderBitmapRef bitmapRef = ...;
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// bitmapRef.read(&reader);
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
