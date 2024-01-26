#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

#include <InteractiveToolkit/Platform/Tool/DebugConsoleIPC.h>
#include <appkit-gl-base/platform/PlatformGL.h> // include gl headers

#include <appkit-gl-base/GLTexture.h>


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <backend/imgui_impl_window_gl.h>
#include <backend/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <functional>
#include <algorithm>


#if UINTPTR_MAX == 0xFFFF
typedef uint16_t ogltex_to_imguitex;
#elif UINTPTR_MAX == 0xFFFFFFFF
typedef uint32_t ogltex_to_imguitex;
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFu
typedef uint64_t ogltex_to_imguitex;
#else
#error TBD pointer size
#endif


// Small = 32x32
// Big = 512x512
enum class IconType : uint8_t
{
    Small_BoxNode = 0,
    Small_BoxNode_Filled,
    Small_Folder_Empty,
    Small_Folder_Filled,
    
    Big_Folder_Empty,
    Big_Folder_Filled,
    Big_File_Generic,


    Count // the last has the count of total icons...
};


// Forwarding Declarations

class MenuItemController;
class ImGuiMenu;

class HierarchyTreeNode;

class View;
class Console;
class Game;
class Hierarchy;
class Inspector;
class Project;
class Scene;

class ImGuiManager;

bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

class ImGUIButtonEx {
    public:
    bool isText;
    std::string raw_txt;
    std::string txt;
    bool clicked;
    float width;

    static ImGUIButtonEx FromText(const std::string &txt);

    void computeWidth();

    void render();

};

void DrawRightAlignedButtonSet(ImGUIButtonEx*array, int count);

const float PropertyNameWidth = 40;

namespace ImGui {
    void BeginChildCustom(const char* name);

    bool DragFloatInverted(const char* label, float *v, float v_speed, float v_min, float v_max, const char* format, float header_width = PropertyNameWidth);
    bool DragFloat2Inverted(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float header_width = PropertyNameWidth);
    bool DragFloat3Inverted(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float header_width = PropertyNameWidth);
    bool DragFloat4Inverted(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float header_width = PropertyNameWidth);

}
