#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
#include <map>
// #include <convertutf/ConvertUTF.h>
// #include <aribeiro/GLFont.h>

namespace AppKit
{

    namespace OpenGL
    {

        //
        // forwarding class declaration
        //
        class GLFont;

        typedef char FontFileBinHeader[16];
        struct FontHeader
        {
            unsigned short mTexW;
            unsigned short mTexH;
            unsigned short nSpaceWidth;
            unsigned short mGlyphHeight;
            unsigned short mGlyphCount;
        };

        struct FontGlyphInfo
        {
            unsigned short texX, texY;
            short x, y, advX, advY; // can be negative
            unsigned short w, h;    // always positive
        };

        class Font
        {
            FontHeader mHeader;
            std::unordered_map<uint32_t, FontGlyphInfo> mGlyphs;
            char *luminancePointer;

            void release();

        public:
            void operator=(const Font &v);
            Font(const Font &font);
            Font();
            ~Font();
            static Font *LoadFromBasof(const char *basofFile, std::string *errorStr = nullptr);

            GLFont *createGLFont();
        };

    }

}