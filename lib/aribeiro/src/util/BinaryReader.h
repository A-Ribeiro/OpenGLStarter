#ifndef BinaryReader__H
#define BinaryReader__H

#include <stdio.h>
#include <string>
#include <vector>
#include <map>

#include <aribeiro/mat4.h>
#include <aribeiro/quat.h>
#include <aribeiro/vec2.h>
#include <aribeiro/vec3.h>
#include <aribeiro/vec4.h>

namespace aRibeiro {


/// \brief Buffered File or Memory Stream Reader.
///
/// This class can be used to read binary data saved from the data-model classes.
///
/// It does work in conjunction with the #aRibeiro::BinaryWriter, e. g., it can read what whas written by that class.
///
/// It uses the ZLIB to decompress the data and check the file integrity using the #MD5 class.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryReader binaryReader;
///
/// binaryReader.readFromFile("file.bin");
///
/// uint32_t readed_number = binaryReader.readUInt32();
/// std::string readed_string = binaryReader.readString();
/// ...
/// \endcode
///
/// \author Alessandro Ribeiro
///
class BinaryReader {

    //FILE* in;

    std::vector<uint8_t> buffer;
    size_t readPos;

    bool eof();
public:

    BinaryReader();

    /// \brief Create a reader from data allocating in the memory
    ///
    /// The default read mode uses the ZLIB and MD5 to open the memory stream.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// uint8_t* buffer_data;
    /// size_t buffer_size;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromBuffer(buffer_data, buffer_size);
    ///
    /// ...
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param data Input data pointer
    /// \param size The amount of bytes in the input data
    /// \param compressed If true, uses ZLIB and MD5 to read the stream
    ///
    void readFromBuffer(const uint8_t* data, size_t size, bool compressed = true);

    /// \brief Create a reader from file
    ///
    /// The default read mode uses the ZLIB and MD5 to open the file.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// ...
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param filename File to read
    /// \param compressed If true, uses ZLIB and MD5 to read the stream
    ///
    void readFromFile(const char* filename, bool compressed = true);
    
    ~BinaryReader();

    void close();

    /// \brief Make a copy of the current buffer in the data pointer, according the size parameter.
    ///
    /// You need to pre-allocate the size you want to read before call this method.
    ///
    /// This method is used as base to implement the other types reading.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // read an integer
    /// int readed_integer;
    /// binaryReader.read( &readed_integer, sizeof(int) );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param data The output buffer pointer to read data to
    /// \param size the size to read in bytes
    ///
    void read( void* data, int size );

    /// \brief Read 1 byte unsigned integer (uint8_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// uint8_t data_readed = binaryReader.readUInt8();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 1 byte unsigned integer (uint8_t)
    ///
    uint8_t readUInt8();

    /// \brief Read 1 byte signed integer (uint8_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// int8_t data_readed = binaryReader.readInt8();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 1 byte signed integer (uint8_t)
    ///
    int8_t readInt8();

    /// \brief Read 2 bytes unsigned integer (uint16_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// uint16_t data_readed = binaryReader.readUInt16();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 2 bytes unsigned integer (uint16_t)
    ///
    uint16_t readUInt16();

    /// \brief Read 2 bytes signed integer (int16_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// int16_t data_readed = binaryReader.readInt16();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 2 bytes signed integer (int16_t)
    ///
    int16_t readInt16();

    /// \brief Read 4 bytes unsigned integer (uint32_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// uint32_t data_readed = binaryReader.readUInt32();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 4 bytes unsigned integer (uint32_t)
    ///
    uint32_t readUInt32();

    /// \brief Read 4 bytes signed integer (int32_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// int32_t data_readed = binaryReader.readInt32();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 4 bytes signed integer (int32_t)
    ///
    int32_t readInt32();

    /// \brief Read 4 bytes single precision float point number (float)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// float data_readed = binaryReader.readFloat();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 4 bytes single precision float point number (float)
    ///
    float readFloat();

    /// \brief Read 16 bytes quaternion representation (quat)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// quat data_readed = binaryReader.readQuat();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 16 bytes quaternion representation (quat)
    ///
    quat readQuat();

    /// \brief Read 8 bytes 2D vector representation (vec2)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// vec2 data_readed = binaryReader.readVec2();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 8 bytes 2D vector representation (vec2)
    ///
    vec2 readVec2();

    /// \brief Read 12 bytes 3D vector representation (vec3)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// vec3 data_readed = binaryReader.readVec3();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 12 bytes 3D vector representation (vec3)
    ///
    vec3 readVec3();

    /// \brief Read 16 bytes 3D vector representation with homogeneous coord (vec4)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// vec4 data_readed = binaryReader.readVec4();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 16 bytes 3D vector representation with homogeneous coord (vec4)
    ///
    vec4 readVec4();

    /// \brief Read 64 bytes 4x4 Matrix representation (mat4)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// mat4 data_readed = binaryReader.readMat4();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 64 bytes 4x4 Matrix representation (mat4)
    ///
    mat4 readMat4();

    /// \brief Read 8bits character sequence string, ASCII (std::string)
    ///
    /// The maximum string size is 65535 characters (0xffff), counting with '\0' at end.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// std::string data_readed = binaryReader.readString();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \return 8bits character sequence string, ASCII (std::string)
    ///
    std::string readString();

    /// \brief Read A STL vector of single precision float
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// std::vector<float> data_readed;
    /// binaryReader.readVectorFloat( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v STL vector of single precision float
    ///
    void readVectorFloat(std::vector<float> *v);

    
    /// \brief Read A STL vector of unsigned 2bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// std::vector<uint16_t> data_readed;
    /// binaryReader.readVectorUInt16( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v STL vector of unsigned 2bytes integer
    ///
    void readVectorUInt16(std::vector<uint16_t> *v);

    /// \brief Read A STL vector of unsigned 4bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// std::vector<uint32_t> data_readed;
    /// binaryReader.readVectorUInt32( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v STL vector of unsigned 4bytes integer
    ///
    void readVectorUInt32(std::vector<uint32_t> *v);

    /// \brief Read An aligned_vector of 8 bytes 2D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// aligned_vector<vec2> data_readed;
    /// binaryReader.readVectorVec2( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_vector of 8 bytes 2D vectors
    ///
    void readVectorVec2( aligned_vector<vec2> *v);

    /// \brief Read An aligned_vector of 12 bytes 3D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// aligned_vector<vec3> data_readed;
    /// binaryReader.readVectorVec3( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_vector of 12 bytes 3D vectors
    ///
    void readVectorVec3( aligned_vector<vec3> *v);

    /// \brief Read An aligned_vector of 16 bytes 3D vectors with Homogeneous coord
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed vector
    /// aligned_vector<vec4> data_readed;
    /// binaryReader.readVectorVec4( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_vector of 16 bytes 3D vectors with Homogeneous coord
    ///
    void readVectorVec4( aligned_vector<vec4> *v);

    /// \brief Read A STL map with key = std::string and value = single precision float
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed map
    /// std::map<std::string,float> data_readed;
    /// binaryReader.readStringMapFloat( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v A STL map with key = std::string and value = single precision float
    ///
    void readStringMapFloat(std::map<std::string,float> *v);

    /// \brief Read A STL map with key = std::string and value = unsigned 4bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed map
    /// std::map<std::string,int32_t> data_readed;
    /// binaryReader.readStringMapInt32( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v A STL map with key = std::string and value = unsigned 4bytes integer
    ///
    void readStringMapInt32(std::map<std::string,int32_t> *v);

    /// \brief Read An aligned_map with key = std::string and value = 8 bytes 2D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed map
    /// aligned_map<std::string,vec2> data_readed;
    /// binaryReader.readStringMapVec2( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_map with key = std::string and value = 8 bytes 2D vectors
    ///
    void readStringMapVec2(aligned_map<std::string,vec2> *v);

    /// \brief Read An aligned_map with key = std::string and value = 12 bytes 3D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed map
    /// aligned_map<std::string,vec3> data_readed;
    /// binaryReader.readStringMapVec3( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_map with key = std::string and value = 12 bytes 3D vectors
    ///
    void readStringMapVec3(aligned_map<std::string,vec3> *v);

    /// \brief Read An aligned_map with key = std::string and value = 16 bytes 3D vectors with Homogeneous coord
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// // readed map
    /// aligned_map<std::string,vec4> data_readed;
    /// binaryReader.readStringMapVec4( &data_readed );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] v aligned_map with key = std::string and value = 16 bytes 3D vectors with Homogeneous coord
    ///
    void readStringMapVec4(aligned_map<std::string,vec4> *v);

    /// \brief Read a generic buffer from the reader.
    ///
    /// It does not make a copy of the buffer, it returns a pointer to the internal buffer that is in the memory.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryReader binaryReader;
    ///
    /// binaryReader.readFromFile("input_file.bin");
    ///
    /// uint8_t *buffer;
    /// uint32_t size;
    /// binaryReader.readBuffer( &buffer, &size );
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param[out] buffer generic buffer from the reader.
    /// \param[out] size the size of the buffer.
    ///
    void readBuffer(uint8_t **buffer, uint32_t *size);

};

/// \brief Read any structure or class that is in an aligned_vector
///
/// The structure or class need to implement the `void read( BinaryReader* reader )` method.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryReader binaryReader;
///
/// binaryReader.readFromFile("input_file.bin");
///
/// class _SSE2_ALIGN_PRE Element {
/// public:
///     vec3 pos;
///     vec3 dir;
///
///     void read( BinaryReader* reader ) {
///         pos = reader->readVec3();
///         dir = reader->readVec3();
///     }
///
///     SSE2_CLASS_NEW_OPERATOR
/// }_SSE2_ALIGN_POS;
///
/// aligned_vector<Element> data_readed;
/// BinaryReader_ReadAlignedVector( &binaryReader, &data_readed );
/// \endcode
///
/// \author Alessandro Ribeiro
/// \param reader the BinaryReader instance
/// \param[out] v the aligned_vector of a custom structure or class
///
template <typename T>
void BinaryReader_ReadAlignedVector(BinaryReader* reader, aligned_vector<T> *v) {
    v->resize(reader->readUInt32());
    for (size_t i = 0; i < v->size(); i++) {
        (*v)[i].read(reader);
    }
}

/// \brief Read any structure or class that is in an aligned_map
///
/// The key must be std::string.
///
/// The value could be a structure or a class.
///
/// The structure or class need to implement the `void read( BinaryReader* reader )` method.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryReader binaryReader;
///
/// binaryReader.readFromFile("input_file.bin");
///
/// class _SSE2_ALIGN_PRE Element {
/// public:
///     vec3 pos;
///     vec3 dir;
///
///     void read( BinaryReader* reader ) {
///         pos = reader->readVec3();
///         dir = reader->readVec3();
///     }
///
///     SSE2_CLASS_NEW_OPERATOR
/// }_SSE2_ALIGN_POS;
///
/// aligned_map<std::string,Element> data_readed;
/// BinaryReader_ReadAlignedStringMap( &binaryReader, &data_readed );
/// \endcode
///
/// \author Alessandro Ribeiro
/// \param reader the BinaryReader instance
/// \param[out] result the aligned_map of a custom structure or class
///
template <typename T>
void BinaryReader_ReadAlignedStringMap(BinaryReader* reader, aligned_map<std::string, T> *result) {
    uint32_t size = reader->readUInt32();
    //std::map<std::string, T> result;
    (*result).clear();
    for (int i = 0; i < size; i++) {
        std::string aux = reader->readString();
        T value;
        value.read(reader);
        (*result)[aux] = value;
    }
}

}

#endif
