#pragma once

#include <appkit-gl-engine/Components/2d/ComponentUI.h>

namespace ui
{
    class ScreenManager;

    enum UIEventEnum
    {
        UIEvent_InputUp = 0,
        UIEvent_InputDown,
        UIEvent_InputLeft,
        UIEvent_InputRight,

        UIEvent_InputActionEnter,
        UIEvent_InputActionBack,

        UIEvent_InputShoulderLeft,
        UIEvent_InputShoulderRight,

        UIEvent_ScreenPush,
        UIEvent_ScreenPop
    };

    const char *UIEventToStr(UIEventEnum v);

    class Screen : public EventCore::HandleCallback
    {
    public:
        virtual ~Screen() = default;
        virtual std::string name() const = 0;
        virtual void resize(const MathCore::vec2i &size) = 0;
        // virtual std::unordered_map<std::string, std::string> get_config_params() const = 0;
        // virtual void set_config_params(const std::unordered_map<std::string, std::string> &params) const = 0;

        virtual void update(Platform::Time *elapsed) = 0;

        virtual std::shared_ptr<AppKit::GLEngine::Transform> initializeTransform(
            AppKit::GLEngine::Engine *engine,
            AppKit::GLEngine::ResourceMap *resourceMap,
            MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
            ScreenManager *screenManager,
            const MathCore::vec2i &size
        ) = 0;

        virtual void triggerEvent(UIEventEnum event) = 0;
    };

    // Simple compile-time color structure for const variables
    struct Color {
        float r, g, b, a;
        
        constexpr Color()
            : r(0), g(0), b(0), a(1) {}

        constexpr Color(float r, float g, float b, float a = 1.0f) 
            : r(r), g(g), b(b), a(a) {}
        
        // Implicit conversion to vec4f
        operator MathCore::vec4f() const {
            return MathCore::vec4f(r, g, b, a);
        }
    };

    // Helper function to convert hex digit to integer
    static constexpr int hexDigitToInt(char c) {
        return (c >= '0' && c <= '9') ? (c - '0') :
               (c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
               (c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
               -1; // Invalid hex digit
    }
    
    // Helper function to convert two hex digits to byte value
    static constexpr int hexToInt(char high, char low) {
        return (hexDigitToInt(high) == -1 || hexDigitToInt(low) == -1) ? 
               -1 : 
               (hexDigitToInt(high) * 16 + hexDigitToInt(low));
    }
    
    // Helper function to count string length at compile time
    static constexpr int constexpr_strlen(const char* str) {
        return (*str == '\0') ? 0 : (1 + constexpr_strlen(str + 1));
    }
    
    static constexpr Color colorFromHex(const char* hex, float alpha = 1.0f)
    {
        // Basic validation - hex must start with # and be 7 or 9 characters
        return (!hex || hex[0] != '#') ? 
               Color(0.0f, 0.0f, 0.0f, alpha) : // Return black on error
               
               // Check length
               (constexpr_strlen(hex) != 7 && constexpr_strlen(hex) != 9) ?
               Color(0.0f, 0.0f, 0.0f, alpha) : // Return black on error
               
               // Parse RGB components
               (hexToInt(hex[1], hex[2]) == -1 || 
                hexToInt(hex[3], hex[4]) == -1 || 
                hexToInt(hex[5], hex[6]) == -1) ?
               Color(0.0f, 0.0f, 0.0f, alpha) : // Return black on error
               
               // Valid color - construct with alpha handling
               Color(hexToInt(hex[1], hex[2]) / 255.0f,
                     hexToInt(hex[3], hex[4]) / 255.0f,
                     hexToInt(hex[5], hex[6]) / 255.0f,
                     (constexpr_strlen(hex) == 9 && hexToInt(hex[7], hex[8]) != -1) ?
                     (hexToInt(hex[7], hex[8]) / 255.0f) : alpha);
    }
    
    // Convenience overload for std::string (not constexpr) - returns vec4f directly
    static MathCore::vec4f colorFromHex(const std::string &hex, float alpha = 1.0f)
    {
        Color color = colorFromHex(hex.c_str(), alpha);
        return color; // Uses implicit conversion
    }


    struct ColorPalette {
        float stroke_thickness;

        Color bg;

        Color primary;
        Color primary_stroke;

        Color active;
        Color active_stroke;

        Color disabled;
        Color disabled_stroke;

        Color text;

        Color scroll_gradient;

        MathCore::vec4f lrp_active(float active_lrp) const {
            return MathCore::OP<MathCore::vec4f>::lerp(primary, active, active_lrp);
        }
        MathCore::vec4f lrp_active_stroke(float active_lrp) const {
            return MathCore::OP<MathCore::vec4f>::lerp(primary_stroke, active_stroke, active_lrp);
        }
    };

}