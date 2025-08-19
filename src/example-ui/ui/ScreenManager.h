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
    };

}
