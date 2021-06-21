#ifndef JPGHelper_h
#define JPGHelper_h

#include <stdlib.h>//NULL

namespace aRibeiro {
    
    /// \brief Read JPG format from files or memory streams.
    ///
    /// \author Alessandro Ribeiro
    ///
    class JPGHelper {
        
    public:
        /// \brief Read JPG format from file
        ///
        /// It returns the raw imagem buffer and: width, height, number of channels, pixel_depth.
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int w, h, chn, depth;
        /// char*bufferChar;
        ///
        /// bufferChar = JPGHelper::readJPG( "file.jpg", &w, &h, &chn, &depth);
        /// if (bufferChar != NULL) {
        ///     ...
        ///     JPGHelper::closeJPG(bufferChar);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param file_name Filename to load
        /// \param[out] w width
        /// \param[out] h height
        /// \param[out] chann channels
        /// \param[out] pixel_depth pixel depth
        /// \param invertY should invert the loaded image vertically
        /// \param[out] gamma the gamma value stored in JPG file
        /// \return The raw image buffer or NULL if cannot open file.
        ///
        static char* readJPG(const char *file_name, int *w, int *h, int *chann, int *pixel_depth, bool invertY = false, float *gamma = NULL);
        
        /// \brief Read JPG format from memory stream
        ///
        /// It returns the raw imagem buffer and: width, height, number of channels, pixel_depth.
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// const char *input_buffer;
        /// int input_buffer_size;
        ///
        /// int w, h, chn, depth;
        /// char*bufferChar;
        ///
        /// bufferChar = JPGHelper::readJPGFromMemory( input_buffer, input_buffer_size, &w, &h, &chn, &depth);
        /// if (bufferChar != NULL) {
        ///     ...
        ///     JPGHelper::closeJPG(bufferChar);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param input_buffer Input raw JPG compressed buffer
        /// \param input_buffer_size Buffer size
        /// \param[out] w width
        /// \param[out] h height
        /// \param[out] chann channels
        /// \param[out] pixel_depth pixel depth
        /// \param invertY should invert the loaded image vertically
        /// \param[out] gamma the gamma value stored in JPG file
        /// \return The raw image buffer or NULL if cannot open file.
        ///
        static char* readJPGFromMemory(const char *input_buffer, int input_buffer_size, int *w, int *h, int *chann, int *pixel_depth, bool invertY = false, float *gamma = NULL);
        
        /// \brief Closes the image buffer after a read or memory write.
        ///
        /// Should be called after any success read or memory write JPG image.
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// int w, h, chn, depth;
        /// char*bufferChar;
        ///
        /// bufferChar = JPGHelper::readJPG( "file.jpg", &w, &h, &chn, &depth);
        /// if (bufferChar != NULL) {
        ///     ...
        ///     JPGHelper::closeJPG(bufferChar);
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param buff pointer to a valid readed buffer
        ///
        static void closeJPG(char*&buff);
        
        
        static bool isJPGFilename(const char* filename);
    };
    
}

#endif

