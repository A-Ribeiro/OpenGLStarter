#include <appkit-gl-engine/AppBase/AppBase.h>

#include <appkit-gl-engine/Engine.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>

namespace AppKit
{
    namespace GLEngine
    {

        // void AppBase::windowResize(const AppKit::Window::WindowEvent& evt) {
        //     if (evt.type == AppKit::Window::WindowEventType::Resized)
        //         this->screenRenderWindow.Viewport = iRect(evt.resized.width, evt.resized.height);
        // }

        AppBase::AppBase()
        {
            // normalization_factor = 1.0f;
            canExitApplication = false;

            window = Engine::Instance()->window;
            // this->screenRenderWindow.Viewport = iRect(window->getSize().width, window->getSize().height);
            // this->screenRenderWindow.inputManager.onWindowEvent.add(EventCore::CallbackWrapper(&AppBase::windowResize, this));

            // windowInputManager = &window->inputManager;

            // WindowSize.OnChange.add(this, &AppBase::OnWindowSizeChanged);
            // WindowSize = window->getSize();

            // screenRenderWindow.setHandleWindowCloseButtonEnabled(true);
            // screenRenderWindow.setViewportFromRealWindowSizeEnabled(true);
            // //screenRenderWindow.setEventForwardingEnabled(true);

            screenRenderWindow = RenderWindowRegion::CreateShared();
            // eventHandlerSet = std::make_shared<EventHandlerSet>();
        }

        AppBase::~AppBase()
        {
        }

        //    virtual void draw() = 0;

        // void AppBase::moveMouseToScreenCenter() const
        // {
        //     AppKit::Window::Devices::Mouse::setPosition(screenCenterWindow, window);
        // }

        // MathCore::vec2f AppBase::windowCoordToAppCoord(const MathCore::vec2i &input) const
        // {
        //     return MathCore::vec2f(input.x, window->getSize().height - 1 - input.y);
        // }

        // MathCore::vec2f AppBase::appCoordRelativeToCenter(const MathCore::vec2f &appCoord) const
        // {
        //     return appCoord - screenCenterApp;
        // }

        // MathCore::vec2f AppBase::normalizeAppCoord(const MathCore::vec2f &appCoord) const
        // {
        //     return appCoord * normalization_factor;
        // }

        // MathCore::vec2f AppBase::windowCoordToNormalizedAppCoordRelativeToCenter(const MathCore::vec2i &input) const
        // {
        //     return (MathCore::vec2f(input.x, window->getSize().height - 1 - input.y) - screenCenterApp) * normalization_factor;
        // }

        void AppBase::exitApp()
        {
            canExitApplication = true;
        }

    }

}
