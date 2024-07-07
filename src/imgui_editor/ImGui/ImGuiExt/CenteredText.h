#pragma once

#include <InteractiveToolkit/ITKCommon/StringUtil.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>


class CenteredText
{
    // Wrapping skips upcoming blanks
    static inline const char *CalcWordWrapNextLineStartA(const char *text, const char *text_end)
    {
        while (text < text_end && ImCharIsBlankA(*text))
            text++;
        if (*text == '\n')
            text++;
        return text;
    }

    ImVec2 CalcTextSizeA(ImFont *font, float size, float max_width, float wrap_width, const char *text_begin, const char *text_end, const char **remaining)
    {
        if (!text_end)
            text_end = text_begin + strlen(text_begin); // FIXME-OPT: Need to avoid this.

        const float line_height = size;
        const float scale = size / font->FontSize;

        ImVec2 text_size = ImVec2(0, 0);
        float line_width = 0.0f;

        const bool word_wrap_enabled = (wrap_width > 0.0f);
        const char *word_wrap_eol = NULL;

        const char *s = text_begin;

        std::u32string aux_str;

        while (s < text_end)
        {
            if (word_wrap_enabled)
            {
                // Calculate how far we can render. Requires two passes on the string data but keeps the code simple and not intrusive for what's essentially an uncommon feature.
                if (!word_wrap_eol)
                    word_wrap_eol = font->CalcWordWrapPositionA(scale, s, text_end, wrap_width - line_width);

                if (s >= word_wrap_eol)
                {
                    if (text_size.x < line_width)
                        text_size.x = line_width;
                    text_size.y += line_height;
                    
                    line_widths.push_back(line_width);
                    lines.push_back( ITKCommon::StringUtil::utf32_to_utf8(aux_str) );
                    aux_str.clear();

                    line_width = 0.0f;
                    word_wrap_eol = NULL;
                    s = CalcWordWrapNextLineStartA(s, text_end); // Wrapping skips upcoming blanks
                    continue;
                }
            }

            // Decode and advance source
            const char *prev_s = s;
            unsigned int c = (unsigned int)*s;
            if (c < 0x80)
                s += 1;
            else
                s += ImTextCharFromUtf8(&c, s, text_end);

            if (c < 32)
            {
                if (c == '\n')
                {
                    text_size.x = ImMax(text_size.x, line_width);
                    text_size.y += line_height;
                    
                    line_widths.push_back(line_width);
                    lines.push_back( ITKCommon::StringUtil::utf32_to_utf8(aux_str) );
                    aux_str.clear();

                    line_width = 0.0f;
                    continue;
                }
                if (c == '\r')
                    continue;
            }

            aux_str += c;
            const float char_width = ((int)c < font->IndexAdvanceX.Size ? font->IndexAdvanceX.Data[c] : font->FallbackAdvanceX) * scale;
            if (line_width + char_width >= max_width)
            {
                s = prev_s;
                break;
            }

            line_width += char_width;
        }

        if (text_size.x < line_width)
            text_size.x = line_width;

        if (line_width > 0 || text_size.y == 0.0f){
            text_size.y += line_height;

            line_widths.push_back(line_width);
            lines.push_back( ITKCommon::StringUtil::utf32_to_utf8(aux_str) );
            aux_str.clear();
        }

        if (remaining)
            *remaining = s;

        return text_size;
    }

    ImVec2 CalcTextSize(const char *text, float wrap_width)
    {
        ImGuiContext &g = *ImGui::GetCurrentContext();

        ImFont *font = g.Font;
        const float font_size = g.FontSize;

        ImVec2 text_size = CalcTextSizeA(font, font_size, FLT_MAX, wrap_width, text, NULL, NULL);

        // Round
        // FIXME: This has been here since Dec 2015 (7b0bf230) but down the line we want this out.
        // FIXME: Investigate using ceilf or e.g.
        // - https://git.musl-libc.org/cgit/musl/tree/src/math/ceilf.c
        // - https://embarkstudios.github.io/rust-gpu/api/src/libm/math/ceilf.rs.html
        text_size.x = IM_TRUNC(text_size.x + 0.99999f);

        return text_size;
    }

public:
    ImFont *font;
    std::string text;
    std::vector<float> line_widths;
    std::vector<std::string> lines;
    ImVec2 text_size;
    ImVec2 text_size_2;
    float text_width_max;

    void setText(const std::string &text, ImFont *font, float text_width_max)
    {
        lines.clear();
        line_widths.clear();

        this->text = text;
        this->font = font;
        this->text_width_max = text_width_max;

        if (font != NULL)
            ImGui::PushFont(font);

        text_size = CalcTextSize(text.c_str(), text_width_max);

        text_size_2 = text_size * 0.5f;

        if (font != NULL)
            ImGui::PopFont();
    }

    void drawTextOriginal(ImVec2 at_pos, ImFont *font)
    {
        if (font != this->font)
            setText(this->text, font, text_width_max);
        if (font != NULL)
            ImGui::PushFont(font);

        ImGui::SetCursorPos(at_pos - text_size_2);

        ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + text_width_max);
        ImGui::Text("%s", text.c_str());
        ImGui::PopTextWrapPos();

        if (font != NULL)
            ImGui::PopFont();
    }

    void drawText(ImVec2 at_pos, ImFont *font)
    {
        if (font != this->font)
            setText(this->text, font, text_width_max);
        if (font != NULL)
            ImGui::PushFont(font);

        float font_size = font->FontSize;
        float line_height = font_size;

        ImVec2 currentPos = -text_size_2;

        for(size_t i=0;i<lines.size();i++) {
            currentPos.x = (text_size.x - line_widths[i]) * 0.5f - text_size_2.x;
            ImGui::SetCursorPos(at_pos + currentPos);
            ImGui::Text("%s", lines[i].c_str());
            currentPos.y += line_height;
        }

        if (font != NULL)
            ImGui::PopFont();
    }
};
