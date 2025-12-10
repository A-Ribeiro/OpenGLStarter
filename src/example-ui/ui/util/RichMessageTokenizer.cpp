#include "RichMessageTokenizer.h"

namespace AppKit
{
    namespace OpenGL
    {
        RichMessageTokenizer::RichMessageTokenizer(const std::string &rich_message, char32_t separator_char)
            : separator_char(separator_char),
              last_char_input(0),
              generator(rich_message)
        {
            this->rich_message_it = generator.begin();
            this->rich_message_it_end = generator.end();
        }

        bool RichMessageTokenizer::is_internal_tag(const std::string &str) const
        {
            return str.length() >= 2 && str[0] == '{' && str[str.length() - 1] == '}';
        }

        std::string RichMessageTokenizer::nextChar(bool *ended)
        {
            bool internal_ended = false;
            std::string result;
            std::string aux = internalNextChar(&internal_ended);
            if (!internal_ended)
            {
                *ended = false;
                result += aux;
                while (is_internal_tag(aux) && !internal_ended)
                {
                    aux = internalNextChar(&internal_ended);
                    result += aux;
                }
            }
            else
                *ended = true;
            return result;
        }

        std::string RichMessageTokenizer::internalNextChar(bool *ended)
        {
            if (rich_message_it == rich_message_it_end)
            {
                *ended = true;
                return "";
            }

            std::vector<char32_t> result;
            // last_char_input = 0;
            char32_t char_input = 0;

            int brace_count = 0;

            while (rich_message_it != rich_message_it_end)
            {
                char_input = *rich_message_it;

                // Check for double braces escape sequences
                if (char_input == U'{' && last_char_input == U'{')
                {
                    // Replace the last '{' with nothing and add single '{'
                    if (result.size() > 0 && result.back() == U'{')
                    {
                        // result.pop_back();
                        brace_count--; // Decrement since we're removing the brace that incremented it
                    }
                    result.push_back(char_input);
                    last_char_input = 0; // Reset to avoid triple brace issues
                    ++rich_message_it;
                    if (brace_count == 0)
                    {
                        *ended = false;
                        return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                    }
                }
                else if (char_input == U'}' && last_char_input == U'}')
                {
                    // Replace the last '}' with nothing and add single '}'
                    if (result.size() > 0 && result.back() == U'}')
                    {
                        // result.pop_back();
                        brace_count++; // Increment since we're removing the brace that decremented it
                    }
                    result.push_back(char_input);
                    last_char_input = 0; // Reset to avoid triple brace issues
                    ++rich_message_it;
                    if (brace_count == 0)
                    {
                        *ended = false;
                        return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                    }
                }
                else if (last_char_input == U'}')
                {
                    if (brace_count < 0)
                        brace_count = 0; // Prevent negative brace count
                    last_char_input = 0; // Reset to avoid triple brace issues
                    *ended = false;
                    return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                }

                // Track brace depth
                if (char_input == U'{')
                {
                    brace_count++;
                    result.push_back(char_input);
                    last_char_input = char_input;
                    ++rich_message_it;
                }
                else if (char_input == U'}')
                {
                    brace_count--;
                    result.push_back(char_input);
                    last_char_input = char_input;
                    ++rich_message_it;

                    if (brace_count == 0)
                    {
                        last_char_input = 0;
                        *ended = false;
                        return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                    }
                }
                else if (brace_count == 0)
                {
                    // Regular character

                    result.push_back(char_input);
                    last_char_input = char_input;
                    ++rich_message_it;

                    *ended = false;
                    return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                }
                else
                {
                    // Regular character

                    result.push_back(char_input);
                    last_char_input = char_input;
                    ++rich_message_it;

                    // return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                }
            }

            *ended = result.size() == 0;
            if (*ended)
                return "";
            return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
        }

        std::string RichMessageTokenizer::nextWord(bool *ended)
        {
            if (rich_message_it == rich_message_it_end)
            {
                if (last_char_input == separator_char)
                {
                    last_char_input = 0;
                    *ended = false;
                    return "";
                }
                else
                {
                    *ended = true;
                    return "";
                }
            }
            std::vector<char32_t> result;
            // last_char_input = 0;
            char32_t char_input = 0;

            int brace_count = 0;

            while (rich_message_it != rich_message_it_end)
            {
                char_input = *rich_message_it;

                // Check for double braces escape sequences
                if (char_input == U'{' && last_char_input == U'{')
                {
                    // Replace the last '{' with nothing and add single '{'
                    if (result.size() > 0 && result.back() == U'{')
                    {
                        // result.pop_back();
                        brace_count--; // Decrement since we're removing the brace that incremented it
                    }
                    result.push_back(char_input);
                    last_char_input = 0; // Reset to avoid triple brace issues
                    ++rich_message_it;
                    continue;
                }
                else if (char_input == U'}' && last_char_input == U'}')
                {
                    // Replace the last '}' with nothing and add single '}'
                    if (result.size() > 0 && result.back() == U'}')
                    {
                        // result.pop_back();
                        brace_count++; // Increment since we're removing the brace that decremented it
                    }
                    result.push_back(char_input);
                    last_char_input = 0; // Reset to avoid triple brace issues
                    ++rich_message_it;
                    continue;
                }
                else if (last_char_input == U'}')
                {
                    if (brace_count < 0)
                        brace_count = 0; // Prevent negative brace count
                    last_char_input = 0; // Reset to avoid triple brace issues
                }

                // Handle separator outside braces
                if (brace_count == 0 && char_input == separator_char)
                {
                    last_char_input = char_input;
                    ++rich_message_it;
                    *ended = false;
                    return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                }
                // Handle newline outside braces
                else if (brace_count == 0 && char_input == U'\n')
                {
                    if (result.size() == 0)
                    {
                        // case the last char is a separator, return empty token
                        if (last_char_input == separator_char)
                        {
                            last_char_input = 0;
                            *ended = false;
                            return "";
                        }
                        last_char_input = char_input;
                        ++rich_message_it;
                        *ended = false;
                        return "\n";
                    }
                    else
                    {
                        *ended = false;
                        return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
                    }
                }
                // Track brace depth
                else if (char_input == U'{')
                {
                    brace_count++;
                    result.push_back(char_input);
                }
                else if (char_input == U'}')
                {
                    brace_count--;
                    result.push_back(char_input);
                }
                // Regular character
                else
                {
                    result.push_back(char_input);
                }

                last_char_input = char_input;
                ++rich_message_it;
            }

            *ended = result.size() == 0;
            if (*ended)
                return "";
            return ITKCommon::StringUtil::utf32_to_utf8(std::u32string(result.data(), result.size()));
        }

        void RichMessageTokenizer::nextLine(std::vector<std::string> *line_output, bool *ended)
        {
            line_output->clear();
            bool local_ended = false;
            *ended = true;
            while (!local_ended)
            {
                std::string token = nextWord(&local_ended);
                if (!local_ended)
                {
                    *ended = false;
                    if (token == "\n")
                        break;
                    line_output->push_back(token);
                }
            }
        }
    }
}
