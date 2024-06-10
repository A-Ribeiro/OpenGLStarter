#include "Console.h"
#include "../ImGuiMenu.h"
#include "../ImGuiManager.h"

const ViewType Console::Type = "Console";

Console::Console() : View(Console::Type)
{
    console = new Platform::Tool::DebugConsoleIPC(Platform::IPC::QueueIPC_READ | Platform::IPC::QueueIPC_WRITE);

    max_history_lines = 5000;
    autoScroll = true;
    scroll_last_item = 0;

    console->printf("[Initializing IPC Console]");
    console->printf("i:line info");
    console->printf("w:line warning");
    console->printf("e:line error");
    console->printf("Some Random Text... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... very long and big... ");
    console->printf("Some Random Text...");
    console->printf("Some Random Text...");
    console->printf("Some Random Text...");
    console->printf("Some Random Text...");
    console->printf("Some Random Text...");
    console->printf("i:line info");
}

Console::~Console()
{
    if (console != NULL)
    {
        delete console;
        console = NULL;
    }
}

View *Console::Init()
{
    ImGuiMenu::Instance()->AddMenu(
        "Window/Console", "", [this]()
        { printf("Window/Console\n"); },
        &this->active);
    return this;
}

void Console::RenderAndLogic()
{
    if (!active)
        return;
    if (scroll_last_item > 0)
        scroll_last_item--;

    auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar; // | ImGuiWindowFlags_AlwaysAutoResize;// | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
    if (ImGui::Begin("Console", NULL, flags))
    {
        on_hover_detector.setState(ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows));
        on_focus_detector.setState(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

        // display_order.push_back(ImGui::GetCurrentWindow());
        // console->runReadLoop();

        // Options menu
        /*
        if (ImGui::BeginPopupContextItem())
        {
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::EndPopup();
        }*/

        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::SameLine();
        bool copy_to_clipboard = ImGui::Button("To Clipboard");
        ImGui::Separator();

        Platform::ObjectBuffer buffer;
        while (console->queue.read(&buffer))
        {
            history.push_back((char *)buffer.data);
            printf("%s\n", history[history.size() - 1].c_str());
            if (autoScroll)
                scroll_last_item = 2;
        }

        while (history.size() > max_history_lines)
        {
            history.erase(history.begin());
        }

        // draw the history lines...
        // const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, 0);

        if (copy_to_clipboard)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        for (auto &str : history)
        {
            int index_start = 0;
            ImVec4 color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            if (ITKCommon::StringUtil::startsWith(str, "e:"))
            {
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                index_start = 2;
            }
            else if (ITKCommon::StringUtil::startsWith(str, "w:"))
            {
                color = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
                index_start = 2;
            }
            else if (ITKCommon::StringUtil::startsWith(str, "i:"))
            {
                color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                index_start = 2;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::PushTextWrapPos();
            ImGui::TextUnformatted(&str.c_str()[index_start]);
            ImGui::PopTextWrapPos();
            ImGui::PopStyleColor();
        }
        ImGui::PopStyleVar();

        if (copy_to_clipboard)
            ImGui::LogFinish();

        if (scroll_last_item > 0 && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }
    else
    {
        on_hover_detector.setState(false);
        on_focus_detector.setState(false);
    }
    ImGui::End();

    computeOnHoverAndOnFocus();
}
