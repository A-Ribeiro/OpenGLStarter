# OpenGLStarter

[Back to HOME](../../index)

## OpenGL Font 2.0

The new font format have more detailed glyph information, and have the outline stroke information also.

The builder can handle a _printf_ like string mounting, and can be rendered directly or though a scene mesh renderer.

Example:

```cpp
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-base/font/GLFont2Builder.h>
using namespace MathCore;
using namespace AppKit::OpenGL;

// ...

StringUtil stringUtil;

GLFont2Builder *fontBuilder_40 = new GLFont2Builder();
fontBuilder_40->load("resources/Roboto-Regular-40.basof2");

GLFont2VBO fontVBO_40;
GLShaderFont2 shaderFont2;

// ...

// Draw code
stringUtil.printf("Elapsed:\n%f", time.deltaTime);

// need to setup current state of font rendering
fontBuilder_40->drawFace = true;
fontBuilder_40->drawStroke = true;
fontBuilder_40->faceColor = vec4f(1, 1, 1, 1);
fontBuilder_40->strokeColor = vec4f(0.25, 0.25, 0.25, 1);
fontBuilder_40->horizontalAlign = GLFont2HorizontalAlign_center;
fontBuilder_40->verticalAlign = GLFont2VerticalAlign_middle;

// build vertex data and upload to the VBO
fontBuilder_40->build(stringUtil.char_ptr());
fontVBO_40.uploadData(fontBuilder_40);

shaderFont2.enable();
shaderFont2.setTexture(0);
fontBuilder_40->glFont2.texture.active(0);

// VBO draw method
shaderFont2.setMatrix(projection * GEN<MathCore::mat4f>::translateHomogeneous(0, -100.0f, 0) );
fontVBO_40.draw(GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);

// Direct draw method
shaderFont2.setMatrix(projection * GEN<MathCore::mat4f>::translateHomogeneous(0, -200.0f, 0));
GLFont2DirectDraw::draw(fontBuilder_40, GLShaderFont2::vPosition, GLShaderFont2::vUV, GLShaderFont2::vColor);

GLTexture::deactive(0);
```

### OpenGL Font 2.0: Generate basof2

The command line tool __font2bitmap__ now generates the new format and the old at the same time.

The input is any Freetype2 supported font format (.ttf, .otf, etc...).

The parameters to the command line tool:

* __-i:__ input file(.ttf, .otf, freetype compatible font)
* __-f:__ face in font to be selected (default: 0)
* __-o:__ output file without extension
* __-s:__ character size in pixels to be exported (minimum:14).
* __-c:__ text file with all characters to be generated in utf8.
* __-p:__ size in pixels of the space between the characters exported.
* __-t:__ outline thickness real number.

Example:

```bash
# Generates a font file called output-20.basof2
# parameters: -s 20 the square pixel size is 20 pixels.
#             -c charset.utf8 the input character list utf-8 file
#             -p 6 the minimum distance in pixels from each 
#                  character from another in the exported atlas.
#             -t 0.33333 the thickness of the outline
font2bitmap -i "input.ttf" -o "output" -s 20 -c charset.utf8 -p 6 -t 0.33333
```

## Deprecated: OpenGL Font

To use fonts in any graphics API is not an easy task, because fonts are created using several techniques. You need a lot of work to undestand the several types of font files formats that exist.

I created an image based font format that comes with the glyph information and the image in the same binary file.

This make easy to port the font to any place.

The name I give to the format is BASOF (Binary ASilva OpenGL Font).

Example of use:

```cpp
// global variable definition
GLFont *font = NULL;

int main(int argc, char* argv[]) {
  //OpenGL Inicialization
  ...
  //load font
  {
    Font* font_raw = Font::LoadFromBasof("resources/goudy_bookletter_1911.basof");
    font = font_raw->createGLFont();
    setNullAndDelete(font_raw);
  }

  //Inside Main Loop
  while (window.isOpen()) {
  
    glClear(GL_COLOR_BUFFER_BIT);
    
    wchar_t text[1024];// = L"Template";
    swprintf(text, 1024,L"%.0f fps", 1.0f / time.unscaledDeltaTime);

    AABB aabb = font->computeBounds(text);
    vec3 center = - aabb.max_box * 0.5f;

    float fontSize = 1.0f/font->getLineHeight() * 2.0f;
    float fontSize_10_percent = fontSize * 0.10f;

    font->getShader()->enable();
    font->getShader()->setMatrix(mvp * GEN<MathCore::mat4f>::scaleHomogeneous(MathCore::vec3f(fontSize_10_percent)) * GEN<MathCore::mat4f>::translateHomogeneous(center));

    font->print(MathCore::vec3f(0, 0.0f, 0.0f), text);
    
    window.display();

    // window event processing...
    ...
  }
}
```
