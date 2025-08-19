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
            ScreenManager *screenManager) = 0;

        virtual void triggerEvent(UIEventEnum event) = 0;
    };

    static MathCore::vec4f colorFromHex(const std::string &hex, float alpha = 1.0f)
    {
        if ((hex.length() != 7 && hex.length() != 9) || hex[0] != '#')
            ITK_ABORT(true, "Invalid hex color format");
        float r, g, b, a = alpha;
        r = (float)std::stoi(hex.substr(1, 2), nullptr, 16);
        g = (float)std::stoi(hex.substr(3, 2), nullptr, 16);
        b = (float)std::stoi(hex.substr(5, 2), nullptr, 16);
        if (hex.length() == 9){
            a = (float)std::stoi(hex.substr(7, 2), nullptr, 16);
            a /= 255.0f;
        }
        return MathCore::vec4f(r / 255.0f, g / 255.0f, b / 255.0f, a);
    }

}