#ifndef FT2_FONT_GENERATOR_H
#define FT2_FONT_GENERATOR_H

#include "texturefontrenderer.h"
#include <convertutf/ConvertUTF.h>

/// \brief Font buffer format type
///
/// Freetype 2 has 4 font formats. These formats are related to the pixel format of the output (raw buffer from the library)
///
/// The best format to use with 3D application is RGBA_8888 because it generates the font with AntiAliasing.
///
/// \author Alessandro Ribeiro
///
enum FT2_format{
    FORMAT_RGBA_8888,//better for AA color pixel fonts with alpha
    FORMAT_RGB_888,
    FORMAT_LUMINANCE_8,//better size in memory, but can have issues with alpha
    FORMAT_BIT_1 // better for bit fonts
};

/// \brief Font size specification types
///
/// There are two ways to specify te font size:
///
/// The first is to tell the API the maximum pixel for a character.
///
/// The second is to use the device specs (screen resolution and DPI) and specify the size in pt.
/// **pt is a font metric to keep the same size across several devices... not so easy to use.
///
/// \author Alessandro Ribeiro
///
enum FT2_size_type{
    SIZE_TYPE_PT,
    SIZE_TYPE_PIXEL
};

/// \brief Device resolution information
///
/// Used with the SIZE_TYPE_PT FT2_size_type.
///
/// Some general notes:
///    windows default DPI = 96
///    mac default DPI = 72
///    iphone4 - 960-by-640-pixel resolution at 326 ppi
///    iphone - 480 x 320 163 pixels per inch
///    ipod touch - 480 x 320 160 pixels per inch
///
/// \author Alessandro Ribeiro
///
struct FT2DeviceInfo{
    int device_width;
    int device_height;
    int device_DPI;
};


/// \brief Font generator configuration parameter set
///
/// Holds all variable necessary to generate the font from the source (.ttf | .odt)
///
/// \author Alessandro Ribeiro
///
class FT2Configuration{
public:
    FT2Configuration(const FT2Configuration& init);

    FT2Configuration(int align=4, // 4 bytes
                     FT2_format format=FORMAT_RGBA_8888, //rgba default
                     //if the size_type choosed is pixel then you must pass just the with and height
                     //   otherwise, you must specify all the others parameters
                     FT2_size_type size_type = SIZE_TYPE_PIXEL,
                     //the unity of the width and height depends on the size_type specified.
                     //    if size_type is SIZE_TYPE_PIXEL then it is pixels
                     //    if size_type is SIZE_TYPE_PT then it is pt
                     int char_max_width = 14,
                     int char_max_height = 14,
                     int device_width = 640,
                     int device_height = 480,
                     int device_DPI = 64, //, // each pixel size will multiply by 1/64 of points
                     int face_to_select = 0
                     );
    ~FT2Configuration();
    void operator=(const FT2Configuration& param);

    int m_alignment;
    FT2_format m_format;
    FT2_size_type m_size_type;
    int m_char_max_width;
    int m_char_max_height;
    FT2DeviceInfo m_deviceInfo;

    int m_face_to_select;//added to select internal font inside a font collection - default is 0
};

#include <map>
#include <vector>
class PixelFontRenderer;
class TextureFontRenderer;


/// \brief Font loaded
///
/// This class holds the pixels and character set loaded from the source font file.
///
/// There isn't a directly call to manipulate fonts. This class exists to feed the textureRenderer to generate the output files.
///
/// \author Alessandro Ribeiro
///
class PixelFont{

public:
    struct GlyphInfo{
        //these values can be negative
        int x,y,advX,advY;
        unsigned int w,h;
        //unsigned int binaryDataBegin;
    };
    PixelFont(int w, int h);
    //aqui tem q passar os parametros do FT2 do face/glyph
    void addGlyph(UTF32 c,
                  int x,
                  int y,
                  int w,
                  int h,
                  int advX,
                  int advY,
                  const void* buffer_w_h,
                  bool inverted = false);
    ~PixelFont();

    bool getGlyph(UTF32 c,
                  GlyphInfo** info,
                  void** data);

    const std::vector<UTF32>& getCharset();

    //PixelFontRenderer getPixelFontRenderer();
    TextureFontRenderer getTextureFontRenderer(int sizeBetweenChars);

    PixelFont* copyInverted();

    int height();
    int width();
private:
    //aloca memoria para um glyph, e retorna os ponteiros de cada coisa
    void glyphAlloc(int w,int h, GlyphInfo** glyph, void** buffer, int* binaryPointer);

std::map<UTF32, int> glyphs;//tem um ponteiro para a posicao do glyph no binaryData
int glyph_w;
int glyph_h;
void* binaryData;
size_t binaryDataSize;

std::vector<UTF32> mCharset;

};

class PixelFontFactory{
public:
    static PixelFont* generatePixelFont(const char* ttf_filename,
                                        const FT2Configuration& configuration,
                                        const UTF32 * charset,
                                        int charCount,
                                        bool inverted);
};

#endif // FT2_FONT_GENERATOR_H
