#pragma once

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include "View/common.h"

class CustomSettings {
public:
    float splitterPos;

    CustomSettings() {
		splitterPos = -1;
    }

	static CustomSettings * Instance() {
		static CustomSettings customSettings;
		return &customSettings;
	}

	void reset() {
		splitterPos = -1;
	}

    static void Init() {
		// Add .ini handle for Splitter
		{
			ImGuiSettingsHandler ini_handler;
			ini_handler.TypeName = "CustomSettings";
			ini_handler.TypeHash = ImHashStr("CustomSettings");
			ini_handler.ClearAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler*)
				{
					//ImGuiContext & g = *ctx;
					//for (ImGuiWindow* window : g.Windows)
					//	window->SettingsOffset = -1;
					//g.SettingsWindows.clear();
				};
			ini_handler.ReadOpenFn = [](ImGuiContext*, ImGuiSettingsHandler*, const char* name)
				{
					//ImGuiID id = ImHashStr(name);
					//ImGuiWindowSettings* settings = ImGui::FindWindowSettingsByID(id);
					//if (settings)
					//	*settings = ImGuiWindowSettings(); // Clear existing if recycling previous entry
					//else
					//	settings = ImGui::CreateNewWindowSettings(name);
					//settings->ID = id;
					//settings->WantApply = true;
					//return (void*)settings;
					if (strcmp(name, "Data") != 0)
						return (void*)nullptr;
					return (void*)1;
				};
			ini_handler.ReadLineFn = [](ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line)
				{
					float splitter_pos;
					if (sscanf(line, "SplitterPos=%f", &splitter_pos) == 1) {
						CustomSettings::Instance()->splitterPos = splitter_pos;
						//settings->Pos = ImVec2ih((short)x, (short)y); 
					}

					//ImGuiWindowSettings* settings = (ImGuiWindowSettings*)entry;
					//int x, y;
					//int i;
					//ImU32 u1;
					//if (sscanf(line, "Pos=%i,%i", &x, &y) == 2) { settings->Pos = ImVec2ih((short)x, (short)y); }
					//else if (sscanf(line, "Size=%i,%i", &x, &y) == 2) { settings->Size = ImVec2ih((short)x, (short)y); }
					//else if (sscanf(line, "ViewportId=0x%08X", &u1) == 1) { settings->ViewportId = u1; }
					//else if (sscanf(line, "ViewportPos=%i,%i", &x, &y) == 2) { settings->ViewportPos = ImVec2ih((short)x, (short)y); }
					//else if (sscanf(line, "Collapsed=%d", &i) == 1) { settings->Collapsed = (i != 0); }
					//else if (sscanf(line, "IsChild=%d", &i) == 1) { settings->IsChild = (i != 0); }
					//else if (sscanf(line, "DockId=0x%X,%d", &u1, &i) == 2) { settings->DockId = u1; settings->DockOrder = (short)i; }
					//else if (sscanf(line, "DockId=0x%X", &u1) == 1) { settings->DockId = u1; settings->DockOrder = -1; }
					//else if (sscanf(line, "ClassId=0x%X", &u1) == 1) { settings->ClassId = u1; }
				};
			ini_handler.ApplyAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler*)
				{
					//ImGuiContext& g = *ctx;
					//for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != nullptr; settings = g.SettingsWindows.next_chunk(settings))
					//	if (settings->WantApply)
					//	{
					//		if (ImGuiWindow* window = ImGui::FindWindowByID(settings->ID))
					//			ApplyWindowSettings(window, settings);
					//		settings->WantApply = false;
					//	}
				};
			ini_handler.WriteAllFn = [](ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
				{
					float splitter_pos = CustomSettings::Instance()->splitterPos;

					buf->appendf("[%s][Data]\n", handler->TypeName);
					buf->appendf("SplitterPos=%f\n", splitter_pos);

					//// Gather data from windows that were active during this session
					//// (if a window wasn't opened in this session we preserve its settings)
					//ImGuiContext& g = *ctx;
					//for (ImGuiWindow* window : g.Windows)
					//{
					//	if (window->Flags & ImGuiWindowFlags_NoSavedSettings)
					//		continue;

					//	ImGuiWindowSettings* settings = ImGui::FindWindowSettingsByWindow(window);
					//	if (!settings)
					//	{
					//		settings = ImGui::CreateNewWindowSettings(window->Name);
					//		window->SettingsOffset = g.SettingsWindows.offset_from_ptr(settings);
					//	}
					//	IM_ASSERT(settings->ID == window->ID);
					//	settings->Pos = ImVec2ih(window->Pos - window->ViewportPos);
					//	settings->Size = ImVec2ih(window->SizeFull);
					//	settings->ViewportId = window->ViewportId;
					//	settings->ViewportPos = ImVec2ih(window->ViewportPos);
					//	IM_ASSERT(window->DockNode == nullptr || window->DockNode->ID == window->DockId);
					//	settings->DockId = window->DockId;
					//	settings->ClassId = window->WindowClass.ClassId;
					//	settings->DockOrder = window->DockOrder;
					//	settings->Collapsed = window->Collapsed;
					//	settings->IsChild = (window->RootWindow != window); // Cannot rely on ImGuiWindowFlags_ChildWindow here as docked windows have this set.
					//	settings->WantDelete = false;
					//}

					//// Write to text buffer
					//buf->reserve(buf->size() + g.SettingsWindows.size() * 6); // ballpark reserve
					//for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != nullptr; settings = g.SettingsWindows.next_chunk(settings))
					//{
					//	if (settings->WantDelete)
					//		continue;
					//	const char* settings_name = settings->GetName();
					//	buf->appendf("[%s][%s]\n", handler->TypeName, settings_name);
					//	if (settings->IsChild)
					//	{
					//		buf->appendf("IsChild=1\n");
					//		buf->appendf("Size=%d,%d\n", settings->Size.x, settings->Size.y);
					//	}
					//	else
					//	{
					//		if (settings->ViewportId != 0 && settings->ViewportId != ImGui::IMGUI_VIEWPORT_DEFAULT_ID)
					//		{
					//			buf->appendf("ViewportPos=%d,%d\n", settings->ViewportPos.x, settings->ViewportPos.y);
					//			buf->appendf("ViewportId=0x%08X\n", settings->ViewportId);
					//		}
					//		if (settings->Pos.x != 0 || settings->Pos.y != 0 || settings->ViewportId == ImGui::IMGUI_VIEWPORT_DEFAULT_ID)
					//			buf->appendf("Pos=%d,%d\n", settings->Pos.x, settings->Pos.y);
					//		if (settings->Size.x != 0 || settings->Size.y != 0)
					//			buf->appendf("Size=%d,%d\n", settings->Size.x, settings->Size.y);
					//		buf->appendf("Collapsed=%d\n", settings->Collapsed);
					//		if (settings->DockId != 0)
					//		{
					//			//buf->appendf("TabId=0x%08X\n", ImHashStr("#TAB", 4, settings->ID)); // window->TabId: this is not read back but writing it makes "debugging" the .ini data easier.
					//			if (settings->DockOrder == -1)
					//				buf->appendf("DockId=0x%08X\n", settings->DockId);
					//			else
					//				buf->appendf("DockId=0x%08X,%d\n", settings->DockId, settings->DockOrder);
					//			if (settings->ClassId != 0)
					//				buf->appendf("ClassId=0x%08X\n", settings->ClassId);
					//		}
					//	}
					//	buf->append("\n");
					//}
				};
			ImGui::AddSettingsHandler(&ini_handler);
		}
    }
};

#if defined(_WIN32)
#pragma warning(pop)
#endif
