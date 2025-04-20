#ifdef _WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4309)
#pragma warning(disable : 4018)
#endif

#include <appkit-gl-base/font/deprecated/Font.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include <appkit-gl-base/font/deprecated/GLFont.h>

#include <InteractiveToolkit/ITKCommon/FileSystem/File.h>

namespace AppKit
{

    namespace OpenGL
    {

        //--------------------------------------------------------------------------
        void Font::release()
        {
            if (luminancePointer != nullptr)
            {
                delete[] luminancePointer;
            }
            luminancePointer = nullptr;
            mGlyphs.clear();
            memset(&mHeader, 0, sizeof(FontHeader));
        }
        //--------------------------------------------------------------------------
        Font& Font::operator=(const Font &v)
        {
            release();
            mHeader = v.mHeader;
            mGlyphs = v.mGlyphs;
            if (v.luminancePointer != nullptr)
            {
                luminancePointer = new char[mHeader.mTexW * mHeader.mTexH];
                memcpy(luminancePointer, v.luminancePointer, mHeader.mTexW * mHeader.mTexH * sizeof(char));
            }
            return *this;
        }
        //--------------------------------------------------------------------------
        Font::Font(const Font &font)
        {
            luminancePointer = nullptr;
            (*this) = font;
        }
        //--------------------------------------------------------------------------
        Font::Font()
        {
            luminancePointer = nullptr;
        }
        //--------------------------------------------------------------------------
        Font::~Font()
        {
            release();
        }
        //--------------------------------------------------------------------------
        Font *Font::LoadFromBasof(const char *basofFile, std::string *errorStr)
        {
            FILE *in = ITKCommon::FileSystem::File::fopen(basofFile, "rb", errorStr);
            if (!in)
                return nullptr;
            
            Font *result = new Font();

            EventCore::ExecuteOnScopeEnd _on_error_cleanup([=](){
                fclose(in);
                delete result;
            });

            FontFileBinHeader binHeader;
            size_t readed_size;
            readed_size = fread(&binHeader, sizeof(FontFileBinHeader), 1, in);
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");

            ON_COND_SET_ERRORSTR_RETURN(memcmp(&binHeader, ".asilva.lum.font", 16) != 0, nullptr, "Invalid format string reading basof file.\n");

            //if (memcmp(&binHeader, ".asilva.lum.font", 16) == 0)
            //{
            // valid basof file :)

            // byte order -- the default pack for structs is 4bytes align ... there are 5 shorts... in the structure (unaligned...)
            //    to read correctly -- need short by short reading.
            readed_size = fread(&result->mHeader.mTexW, sizeof(short), 1, in); // texW
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
            readed_size = fread(&result->mHeader.mTexH, sizeof(short), 1, in); // texH
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
            readed_size = fread(&result->mHeader.nSpaceWidth, sizeof(short), 1, in);
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
            readed_size = fread(&result->mHeader.mGlyphHeight, sizeof(short), 1, in); // new line height
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
            readed_size = fread(&result->mHeader.mGlyphCount, sizeof(unsigned short), 1, in); // numberOfGlyphs
            ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");

            for (int i = 0; i < result->mHeader.mGlyphCount; i++)
            {
                uint32_t c;
                FontGlyphInfo glyphInfo;
                readed_size = fread(&c, sizeof(uint32_t), 1, in);
                ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
                readed_size = fread(&glyphInfo, sizeof(glyphInfo), 1, in);
                ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
                result->mGlyphs[c] = glyphInfo;
            }

            unsigned int bufferTotalSize = result->mHeader.mTexW * result->mHeader.mTexH;
            result->luminancePointer = new char[bufferTotalSize];

            // RLE reverse
            unsigned char rle_count = 0;
            unsigned int bufferInsertPos = 0;
            while (!feof(in) && bufferInsertPos < bufferTotalSize)
            {
                char c;
                readed_size = fread(&c, sizeof(char), 1, in);
                ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
                if (c == '\x00' || c == '\xff')
                {
                    // rle decompress
                    readed_size = fread(&rle_count, sizeof(char), 1, in);
                    ON_COND_SET_ERRORSTR_RETURN(readed_size != 1, nullptr, "Error to read basof file.\n");
                    memset(&result->luminancePointer[bufferInsertPos], (unsigned char)c, rle_count);
                    bufferInsertPos += rle_count;
                }
                else
                {
                    result->luminancePointer[bufferInsertPos] = c;
                    bufferInsertPos++;
                }
            }
            //}
            
            // cancel on error cleanup callback
            _on_error_cleanup.callback = nullptr;

            fclose(in);
            return result;
        }
        //--------------------------------------------------------------------------
        GLFont *Font::createGLFont()
        {
            GLFont *result = new GLFont(mHeader, mGlyphs, luminancePointer);
            return result;
        }
        //--------------------------------------------------------------------------

    }

}
