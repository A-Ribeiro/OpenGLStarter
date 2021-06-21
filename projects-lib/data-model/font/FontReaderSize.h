#ifndef fontreader_size_h__
#define fontreader_size_h__

#include <aribeiro/aribeiro.h>

namespace DataModel {

    /// \brief Store a size information (width, height)
    ///
    /// \author Alessandro Ribeiro
    ///
    struct FontReaderSize {
        uint32_t w;///<width
        uint32_t h;///<height

        /// \brief Read the size information from a #aRibeiro::BinaryReader
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <data-model/data-model.h>
        /// using namespace aRibeiro;
        /// using namespace DataModel;
        ///
        /// FontReaderSize size = ...;
        ///
        /// ...
        ///
        /// BinaryReader reader;
        /// reader.readFromFile("file.input");
        ///
        /// size.read(&reader);
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
