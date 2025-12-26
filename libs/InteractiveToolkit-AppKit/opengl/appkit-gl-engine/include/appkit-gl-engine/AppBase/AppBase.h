#pragma once

#include <InteractiveToolkit/EventCore/Event.h>
#include <AppKit/window/Window.h>

#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>
#include <appkit-gl-engine/AppBase/EventHandlerSet.h>

// #ifndef AppBase__H_
// #define AppBase__H_

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <aRibeiroPlatform/aRibeiroPlatform.h>
// #include <appkit-gl-base/opengl-wrapper.h>
// #include <window-gl-wrapper/window-gl-wrapper.h>
// #include <appkit-gl-engine/RenderWindowRegion.h>

namespace AppKit
{
    namespace GLEngine
    {

        class RenderWindowRegion;
        class Engine;

        class AppBase : public EventCore::HandleCallback
        {

            // void OnWindowSizeChanged(EventCore::Property<MathCore::vec2i>* prop) {
            //     screenCenterWindow.x = prop->value.width / 2;
            //     screenCenterWindow.y = prop->value.height / 2;

            //     screenCenterApp.x = screenCenterWindow.x;
            //     screenCenterApp.y = prop->value.height - 1 - screenCenterWindow.y;

            //     if (screenCenterWindow.x > screenCenterWindow.y)
            //         normalization_factor = 1.0f / (float)screenCenterWindow.y;
            //     else
            //         normalization_factor = 1.0f / (float)screenCenterWindow.x;
            // }

            // float normalization_factor;

            // void windowResize(const AppKit::Window::WindowEvent& evt);
        public:
            bool canExitApplication;

            // MathCore::vec2f screenCenterApp;           // Mouse FPS controller helper
            // MathCore::vec2i screenCenterWindow;// used for FPS like mouse move

            // //
            // // Properties
            // //
            // EventCore::Property<MathCore::vec2i> WindowSize;
            // EventCore::Property<MathCore::vec2f> MousePos;
            // EventCore::Property<MathCore::vec2f> MousePosRelatedToCenter;
            // EventCore::Property<MathCore::vec2f> MousePosRelatedToCenterNormalized;

            // //
            // // Events
            // //
            // AppKit::Window::Window* window;
            // AppKit::Window::InputManager* windowInputManager;

            // CallEvent OnLostFocus;
            // CallEvent OnGainFocus;

            // EventCore::Event<void(Platform::Time *time)> OnPreUpdate;
            // EventCore::Event<void(Platform::Time *time)> OnUpdate;
            // EventCore::Event<void(Platform::Time *time)> OnLateUpdate;

            // EventCore::Event<void(Platform::Time *time)> OnAfterGraphPrecompute;

            // global events
            EventCore::Event<void()> OnLostFocus;
            EventCore::Event<void()> OnGainFocus;

            AppKit::Window::GLWindow *window;
            std::shared_ptr<RenderWindowRegion> screenRenderWindow;
            //std::shared_ptr<EventHandlerSet> eventHandlerSet;

            AppBase();

            virtual ~AppBase();

            virtual void draw() = 0;

            // void moveMouseToScreenCenter() const ;

            // MathCore::vec2f windowCoordToAppCoord(const MathCore::vec2i &input)const;

            // MathCore::vec2f appCoordRelativeToCenter(const MathCore::vec2f& appCoord)const;

            // MathCore::vec2f normalizeAppCoord(const MathCore::vec2f& appCoord)const ;

            // MathCore::vec2f windowCoordToNormalizedAppCoordRelativeToCenter(const MathCore::vec2i& input)const ;

            void exitApp();

            friend class Engine;
        };

    }

}