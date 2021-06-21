# OpenGLStarter

[Back to HOME](../../index)

## Read JPG files

You can read images using the JPG file format as shown below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

int w, h, chn, depth;
char *bufferChar;

bufferChar = JPGHelper::readJPG( "file.jpg", &w, &h, &chn, &depth);

if (bufferChar != NULL) {
    //
    // use the bufferChar to access image pixels
    //
    JPGHelper::closeJPG(bufferChar);
}
```

## Read and Write PNG files

You can read or write images using the PNG file format as shown below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//
// Reading imagem from PNG file
//
int w, h, channels, depth;
const char* filename = "inputfile.png";
char* buffer = PNGHelper::readPNG(filename, &w, &h, &channels, &depth);
if (buffer == NULL){
  fprintf(stderr, "error to load image from file: %s\n", filename);
  exit(-1);
}
//
// Writing image to PNG file
//
PNGHelper::writePNG("outputfile.png", width, height, 1, Gray_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 2, GrayAlpha_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 3, RGB_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 4, RGBA_buffer);
// use the image content
PNGHelper::closePNG(buffer);//release the allocated buffer
```

## Read and Write to Memory Streams

You can use memory streams either in JPG and PNG classes.

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//input buffer
const char *input_buffer;
int input_buffer_size;

//pixels output
int w, h, chn, depth;
char *bufferChar;

//
// JPG reading
//
bufferChar = JPGHelper::readJPGFromMemory( input_buffer, input_buffer_size, 
                                           &w, &h, &chn, &depth);
if (bufferChar != NULL) {
    ...
    JPGHelper::closeJPG(bufferChar);
}

//
// PNG reading
//
bufferChar = PNGHelper::readPNGFromMemory( input_buffer, input_buffer_size, 
                                           &w, &h, &chn, &depth);
if (bufferChar != NULL) {
    ...
    PNGHelper::closePNG(bufferChar);
}
```
