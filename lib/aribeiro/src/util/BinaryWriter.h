#ifndef BinaryWritter__H
#define BinaryWritter__H

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

/// \brief Buffered File or Memory Stream Writer.
///
/// This class can be used to write binary data. Used by the data-model classes.
///
/// It does work in conjunction with the #aRibeiro::BinaryReader, e. g., it can write what that class reads.
///
/// It uses the ZLIB to compress the data and write the file integrity using the #MD5 class.
///
/// The write occurs when you call the #close method.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryWriter binaryWriter;
///
/// binaryWriter.writeToFile("file.bin");
///
/// uint32_t number_to_write;
/// std::string string_to_write;
///
/// binaryWriter.writeUInt32( number_to_write );
/// binaryWriter.writeString( string_to_write );
///
/// // write the buffer to file
/// binaryWriter.close();
/// \endcode
///
/// \author Alessandro Ribeiro
///
class BinaryWriter {

    std::string name;

    bool compress;
    bool _writeToFile;
public:

    std::vector<uint8_t> buffer; ///< Used when the write mode is set to memory.

    BinaryWriter();
    
    /// \brief Create a writer to file
    ///
    /// The default write mode uses the ZLIB and MD5 to write the file.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// ...
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param filename File to write
    /// \param compressed If true, uses ZLIB and MD5 to write the stream
    ///
    void writeToFile(const char* filename, bool compress = true);

    /// \brief Create a writer to data allocating in the memory
    ///
    /// The default write mode uses the ZLIB and MD5 to write the memory stream.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToBuffer();
    ///
    /// ...
    ///
    /// binaryWriter.close();
    ///
    /// // after write you can use the #buffer variable to write the data.
    /// FILE *out = fopen("file.output", "wb");
    /// if ( out ) {
    ///     fwrite( &binaryWriter.buffer[0], sizeof(uint8_t), binaryWriter.buffer.size(), out);
    ///     fclose( out );
    /// }
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param compressed If true, uses ZLIB and MD5 to read the stream
    ///
    void writeToBuffer(bool compress = true);

    ~BinaryWriter();

    /// \brief Reset the current write state and start it over
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// ...
    ///
    /// binaryWriter.reset();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    void reset();

    /// \brief Causes the current write state to be flushed to file.
    ///
    /// It cannot call write methods again after call this method, it could lead to an unpredictable behaviour.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// ...
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    void close();

    /// \brief Write a generic buffer with a size
    ///
    /// This method is used as base to implement the other writing types.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// // write an integer
    /// int integer_to_write;
    /// binaryWriter.write( &integer_to_write, sizeof(int) );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param data The pointer to the data to write
    /// \param size The size to write in bytes
    ///
    void write(void *data, size_t size);

    /// \brief Write 1 byte unsigned integer (uint8_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// uint8_t data_to_write;
    /// binaryWriter.writeUInt8( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 1 byte unsigned integer (uint8_t)
    ///
    void writeUInt8(uint8_t v);

    /// \brief Write 1 byte signed integer (int8_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// int8_t data_to_write;
    /// binaryWriter.writeInt8( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 1 byte signed integer (int8_t)
    ///
    void writeInt8(int8_t v);

    /// \brief Write 2 bytes unsigned integer (uint16_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// uint16_t data_to_write;
    /// binaryWriter.writeUInt16( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 2 bytes unsigned integer (uint16_t)
    ///
    void writeUInt16(uint16_t v);

    /// \brief Write 2 bytes signed integer (int16_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// int16_t data_to_write;
    /// binaryWriter.writeInt16( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 2 bytes signed integer (int16_t)
    ///
    void writeInt16(int16_t v);

    /// \brief Write 4 bytes unsigned integer (uint32_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// uint32_t data_to_write;
    /// binaryWriter.writeUInt32( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 4 bytes unsigned integer (uint32_t)
    ///
    void writeUInt32(uint32_t v);

    /// \brief Write 4 bytes signed integer (int32_t)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// int32_t data_to_write;
    /// binaryWriter.writeInt32( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 4 bytes signed integer (int32_t)
    ///
    void writeInt32(int32_t v);

    /// \brief Write 4 bytes single precision float point number (float)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// float data_to_write;
    /// binaryWriter.writeFloat( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 4 bytes single precision float point number (float)
    ///
    void writeFloat(float v);

    /// \brief Write 16 bytes quaternion representation (quat)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// quat data_to_write;
    /// binaryWriter.writeQuat( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 16 bytes quaternion representation (quat)
    ///
    void writeQuat(const quat &v);

    /// \brief Write 8 bytes 2D vector representation (vec2)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// vec2 data_to_write;
    /// binaryWriter.writeVec2( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 8 bytes 2D vector representation (vec2)
    ///
    void writeVec2(const vec2 &v);

    /// \brief Write 12 bytes 3D vector representation (vec3)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// vec3 data_to_write;
    /// binaryWriter.writeVec3( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 12 bytes 3D vector representation (vec3)
    ///
    void writeVec3(const vec3 &v);

    /// \brief Write 16 bytes 3D vector representation with homogeneous coord (vec4)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// vec4 data_to_write;
    /// binaryWriter.writeVec4( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 16 bytes 3D vector representation with homogeneous coord (vec4)
    ///
    void writeVec4(const vec4 &v);

    /// \brief Write 64 bytes 4x4 Matrix representation (mat4)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// mat4 data_to_write;
    /// binaryWriter.writeMat4( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v 64 bytes 4x4 Matrix representation (mat4)
    ///
    void writeMat4(const mat4 &v);

    /// \brief Write 8bits character sequence string, ASCII (std::string)
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::string data_to_write;
    /// binaryWriter.writeString( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param s 8bits character sequence string, ASCII (std::string)
    ///
    void writeString(const std::string &s);

    /// \brief Write STL vector of single precision float
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::vector<float> data_to_write;
    /// binaryWriter.writeVectorFloat( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v STL vector of single precision float
    ///
    void writeVectorFloat(const std::vector<float> &v);

    /// \brief Write STL vector of unsigned 2bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::vector<uint16_t> data_to_write;
    /// binaryWriter.writeVectorUInt16( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v STL vector of unsigned 4bytes integer
    ///
    void writeVectorUInt16(const std::vector<uint16_t> &v);

    /// \brief Write STL vector of unsigned 4bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::vector<uint32_t> data_to_write;
    /// binaryWriter.writeVectorUInt32( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v STL vector of unsigned 4bytes integer
    ///
    void writeVectorUInt32(const std::vector<uint32_t> &v);

    /// \brief Write aligned_vector of 8 bytes 2D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_vector<vec2> data_to_write;
    /// binaryWriter.writeVectorVec2( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_vector of 8 bytes 2D vectors
    ///
    void writeVectorVec2(const aligned_vector<vec2> &v);

    /// \brief Write aligned_vector of 12 bytes 3D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_vector<vec3> data_to_write;
    /// binaryWriter.writeVectorVec3( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_vector of 12 bytes 3D vectors
    ///
    void writeVectorVec3(const aligned_vector<vec3> &v);

    /// \brief Write aligned_vector of 16 bytes 3D vectors with Homogeneous coord
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_vector<vec4> data_to_write;
    /// binaryWriter.writeVectorVec4( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_vector of 16 bytes 3D vectors with Homogeneous coord
    ///
    void writeVectorVec4(const aligned_vector<vec4> &v);

    /// \brief Write STL map with key = std::string and value = single precision float
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::map<std::string,float> data_to_write;
    /// binaryWriter.writeStringMapFloat( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v A STL map with key = std::string and value = single precision float
    ///
    void writeStringMapFloat(const std::map<std::string,float> &v);

    /// \brief Write STL map with key = std::string and value = unsigned 4bytes integer
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// std::map<std::string,int32_t> data_to_write;
    /// binaryWriter.writeStringMapInt32( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v A STL map with key = std::string and value = unsigned 4bytes integer
    ///
    void writeStringMapInt32(const std::map<std::string,int32_t> &v);

    /// \brief Write aligned_map with key = std::string and value = 8 bytes 2D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_map<std::string,vec2> data_to_write;
    /// binaryWriter.writeStringMapVec2( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_map with key = std::string and value = 8 bytes 2D vectors
    ///
    void writeStringMapVec2(const aligned_map<std::string,vec2> &v);

    /// \brief Write aligned_map with key = std::string and value = 12 bytes 3D vectors
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_map<std::string,vec3> data_to_write;
    /// binaryWriter.writeStringMapVec3( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_map with key = std::string and value = 12 bytes 3D vectors
    ///
    void writeStringMapVec3(const aligned_map<std::string,vec3> &v);

    /// \brief Write aligned_map with key = std::string and value = 16 bytes 3D vectors with Homogeneous coord
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// aligned_map<std::string,vec4> data_to_write;
    /// binaryWriter.writeStringMapVec4( data_to_write );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param v aligned_map with key = std::string and value = 16 bytes 3D vectors with Homogeneous coord
    ///
    void writeStringMapVec4(const aligned_map<std::string,vec4> &v);

    /// \brief Write a generic buffer from the writer.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// BinaryWriter binaryWriter;
    ///
    /// binaryWriter.writeToFile("file.bin");
    ///
    /// uint8_t *buffer;
    /// uint32_t size;
    /// binaryWriter.writeBuffer( &buffer, &size );
    ///
    /// binaryWriter.close();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    /// \param buffer generic buffer.
    /// \param size the size of the buffer.
    ///
    void writeBuffer(const uint8_t* buffer, uint32_t size);

};


/// \brief Write any structure or class that is in an aligned_vector
///
/// The structure or class need to implement the `void write( BinaryWriter* writer )` method.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryWriter binaryWriter;
///
/// binaryWriter.writeToFile("file.bin");
///
/// class _SSE2_ALIGN_PRE Element {
/// public:
///     vec3 pos;
///     vec3 dir;
///
///     void write( BinaryWriter* writer ) {
///         writer->writeVec3( pos );
///         writer->writeVec3( dir );
///     }
///
///     SSE2_CLASS_NEW_OPERATOR
/// }_SSE2_ALIGN_POS;
///
/// aligned_vector<Element> data_to_write;
/// BinaryWriter_WriteAlignedVector( &binaryWriter, &data_to_write );
///
/// binaryWriter.close();
/// \endcode
///
/// \author Alessandro Ribeiro
/// \param[out] writer the BinaryWriter instance
/// \param v the aligned_vector of a custom structure or class
///
template <typename T>
void BinaryWriter_WriteAlignedVector(BinaryWriter* writer, const aligned_vector<T> &v) {
    writer->writeUInt32(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        v[i].write(writer);
    }
}

/// \brief Write any structure or class that is in an aligned_map
///
/// The key must be std::string.
///
/// The value could be a structure or a class.
///
/// The structure or class need to implement the `void write( BinaryWriter* writer )` method.
///
/// Example:
///
/// \code
/// #include <aribeiro/aribeiro.h>
/// using namespace aRibeiro;
///
/// BinaryWriter binaryWriter;
///
/// binaryWriter.writeToFile("file.bin");
///
/// class _SSE2_ALIGN_PRE Element {
/// public:
///     vec3 pos;
///     vec3 dir;
///
///     void write( BinaryWriter* writer ) {
///         writer->writeVec3( pos );
///         writer->writeVec3( dir );
///     }
///
///     SSE2_CLASS_NEW_OPERATOR
/// }_SSE2_ALIGN_POS;
///
/// aligned_map<std::string,Element> data_to_write;
/// BinaryWriter_WriteAlignedStringMap( &binaryWriter, &data_to_write );
///
/// binaryWriter.close();
/// \endcode
///
/// \author Alessandro Ribeiro
/// \param[out] writer the BinaryWriter instance
/// \param v the aligned_map of a custom structure or class
///
template <typename T>
void BinaryWriter_WriteAlignedStringMap(BinaryWriter* writer, const aligned_map<std::string, T> &v) {
    writer->writeUInt32(v.size());
    typename aligned_map<std::string, T>::const_iterator it;
    for (it = v.begin(); it != v.end(); it++) {
        writeString(it->first);
        (it->second).write(writer);
    }
}

}


#endif
