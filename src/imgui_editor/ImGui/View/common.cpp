#include "common.h"
// #include <limits>

// #define customMax(a,b) (((a)>(b))?(a):(b))

bool Splitter(bool split_vertically, float thickness, float *size1, float *size2, float min_size1, float min_size2, float splitter_long_axis_size)
{
    using namespace ImGui;
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + ImGui::CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return ImGui::SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

ImGUIButtonEx ImGUIButtonEx::FromText(const std::string &txt)
{
    ImGUIButtonEx result;
    result.clicked = false;
    result.raw_txt = ITKCommon::StringUtil::tokenizer(txt, "#")[0];
    result.txt = txt;
    result.isText = true;
    result.width = 0;
    return result;
}

void ImGUIButtonEx::computeWidth()
{
    width = ImGui::CalcTextSize(raw_txt.c_str()).x + ImGui::GetStyle().FramePadding.x * 2.0f;
}

void ImGUIButtonEx::render()
{
    clicked = ImGui::Button(txt.c_str());
}

void DrawRightAlignedButtonSet(ImGUIButtonEx *array, int count)
{

    float total = 0;
    for (int i = 0; i < count; i++)
    {
        array[i].computeWidth();
        total += array[i].width;
    }

    float x;

    x = ImGui::GetContentRegionAvail().x - total;
    for (int i = 0; i < count; i++)
    {
        ImGui::SameLine(x);
        array[i].render();
        x += array[i].width;
    }
}

namespace ImGui
{
    void BeginChildCustom(const char *name)
    {
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 0));
        ImGui::Begin(name, NULL, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground);
    }

    bool DragFloatInverted(const char *label, float *v, float v_speed, float v_min, float v_max, const char *format, float header_width)
    {
        char aux[64];
        sprintf(aux, "##%s", label);

        ImGui::AlignTextToFramePadding();
        // ImGui::SetNextItemWidth(header_width);
        float spacing = MathCore::OP<float>::maximum(header_width - ImGui::CalcTextSize(label).x, 0);
        ImGui::Dummy(ImVec2(spacing, 0));
        ImGui::SameLine();
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        bool r = ImGui::DragFloat(aux, v, 0.001f, 0, 0, "%.3f", ImGuiSliderFlags_Vertical);
        ImGui::PopItemWidth();

        return r;
    }

    bool DragFloat2Inverted(const char *label, float v[2], float v_speed, float v_min, float v_max, const char *format, float header_width)
    {
        char aux[64];
        sprintf(aux, "##%s", label);

        ImGui::AlignTextToFramePadding();
        // ImGui::SetNextItemWidth(header_width);
        float spacing = MathCore::OP<float>::maximum(header_width - ImGui::CalcTextSize(label).x, 0);
        ImGui::Dummy(ImVec2(spacing, 0));
        ImGui::SameLine();
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        bool r = ImGui::DragFloat2(aux, v, 0.001f, 0, 0, "%.3f", ImGuiSliderFlags_Vertical);
        ImGui::PopItemWidth();

        return r;
    }

    bool DragFloat3Inverted(const char *label, float v[3], float v_speed, float v_min, float v_max, const char *format, float header_width)
    {
        char aux[64];
        sprintf(aux, "##%s", label);

        ImGui::AlignTextToFramePadding();

        // ImGui::SetNextItemWidth(header_width);
        float spacing = MathCore::OP<float>::maximum(header_width - ImGui::CalcTextSize(label).x, 0);
        ImGui::Dummy(ImVec2(spacing, 0));
        ImGui::SameLine();
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        bool r = ImGui::DragFloat3(aux, v, 0.001f, 0, 0, "%.3f", ImGuiSliderFlags_Vertical);
        ImGui::PopItemWidth();

        return r;
    }

    bool DragFloat4Inverted(const char *label, float v[4], float v_speed, float v_min, float v_max, const char *format, float header_width)
    {
        char aux[64];
        sprintf(aux, "##%s", label);

        ImGui::AlignTextToFramePadding();
        // ImGui::SetNextItemWidth(header_width);
        float spacing = MathCore::OP<float>::maximum(header_width - ImGui::CalcTextSize(label).x, 0);
        ImGui::Dummy(ImVec2(spacing, 0));
        ImGui::SameLine();
        ImGui::Text("%s", label);
        ImGui::SameLine();
        ImGui::PushItemWidth(-FLT_MIN);
        bool r = ImGui::DragFloat4(aux, v, 0.001f, 0, 0, "%.3f", ImGuiSliderFlags_Vertical);
        ImGui::PopItemWidth();

        return r;
    }

}