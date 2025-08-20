#pragma once

#include <appkit-gl-engine/Components/2d/ComponentUI.h>
#include "./common.h"

namespace ui
{

    class ScreenManager : public EventCore::HandleCallback
    {
        std::unordered_map<std::string, std::unique_ptr<Screen>> screen_map;

    public:
        std::vector<Screen *> screen_stack;

        std::shared_ptr<AppKit::GLEngine::Transform> uiRoot;

        void open_screen(const std::string &name);
        void push_screen(const std::string &name);
        void pop_screen();

        void close_all();

        void load_screens(AppKit::GLEngine::Engine *engine,
                          AppKit::GLEngine::ResourceMap *resourceMap,
                          MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
                          std::vector<std::unique_ptr<Screen>> &screens);
        void unload_screens();

        void triggerEvent(UIEventEnum event);

        void resize(const MathCore::vec2i &size);
        void update(Platform::Time *elapsed);

        ColorPalette colorPalette;
        void setColorPalette(const ColorPalette &palette);
    };

    namespace Pallete
    {
        static const ColorPalette Blush{
            3.0f,                          // float stroke_thickness;
            colorFromHex("#bbbbff", 1.0f), // Color primary;
            colorFromHex("#610081", 1.0f), // Color primary_stroke;
            colorFromHex("#6192d1", 1.0f), // Color active;
            colorFromHex("#003780", 1.0f), // Color active_stroke;
            colorFromHex("#dedede", 1.0f), // Color disabled;
            colorFromHex("#404040", 1.0f), // Color disabled_stroke;
            colorFromHex("#000000", 1.0f), // Color text;
        };
    }

}
