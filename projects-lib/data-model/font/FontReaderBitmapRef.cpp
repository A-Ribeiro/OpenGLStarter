#include "FontReaderBitmapRef.h"

namespace DataModel {

    void FontReaderBitmapRef::read(aRibeiro::BinaryReader * reader) {
        top = reader->readInt16();
        left = reader->readInt16();
        bitmapBounds.read(reader);
    }

}
