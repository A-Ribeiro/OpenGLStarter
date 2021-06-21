#include "FontReader.h"


namespace DataModel {

    //private copy constructores, to avoid copy...
    FontReader::FontReader(const FontReader& v){}
    void FontReader::operator=(const FontReader& v){}


    void FontReader::clear() {
        for (size_t i = 0; i < glyphs.size(); i++)
            delete glyphs[i];
        glyphs.clear();
        if (bitmap != NULL)
            aRibeiro::PNGHelper::closePNG(bitmap);
        bitmap = NULL;
    }

    void FontReader::readGlyphTable(aRibeiro::BinaryReader *reader) {
        size = reader->readFloat();
        space_width = reader->readFloat();
        new_line_height = reader->readFloat();

        //can be used to double check the PNG decompression image resolution
        bitmapSize.read(reader);

        uint16_t glyphCount = reader->readUInt16();
        for (uint16_t i = 0; i < glyphCount; i++) {
            FontReaderGlyph * glyph = new FontReaderGlyph();
            glyph->read(reader);
            glyphs.push_back(glyph);
        }
    }

    void FontReader::readBitmap(aRibeiro::BinaryReader *reader) {
        uint8_t *pngBuffer;
        uint32_t pngBufferSize;
        reader->readBuffer(&pngBuffer, &pngBufferSize);

        int w, h, chann, pixel_depth;
        bitmap = aRibeiro::PNGHelper::readPNGFromMemory((char*)pngBuffer, pngBufferSize, &w, &h, &chann, &pixel_depth);

        ARIBEIRO_ABORT(bitmap == NULL, "Error to load image from font definition.\n");
        ARIBEIRO_ABORT(w != bitmapSize.w, "Missmatch font resolution reference.\n");
        ARIBEIRO_ABORT(h != bitmapSize.h, "Missmatch font resolution reference.\n");
        ARIBEIRO_ABORT(chann != 1, "FontBitmap not grayscale.\n");
        ARIBEIRO_ABORT(pixel_depth != 8, "FontBitmap not 8 bits per component.\n");
    }

    FontReader::FontReader() {
        bitmap = NULL;
    }

    FontReader::~FontReader() {
        clear();
    }

    void FontReader::readFromFile(const std::string &filename) {
        clear();

        aRibeiro::BinaryReader reader;
        reader.readFromFile(filename.c_str());

        readGlyphTable(&reader);
        readBitmap(&reader);

        reader.close();
    }

    void FontReader::readFromFile(const std::string &glyph, const std::string &png_grayscale_8bits) {
        clear();

        {
            aRibeiro::BinaryReader reader;
            reader.readFromFile(glyph.c_str());
            readGlyphTable(&reader);
            reader.close();
        }

        {
            /*
            BinaryReader reader;
            reader.readFromFile(png_grayscale_8bits.c_str(), false);
            readBitmap(&reader);
            reader.close();
            */
            int w, h, chann, pixel_depth;
            bitmap = aRibeiro::PNGHelper::readPNG(png_grayscale_8bits.c_str(), &w, &h, &chann, &pixel_depth);

            ARIBEIRO_ABORT(bitmap == NULL, "Error to load image from font definition.\n");
            ARIBEIRO_ABORT(w != bitmapSize.w, "Missmatch font resolution reference.\n");
            ARIBEIRO_ABORT(h != bitmapSize.h, "Missmatch font resolution reference.\n");
            ARIBEIRO_ABORT(chann != 1, "FontBitmap not grayscale.\n");
            ARIBEIRO_ABORT(pixel_depth != 8, "FontBitmap not 8 bits per component.\n");
        }
    }

}

