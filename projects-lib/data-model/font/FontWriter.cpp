#include "FontWriter.h"

namespace DataModel {

    FontWriter::FontWriter(const FontWriter& v){}
    void FontWriter::operator=(const FontWriter& v){}

    FontWriter::FontWriter() {

    }

    FontWriter::~FontWriter() {
        std::map<uint32_t, FontWriterGlyph*>::iterator it;
        for (it = glyphmap.begin(); it != glyphmap.end(); it++)
            delete it->second;
        glyphmap.clear();
    }

    void FontWriter::initFromAtlas(Atlas *_atlas, float _max_square_size_px, float _space_width, float _new_line_height) {
        atlas = _atlas;
        max_square_size_px = _max_square_size_px;
        new_line_height = _new_line_height;
        space_width = _space_width;
    }

    void FontWriter::setCharacter(uint32_t charcode,
        float advancex,
        uint16_t ftop,
        uint16_t fleft,
        AtlasElement* atlasElementFace,
        uint16_t stop,
        uint16_t sleft,
        AtlasElement* atlasElementStroke) {

        ARIBEIRO_ABORT((glyphmap.find(charcode) != glyphmap.end()), "Trying to insert the same character twice.\n");
        //if (glyphmap.find(charcode) != glyphmap.end()) {
        //    fprintf(stderr, "FontWriter - Error: Trying to insert the same character twice.\n");
        //    exit(-1);
        //}

        glyphmap[charcode] = new FontWriterGlyph(
            advancex, ftop, fleft, atlasElementFace,
            stop, sleft, atlasElementStroke);
    }

    void FontWriter::save(const char* filename) {
        aRibeiro::BinaryWriter writer;
        writer.writeToFile(filename);
        writeGlyphTable(&writer);
        writeBitmap(&writer);
        writer.close();
    }

    void FontWriter::writeGlyphTable(aRibeiro::BinaryWriter *writer) {
        writer->writeFloat(max_square_size_px);
        writer->writeFloat(space_width);
        writer->writeFloat(new_line_height);

        writer->writeUInt32(atlas->textureResolution.w);
        writer->writeUInt32(atlas->textureResolution.h);

        writer->writeUInt16((uint16_t)glyphmap.size());
        std::map<uint32_t, FontWriterGlyph*>::iterator it;
        for (it = glyphmap.begin(); it != glyphmap.end(); it++) {
            writer->writeUInt32(it->first);//charcode
            it->second->write(writer);//all glyph information
        }
    }
    void FontWriter::writeBitmap(aRibeiro::BinaryWriter *writer) {
        uint8_t *grayBuffer = atlas->createA();
        int size;
        char* result = aRibeiro::PNGHelper::writePNGToMemory(&size, atlas->textureResolution.w, atlas->textureResolution.h, 1, (char*)grayBuffer);

        ARIBEIRO_ABORT(result == NULL, "Error to write PNG to memory.\n");

        atlas->releaseA(&grayBuffer);

        writer->writeBuffer((uint8_t*)result, size);

        aRibeiro::PNGHelper::closePNG(result);
    }


    void FontWriter::saveGlyphTable(const char* filename) {
        aRibeiro::BinaryWriter writer;
        writer.writeToFile(filename);
        writeGlyphTable(&writer);
        writer.close();
    }

}
