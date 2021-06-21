# OpenGLStarter

[Back to HOME](../index)

## Zlib Wrapper

The class __ZLIB__ wrapps Zlib library.

You can compress and uncompress any buffer.

This wrapper uses the __md5-wrapper__ to check the integrity of the compressed buffer.

Take a look at the compress example below:

```cpp
#include <aribeiro/aribeiro.h>
#include <zlib-wrapper/zlib-wrapper.h>
using namespace aRibeiro;
using namespace zlibWrapper;

ZLIB zlib_instance;

// uncompressed input buffer
char* buffer;
int size;

zlib_instance.compress( buffer, size );

// The result will be in the zlib_instance.zlibOutput
// ...
```

Take a look at the uncompress example below:

```cpp
#include <aribeiro/aribeiro.h>
#include <zlib-wrapper/zlib-wrapper.h>
using namespace aRibeiro;
using namespace zlibWrapper;

ZLIB zlib_instance;

// compressed input buffer
char* buffer;
int size;

zlib_instance.uncompress( buffer, size );

// The result will be in the zlib_instance.zlibOutput
// ...
```
