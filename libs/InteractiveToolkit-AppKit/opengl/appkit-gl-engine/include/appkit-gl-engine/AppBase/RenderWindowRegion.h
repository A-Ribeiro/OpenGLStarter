#pragma once

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <aRibeiroPlatform/aRibeiroPlatform.h>
// #include <appkit-gl-base/opengl-wrapper.h>
// #include <window-gl-wrapper/window-gl-wrapper.h>

#include <AppKit/window/Window.h>

#include <InteractiveToolkit/Platform/Time.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/Property.h>

#include <appkit-gl-engine/types/iRect.h>
#include <vector>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLRenderBuffer.h>
#include <appkit-gl-base/GLDynamicFBO.h>

namespace AppKit
{
    namespace GLEngine
    {

        // BEGIN_DECLARE_DELEGATE(CallEvent)
        // CALL_PATTERN() END_DECLARE_DELEGATE;
        // BEGIN_DECLARE_DELEGATE(UpdateEvent, Platform::Time *time)
        // CALL_PATTERN(time) END_DECLARE_DELEGATE;

        // could be an FBO, or a Screen

        class RenderWindowRegion : public EventCore::HandleCallback
        {
            bool forward_events;
            bool handle_window_close;
            bool force_viewport_from_real_window_size;
            // AppKit::GLEngine::iRect viewport;
            float normalization_factor;

            // translate the events to the inner viewports...
            std::vector<std::shared_ptr<RenderWindowRegion>> innerWindowList;

            std::weak_ptr<RenderWindowRegion> parentRef;

            std::weak_ptr<RenderWindowRegion> mSelf;

            void OnWindowViewportChanged(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue);

            // void viewport_set_internal(const AppKit::GLEngine::iRect &viewport);

            // void OnWindowSizeChanged(EventCore::Property<MathCore::vec2i>* prop);

            // window size and close window events
            // void onWindowEvent_WindowSize_CloseWindow(const AppKit::Window::WindowEvent& evt);

            // all window events
            void onWindowEvent(const AppKit::Window::WindowEvent &evt);
            void onKeyboardEvent(const AppKit::Window::KeyboardEvent &evt);
            void onMouseEvent(const AppKit::Window::MouseEvent &evt);
            void onJoystickEvent(const AppKit::Window::JoystickEvent &evt);
            void onTouchEvent(const AppKit::Window::TouchEvent &evt);
            void onSensorEvent(const AppKit::Window::SensorEvent &evt);

        public:
            AppKit::OpenGL::GLTexture *color_buffer;
            // AppKit::OpenGL::GLTexture *z_buffer;
            AppKit::OpenGL::GLRenderBuffer *z_render_buffer;
            AppKit::OpenGL::GLDynamicFBO *fbo;

            //
            // Variables
            //

            MathCore::vec2f screenCenterF; // Mouse FPS controller helper
            MathCore::vec2i screenCenterI; // used for FPS like mouse move

            float viewportScaleFactor;
            MathCore::vec2f windowToCameraScale;
            MathCore::vec2f cameraToWindowScale;

            //
            // Properties
            //
            // EventCore::Property<MathCore::vec2i> WindowSize;

            EventCore::Property<AppKit::GLEngine::iRect> WindowViewport; // before apply viewportScaleFactor
            EventCore::Property<AppKit::GLEngine::iRect> CameraViewport; // after apply viewportScaleFactor
            
            EventCore::Property<MathCore::vec2i> iMousePosLocal; // y is in window local coord, increase to bottom (inverted).

            EventCore::Property<MathCore::vec2f> MousePos;
            EventCore::Property<MathCore::vec2f> MousePosRelatedToCenter;
            EventCore::Property<MathCore::vec2f> MousePosRelatedToCenterNormalized;

            //
            // Application Logic Events
            //

            // AppKit::Window::Window* window;

            AppKit::Window::InputManager inputManager;

            EventCore::Event<void(Platform::Time *)> OnPreUpdate;
            EventCore::Event<void(Platform::Time *)> OnUpdate;
            EventCore::Event<void(Platform::Time *)> OnLateUpdate;

            EventCore::Event<void(Platform::Time *)> OnAfterGraphPrecompute;

            EventCore::Event<void(Platform::Time *)> OnAfterOverlayDraw;

            RenderWindowRegion();
            ~RenderWindowRegion();

            AppKit::GLEngine::iRect getWindowViewport() const;

            std::shared_ptr<RenderWindowRegion> setWindowViewport(const AppKit::GLEngine::iRect &viewport);
            std::shared_ptr<RenderWindowRegion> createFBO();
            std::shared_ptr<RenderWindowRegion> setEventForwardingEnabled(bool v);
            std::shared_ptr<RenderWindowRegion> setHandleWindowCloseButtonEnabled(bool v);
            std::shared_ptr<RenderWindowRegion> setViewportFromRealWindowSizeEnabled(bool v);

            void forceViewportFromRealWindowSize();
            void forceMouseToCoord(const MathCore::vec2i &iPos) const;

            int getChildCount() const;
            std::shared_ptr<RenderWindowRegion> getChild(int at) const;
            void addChild(std::shared_ptr<RenderWindowRegion> child);
            void removeChild(std::shared_ptr<RenderWindowRegion> child);
            void clearChildren();

            void moveMouseToScreenCenter() const;

            MathCore::vec2i windowToLocal(const MathCore::vec2i &input) const;

            MathCore::vec2f localCoordToAppCoord(const MathCore::vec2i &input) const;

            MathCore::vec2f appCoordRelativeToCenter(const MathCore::vec2f &appCoord) const;

            MathCore::vec2f normalizeAppCoord(const MathCore::vec2f &appCoord) const;

            MathCore::vec2f localCoordToNormalizedAppCoordRelativeToCenter(const MathCore::vec2i &input) const;

            bool isLocalInsideViewport(const MathCore::vec2i &input) const;

            inline std::shared_ptr<RenderWindowRegion> self()
            {
                return std::shared_ptr<RenderWindowRegion>(mSelf);
            }

            static inline std::shared_ptr<RenderWindowRegion> CreateShared()
            {
                auto result = std::make_shared<RenderWindowRegion>();
                result->mSelf = std::weak_ptr<RenderWindowRegion>(result);
                return result;
            }
        };

    }

}