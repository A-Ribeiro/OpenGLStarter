# OpenGLStarter

[Back to HOME](../../index)

## String Util

The class __StringUtil__ could be used to construct strings in memory using the _printf_ style format.

It also contains static methods that implements common string operations.

String construction example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

//input variable
float fps;

//StringUtil instance
StringUtil stringUtil;

// 8bit char example
const char* output_char_string = stringUtil.printf("fps: %.2f", fps);

// wchar example
const wchar_t* output_wchar_string = stringUtil.wprintf(L"fps: %.2f", fps);
```

Static methods listing:

* __toWString:__ Converts a std::string to a std::wstring
* __toString:__ Converts a std::wstring to a std::string
* __startsWith:__ Check if a std::string starts with another std::string
* __endsWith:__ Check if a std::string ends with another std::string
* __toLower:__ Converts a std::string to lower case
* __contains:__  Check if a std::string contains another std::string

Example:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

{
    std::string input;
    std::wstring output = StringUtil::toWString(input);
}

{
    std::wstring input;
    std::string output = StringUtil::toString(input);
}

{
    std::string input;
    std::string output = StringUtil::toLower(input);
}

{
    std::string input;
    if ( StringUtil::startsWith(input, "prefix") ) {
        // ...
    }
}

{
    std::string input;
    if ( StringUtil::endsWith(input, "postfix") ) {
        // ...
    }
}

{
    std::string input;
    if ( StringUtil::contains(input, "inside") ) {
        // ...
    }
}
```
