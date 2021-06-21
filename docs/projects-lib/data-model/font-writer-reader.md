# OpenGLStarter

[Back to HOME](../../index)

## Font Writer and Reader

There are definitions of writing and reading font glyph information that is not specific for any render API.

The font file definition was created to deal with font export/import.

### Export

To export font information, it is needed to use the Atlas font information.

The example below show how to create a basof2 (Binary ASilva OpenGL Font, version 2.0) font format from scratch.

```cpp
#include <aribeiro/aribeiro.h>
#include <data-model/data-model.h>
#include <ft2-wrapper/ft2-wrapper.h>
using namespace aRibeiro;
using namespace DataModel;
using namespace ft2Wrapper;

FontWriter fontWriter;

//load utf8 data
UTF8data * fileContent = UTF8data::readFromFile("charset.utf8");
UTF32data *utf32data = new UTF32data(fileContent);
delete fileContent;
UTF32* char_array = utf32data->begin();


//configura FT2 and Atlas
FT2 ft2;
ft2.readFromFile_PX(parameters.inputFile,
                    parameters.faceToSelect,
                    parameters.outlineThickness,
                    parameters.characterSize,
                    parameters.characterSize);

float newLineHeight = (float)parameters.characterSize;

float spaceWidth = (float)parameters.characterSize;
FT2Glyph *glyph_white_space = ft2.generateGlyph(L' ');
if (glyph_white_space != NULL) {
    spaceWidth = glyph_white_space->advancex;
    ft2.releaseGlyph(&glyph_white_space);
}

Atlas atlas(parameters.spaceBetweenChar,parameters.spaceBetweenChar);

fontWriter.initFromAtlas(&atlas, parameters.characterSize, spaceWidth, newLineHeight);

//generate all characters
for (int i=0;i<utf32data->count();i++){
    if (char_array[i] == L' ')
        continue;

    FT2Glyph *glyph = ft2.generateGlyph(char_array[i]);
    if (glyph == NULL){
        printf("Glyph not found: %u\n", char_array[i]);
        continue;
    }

    AtlasElement* atlasElementFace = atlas.addElement(UInt32toStringHEX(char_array[i]),
                                                    glyph->normalRect.w,
                                                    glyph->normalRect.h);

    atlasElementFace->copyFromRGBABuffer(glyph->bitmapRGBANormal, glyph->normalRect.w * 4);

    AtlasElement* atlasElementStroke = atlas.addElement(
        UInt32toStringHEX(char_array[i]) + std::string("s"),
        glyph->strokeRect.w,
        glyph->strokeRect.h
    );

    atlasElementStroke->copyFromRGBABuffer(glyph->bitmapRGBAStroke, glyph->strokeRect.w * 4);

    fontWriter.setCharacter(
        char_array[i],
        glyph->advancex,
        glyph->normalRect.top,
        glyph->normalRect.left,
        atlasElementFace,
        glyph->strokeRect.top,
        glyph->strokeRect.left,
        atlasElementStroke);

    ft2.releaseGlyph(&glyph);
}

atlas.organizePositions(false);

fontWriter.save("output.basof2");
```

### Import

To import the font, you can use the FontReader.

It reads the glyph table and the image stored in the basof2 format.

```cpp
#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <data-model/data-model.h>


DataModel::FontReader reader;
reader.readFromFile("input.basof2");

size = reader.size;
space_width = reader.space_width;
new_line_height = reader.new_line_height;

glyphs.clear();
for(size_t i=0;i<reader.glyphs.size();i++){
    //
    // For each glyph, convert them to be compatible with OpenGL.
    //
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
```
