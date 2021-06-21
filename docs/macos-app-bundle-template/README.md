
## MacOS App

Before create an app in mac OS, you need to build the executable binary.

After that you need to setup a folder with a specific structure.

I'll show how to build it using the __shader-examples__ project  

### 1) Copy the template folder

Inside the __doc/macos-app-bundle-template/template-app__ there is a base folder structure to bundle an app.

Copy that folder to a folder with the name of the project with a __.app__ extension.

```bash
cp -R ../../../doc/macos-app-bundle-template/template-app shader-examples.app
```
  
### 2) Use otool to check the dependecies

```bash
otool -L shader-examples
```

The result:

```
shader-examples:

/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL (compatibility version 1.0.0, current version 1.0.0)
/System/Library/Frameworks/Foundation.framework/Versions/C/Foundation (compatibility version 300.0.0, current version 1560.12.0)
/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit (compatibility version 45.0.0, current version 1671.10.106)
/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit (compatibility version 1.0.0, current version 275.0.0)
/System/Library/Frameworks/Carbon.framework/Versions/A/Carbon (compatibility version 2.0.0, current version 158.0.0)
/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1252.200.5)
@rpath/../Frameworks/freetype.framework/Versions/A/freetype (compatibility version 18.0.0, current version 18.4.0)
/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 400.9.4)
/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation (compatibility version 150.0.0, current version 1560.12.0)
/System/Library/Frameworks/CoreGraphics.framework/Versions/A/CoreGraphics (compatibility version 64.0.0, current version 1247.4.1)
/System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices (compatibility version 1.0.0, current version 934.0.0)
/usr/lib/libobjc.A.dylib (compatibility version 1.0.0, current version 228.0.0)
```

Now we know that the freetype library is used by the project.

### 3) Copy the library dependency to the app folder

You need to do this to every framework the app use. In the case of the __shader-examples__, it uses just the freetype.

```bash
cp -R ../../../lib/sfml/extlibs/libs-osx/Frameworks/freetype.framework shader-examples.app/Contents/MacOS/Frameworks/freetype.framework
```

### 4) Use the install_name_tool

Update the binary rpath to executable_path relative reference using the install_name_tool.

```
install_name_tool -change @rpath/../Frameworks/freetype.framework/Versions/A/freetype @executable_path/Frameworks/freetype.framework/Versions/A/freetype shader-examples
```

### 5) Check the new relative reference using otool

```
otool -L shader-examples
```

The result:

```
shader-examples:

/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL (compatibility version 1.0.0, current version 1.0.0)
/System/Library/Frameworks/Foundation.framework/Versions/C/Foundation (compatibility version 300.0.0, current version 1560.12.0)
/System/Library/Frameworks/AppKit.framework/Versions/C/AppKit (compatibility version 45.0.0, current version 1671.10.106)
/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit (compatibility version 1.0.0, current version 275.0.0)
/System/Library/Frameworks/Carbon.framework/Versions/A/Carbon (compatibility version 2.0.0, current version 158.0.0)
/usr/lib/libSystem.B.dylib (compatibility version 1.0.0, current version 1252.200.5)
@executable_path/Frameworks/freetype.framework/Versions/A/freetype (compatibility version 18.0.0, current version 18.4.0)
/usr/lib/libc++.1.dylib (compatibility version 1.0.0, current version 400.9.4)
/System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation (compatibility version 150.0.0, current version 1560.12.0)
/System/Library/Frameworks/CoreGraphics.framework/Versions/A/CoreGraphics (compatibility version 64.0.0, current version 1247.4.1)
/System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices (compatibility version 1.0.0, current version 934.0.0)
/usr/lib/libobjc.A.dylib (compatibility version 1.0.0, current version 228.0.0)
```

### 6) Copy the executable binary to the bundle

```bash
cp shader-examples shader-examples.app/Contents/MacOS/
```

### 7) Copy the resources to the bundle

```
cp -R ../../../projects/shader-examples/resources shader-examples.app/Contents/MacOS/
```

## Finish the configuration

You need to edit the __shader-examples.app/Contents/Info.plist__ with the app information: name, bundleid, version, etc...

After that you need to edit the __shader-examples.app/Contents/MacOS/start.sh__ and change the __./shader-examples__ to the name of the binary you copied.

And finally you need to create the app icon. The format supported is the __.icns__ file format. You can change the icon located in the path: __shader-examples.app/Contents/Resources/icon.icns__.

Now you can compress the __shader-examples.app__ and give to your friends.

