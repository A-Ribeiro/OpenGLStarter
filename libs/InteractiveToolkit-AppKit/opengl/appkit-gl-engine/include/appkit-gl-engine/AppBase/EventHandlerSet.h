#pragma once

#include <InteractiveToolkit/EventCore/EventCore.h>
#include <InteractiveToolkit/Platform/Time.h>

#include <appkit-gl-engine/StartEventManager.h>

namespace AppKit
{
    namespace GLEngine
    {
        class EventHandlerSet : public EventCore::HandleCallback
        {
        public:
            virtual ~EventHandlerSet() = default;

            EventCore::Event<void(Platform::Time *)> OnPreUpdate;
            EventCore::Event<void(Platform::Time *)> OnUpdate;
            EventCore::Event<void(Platform::Time *)> OnLateUpdate;
            EventCore::Event<void(Platform::Time *)> OnAfterGraphPrecompute;
            EventCore::Event<void(Platform::Time *)> OnAfterOverlayDraw;

            StartEventManager startEventManager;
        };
    }
}
