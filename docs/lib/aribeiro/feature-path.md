# OpenGLStarter

[Back to HOME](../../index)

## Platform Path

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
