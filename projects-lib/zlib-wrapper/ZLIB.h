#ifndef zlibwrapper_ZLIB__H__
#define zlibwrapper_ZLIB__H__

#include <aribeiro/aribeiro.h>
//using namespace aRibeiro;
namespace zlibWrapper {
    /// \brief Wrapper class to the zlib library
    ///
    /// \author Alessandro Ribeiro
    ///
    class ZLIB {
    public:

        std::vector<uint8_t> zlibOutput;///< will have the result (the compressed or decompressed buffer).

        /// \brief Compress a block of data
        ///
        /// The result will be available through the #zlibOutput STL vector.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <zlib-wrapper/zlib-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace zlibWrapper;
        ///
        /// ZLIB zlib_instance;
        ///
        /// ...
        ///
        /// char* buffer;
        /// int size;
        /// zlib_instance.compress( buffer, size );
        ///
        /// // The result will be in the zlib_instance.zlibOutput
        /// ...
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param data buffer to compress
        /// \param size the size of the buffer in bytes
        /// \param OnData callback. Called when the compression progress is done.
        ///
        void compress(const uint8_t* data, uint32_t size,
            const aRibeiro::aribeiro_OnDataMethodPtrType &OnData = NULL);

        /// \brief Decompress a block of data
        ///
        /// The result will be available through the #zlibOutput STL vector.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// #include <zlib-wrapper/zlib-wrapper.h>
        /// using namespace aRibeiro;
        /// using namespace zlibWrapper;
        ///
        /// ZLIB zlib_instance;
        ///
        /// ...
        ///
        /// char* buffer;
        /// int size;
        /// zlib_instance.uncompress( buffer, size );
        ///
        /// // The result will be in the zlib_instance.zlibOutput
        /// ...
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param data buffer to decompress
        /// \param size the size of the buffer in bytes
        /// \param OnData callback. Called when the decompression progress is done.
        ///
        void uncompress(const uint8_t* data, uint32_t size,
            const aRibeiro::aribeiro_OnDataMethodPtrType &OnData = NULL);
    };
}

#endif
