#pragma once

#include <AppKit/window/Window.h>

#include <InteractiveToolkit/Platform/Time.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/Property.h>

#include <appkit-gl-engine/types/iRect.h>
#include <vector>

#include <appkit-gl-base/GLTexture.h>
#include <appkit-gl-base/GLRenderBuffer.h>
#include <appkit-gl-base/GLDynamicFBO.h>

#include <appkit-gl-engine/ToShared.h>

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

            MathCore::vec2f screenCenterF_OriginBottom; // Mouse FPS controller helper
            MathCore::vec2i screenCenterI_OriginBottom; // used for FPS like mouse move

            float viewportScaleFactor;
            MathCore::vec2f windowToCameraScale;
            MathCore::vec2f cameraToWindowScale;

            //
            // Properties
            //
            // EventCore::Property<MathCore::vec2i> WindowSize;

            // OpenGL Viewport has y coord inverted, so we need to convert it
            EventCore::Property<AppKit::GLEngine::iRect> WindowViewport; // before apply viewportScaleFactor
            EventCore::Property<MathCore::vec2f> CameraScreenSize; // after apply viewportScaleFactor
            
            EventCore::Property<MathCore::vec2i> iMousePosLocal_OriginTop;

            EventCore::Property<MathCore::vec2f> MousePos_OriginBottom;
            EventCore::Property<MathCore::vec2f> MousePosRelatedToCenter_OriginBottom;
            EventCore::Property<MathCore::vec2f> MousePosRelatedToCenterNormalized_OriginBottom;

            //
            // Application Logic Events
            //

            // AppKit::Window::Window* window;

            AppKit::Window::InputManager inputManager;

            // EventCore::Event<void(Platform::Time *)> OnPreUpdate;
            // EventCore::Event<void(Platform::Time *)> OnUpdate;
            // EventCore::Event<void(Platform::Time *)> OnLateUpdate;

            // EventCore::Event<void(Platform::Time *)> OnAfterGraphPrecompute;

            // EventCore::Event<void(Platform::Time *)> OnAfterOverlayDraw;

            RenderWindowRegion();
            ~RenderWindowRegion();

            AppKit::GLEngine::iRect getWindowViewport() const;
            MathCore::vec2i getCameraScreenSizei() const;

            std::shared_ptr<RenderWindowRegion> setWindowViewport(const AppKit::GLEngine::iRect &viewport);
            std::shared_ptr<RenderWindowRegion> createFBO();
            std::shared_ptr<RenderWindowRegion> setEventForwardingEnabled(bool v);
            std::shared_ptr<RenderWindowRegion> setHandleWindowCloseButtonEnabled(bool v);
            std::shared_ptr<RenderWindowRegion> setViewportFromRealWindowSizeEnabled(bool v);

            void forceViewportFromRealWindowSize();
            void forceMouseToCoord_OriginTop(const MathCore::vec2i &iPos_origin_top);
            void forceMouseToCoord_OriginBottom(const MathCore::vec2i &iPos_origin_bottom);

            int getChildCount() const;
            std::shared_ptr<RenderWindowRegion> getChild(int at) const;
            std::shared_ptr<RenderWindowRegion> addChild(std::shared_ptr<RenderWindowRegion> child);
            void removeChild(std::shared_ptr<RenderWindowRegion> child);
            void clearChildren();

            void moveMouseToScreenCenter();

            MathCore::vec2i windowOriginTop_To_LocalOriginTop(const MathCore::vec2i &mouse_origin_top) const;

            MathCore::vec2i invertYCoordi(const MathCore::vec2i &mouse_origin_top) const;
            MathCore::vec2f invertYCoordf(const MathCore::vec2f &mouse_origin_top) const;

            MathCore::vec2f appCoordRelativeToCenter_OriginBottom(const MathCore::vec2f &appCoord_origin_bottom) const;

            MathCore::vec2f normalizeAppCoord(const MathCore::vec2f &appCoord) const;

            MathCore::vec2f mouse_OriginTop_To_NormalizedAppCoordRelativeToCenter_OriginBottom(const MathCore::vec2i &mouse_coord_origin_top) const;

            bool isLocalInsideViewport_OriginBottom(const MathCore::vec2i &coord_origin_bottom) const;

            inline std::shared_ptr<RenderWindowRegion> self() const
            {
                return ToShared<RenderWindowRegion>(mSelf);
            }

            inline std::shared_ptr<RenderWindowRegion> parent() const
            {
                return ToShared<RenderWindowRegion>(parentRef);
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