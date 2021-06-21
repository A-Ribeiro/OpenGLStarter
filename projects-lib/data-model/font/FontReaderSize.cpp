#include "FontReaderSize.h"

namespace DataModel {
    void FontReaderSize::read(aRibeiro::BinaryReader * reader) {
        w = reader->readUInt32();
        h = reader->readUInt32();
    }
}
