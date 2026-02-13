#include <appkit-gl-base/font/GLFont2.h>

#include <InteractiveToolkit-Extension/font/FontReader.h>

namespace AppKit
{

    namespace OpenGL
    {

        GLFont2Triangles GLFont2::AtlasRectToTriangles(const ITKExtension::Atlas::AtlasRect &rect, const ITKExtension::Font::FontReaderSize &textureRes)
        {
            GLFont2Triangles result;

            result.position[0] = MathCore::vec3f(0, rect.h, 0);
            result.position[1] = MathCore::vec3f(rect.w, 0, 0);
            result.position[2] = MathCore::vec3f(rect.w, rect.h, 0);

            result.position[3] = MathCore::vec3f(0, rect.h, 0);
            result.position[4] = MathCore::vec3f(0, 0, 0);
            result.position[5] = MathCore::vec3f(rect.w, 0, 0);

            MathCore::vec2f texRes_inv = 1.0f / MathCore::vec2f(textureRes.w, textureRes.h);

            result.uv[0] = MathCore::vec2f(rect.x, rect.y) * texRes_inv;
            result.uv[1] = MathCore::vec2f(rect.x + rect.w, rect.y + rect.h) * texRes_inv;
            result.uv[2] = MathCore::vec2f(rect.x + rect.w, rect.y) * texRes_inv;

            result.uv[3] = MathCore::vec2f(rect.x, rect.y) * texRes_inv;
            result.uv[4] = MathCore::vec2f(rect.x, rect.y + rect.h) * texRes_inv;
            result.uv[5] = MathCore::vec2f(rect.x + rect.w, rect.y + rect.h) * texRes_inv;

            return result;
        }

        GLFont2::GLFont2() {}

        const GLFont2Glyph *GLFont2::getGlyph(uint32_t charcode) const
        {

            std::unordered_map<uint32_t, GLFont2Glyph>::const_iterator it;

            it = glyphs.find(charcode);
            if (it == glyphs.end())
                return nullptr;
            return &(it->second);
        }

        void GLFont2::loadFromBasof2(const std::string &filename, bool force_srgb)
        {
            ITKExtension::Font::FontReader reader;
            reader.readFromFile(filename);

            size = reader.size;
            space_width = reader.space_width;
            new_line_height = reader.new_line_height;

            polygonGlyphSrc.resize(reader.glyphs.size());
            for (size_t i = 0; i < reader.glyphs.size(); i++)
            {
                auto &polygonGlyph = this->polygonGlyphSrc[i];
                auto &glyph = reader.glyphs[i];
                
                polygonGlyph.charcode = glyph.charcode;
                polygonGlyph.advancex = MathCore::vec3f(glyph.advancex, 0, 0);
                polygonGlyph.contour = std::move(glyph.contour);
            }

            glyphs.clear();
            for (const auto &fontGlyph : reader.glyphs)
            {
                GLFont2Glyph glyph;

                glyph.advancex = MathCore::vec3f(fontGlyph.advancex, 0, 0);

                glyph.face.move_before_draw = MathCore::vec3f(
                    fontGlyph.face.left,
                    -(fontGlyph.face.bitmapBounds.h - fontGlyph.face.top),
                    0);
                glyph.face.height = glyph.face.move_before_draw.y + (float)fontGlyph.face.bitmapBounds.h;
                glyph.face.triangles = AtlasRectToTriangles(fontGlyph.face.bitmapBounds, reader.bitmapSize);
                glyph.face.bitmapBounds = fontGlyph.face.bitmapBounds;

                glyph.stroke.move_before_draw = MathCore::vec3f(
                    fontGlyph.stroke.left,
                    -(fontGlyph.stroke.bitmapBounds.h - fontGlyph.stroke.top),
                    0);
                glyph.stroke.height = glyph.stroke.move_before_draw.y + (float)fontGlyph.stroke.bitmapBounds.h;
                glyph.stroke.triangles = AtlasRectToTriangles(fontGlyph.stroke.bitmapBounds, reader.bitmapSize);
                glyph.stroke.bitmapBounds = fontGlyph.stroke.bitmapBounds;

                glyphs[fontGlyph.charcode] = glyph;

            }

            texture.uploadBufferRGBA_8888(reader.bitmap_rgba, reader.bitmapSize.w, reader.bitmapSize.h, force_srgb);
        }

    }

}
