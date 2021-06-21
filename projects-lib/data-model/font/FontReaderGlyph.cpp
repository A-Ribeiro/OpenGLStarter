#include "FontReaderGlyph.h"


namespace DataModel {
    void FontReaderGlyph::read(aRibeiro::BinaryReader * reader) {
        charcode = reader->readUInt32();
        advancex = reader->readFloat();
        face.read(reader);
        stroke.read(reader);
    }
}
