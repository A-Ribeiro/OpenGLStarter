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

            bool is_internal_tag(const std::string &str) const;
            std::string internalNextChar(bool *ended);

        public:
            RichMessageTokenizer() = default;
            RichMessageTokenizer(const std::string &rich_message, char32_t separator_char = U' ');

            //assignment operator
            RichMessageTokenizer &operator=(const RichMessageTokenizer &other)
            {
                if (this != &other)
                {
                    this->generator = other.generator;
                    this->rich_message_it = this->generator.copyChangeRef(other.rich_message_it);
                    this->rich_message_it_end = this->generator.copyChangeRef(other.rich_message_it_end);
                    this->separator_char = other.separator_char;
                    this->last_char_input = other.last_char_input;
                }
                return *this;
            }
            // Move assignment operator
            RichMessageTokenizer &operator=(RichMessageTokenizer &&other) noexcept
            {
                if (this != &other)
                {
                    this->generator = std::move(other.generator);
                    this->rich_message_it = this->generator.copyChangeRef(other.rich_message_it);
                    this->rich_message_it_end = this->generator.copyChangeRef(other.rich_message_it_end);
                    this->separator_char = other.separator_char;
                    this->last_char_input = other.last_char_input;
                }
                return *this;
            }

            std::string nextChar(bool *ended);
            std::string nextWord(bool *ended);
            void nextLine(std::vector<std::string> *line_output, bool *ended);
        };
    }
}
