# OpenGLStarter

[Back to HOME](../../index)

## Binary Stream

There are two classes to deal with binary streams.

The __BinaryReader__ and the __BinaryWritter__.

## BinaryReader

With the binary reader you can read raw streams or read _zlib_ compressed streams.

The example below read a raw binary stream from a file:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

BinaryReader binaryReader;

// the second parameter means: false -> not use compression
binaryReader.readFromFile("file.bin", false);

uint32_t readed_number = binaryReader.readUInt32();
std::string readed_string = binaryReader.readString();
```

## BinaryWriter

With the binary writer you can write raw streams or write _zlib_ compressed streams.

The example below writes a raw binary stream to a file:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

BinaryWriter binaryWriter;

// the second parameter means: false -> not use compression
binaryWriter.writeToFile("file.bin", false);

uint32_t number_to_write;
std::string string_to_write;

binaryWriter.writeUInt32( number_to_write );
binaryWriter.writeString( string_to_write );

// write the buffer to file
binaryWriter.close();
```

## Supported Types for Reading or Writing

There are types supported directly from the base class (__BinaryReader__/__BinaryWriter__), like:

* readUInt8 / writeUInt8
* readInt8 / writeInt8
* readUInt16 / writeUInt16
* readInt16 / writeInt16
* readUInt32 / writeUInt32
* readInt32 / writeInt32
* readFloat / writeFloat
* readQuat / writeQuat
* readVec2 / writeVec2
* readVec3 / writeVec3
* readVec4 / writeVec4
* readMat4 / writeMat4
* readString / writeString
* readVectorFloat / writeVectorFloat
* readVectorUInt16 / writeVectorUInt16
* readVectorUInt32 / writeVectorUInt32
* readVectorVec2 / writeVectorVec2
* readVectorVec3 / writeVectorVec3
* readVectorVec4 / writeVectorVec4
* readStringMapFloat / writeStringMapFloat
* readStringMapInt32 / writeStringMapInt32
* readStringMapVec2 / writeStringMapVec2
* readStringMapVec3 / writeStringMapVec3
* readStringMapVec4 / writeStringMapVec4
* readBuffer / writeBuffer

## Vector or StringMap of Complex Class Structure

If you have a class that have a complex structure, you can read / write using a special template created for this.

The templates are:

* BinaryReader_ReadAlignedVector
* BinaryReader_ReadAlignedStringMap
* BinaryWriter_WriteAlignedVector
* BinaryWriter_WriteAlignedStringMap

Take a look at the example below of reading a vector:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class _SSE2_ALIGN_PRE Element {
public:
    vec3 pos;
    vec3 dir;
    
    // implementation of the read method 
    // to use inside the template
    void read( BinaryReader* reader ) {
        pos = reader->readVec3();
        dir = reader->readVec3();
    }
    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;

BinaryReader binaryReader;
binaryReader.readFromFile("input_file.bin", false);

aligned_vector<Element> data_readed;

BinaryReader_ReadAlignedVector( &binaryReader, &data_readed );

// now can use the data_readed ...
```

Take a look at the example below of reading a string map:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class _SSE2_ALIGN_PRE Element {
public:
    vec3 pos;
    vec3 dir;

    // implementation of the read method 
    // to use inside the template
    void read( BinaryReader* reader ) {
        pos = reader->readVec3();
        dir = reader->readVec3();
    }

    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;

BinaryReader binaryReader;

binaryReader.readFromFile("input_file.bin", false);

aligned_map<std::string,Element> data_readed;

BinaryReader_ReadAlignedStringMap( &binaryReader, &data_readed );

// now can use the data_readed ...
```

Now the writing example of a vector write:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class _SSE2_ALIGN_PRE Element {
public:
    vec3 pos;
    vec3 dir;

    // implementation of the write method 
    // to use inside the template
    void write( BinaryWriter* writer ) {
        writer->writeVec3( pos );
        writer->writeVec3( dir );
    }
    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;

BinaryWriter binaryWriter;

binaryWriter.writeToFile("file.bin", false);

aligned_vector<Element> data_to_write;
BinaryWriter_WriteAlignedVector( &binaryWriter, &data_to_write );

binaryWriter.close();
```

And the writing example of a string map write:

```cpp
#include <aribeiro/aribeiro.h>
using namespace aRibeiro;

class _SSE2_ALIGN_PRE Element {
public:
    vec3 pos;
    vec3 dir;

    // implementation of the write method 
    // to use inside the template
    void write( BinaryWriter* writer ) {
        writer->writeVec3( pos );
        writer->writeVec3( dir );
    }
    SSE2_CLASS_NEW_OPERATOR
}_SSE2_ALIGN_POS;

BinaryWriter binaryWriter;

binaryWriter.writeToFile("file.bin", false);

aligned_map<std::string,Element> data_to_write;
BinaryWriter_WriteAlignedStringMap( &binaryWriter, &data_to_write );

binaryWriter.close();
```

## File Formats That Uses Compressed Binary Streams

The framework have three file formats that uses directly these classes: 

* __bams__: Binary Asilva Mesh Scene. Stores 3DModel information.
* __asbgt2__: ASilva Binary Glyph Table, version 2.0. Stores the glyph table data information of a font file.
* __basof2__: Binary ASilva OpenGL Font, version 2.0. Stores two sets of  glyph table data and image. The first set is the front face information of a font file, and the second is the outline face information of a font file.
