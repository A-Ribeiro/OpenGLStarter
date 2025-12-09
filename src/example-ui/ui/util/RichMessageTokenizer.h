#include <InteractiveToolkit/ITKCommon/StringUtil.h>

namespace AppKit
{
    namespace OpenGL
    {
        class RichMessageTokenizer
        {
            ITKCommon::StringUtil::UTF32StringGenerateIteratorFromStringCopy generator;
            ITKCommon::StringUtil::utf8_to_utf32_iterator rich_message_it;
            ITKCommon::StringUtil::utf8_to_utf32_iterator rich_message_it_end;
            char32_t separator_char;
            char32_t last_char_input;

            //std::string rich_message;

        public:
            RichMessageTokenizer(const std::string &rich_message, char32_t separator_char = U' ');

            std::string nextToken(bool *ended);
            void nextLine(std::vector<std::string> *line_output, bool *ended);

        };
    }
}
