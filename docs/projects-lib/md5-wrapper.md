# OpenGLStarter

[Back to HOME](../index)

## MD5 Wrapper

The class __MD5__ wrapps MD5 library.

You can compute the MD5 hash from any file of buffer.

See the example below:

```cpp
#include <aribeiro/aribeiro.h>
#include <md5-wrapper/md5-wrapper.h>
using namespace aRibeiro;
using namespace md5Wrapper;

//input buffer
char* buffer;
int size;

//output hash
unsigned char outputbuffer[16];

MD5::get16bytesHashFromBytes( buffer, size, outputbuffer );
```
