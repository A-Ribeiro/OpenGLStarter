#ifndef string__util__h__
#define string__util__h__

#include <vector>
#include <string>

namespace aRibeiro {

    /// \brief Common string operation for use with the font renderer and generic string tests
    ///
    /// \author Alessandro Ribeiro
    ///
    class StringUtil {
        //private copy constructores, to avoid copy...
        StringUtil(const StringUtil& v);
        void operator=(const StringUtil&v);

    public:

        StringUtil();

        std::vector<char> char_buffer;///< Result of the use of the #StringUtil::printf method
        std::vector<wchar_t> wchar_buffer;///< Result of the use of the #StringUtil::wprintf method

        /// \brief Get the RAW char string pointer
        ///
        /// \author Alessandro Ribeiro
        /// \return pointer to the result of the #StringUtil::printf method
        ///
        const char* char_ptr()const;

        /// \brief Get the RAW wchar_t string pointer
        ///
        /// \author Alessandro Ribeiro
        /// \return pointer to the result of the #StringUtil::wprintf method
        ///
        const wchar_t* wchar_ptr()const;

        /// \brief Wrapper of the stdio printf to work with std::string type
        ///
        /// It is possible to use it to generate a string that will be rendered as font glyphs.
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// float fps;
        /// StringUtil stringUtil;
        ///
        /// const char* output_char_string = stringUtil.printf("fps: %.2f", fps);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param format stdio printf like input string format descriptor
        /// \param ... va_list args
        /// \return char pointer to the generated string
        ///
        const char* printf(const char* format, ...);

        /// \brief Wrapper of the stdio wprintf to work with std::wstring type
        ///
        /// It is possible to use it to generate a string that will be rendered as font glyphs.
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// float fps;
        /// StringUtil stringUtil;
        ///
        /// const wchar_t* output_wchar_string = stringUtil.wprintf(L"fps: %.2f", fps);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param format stdio wprintf like input string format descriptor
        /// \param ... va_list args
        /// \return wchar_t pointer to the generated string
        ///
        const wchar_t* wprintf(const wchar_t* format, ...);

        /// \brief Convert any char string to wchar_t string
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// std::wstring output = StringUtil::toWString(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str char string
        /// \return wchar_t string
        ///
        static std::wstring toWString(const std::string &str);
        static std::string toString(const std::wstring &wstr);

        /// \brief test if a string starts with another string
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// if ( StringUtil::startsWith(input, "prefix") ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str input string
        /// \param prefix string to check if str starts with
        /// \return true if the str starts with prefix
        ///
        static bool startsWith(const std::string str, const std::string prefix);

        /// \brief test if a string ends with another string
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// std::string input;
        ///
        /// if ( StringUtil::endsWith(input, ".png") ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param str input string
        /// \param sufix string to check if str ends with
        /// \return true if the str ends with sufix
        ///
        static bool endsWith(const std::string str, const std::string sufix);
        
        
        static std::string toLower(const std::string str);

        static bool contains(const std::string str, const std::string v);

    };

}

#endif
