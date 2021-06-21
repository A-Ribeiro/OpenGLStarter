#ifndef Font___H
#define Font___H

#include <map>
#include <convertutf/ConvertUTF.h>
//#include <aribeiro/GLFont.h>

namespace openglWrapper {

    //
    //forwarding class declaration
    //
    class GLFont;

    typedef char FontFileBinHeader[16];
    struct FontHeader {
        unsigned short mTexW;
        unsigned short mTexH;
        unsigned short nSpaceWidth;
        unsigned short mGlyphHeight;
        unsigned short mGlyphCount;
    };

    struct FontGlyphInfo {
        unsigned short texX, texY;
        short x, y, advX, advY;//can be negative
        unsigned short w, h;//always positive
    };

    class Font {
        FontHeader mHeader;
        std::map<UTF32, FontGlyphInfo> mGlyphs;
        char* luminancePointer;

        void release();
    public:
        void operator=(const Font& v);
        Font(const Font& font);
        Font();
        virtual ~Font();
        static Font* LoadFromBasof(const char* basofFile);

        GLFont* createGLFont();
    };

}

#endif
