#ifndef TEXTUREFONTRENDERER_H
#define TEXTUREFONTRENDERER_H

#include <convertutf/ConvertUTF.h>

class PixelFont;

#include <map>

/// \brief Texture Renderer (render generator...)
///
/// This file can be used to export the font from FT2 - but it cannot be used to load it...
///   The loaders will be coded in another source...
/// \author Alessandro Ribeiro
///
class TextureFontRenderer{

public:
    struct GlyphInfo{
        unsigned short texX,texY;
        short x,y,advX,advY;//can be negative
        unsigned short w,h;//always positive
    };

private:
    char* textureBuffer;
    int mW;
    int mH;

    //glyph index in the texture
    std::map<UTF32, GlyphInfo> glyphs;
    int glyph_w;
    int glyph_h;

public:

    /// \brief Constructor - TextureFontRenderer
    ///
    /// It tries to create a texture from the loaded pixelFont and its internal charset.
    /// The texture dimension output is power-of-two.
    ///
    /// \author Alessandro Ribeiro
    /// \param pixelFont The font character set source to generate the texture and glyph table.
    /// \param w_h Initial size of the texture (square). It grows as needed.
    /// \param space_between_chars Space Minimum space between characters in the exported texture.
    ///
    TextureFontRenderer(PixelFont *pixelFont, int w_h=16, int space_between_chars = 5);

    ~TextureFontRenderer();

    TextureFontRenderer(const TextureFontRenderer& param);
    void operator=(const TextureFontRenderer& param);

    /// \brief Font exports (compatible)
    ///
    /// Exports the texture into a .png image and the glyph table into a text file.
    ///
    /// \author Alessandro Ribeiro
    /// \param file_meta The output file to write the glyph table
    /// \param file_png The output image file to write the texture image
    ///
    void exportSFont(const char* file_meta, const char* file_png, const char* file_png_gray);

    /// \brief Font exports (opengl)
    ///
    /// Exports the whole font information into a binary file.
    /// It is exported the RAW luminance information plus the binary glyph table.
    ///
    /// \author Alessandro Ribeiro
    /// \param outfileBin The output file
    ///
    void exportGLPack(const char* outfileBin);
    
    /// \brief Font exports (mixed)
    ///
    /// Exports the texture into a .png file and 
    /// the binary glyph table into another file.
    ///
    /// \author Alessandro Ribeiro
    /// \param tablefileBin The output file to write the glyph table
    /// \param file_png The output image file to write the texture image
    ///
    void exportASilvaBinTable(const char* tablefileBin, const char* file_png, const char* file_png_gray);

    GlyphInfo* getGlyph(UTF32 c);
};

#endif // TEXTUREFONTRENDERER_H
