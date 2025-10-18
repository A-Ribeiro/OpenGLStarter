#pragma once

#include <appkit-gl-engine/Components/2d/ComponentUI.h>
#include "./common.h"
#include "./Pallete.h"

namespace ui
{

    class ScreenManager : public EventCore::HandleCallback
    {
        std::unordered_map<std::string, std::unique_ptr<Screen>> screen_map;

    public:
        std::vector<Screen *> screen_stack;

        std::shared_ptr<AppKit::GLEngine::Transform> uiRoot;
        MathCore::vec2f current_size;
        std::shared_ptr<AppKit::GLEngine::Components::ComponentCamera> camera;

        template <typename T>
        T *screen()
        {
            const char *name = T::Name;
            auto it = screen_map.find(name);
            if (it == screen_map.end())
            {
                printf("[ScreenManager] Screen %s not found\n", name);
                return nullptr;
            }
            for (auto &item : screen_stack)
            {
                if (item->name().compare(name) == 0)
                {
                    printf("[ScreenManager] Screen %s already in stack\n", name);
                    return nullptr;
                }
            }
            return (T *)it->second.get();
        }

        bool screen_in_top_byName(const std::string &name) const;
        bool screen_in_top_byInstance(const Screen *screen) const;

        void open_screen(const std::string &name);
        void push_screen(const std::string &name);
        void pop_screen();

        void close_all();

        void load_screens(AppKit::GLEngine::Engine *engine,
                          AppKit::GLEngine::ResourceMap *resourceMap,
                          MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
                          std::vector<std::unique_ptr<Screen>> &screens,
                          const MathCore::vec2i &size);
        void unload_screens();

        void triggerEvent(UIEventEnum event);

        void resize(const MathCore::vec2i &size);
        void update(Platform::Time *elapsed);

        ColorPalette colorPalette;
        void setColorPalette(const ColorPalette &palette);

        void printUIHierarchy();
    };
}
