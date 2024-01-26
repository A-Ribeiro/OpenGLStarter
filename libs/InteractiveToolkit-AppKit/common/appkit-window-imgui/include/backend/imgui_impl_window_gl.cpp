#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui_impl_window_gl.h"
using namespace AppKit::Window;


const ImGuiKey imgui_reverse_mapping[] = {
        ImGuiKey_A,            //!< The A key
        ImGuiKey_B,            //!< The B key
        ImGuiKey_C,            //!< The C key
        ImGuiKey_D,            //!< The D key
        ImGuiKey_E,            //!< The E key
        ImGuiKey_F,            //!< The F key
        ImGuiKey_G,            //!< The G key
        ImGuiKey_H,            //!< The H key
        ImGuiKey_I,            //!< The I key
        ImGuiKey_J,            //!< The J key
        ImGuiKey_K,            //!< The K key
        ImGuiKey_L,            //!< The L key
        ImGuiKey_M,            //!< The M key
        ImGuiKey_N,            //!< The N key
        ImGuiKey_O,            //!< The O key
        ImGuiKey_P,            //!< The P key
        ImGuiKey_Q,            //!< The Q key
        ImGuiKey_R,            //!< The R key
        ImGuiKey_S,            //!< The S key
        ImGuiKey_T,            //!< The T key
        ImGuiKey_U,            //!< The U key
        ImGuiKey_V,            //!< The V key
        ImGuiKey_W,            //!< The W key
        ImGuiKey_X,            //!< The X key
        ImGuiKey_Y,            //!< The Y key
        ImGuiKey_Z,            //!< The Z key
        ImGuiKey_0,         //!< The 0 key
        ImGuiKey_1,         //!< The 1 key
        ImGuiKey_2,         //!< The 2 key
        ImGuiKey_3,         //!< The 3 key
        ImGuiKey_4,         //!< The 4 key
        ImGuiKey_5,         //!< The 5 key
        ImGuiKey_6,         //!< The 6 key
        ImGuiKey_7,         //!< The 7 key
        ImGuiKey_8,         //!< The 8 key
        ImGuiKey_9,         //!< The 9 key
        ImGuiKey_Escape,       //!< The Escape key
        ImGuiKey_LeftCtrl,     //!< The left Control key
        ImGuiKey_LeftShift,       //!< The left Shift key
        ImGuiKey_LeftAlt,         //!< The left Alt key
        ImGuiKey_LeftSuper,      //!< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
        ImGuiKey_RightCtrl,     //!< The right Control key
        ImGuiKey_RightShift,       //!< The right Shift key
        ImGuiKey_RightAlt,         //!< The right Alt key
        ImGuiKey_RightSuper,      //!< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
        ImGuiKey_Menu,         //!< The Menu key
        ImGuiKey_LeftBracket,     //!< The [ key
        ImGuiKey_RightBracket,     //!< The ] key
        ImGuiKey_Semicolon,    //!< The ; key
        ImGuiKey_Comma,        //!< The , key
        ImGuiKey_Period,       //!< The . key
        ImGuiKey_Apostrophe,        //!< The ' key
        ImGuiKey_Slash,        //!< The / key
        ImGuiKey_Backslash,    //!< The \ key
        ImGuiKey_GraveAccent,        //!< The ~ key
        ImGuiKey_Equal,        //!< The = key
        ImGuiKey_Minus,       //!< The - key (hyphen)
        ImGuiKey_Space,        //!< The Space key
        ImGuiKey_Enter,        //!< The Enter/Return keys
        ImGuiKey_Backspace,    //!< The Backspace key
        ImGuiKey_Tab,          //!< The Tabulation key
        ImGuiKey_PageUp,       //!< The Page up key
        ImGuiKey_PageDown,     //!< The Page down key
        ImGuiKey_End,          //!< The End key
        ImGuiKey_Home,         //!< The Home key
        ImGuiKey_Insert,       //!< The Insert key
        ImGuiKey_Delete,       //!< The Delete key
        ImGuiKey_KeypadAdd,          //!< The + key
        ImGuiKey_KeypadSubtract,     //!< The - key (minus, usually from numpad)
        ImGuiKey_KeypadMultiply,     //!< The * key
        ImGuiKey_KeypadDivide,       //!< The / key
        ImGuiKey_LeftArrow,         //!< Left arrow
        ImGuiKey_RightArrow,        //!< Right arrow
        ImGuiKey_UpArrow,           //!< Up arrow
        ImGuiKey_DownArrow,         //!< Down arrow
        ImGuiKey_Keypad0,      //!< The numpad 0 key
        ImGuiKey_Keypad1,      //!< The numpad 1 key
        ImGuiKey_Keypad2,      //!< The numpad 2 key
        ImGuiKey_Keypad3,      //!< The numpad 3 key
        ImGuiKey_Keypad4,      //!< The numpad 4 key
        ImGuiKey_Keypad5,      //!< The numpad 5 key
        ImGuiKey_Keypad6,      //!< The numpad 6 key
        ImGuiKey_Keypad7,      //!< The numpad 7 key
        ImGuiKey_Keypad8,      //!< The numpad 8 key
        ImGuiKey_Keypad9,      //!< The numpad 9 key
        ImGuiKey_F1,           //!< The F1 key
        ImGuiKey_F2,           //!< The F2 key
        ImGuiKey_F3,           //!< The F3 key
        ImGuiKey_F4,           //!< The F4 key
        ImGuiKey_F5,           //!< The F5 key
        ImGuiKey_F6,           //!< The F6 key
        ImGuiKey_F7,           //!< The F7 key
        ImGuiKey_F8,           //!< The F8 key
        ImGuiKey_F9,           //!< The F9 key
        ImGuiKey_F10,          //!< The F10 key
        ImGuiKey_F11,          //!< The F11 key
        ImGuiKey_F12,          //!< The F12 key
        ImGuiKey_None,          //!< The F13 key
        ImGuiKey_None,          //!< The F14 key
        ImGuiKey_None,          //!< The F15 key
        ImGuiKey_Pause,         //!< The Pause key
    };

void OnWindowEvent(const WindowEvent& event){
    ImGuiIO& io = ImGui::GetIO();

    switch(event.type){
        case WindowEventType::Closed:
            if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(io.BackendPlatformUserData))
                viewport->PlatformRequestClose = true;
        break;
        case WindowEventType::Resized:{
            if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(io.BackendPlatformUserData))
                viewport->PlatformRequestResize = true;
        }break;
        case WindowEventType::LostFocus:
            io.AddFocusEvent(false);
        break;
        case WindowEventType::GainedFocus:
            io.AddFocusEvent(true);
            {
                //AppKit::Window::Window *window = (AppKit::Window::Window *)io.BackendPlatformUserData;
                WindowUserData* userData = (WindowUserData*)io.BackendPlatformUserData;
                MathCore::vec2i mouse = Devices::Mouse::getPosition(userData->window);
                io.AddMousePosEvent(mouse.x, mouse.y);
            }
        break;

        //window related events
        case WindowEventType::TextEntered: 
            io.AddInputCharacter(event.textEntered);
        break;
        case WindowEventType::MouseEntered:{
            //AppKit::Window::Window *window = (AppKit::Window::Window *)io.BackendPlatformUserData;
            WindowUserData* userData = (WindowUserData*)io.BackendPlatformUserData;
            MathCore::vec2i mouse = Devices::Mouse::getPosition(userData->window);
            io.AddMousePosEvent(mouse.x, mouse.y);
        }
        break;
        case WindowEventType::MouseLeft:
            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
        break;
    }
}
void OnKeyboardEvent(const KeyboardEvent& event){
    if (event.code == Devices::KeyCode::Unknown)
        return;
    ImGuiKey key = imgui_reverse_mapping[(int)event.code];
    bool down = event.type == KeyboardEventType::KeyPressed;
    ImGuiIO& io = ImGui::GetIO();

    //update modifiers
    io.AddKeyEvent(ImGuiKey_ModCtrl, event.control);
    io.AddKeyEvent(ImGuiKey_ModShift, event.shift);
    io.AddKeyEvent(ImGuiKey_ModAlt, event.alt);
    io.AddKeyEvent(ImGuiKey_ModSuper, event.system);

    //send key efectivelly
    io.AddKeyEvent(key, down);
    //io.SetKeyEventNativeData(key, keycode, scancode); // To support legacy indexing (<1.87 user code)
}
void OnMouseEvent(const MouseEvent& event){

    ImGuiIO& io = ImGui::GetIO();

    //update modifiers
    io.AddKeyEvent(ImGuiKey_ModCtrl, Devices::Keyboard::isPressed(Devices::KeyCode::LControl) || Devices::Keyboard::isPressed(Devices::KeyCode::RControl) );
    io.AddKeyEvent(ImGuiKey_ModShift, Devices::Keyboard::isPressed(Devices::KeyCode::LShift) || Devices::Keyboard::isPressed(Devices::KeyCode::RShift) );
    io.AddKeyEvent(ImGuiKey_ModAlt, Devices::Keyboard::isPressed(Devices::KeyCode::LAlt) || Devices::Keyboard::isPressed(Devices::KeyCode::RAlt) );
    io.AddKeyEvent(ImGuiKey_ModSuper, Devices::Keyboard::isPressed(Devices::KeyCode::LSystem) || Devices::Keyboard::isPressed(Devices::KeyCode::RSystem) );

    switch(event.type) {

        case MouseEventType::VerticalWheelScrolled:
            #if defined(__APPLE__)
            io.AddMouseWheelEvent(0, event.wheelDelta * 0.1f);
            #else
            io.AddMouseWheelEvent(0, event.wheelDelta);
            #endif
        break;
        case MouseEventType::HorizontalWheelScrolled:
            #if defined(__APPLE__)
            io.AddMouseWheelEvent(event.wheelDelta * 0.1f, 0);
            #else
            io.AddMouseWheelEvent(event.wheelDelta, 0);
            #endif
        break;
        case MouseEventType::ButtonPressed:
        case MouseEventType::ButtonReleased: {
            bool down = event.type == MouseEventType::ButtonPressed;
            //mouse button event
            int button = (int)event.button;
            if (button >= 0 && button < ImGuiMouseButton_COUNT)
                io.AddMouseButtonEvent(button, down);
        }
        break;
        case MouseEventType::Moved: {
            MathCore::vec2i pos = event.position;
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
                //AppKit::Window::Window *window = (AppKit::Window::Window *)io.BackendPlatformUserData;
                WindowUserData* userData = (WindowUserData*)io.BackendPlatformUserData;
                pos += userData->window->getPosition();
            }
            io.AddMousePosEvent((float)pos.x, (float)pos.y);
        } break;
    }

}
void OnJoystickEvent(const JoystickEvent& event){
}
void OnTouchEvent(const TouchEvent& event){
}
void OnSensorEvent(const SensorEvent& event){
}

void ImGui_WindowGL_InitForOpenGL(const WindowUserData& windowUserData){

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend capabilities flags

    WindowUserData* userData = new WindowUserData();
    *userData = windowUserData;

    io.BackendPlatformUserData = (void*)userData;
    io.BackendPlatformName = "imgui_impl_window_gl";
    io.MouseDrawCursor = true;
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    //io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
#if GLFW_HAS_MOUSE_PASSTHROUGH || (GLFW_HAS_WINDOW_HOVERED && defined(_WIN32))
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)
#endif

    //io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    //io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    //io.ClipboardUserData = bd->Window;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)userData;
    //if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    //    ImGui_ImplGlfw_InitPlatformInterface();

    userData->inputManager->onWindowEvent.add(OnWindowEvent);
    userData->inputManager->onKeyboardEvent.add(OnKeyboardEvent);
    userData->inputManager->onMouseEvent.add(OnMouseEvent);
    userData->inputManager->onJoystickEvent.add(OnJoystickEvent);
    userData->inputManager->onTouchEvent.add(OnTouchEvent);
    userData->inputManager->onSensorEvent.add(OnSensorEvent);
}

void ImGui_WindowGL_Shutdown(){
    ImGuiIO& io = ImGui::GetIO();

    WindowUserData* userData = (WindowUserData*)io.BackendPlatformUserData;

    userData->inputManager->onWindowEvent.remove(OnWindowEvent);
    userData->inputManager->onKeyboardEvent.remove(OnKeyboardEvent);
    userData->inputManager->onMouseEvent.remove(OnMouseEvent);
    userData->inputManager->onJoystickEvent.remove(OnJoystickEvent);
    userData->inputManager->onTouchEvent.remove(OnTouchEvent);
    userData->inputManager->onSensorEvent.remove(OnSensorEvent);

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;

    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = NULL;

    if (userData != NULL){
        delete userData;
        userData = NULL;
    }
}

void ImGui_WindowGL_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    //AppKit::Window::Window *window = (AppKit::Window::Window *)io.BackendPlatformUserData;
    WindowUserData* userData = (WindowUserData*)io.BackendPlatformUserData;

    MathCore::vec2i windowSize = userData->window->getSize();

    io.DisplaySize = ImVec2((float)windowSize.width, (float)windowSize.height);
    io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);
}

