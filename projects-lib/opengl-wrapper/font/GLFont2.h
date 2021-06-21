#ifndef GLFont2___H
#define GLFont2___H

#include <glew/glew.h>
#include <map>
#include <vector>
#include <convertutf/ConvertUTF.h>

#include <aribeiro/all_math.h>
//#include <aribeiro/GLStripText.h>
#include <opengl-wrapper/Font.h>
#include <opengl-wrapper/GLTexture.h>
#include <opengl-wrapper/GLShaderFont.h>
#include <opengl-wrapper/GLVertexBufferObject.h>

#include <data-model/data-model.h>

#include <wchar.h> // to define swprintf(...);

namespace openglWrapper {
    
    /// \brief A quad representation to a face or stroke information
    ///
    /// The quad has two triangles.
    ///
    /// \author Alessandro Ribeiro
    ///
    struct _SSE2_ALIGN_PRE GLFont2Triangles {
        aRibeiro::vec2 uv[6];
        aRibeiro::vec3 position[6];
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    /// \brief A definition for a font face or font stroke
    ///
    /// \author Alessandro Ribeiro
    ///
    struct _SSE2_ALIGN_PRE GLFont2BitmapRef {
        aRibeiro::vec3 move_before_draw;
        GLFont2Triangles triangles;
        float height;
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    /// \brief A complete definition for a font face or font stroke
    ///
    /// This representation has the advancex information along with the<br />
    /// information needed to effective draw a face or a stroke
    ///
    /// \author Alessandro Ribeiro
    ///
    struct _SSE2_ALIGN_PRE GLFont2Glyph {
        aRibeiro::vec3 advancex;
        GLFont2BitmapRef face;
        GLFont2BitmapRef stroke;
        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

    /// \brief A complete definition for a font
    ///
    /// The font could be seen as a set of faces and strokes related to a char code (uint32_t).
    ///
    /// Used as base information by the #GLFont2Builder to construct texts.
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE GLFont2 {
        //private copy constructores, to avoid copy...
        GLFont2(const GLFont2& v);
        void operator=(const GLFont2&v);

        GLFont2Triangles AtlasRectToTriangles(const DataModel::AtlasRect & rect, const DataModel::FontReaderSize & textureRes);
    public:
        float size;///<the size of the loaded font in pixels units
        float space_width;///<the advance x when calculating space character ' '
        float new_line_height;///<the size of a new line character in pixels units
        GLTexture texture;///<the texture with the grayscale alphamap bitmap
        
        aRibeiro::aligned_map<uint32_t,GLFont2Glyph> glyphs;///<all glyphs exported with the loaded font

        GLFont2();

        /// \brief Query for a glyph inside this font character set
        ///
        /// This method will return NULL if the char code is not found in this set.
        ///
        /// \author Alessandro Ribeiro
        /// \param charcode UTF32 char code
        /// \return the #GLFont2Glyph of the char code or NULL.
        ///
        const GLFont2Glyph *getGlyph(uint32_t charcode)const;

        /// \brief Loads the font definition from a basof2 file.
        ///
        /// The basof2 is a custom font format exporter created in this framework.<br />
        /// It means: Binary ASilva OpenGL Font, version 2.0
        ///
        /// \author Alessandro Ribeiro
        /// \param filename The basof2 file.
        ///
        void loadFromBasof2(const std::string &filename);

        SSE2_CLASS_NEW_OPERATOR
    }_SSE2_ALIGN_POS;

}

#endif
