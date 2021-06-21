#ifdef _WIN32
#pragma warning(disable:4996)
#pragma warning(disable:4244)
#pragma warning(disable:4309)
#pragma warning(disable:4018)
#endif

#include "Font.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include <opengl-wrapper/GLFont.h>

namespace openglWrapper {

    //--------------------------------------------------------------------------
    void Font::release() {
        if (luminancePointer != NULL) {
            delete[] luminancePointer;
        }
        luminancePointer = NULL;
        mGlyphs.clear();
        memset(&mHeader, 0, sizeof(FontHeader));
    }
    //--------------------------------------------------------------------------
    void Font::operator=(const Font& v) {
        release();
        mHeader = v.mHeader;
        mGlyphs = v.mGlyphs;
        if (v.luminancePointer != NULL) {
            luminancePointer = new char[mHeader.mTexW*mHeader.mTexH];
            memcpy(luminancePointer, v.luminancePointer, mHeader.mTexW*mHeader.mTexH * sizeof(char));
        }
    }
    //--------------------------------------------------------------------------
    Font::Font(const Font& font) {
        luminancePointer = NULL;
        (*this) = font;
    }
    //--------------------------------------------------------------------------
    Font::Font() {
        luminancePointer = NULL;
    }
    //--------------------------------------------------------------------------
    Font::~Font() {
        release();
    }
    //--------------------------------------------------------------------------
    Font* Font::LoadFromBasof(const char* basofFile) {
        Font *result = new Font();
        FILE* in = fopen(basofFile, "rb");
        if (in) {
            FontFileBinHeader binHeader;
            fread(&binHeader, sizeof(FontFileBinHeader), 1, in);
            if (memcmp(&binHeader, ".asilva.lum.font", 16) == 0) {
                //valid basof file :)

                //byte order -- the default pack for structs is 4bytes align ... there are 5 shorts... in the structure (unaligned...)
                //   to read correctly -- need short by short reading.
                fread(&result->mHeader.mTexW, sizeof(short), 1, in);//texW
                fread(&result->mHeader.mTexH, sizeof(short), 1, in);//texH
                fread(&result->mHeader.nSpaceWidth, sizeof(short), 1, in);
                fread(&result->mHeader.mGlyphHeight, sizeof(short), 1, in);//new line height
                fread(&result->mHeader.mGlyphCount, sizeof(unsigned short), 1, in);//numberOfGlyphs

                for (int i = 0; i < result->mHeader.mGlyphCount; i++) {
                    UTF32 c;
                    FontGlyphInfo glyphInfo;
                    fread(&c, sizeof(UTF32), 1, in);
                    fread(&glyphInfo, sizeof(glyphInfo), 1, in);
                    result->mGlyphs[c] = glyphInfo;
                }

                unsigned int bufferTotalSize = result->mHeader.mTexW*result->mHeader.mTexH;
                result->luminancePointer = new char[bufferTotalSize];

                //RLE reverse
                unsigned char rle_count = 0;
                unsigned int bufferInsertPos = 0;
                while (!feof(in) && bufferInsertPos < bufferTotalSize) {
                    char c;
                    fread(&c, sizeof(char), 1, in);
                    if (c == '\x00' || c == '\xff') {
                        //rle decompress
                        fread(&rle_count, sizeof(char), 1, in);
                        memset(&result->luminancePointer[bufferInsertPos], (unsigned char)c, rle_count);
                        bufferInsertPos += rle_count;
                    }
                    else {
                        result->luminancePointer[bufferInsertPos] = c;
                        bufferInsertPos++;
                    }
                }
            }
            fclose(in);
        }
        return result;
    }
    //--------------------------------------------------------------------------
    GLFont* Font::createGLFont(){
        GLFont *result = new GLFont(mHeader, mGlyphs, luminancePointer);
        return result;
    }
    //--------------------------------------------------------------------------

}
