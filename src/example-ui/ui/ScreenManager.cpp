#include "./ScreenManager.h"
#include "ScreenMain.h"

namespace ui
{

    void ScreenManager::open_screen(const std::string &name)
    {
        if (screen_map.find(name) == screen_map.end())
        {
            printf("[ScreenManager] Screen %s not found\n", name.c_str());
            return;
        }
        // if (screen_stack.size() > 0 && screen_stack.back()->name() == name)
        // {
        //     printf("[ScreenManager] Screen %s already in stack\n", name.c_str());
        //     return;
        // }
        close_all();
        push_screen(name);
    }

    void ScreenManager::push_screen(const std::string &name)
    {
        if (screen_map.find(name) == screen_map.end())
        {
            printf("[ScreenManager] Screen %s not found\n", name.c_str());
            return;
        }
        for (auto &item : screen_stack)
        {
            if (item->name() == name)
            {
                printf("[ScreenManager] Screen %s already in stack\n", name.c_str());
                return;
            }
        }
        screen_stack.push_back(screen_map[name].get());
        screen_stack.back()->triggerEvent(UIEventEnum::UIEvent_ScreenPush);
    }

    void ScreenManager::pop_screen()
    {
        if (screen_stack.size() > 0)
        {
            screen_stack.back()->triggerEvent(UIEventEnum::UIEvent_ScreenPop);
            screen_stack.pop_back();
        }
    }

    void ScreenManager::close_all()
    {
        while (screen_stack.size() > 0)
            pop_screen();
    }

    void ScreenManager::load_screens(AppKit::GLEngine::Engine *engine,
                                     AppKit::GLEngine::ResourceMap *resourceMap,
                                     MathCore::MathRandomExt<ITKCommon::Random32> *mathRandom,
                                     std::vector<std::unique_ptr<Screen>> &screens,
                                     const MathCore::vec2i &size)
    {
        if (uiRoot)
            return;
        printf("[ScreenManager] load_screens\n");
        for (auto &screen : screens)
        {
            auto name = screen->name();
            screen_map[name] = std::move(screen);
        }

        uiRoot = AppKit::GLEngine::Transform::CreateShared("UI Root");
        for (auto &entry : screen_map)
        {
            auto transform = entry.second->initializeTransform(
                engine,
                resourceMap,
                mathRandom,
                this,
                size);
            uiRoot->addChild(transform);
        }
    }

    void ScreenManager::unload_screens() {}

    void ScreenManager::triggerEvent(UIEventEnum event)
    {
        printf("[ScreenManager] %s\n", UIEventToStr(event));
        if (screen_stack.size() > 0)
            screen_stack.back()->triggerEvent(event);
    }

    void ScreenManager::resize(const MathCore::vec2i &size)
    {
        printf("[ScreenManager] resize %i %i\n", size.width, size.height);
        for (auto &screen : screen_map)
            screen.second->resize(size);
    }

    void ScreenManager::update(Platform::Time *elapsed)
    {
        for (auto &screen : screen_stack)
            screen->update(elapsed);
    }

    void ScreenManager::setColorPalette(const ColorPalette &palette)
    {
        this->colorPalette = palette;
    }

}
