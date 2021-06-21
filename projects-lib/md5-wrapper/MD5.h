#ifndef MD5__H___
#define MD5__H___

#include <string>
namespace md5Wrapper {
    /// \brief Wrapper class to the md5 library
    ///
    /// \author Alessandro Ribeiro
    ///
    class MD5
    {
    public:
        /// \brief Compute the MD5 hash bytes and return the hex string from a pointer to memory.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <md5-wrapper/md5-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace md5Wrapper;
        ///
        /// ...
        ///
        /// char* buffer;
        /// int size;
        /// std::string hex_string = MD5::getHexStringHashFromBytes( buffer, size );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param buffer input buffer to compute the MD5
        /// \param size the size of the buffer in bytes
        /// \return Hex String of the 16bytes MD hash
        ///
        static std::string getHexStringHashFromBytes(const char* buffer, int size);

        /// \brief Compute the MD5 hash bytes from a pointer to memory.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <md5-wrapper/md5-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace md5Wrapper;
        ///
        /// ...
        ///
        /// char* buffer;
        /// int size;
        /// unsigned char outputbuffer[16];
        /// MD5::get16bytesHashFromBytes( buffer, size, outputbuffer );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param buffer input buffer to compute the MD5
        /// \param size the size of the buffer in bytes
        /// \param[out] outBuffer the 16 byte output buffer
        ///
        static void get16bytesHashFromBytes(const char* buffer, int size, unsigned char outBuffer[16]);

        /// \brief Compute the MD5 hash bytes and return the hex string from a file.
        ///
        /// It will not allocate the entire file to memory to compute the md5.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <md5-wrapper/md5-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace md5Wrapper;
        ///
        /// ...
        ///
        /// std::string hex_string = MD5::getHexStringHashFromFile( "input_file.bin" );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param filename input file to compute the MD5
        /// \return Hex String of the 16bytes MD hash
        ///
        static std::string getHexStringHashFromFile(const char* filename);

        /// \brief Compute the MD5 hash bytes from a file.
        ///
        /// It will not allocate the entire file to memory to compute the md5.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <md5-wrapper/md5-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace md5Wrapper;
        ///
        /// ...
        ///
        /// unsigned char outputbuffer[16];
        /// MD5::get16bytesHashFromFile( "input_file.bin", outputbuffer );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param filename input file to compute the MD5
        /// \param[out] outBuffer the 16 byte output buffer
        ///
        static void get16bytesHashFromFile(const char* filename, unsigned char outBuffer[16]);

        /// \brief Converts the 16bytes representation to a 32 character string.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <md5-wrapper/md5-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace md5Wrapper;
        ///
        /// ...
        ///
        /// unsigned char inputBuffer[16];
        /// std::string result = MD5::_16BytesToHexString( inputBuffer );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param md5 md5 16 bytes input
        /// \return Hex String of the 16bytes MD hash
        ///
        static std::string _16BytesToHexString(unsigned char md5[16]);
    };
}

#endif