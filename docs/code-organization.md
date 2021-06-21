# OpenGLStarter

[Back to HOME](index)

## Code Organization And Namespace

The structure of the CMake configuration uses a file called CMakeLists.txt in the base folder of each library or binary project you want.

The folder hierarchy of this framework is shown below: 

```
[OpenGLStarter/]
+--CMakeLists.txt
+--[AlessandroRibeiro/] -> Contains my the music albums I published
|  +--...
+--[cmake/] -> Contains the CMake additional commands to handle project configuration
|  +--...
+--[cmake-modules/] -> Contains the CMake find scripts
|  +--...
+--[docs/] -> Contains the framework documentation
|  +--...
+--[include/] -> This folder is created to contains all includes needed by the projects
|  +--...
+--[lib/] -> Contains the 3rd parties Libraries and the aRibeiro library
|  +--[aribeiro/]
|  |  +--CMakeLists.txt
|  |  +--...
|  +--...
+--[projects/] -> Contains the binary projects
|  +--[tech-demo/]
|  |  +--CMakeLists.txt
|  |  +--...
|  +--...
+--[projects-lib/] -> Contains middleware libs, wrapper libs, or derivated libs
|  +--[mini-gl-engine/]
|  |  +--CMakeLists.txt
|  |  +--...
|  +--...
+--[tools/] -> Contains the binary projects of tools
   +--[font2bitmap/]
   |  +--CMakeLists.txt
   |  +--...
   +--...
```

### Global Include and External Library In-Place Copy

There are custom commands thats create and copy the relevant header files to the __/include__ directory automatically.

__For example:__

If I need to use the glew library, so I'll create the __/lib/glew__ and add the __CMakeLists.txt__ using the framework special header command:

```
if (OpenGLStarter_Integration)
    copy_headers_to_include_directory(${PROJECT_NAME} ${PUBLIC_HEADERS})
endif()
```

When you generate the platform IDE project, CMake will create a __/include/glew__ with the library headers.

And the global include directory __/include__ will be configured in the target IDE project.

Any binary project that uses this library (glew) could simply include the header as shown below:

```cpp
#include <glew/glew.h>
...
```

__Why?__

Because MS Windows don't handle include libraries so nicely like linux.

In this framework, when you keep the library importing as described above, will guarantee that in any OS (linux, mac, windows, raspberry Pi) will have the same way to include the library.

The cons is that every library will need to use the framework CMake command. And will need to copy the 3rdparty library to the framework path also.

# Framework Code

You can take a look at the framework and see that there are a lot of code from the framework.

You can consider that every directory is a framework code.

The excepteption are the external libraries.

They are located inside the directory __/lib__.

__NOTICE:__ the __/lib/aribeiro__ is the main framework library and the other directories inside __/lib__ are 3rdparty.

I'll briefly explain the main directories of the framework below.

## aRibeiro Library [/lib/aribeiro]

This is the main library of this framework.

It defines the following namespaces:

* __aRibeiro__: Contains all math, platform, and class utilities that can be used in any kind of project (3D, console, etc...).
* __aRibeiro::collision__: Contains specifically collision code.

To use the library, you need to add the following lines to your code:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;
...
```

## Wrapper Libraries and Internal Libraries [/projects-lib]

This directory contains library projects (projects that will not generate an executable).

I reserved this directory to put:

* __Wrapper Libraries:__ Libraries that creates abstraction to external libraries:
    * __ft2-wrapper:__ library that creates abstraction of the __/lib/freetype__.
    * __zlib-wrapper:__ library that creates abstraction of the __/lib/zlib__.
* __Derived Libraries:__ Libraries that have code derived from the __/lib/aribeiro__ but it is not a base implementation:
    * __data-model:__ library with atlas, font and 3D model structure definition.
    * __mini-gl-engine:__ library that have rendering algorithms that abstracts the use of other libraries.
* __Independent Libraries:__ Libraries that is from the framework but don't use any other dependency directly:
    * __rtvideo:__ handle video capturing from .H264 supported devices.

## Projects [/projects]

This directory contain projects that generates executables.

All executable projects could include any library configured inside __/lib__ and __/projects-lib__;

After a project is configured, you can run the CMake to generate the IDE projects.

After generate the IDE projects you can build the project.

For example: to build the __/projects/tech-demo-particle__ on linux, you could to run:

```bash
make tech-demo-particle
```

[//]: # (# External Code)
[//]: # (## External Libraries [/lib])
