# OpenGLStarter

[Back to HOME](../../index)

## Pixel Based Atlas Creator

The class __Atlas__ can create atlas images.

It is needed to add individual sprites, and call __organizePositions__ to create the atlas.

Example:

```cpp
#include <aribeiro/aribeiro.h>
#include <data-model/data-model.h>
using namespace aRibeiro;
using namespace DataModel;

// Create an atlas with the space between elements of 8 pixels
Atlas atlas( 8, 8 );

// load RGBA buffers and create an element in the atlas
// you can do this for all elements you want to add to this atlas
{
    //rgbaElement -> external image to add to the atlas
    char* rgbaElement;
    int width, height;
    AtlasElement* atlasElement = atlas.addElement("element",width,height);
    atlasElement->copyFromRGBABuffer(rgbaElement, width * 4);
}

// after add all elements
// compute the final atlas image
atlas.organizePositions(true); // fast mode: true

// export the atlas image
atlas.savePNG("atlas.png");

// export the atlas internal sprites definitions
atlas.writeTable("atlas.bin");
```
