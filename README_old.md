# OpenGLStarter

> For this is how God loved the world:<br/>
> he gave his only Son, so that everyone<br/>
> who believes in him may not perish<br/>
> but may have eternal life.
> 
> John 3:16

The code in this framework has been created over the years.

I used it for personal coding, to make games and interactive software.

If you are a starter in OpenGL graphics programming, you can use this framework as a starting point to create your projects.

All code are implemented in a lightweight c++ functions and classes.

I hope it could help you.

To see other projects, you can visit [my website](http://alessandroribeiro.thegeneralsolution.com/en/).

## How To Build

After several years programming, I used to use the makefile system. When you use it, you need to create one project build configuration file for each platform you want to compile the project (Operation System: Windows, Linux, Mac, ...).

After a project I participated in the company I worked, one programmer show us the CMake build system.

When you use CMake, you put the files of your software in the configuration files and at the end, it doesn't compile the project. The CMake creates the native platform project files from the configuration files.

With CMake you can create, from the same source, the Windows Visual Studio project, the Linux Makefile project or the MacOS XCode project.

It makes the cross-platform compilation very easy.

### Step-by-Step

1. __Install CMake__<br/>
Download and install the CMake from their website [here](https://cmake.org/download/).

2. __Setup the source folder and the build folder__ <br/>
Open CMake. You will see two input boxes. <br/>
The first you will put the OpenGLStarter folder. <br/>
The Second you will put the location you want to build the project. <br/>
Look at the example: <br/>
![CMake folder setup](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/cmake-folder-setup.png)

3. __Select the platform specific project you want to create__ <br/>
When you click in the _configure_ for the first time, you need to select the IDE or the build system you want CMake to generate for you. <br/>
![CMake build system selection](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/cmake-ide-select.png)

4. __Click in configure until all the red parameters gone__ <br/>
CMake analyses your system for the compiler and build system you selected. You need to click several times in the _configure_ button until all red parameters gone. <br/>
![CMake red parameters](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/cmake-red-parameters.png)

5. __Click in generate__ <br/>
After these steps, you can open in the IDE you created the project located in the build folder. <br/>
![CMake generated](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/cmake-generate.png)

## How to Create a new Project

The structure of the CMake configuration uses a file called CMakeLists.txt in the base folder of each library or binary project you want.

To create a new project, you need to create a folder with the same pattern that is in the framework.

The folder hierarchy of this framework has three base folders: 

* __lib__: for libraries
* __projects__: for binary projects
* __tools__: for binary tools

You can see them in the three structure below:

```
OpenGLStarter
+--lib
|  +--aribeiro
|  |  +--CMakeLists.txt
|  |  +--...
|  +--...
+--projects
|  +--template
|  |  +--CMakeLists.txt
|  |  +--...
|  +--...
+--tools
|  +--...
+--CMakeLists.txt
+--...
```

To create a new project you need to copy the template folder inside the projects folder and add it to the CMakeLists.txt of the root folder.

After that you can starting coding your new project.

__NOTICE:__ Every time you modify any CMakeLists.txt file, you need to configure and generate the IDE project you are using.

## Features and Resources

The namespace aRibeiro has several code to help you to make some basic tasks.

You can do operation on vectors, matrix and quaternion, use interpolation from Robert Penner's equations, use shaders, print text with custom fonts, use platform specific thread system, use platofm specific time functions, manage some path operations, etc...

Before do any operation, you need to include the __aribeiro__ library. You can do that as shown in the example below:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
```

### Vector, 4x4 Matrix and Quaternion Math

The library has a lot of math code implemented. It includes general operations and complex also.

Some example:

```cpp
//vector operation
vec3 a(1.0f,0.0f,0.0f);
vec3 b(0.0f,1.0f,0.0f);
vec3 ab = a-b;
//matrix multiplication
mat4 rotation = rotate( DEG2RAD(30.0f), 1.0f,0.0f,0.0f );
vec3 ab_rotated = toVec3(rotation * toPtn4(ab));
//quaternion
quat qrotationA = quatFromEuler(DEG2RAD(30.0f),0.0f,0.0f);
quat qrotationB = quatFromEuler(0.0f,DEG2RAD(30.0f),0.0f);
quat qrotation = slerp(qrotationA, qrotationB, 0.5f);
rotation = toMat4(qrotation);
```

### C++ Robert Penner's Easing Equations

[Robert Penner](http://robertpenner.com/easing/) created a set of interpolation functions considering the smooth variation of a parameter using several equations.

His equations create a different dynamics feeling when you interpolate the values.

The library has all the equations Robert Penner created, and if you want, you can create your own interpolation function also.

Look at the example below:

```cpp
//with numbers
{
  float a = 0.0f;
  float b = 1.0f;
  float c = easeOutExpo(a,b,0.5f);
}
//with vectors
{
  vec3 a(1.0f,0.0f,0.0f);
  vec3 b(0.0f,1.0f,0.0f);
  vec3 c = easeOutElastic(a,b,0.5f);
}
//with tensors
{
  tensor4 a = translate(vec3(1.0f,0.0f,0.0f)).asTensor4();
  tensor4 b = translate(vec3(0.0f, 1.0f, 0.0f)).asTensor4();
  tensor4 c = easeOutCirc(a,b,0.5f);
  mat4 result = c.asMat4();
}
```

The project __aribeiro-text-easing-eq__ render all equations. You can see the print screen below:

![Robert Penner's equations](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/robert-penner-easing-eq.png)

### Shaders

The old OpenGL library is easy to do the first setup because it comes with a lot of fixed functions already implemented in the API.

After the programmable graphics hardware, the new version of OpenGL becomes more flexible. Now is a bit more complicated to write CG software, because you need to program the GPU also.

The library comes with 4 shaders already implemented to do the following tasks:

* Draw primitives with solid colors
* Draw primitives with a texture and a color
* Draw fonts
* Draw YUV buffers

Example:

```cpp
// global variable definition
GLShaderColor *shaderColor = NULL;

int main(int argc, char* argv[]) {
  //OpenGL Inicialization
  ...
  shaderColor = new GLShaderColor();

  //Inside Main Loop
  while (window.isOpen()) {
  
    glClear(GL_COLOR_BUFFER_BIT);
    
    shaderColor->enable();
    // Set Shader Uniforms
    shaderColor->setColor(vec4(vec3(0.8f), 1.0));
    shaderColor->setMatrix(mvp);

    // Draw some triangles buffer
    glEnableVertexAttribArray(shader->vPosition);
    glVertexAttribPointer(shader->vPosition, COORDS_PER_POS, GL_FLOAT, false, STRUCTURE_STRIDE_BYTES_POS, vertexBuffer);
    glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
    glDisableVertexAttribArray(shader->vPosition);
    
    window.display();

    // window event processing...
    ...
  }
}
```

### OpenGL Font

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
    font->getShader()->setMatrix(mvp * scale(vec3(fontSize_10_percent)) * translate(center));

    font->print(vec3(0, 0.0f, 0.0f), text);
    
    window.display();

    // window event processing...
    ...
  }
}
```

#### Font tool: font2bitmap

To create the BASOF file you need to build the font2bitmap tool.

It receives as parameter the font file (could be ttf, otf or other freetype2 compatible format) and generates the .basof file.

Example of use:

```
font2bitmap -i goudy_bookletter_1911.otf -o goudy_bookletter_1911 -m opengl -s 60 -c charset.utf8 -p 5
```

In this example we generate from input __goudy_bookletter_1911.otf__ the output __goudy_bookletter_1911.basof__, generate the __opengl__ type, the size of the pixel matrix is __60x60__ pixels, we use the __charset.utf8__ as glyph set and the space between the letters in pixel is __5__.

### Load PNG Images

The framework can load PNG image files directly to OpenGL textures. It supports the following formats:

* Gray (8bits per pixel)
* RGB (24bits per pixel)
* RGBA (32bits per pixel)

You can use the code below:

```cpp
GLTexture *image = GLTexture::loadFromPNG("resources/filename.png");
// use the texture as you want
image->active(0); // active the texture in the opengl texture unit 0
// ...
image->deactive(0); // deactive the texture in the opengl texture unit 0
// ...
// at the end you need to release it
setNullAndDelete(image);
```

### Platform Thread and Mutex

You can find a lot of cross-platform implementation of threads and mutex. This is another implementation.

I thought the usage to be in the same Java thread fashion.

You can create a thread that points to a class method or a function.

The thread has the interrupt attribute. To be possible to send a kind of signal to the thread you want to finalize.

To use the PlatformMutex, you just need to create an object and call lock or unlock method. The base implementation is recursive.

Example:

```cpp
PlatformMutex mutex;

void thread_function() {
  printf("thread Start...\n");
  mutex.lock();
  printf("thread Critical Section Enter Success !!!\n");
  mutex.unlock();

  printf("thread waiting interrupt from another thread...\n");
  while (!PlatformThread::isCurrentThreadInterrupted()) {
    //avoid 100% CPU using by this thread
    PlatformSleep::sleepMillis(3000);
  }

  printf("thread Interrupt Detected !!!\n");
  printf("thread End...\n");
}


void main(int argc, char* argv[]) {
  PlatformThread thread(thread_function);

  printf("  MainThread: Mutex lock before thread start\n");
  mutex.lock();

  thread.start();
  PlatformSleep::sleepSec(3);

  printf("  MainThread: Mutex unlock\n");
  mutex.unlock();
	
  printf("  MainThread: Interrupt and Waiting thread To Finish!!!\n");
  thread.interrupt();
  thread.wait();
	
  printf("  MainThread: Thread Finish Detected!!!\n");
}
```

### Platform Time and Sleep

This is a high precision, cross-platform time function implementation.

The PlatformTime object computes the time deltas and pass it to the application to write their interactive code.

To use the PlatformTime class you need to call the update method (once per frame), to compute the delta.

To use the PlatformSleep, you need to call the sleepMillis, sleepSec or busySleepMicro.

Example:

```cpp
PlatformTime time;
time.timeScale = 0.5f;
time.update();
PlatformSleep::sleepMillis(2000);
time.update();
	
printf("time.deltaTime (2 secs): %f\n", time.deltaTime);
printf("time.unscaledDeltaTime (2 secs): %f\n", time.unscaledDeltaTime);
```

### Platform Path

I created this class to manage the binary working directory. You can change the work directory on the fly.

The common use is to set the work directory to the binary directory. This make easy to load relative path files.

Example:

```cpp
void main(int argc, char* argv[]) {
  PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));
}
```

Another use for this class is to get the save game folder. In windows it returns a path inside the save game directory, in linux it returns the user path at home.

```cpp
void main(int argc, char* argv[]) {
  std::string saveGamePath = PlatformPath::getSaveGamePath("CompanyName", "GameName");
  //result in windows: C:\Users\<username>\Saved Games\CompanyName\GameName
  //result in linux: /home/<username>/.CompanyName/GameName
}
```

### General Utilities

* __C++ Method and Function Delegation__ <br/>
Yes you can use method delegation with this framework. <br/>
Method delegation is a way to pass a method from a class as parameter to another. <br/>
It can be used to implement events. <br/>
See the example below: <br/>
```cpp
// first you need to declare the delegate type
BEGIN_DECLARE_DELEGATE(DelegateWithIntParameter, int v) CALL_PATTERN (v) END_DECLARE_DELEGATE;

//functions you want to call
void DelegateFunctionOutside(int v) {
  printf("  executed DelegateFunctionOutside: %i\n", v);
}

//class definition
class DelegateTest{
public:
  void int1(int v) {
    printf("  executed method int1: %i\n", v);
  }
  void int2(int v) {
    printf("  executed method int2: %i\n", v);
  }
};

void main(int argc, char* argv[]) {

  //create instance of DelegateTest
  DelegateTest delegateTestObj;

  //now you can declare an object of the type DelegateWithIntParameter
  DelegateWithIntParameter OnInt;

  //add the calls you want to do
  OnInt.add(&delegateTestObj, &DelegateTest::int1);
  OnInt.add(&delegateTestObj, &DelegateTest::int2);
  OnInt.add(&DelegateFunctionOutside);
  
  //call the event with parameter 10
  OnInt(10);
}
```

* __C++ Properties__ <br/>
Yes you can use properties with this framework. <br/>
A Property is a value that can have listeners attached to it when it is modified. <br/>
It can be used to implement events related to value modification. <br/>
See the example below: <br/>
```cpp
// first you need to declare the property
Property<vec2> Size;

//functions you want to call
void OnSizeChange(Property<vec2> *prop) {
  ... use the prop->value
}

//set the listener
Size.onChange.add(&OnSizeChange);

// to change the property value
Size = vec2( 1.0f );

// to get the property value
vec2 result = Size;
```

* __Get Machine Mac Address__ <br/>
Returns the first mac address that are installed in the computer. <br/>
Example: <br/>
```cpp
std::vector<unsigned char> macaddr;
macaddr = getFirstMacAddress();
```

* __Read and Write PNG files__ <br/>
You can load and write .png files. <br/>
example: <br/>
```cpp
//
// Writing image to PNG file
//
PNGHelper::writePNG("outputfile.png", width, height, 1, Gray_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 2, GrayAlpha_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 3, RGB_buffer);
PNGHelper::writePNG("outputfile.png", width, height, 4, RGBA_buffer);
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
// use the image content
PNGHelper::closePNG(buffer);//release the allocated buffer
```

* __Handle the Transformation Stack__ <br/>
To make easy to do hierarchy operations with any type. <br/>
```cpp
TransformStack<mat4> projection;
projection.push();
projection.top = mat4::IdentityMatrix;
//use the projection matrix...
projection.pop();
```

* __Random Generator__ <br/>
To generate random numbers with all types from the framework. <br/>
```cpp
vec2 vec = Random::getVec2();
quat rot = Random::getQuat();
```

# Example Projects

## Easing Equations Test

This project is used to render in OpenGL all the easing equations the framework has.

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/robert-penner-easing-eq.png)

## Game Invader

This project has an interactive application structure. It handles the user input to be able to run the game. It does work with XBox 360 Joystick and PC Keyboard.

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/game-invader.png)

## Game Tetris

This project has an interactive application structure. It handles the user input to be able to run the game. It does work with a PC Keyboard.

This project has the initial version of a GUI library I constructed, that handle drawing of images, sliders, image buttons and text buttons.

It uses the TransformStack class and it has a basic render state manager to draw all the elements.

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/game-tetris.png)

## Post Processing Example

Demo project to test the GLFramebufferObject and GLPostProcess classes.

I implemented two versions of the post processing. One version has the forward implementation, using non-power-of-two textures with the screen resolution as input.

The second I implemented the FBOs to be in power-of-two textures, but I setup the viewport to use just the rendered area. For example: if the screen is: 640x480, then the FBO will be 1024x512, but the viewport will be setup to be bound to 640x480 inside this texture.

I tested in my mac (early 2011) with Intel HD3000. The power-of-two implementation runs with a frame drop from 2 to 4. I used the same effect to render several times in one frame (24 times). While the non-power-of-two is running at 16 FPS the power-of-two runs 12 FPS. I expected the power-of-two could be faster, but in my case it doesn't.

I uploaded the project I used to test. You might test in your card if you want. I read that the mobile version of OpenGL reads the power-of-two textures 10 to 20 times faster, maybe this implementation is better in the mobile platform.

Look at the image below:

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/post-processing.png)

## Shader Examples

This project has an interactive application structure. It handles the user mouse input.

This project uses the GUI library from tetris game.

It has a ShaderManager class to handle the loading and setup of all shaders this demo uses.

This project is divided in categories of shaders as follow:

### Detail - ShaderBumpMapping

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/01BumpMapping.png)

### Detail - ShaderConeStepMappingOriginal

The framework has a tool called __conestep-generator__ to compute the conemap texture from a depth map texture.

You can use the tool as the example below:

```
conestep-generator <filepath>/<filename>.png
```

The tool will generate the normal map texture and the cone map texture.

You can see the example of the cone step mapping below:

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/02ConeStepMapping.png)

### Radiometric - ShaderBrightnessContrast

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/03BrightnessContrast.png)

### Radiometric - ShaderGrayScaleInternet

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/04GrayScaleInternet.png)

### Radiometric - ShaderGrayScaleHumanVisualSystem

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/05GrayScaleHumanVisualSystem.png)

### Radiometric - ShaderGrayScaleHueSaturationValue

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/06GrayScaleHueSaturationValue.png)

### Filter - ShaderLaplace

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/07Laplace.png)

### Filter - ShaderBlur

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/08Blur.png)

### Ilumination - ShaderBlinPhongVertex

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/09BlinPhongVertex.png)

### Ilumination - ShaderBlinPhongPixel

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/10BlinPhongPixel.png)

### Texture - ShaderTwoTextures

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/11TwoTextures.png)

### Texture - ShaderProceduralTexture

![](https://github.com/A-Ribeiro/OpenGLStarter/raw/master/docs/images/shaders/12ProceduralTexture.png)


# Alessandro Ribeiro's Content

I have an instrumental music album I recorded playing electric guitar.

I put the audio in .ogg format in the framework. The audio is used in all game's project as background music.

The album is distributes by CDBaby, and they will take care of the Digital Rights Management (DRM). 

I'm telling you that because if you use the audio in any youtube video, for example, you will not be able to turn on the video monetization.

# 3rdparty's Content

I used the Jesus image from [Reji Joseph website](http://www.rejijoseph.com/search/label/Heavenly).

# 3rdparty Libraries

This framework uses the following 3rdparty libraries:

* __assimp__ (for assimp2bams tool)
* __convertutf__ (to manage UTF8 and UTF32 character set)
* __freetype__ (for font2bitmap tool)
* __glew__ (to manage OpenGL extensions)
* __libpng__ (to load and write PNG images)
* __md5__ (to aid integrity check of the readers and writers)
* __randomlib__ (to generate random values)
* __rtaudio__ (to implement direct input/output to audio devices)
* __sfml__ (to open OpenGL window, play sounds and handle Input events from keyboard, mouse and joystick)
* __zlib__ (to compile libpng and compress/decompress generic streams)
