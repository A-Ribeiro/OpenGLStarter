# OpenGLStarter

[Back to HOME](../index)

## FreeType2 Wrapper

The class __FT2__ wrapps FreeType2 library.

To use it you need a font file input (.ttf, .otf, etc...) and a charset to read from the font.

See the example below:

```cpp
#include <aribeiro/aribeiro.h>
#include <ft2-wrapper/ft2-wrapper.h>
using namespace aRibeiro;
using namespace ft2Wrapper;

FT2 ft2;
ft2.readFromFile_PX(
    "input_file.ttf", // inputFile
    0, // face to select
    1.2f, // outline thickness
    60,// characterSize w
    60 // characterSize h
);

// char code to query from freetype2
UTF32* char_array;

for ( int i=0 ; i < char_count ; i++ ) {
    FT2Glyph *glyph = ft2.generateGlyph(char_array[i]);
    if (glyph == NULL) {
        printf("Glyph not found: %u\n", char_array[i]);
        continue;
    }
    // here you can use the glyph bitmap information
    //
    // ...
    //
    ft2.releaseGlyph(&glyph);
}
```
