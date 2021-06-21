# OpenGLStarter

[Back to HOME](index)

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
