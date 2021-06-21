#include "GLFont2.h"

using namespace aRibeiro;

namespace openglWrapper {

    //private copy constructores, to avoid copy...
    GLFont2::GLFont2(const GLFont2& v){}
    void GLFont2::operator=(const GLFont2&v){}

    GLFont2Triangles GLFont2::AtlasRectToTriangles(const DataModel::AtlasRect & rect, const DataModel::FontReaderSize & textureRes) {
        GLFont2Triangles result;

        result.position[0] = vec3(0, rect.h,0);
        result.position[1] = vec3(rect.w,0,0);
        result.position[2] = vec3(rect.w, rect.h, 0);

        result.position[3] = vec3(0, rect.h,0);
        result.position[4] = vec3(0,0,0);
        result.position[5] = vec3(rect.w, 0, 0);

        vec2 texRes_inv = 1.0f / vec2(textureRes.w,textureRes.h);

        result.uv[0] = vec2(rect.x          , rect.y) * texRes_inv;
        result.uv[1] = vec2(rect.x + rect.w , rect.y + rect.h) * texRes_inv;
        result.uv[2] = vec2(rect.x + rect.w , rect.y) * texRes_inv;

        result.uv[3] = vec2(rect.x          , rect.y) * texRes_inv;
        result.uv[4] = vec2(rect.x          , rect.y + rect.h) * texRes_inv;
        result.uv[5] = vec2(rect.x + rect.w , rect.y + rect.h) * texRes_inv;

        return result;
    }

    GLFont2::GLFont2(){}

    const GLFont2Glyph *GLFont2::getGlyph(uint32_t charcode)const{
        
        aligned_map<uint32_t,GLFont2Glyph>::const_iterator it;

        it = glyphs.find(charcode);
        if (it == glyphs.end())
            return NULL;
        return &(it->second);
    }

    void GLFont2::loadFromBasof2(const std::string &filename){
        DataModel::FontReader reader;
        reader.readFromFile(filename);
        
        size = reader.size;
        space_width = reader.space_width;
        new_line_height = reader.new_line_height;

        glyphs.clear();
        for(size_t i=0;i<reader.glyphs.size();i++){
            DataModel::FontReaderGlyph* fontGlyph = reader.glyphs[i];
            GLFont2Glyph glyph;

            glyph.advancex = vec3(fontGlyph->advancex,0,0);
            
            glyph.face.move_before_draw = vec3(
                fontGlyph->face.left,
                - (fontGlyph->face.bitmapBounds.h - fontGlyph->face.top),
                0
            );
            glyph.face.height = glyph.face.move_before_draw.y + (float)fontGlyph->face.bitmapBounds.h;
            glyph.face.triangles = AtlasRectToTriangles(fontGlyph->face.bitmapBounds, reader.bitmapSize);

            glyph.stroke.move_before_draw = vec3(
                fontGlyph->stroke.left,
                - (fontGlyph->stroke.bitmapBounds.h - fontGlyph->stroke.top),
                0
            );
            glyph.stroke.height = glyph.stroke.move_before_draw.y + (float)fontGlyph->stroke.bitmapBounds.h;
            glyph.stroke.triangles = AtlasRectToTriangles(fontGlyph->stroke.bitmapBounds, reader.bitmapSize);

            glyphs[fontGlyph->charcode] = glyph;
        }

        texture.uploadBufferAlpha8(reader.bitmap, reader.bitmapSize.w, reader.bitmapSize.h);
    }

}

