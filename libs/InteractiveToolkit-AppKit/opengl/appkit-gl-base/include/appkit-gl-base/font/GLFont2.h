#pragma once

#include <appkit-gl-base/platform/PlatformGL.h>
// #include <glew/glew.h>
#include <map>
#include <vector>
// #include <convertutf/ConvertUTF.h>

//#include <aRibeiroCore/all_math.h>
// #include <aribeiro/GLStripText.h>
//#include <appkit-gl-base/Font.h>
#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/shaders/GLShaderFont.h>
#include <appkit-gl-base/GLVertexBufferObject.h>

//#include <aRibeiroData/aRibeiroData.h>

#include <InteractiveToolkit-Extension/atlas/AtlasRect.h>
#include <InteractiveToolkit-Extension/font/FontReaderSize.h>

#include <wchar.h> // to define swprintf(...);

namespace AppKit
{

    namespace OpenGL
    {

        /// \brief A quad representation to a face or stroke information
        ///
        /// The quad has two triangles.
        ///
        /// \author Alessandro Ribeiro
        ///
        struct GLFont2Triangles
        {
            MathCore::vec2f uv[6];
            MathCore::vec3f position[6];
        };

        /// \brief A definition for a font face or font stroke
        ///
        /// \author Alessandro Ribeiro
        ///
        struct GLFont2BitmapRef
        {
            MathCore::vec3f move_before_draw;
            GLFont2Triangles triangles;
            float height;
        };

        /// \brief A complete definition for a font face or font stroke
        ///
        /// This representation has the advancex information along with the<br />
        /// information needed to effective draw a face or a stroke
        ///
        /// \author Alessandro Ribeiro
        ///
        struct GLFont2Glyph
        {
            MathCore::vec3f advancex;
            GLFont2BitmapRef face;
            GLFont2BitmapRef stroke;
        };

        /// \brief A complete definition for a font
        ///
        /// The font could be seen as a set of faces and strokes related to a char code (uint32_t).
        ///
        /// Used as base information by the #GLFont2Builder to construct texts.
        ///
        /// \author Alessandro Ribeiro
        ///
        class GLFont2
        {
            GLFont2Triangles AtlasRectToTriangles(const ITKExtension::Atlas::AtlasRect &rect, const ITKExtension::Font::FontReaderSize &textureRes);

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            GLFont2(const GLFont2 &v) = delete;
            GLFont2& operator=(const GLFont2 &v) = delete;

            float size;            ///< the size of the loaded font in pixels units
            float space_width;     ///< the advance x when calculating space character ' '
            float new_line_height; ///< the size of a new line character in pixels units
            GLTexture texture;     ///< the texture with the grayscale alphamap bitmap

            std::unordered_map<uint32_t, GLFont2Glyph> glyphs; ///< all glyphs exported with the loaded font

            GLFont2();

            /// \brief Query for a glyph inside this font character set
            ///
            /// This method will return nullptr if the char code is not found in this set.
            ///
            /// \author Alessandro Ribeiro
            /// \param charcode UTF32 char code
            /// \return the #GLFont2Glyph of the char code or nullptr.
            ///
            const GLFont2Glyph *getGlyph(uint32_t charcode) const;

            /// \brief Loads the font definition from a basof2 file.
            ///
            /// The basof2 is a custom font format exporter created in this framework.<br />
            /// It means: Binary ASilva OpenGL Font, version 2.0
            ///
            /// \author Alessandro Ribeiro
            /// \param filename The basof2 file.
            ///
            void loadFromBasof2(const std::string &filename);
        };

    }

}