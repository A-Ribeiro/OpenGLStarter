#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>

#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {

        // all window events
        void RenderWindowRegion::onWindowEvent(const AppKit::Window::WindowEvent &evt)
        {

            if (handle_window_close || force_viewport_from_real_window_size)
            {
                switch (evt.type)
                {
                case AppKit::Window::WindowEventType::Closed:
                    if (handle_window_close)
                    {
                        AppKit::GLEngine::AppBase *app = AppKit::GLEngine::Engine::Instance()->app;
                        if (app != nullptr)
                            app->exitApp();
                    }
                    break;
                case AppKit::Window::WindowEventType::Resized:
                    if (force_viewport_from_real_window_size)
                    {
                        setWindowViewport(AppKit::GLEngine::iRect(evt.resized.width, evt.resized.height));
                    }
                    break;
                default:
                    break;
                }
            }

            switch (evt.type)
            {
            case AppKit::Window::WindowEventType::Resized:
            {
                auto engine = AppKit::GLEngine::Engine::Instance();
                auto glContextConfig = engine->window->getGLContextConfig();
                // if (engine->isIntelCard)
                {
                    if (glContextConfig.sRgbCapable)
                        glDisable(GL_FRAMEBUFFER_SRGB);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                }
                break;
            }
            default:
                break;
            }

            if (!forward_events)
                return;
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onWindowEvent(evt);
            }
        }
        void RenderWindowRegion::onKeyboardEvent(const AppKit::Window::KeyboardEvent &evt)
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onKeyboardEvent(evt);
            }
        }
        void RenderWindowRegion::onMouseEvent(const AppKit::Window::MouseEvent &evt_origin_top)
        {
            AppKit::Window::MouseEvent local_evt_origin_bottom = evt_origin_top;

            // transform to local coordinates
            // local_evt_origin_bottom.position = windowOriginTop_To_LocalOriginTop(local_evt_origin_bottom.position);

            // invert to get bottom reference
            local_evt_origin_bottom.position = invertYCoordi(local_evt_origin_bottom.position);

            local_evt_origin_bottom.position.x -= WindowViewport.c_val().x;
            // local_evt.position.y = parent->Viewport.c_ptr()->h -1 - Viewport.c_ptr()->h - local_evt.position.y,
            local_evt_origin_bottom.position.y -= WindowViewport.c_val().y;

            if (evt_origin_top.type == AppKit::Window::MouseEventType::ButtonPressed &&
                !isLocalInsideViewport_OriginBottom(local_evt_origin_bottom.position))
                return;

            switch (evt_origin_top.type)
            {
            case AppKit::Window::MouseEventType::ButtonPressed:
            case AppKit::Window::MouseEventType::ButtonReleased:
            case AppKit::Window::MouseEventType::Moved:
            {
                iMousePosLocal_OriginTop = invertYCoordi(local_evt_origin_bottom.position);

                MathCore::vec2f appCoord_origin_bottom = (MathCore::vec2f)local_evt_origin_bottom.position;

                MousePos_OriginBottom = appCoord_origin_bottom;
                appCoord_origin_bottom -= screenCenterF_OriginBottom;
                MousePosRelatedToCenter_OriginBottom = appCoord_origin_bottom;
                // appCoord = app->normalizeAppCoord(appCoord);
                appCoord_origin_bottom *= normalization_factor;
                MousePosRelatedToCenterNormalized_OriginBottom = appCoord_origin_bottom;

                // printf(" viewport: %i %i\n", WindowViewport.c_val().x, WindowViewport.c_val().y);
                // printf("     mousePos: %f %f\n", MousePos_OriginBottom.c_val().x, MousePos_OriginBottom.c_val().y);
                break;
            }
            default:
            {
                break;
            }
            }

            if (!forward_events)
                return;

            AppKit::Window::MouseEvent local_evt_origin_top_child = local_evt_origin_bottom;
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                local_evt_origin_top_child.position = innerWindowList[i]->invertYCoordi(local_evt_origin_top_child.position);
                innerWindowList[i]->inputManager.onMouseEvent(local_evt_origin_top_child);
            }
        }

        void RenderWindowRegion::onJoystickEvent(const AppKit::Window::JoystickEvent &evt)
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onJoystickEvent(evt);
            }
        }
        void RenderWindowRegion::onTouchEvent(const AppKit::Window::TouchEvent &evt)
        {
            AppKit::Window::TouchEvent local_evt = evt;
            local_evt.position.x -= WindowViewport.c_ptr()->x;
            local_evt.position.y -= WindowViewport.c_ptr()->y;

            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onTouchEvent(local_evt);
            }
        }
        void RenderWindowRegion::onSensorEvent(const AppKit::Window::SensorEvent &evt)
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onSensorEvent(evt);
            }
        }

        RenderWindowRegion::RenderWindowRegion()
        //: Viewport(this, &RenderWindowRegion::getViewport, &RenderWindowRegion::viewport_set_internal)
        {
            viewportScaleFactor = 1.0f;
            forward_events = false;
            handle_window_close = false;
            force_viewport_from_real_window_size = false;
            WindowViewport = AppKit::GLEngine::iRect(0, 0, 0, 0);
            CameraScreenSize = MathCore::vec2f(0, 0);
            normalization_factor = 1.0f;
            fbo = nullptr;
            color_buffer = nullptr;
            // z_buffer = nullptr;
            z_render_buffer = nullptr;
            screenCenterI_OriginBottom = MathCore::vec2i(0, 0);
            screenCenterF_OriginBottom = MathCore::vec2f(0, 0);

            parentRef.reset(); // = nullptr;

            inputManager.onMouseEvent.add(&RenderWindowRegion::onMouseEvent, this);
            inputManager.onWindowEvent.add(&RenderWindowRegion::onWindowEvent, this);
            WindowViewport.OnChange.add(&RenderWindowRegion::OnWindowViewportChanged, this);
        }
        RenderWindowRegion::~RenderWindowRegion()
        {
            // this->setEventForwardingEnabled(false);

            inputManager.onMouseEvent.remove(&RenderWindowRegion::onMouseEvent, this);
            inputManager.onWindowEvent.remove(&RenderWindowRegion::onWindowEvent, this);
            WindowViewport.OnChange.remove(&RenderWindowRegion::OnWindowViewportChanged, this);

            // handle_window_close = false;
            // force_viewport_from_real_window_size = false;
            // viewport = AppKit::GLEngine::iRect(0,0,0,0);
            // normalization_factor = 1.0f;

            if (fbo != nullptr)
            {
                delete fbo;
                fbo = nullptr;
            }
            if (color_buffer != nullptr)
            {
                delete color_buffer;
                color_buffer = nullptr;
            }
            // delete z_buffer);
            if (z_render_buffer != nullptr)
            {
                delete z_render_buffer;
                z_render_buffer = nullptr;
            }
            // screenCenterI = MathCore::vec2i::Create(0,0);
        }

        void RenderWindowRegion::OnWindowViewportChanged(const AppKit::GLEngine::iRect &value, const AppKit::GLEngine::iRect &oldValue)
        {

            AppKit::GLEngine::iRect viewport = value;

            screenCenterI_OriginBottom.x = viewport.w / 2;
            screenCenterI_OriginBottom.y = viewport.h / 2;

            screenCenterF_OriginBottom.x = screenCenterI_OriginBottom.x;
            screenCenterF_OriginBottom.y = screenCenterI_OriginBottom.y;

            if (screenCenterI_OriginBottom.x > screenCenterI_OriginBottom.y)
                normalization_factor = 1.0f / (float)screenCenterI_OriginBottom.y;
            else
                normalization_factor = 1.0f / (float)screenCenterI_OriginBottom.x;

            MathCore::vec2f size = MathCore::vec2f(viewport.w, viewport.h) / viewportScaleFactor;
            MathCore::vec2i sizei = (MathCore::vec2i)(MathCore::OP<MathCore::vec2f>::ceil(size) + 0.5f);

            if (fbo != nullptr)
            {
                fbo->setSize(sizei.width, sizei.height);
            }

            // auto new_camera_viewport = AppKit::GLEngine::iRect(
            //     //viewport.x, viewport.y,
            //     sizei.width,
            //     sizei.height
            // );

            windowToCameraScale.width = (float)size.width / (float)viewport.w;
            windowToCameraScale.height = (float)size.height / (float)viewport.h;

            cameraToWindowScale.width = (float)viewport.w / (float)size.width;
            cameraToWindowScale.height = (float)viewport.h / (float)size.height;
            // cameraToWindowScale

            // CameraScreenSize = MathCore::vec2f(
            //     //viewport.x, viewport.y,
            //     sizei.width,
            //     sizei.height
            // );
            CameraScreenSize = size;
        }

        AppKit::GLEngine::iRect RenderWindowRegion::getWindowViewport() const
        {
            return *WindowViewport.c_ptr();
        }

        MathCore::vec2i RenderWindowRegion::getCameraScreenSizei() const
        {
            return MathCore::vec2i(MathCore::OP<MathCore::vec2f>::ceil(CameraScreenSize.c_val()) + 0.5f);
        }

        // void RenderWindowRegion::viewport_set_internal(const AppKit::GLEngine::iRect &viewport)
        // {
        //     setViewport(viewport);
        // }
        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::setWindowViewport(const AppKit::GLEngine::iRect &viewport)
        {
            // this->viewport = viewport;

            WindowViewport = viewport;

            return this->self();
        }

        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::createFBO()
        {
            if (fbo != nullptr)
                return this->self();

            auto sizei = getCameraScreenSizei();

            int w = sizei.x;
            int h = sizei.y;

            fbo = new AppKit::OpenGL::GLDynamicFBO();
            fbo->setSize(w, h);

            color_buffer = new AppKit::OpenGL::GLTexture(w, h, GL_RGB);
            // z_buffer = new AppKit::OpenGL::GLTexture(viewport.w,viewport.h,GL_DEPTH_COMPONENT24);
            z_render_buffer = new AppKit::OpenGL::GLRenderBuffer(w, h, GL_DEPTH_COMPONENT24);

            fbo->enable();

            fbo->setColorAttachment(color_buffer, 0);
            // fbo->setDepthTextureAttachment(z_buffer);
            fbo->setDepthRenderBufferAttachment(z_render_buffer);

            fbo->checkAttachment();

            fbo->disable();

            return this->self();
        }
        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::setEventForwardingEnabled(bool v)
        {
            forward_events = v;
            // this->handle_window_close = handle_window_close;
            // this->force_viewport_from_real_window_size = force_viewport_from_real_window_size;

            // inputManager.onWindowEvent.remove(this, &RenderWindowRegion::onWindowEvent);
            inputManager.onKeyboardEvent.remove(&RenderWindowRegion::onKeyboardEvent, this);
            // inputManager.onMouseEvent.remove(this, &RenderWindowRegion::onMouseEvent);
            inputManager.onJoystickEvent.remove(&RenderWindowRegion::onJoystickEvent, this);
            inputManager.onTouchEvent.remove(&RenderWindowRegion::onTouchEvent, this);
            inputManager.onSensorEvent.remove(&RenderWindowRegion::onSensorEvent, this);

            if (v)
            {
                // inputManager.onWindowEvent.add(this, &RenderWindowRegion::onWindowEvent);
                inputManager.onKeyboardEvent.add(&RenderWindowRegion::onKeyboardEvent, this);
                // inputManager.onMouseEvent.add(this, &RenderWindowRegion::onMouseEvent);
                inputManager.onJoystickEvent.add(&RenderWindowRegion::onJoystickEvent, this);
                inputManager.onTouchEvent.add(&RenderWindowRegion::onTouchEvent, this);
                inputManager.onSensorEvent.add(&RenderWindowRegion::onSensorEvent, this);
            }

            // if (force_viewport_from_real_window_size)
            //     forceViewportFromRealWindowSize();

            return this->self();
        }

        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::setHandleWindowCloseButtonEnabled(bool v)
        {
            this->handle_window_close = v;
            return this->self();
        }

        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::setViewportFromRealWindowSizeEnabled(bool v)
        {
            this->force_viewport_from_real_window_size = v;
            if (force_viewport_from_real_window_size)
                forceViewportFromRealWindowSize();
            return this->self();
        }

        void RenderWindowRegion::forceViewportFromRealWindowSize()
        {
            AppKit::GLEngine::AppBase *app = AppKit::GLEngine::Engine::Instance()->app;
            if (app == nullptr)
                return;

            MathCore::vec2i size = app->window->getSize();
            setWindowViewport(AppKit::GLEngine::iRect(size.width, size.height));
        }

        void RenderWindowRegion::forceMouseToCoord_OriginTop(const MathCore::vec2i &iPos_origin_top) const
        {
            auto parent = ToShared(parentRef);

            if (parent == nullptr && force_viewport_from_real_window_size)
            {

                AppKit::GLEngine::AppBase *app = AppKit::GLEngine::Engine::Instance()->app;
                if (app == nullptr)
                    return;

                AppKit::Window::Devices::Mouse::setPosition(iPos_origin_top, app->window);
            }
            else
            {
                MathCore::vec2i iPos_origin_bottom = invertYCoordi(iPos_origin_top);

                MathCore::vec2i iParentPos_bottom;

                iParentPos_bottom.x = iPos_origin_bottom.x + WindowViewport.c_ptr()->x;
                iParentPos_bottom.y = iPos_origin_bottom.y + WindowViewport.c_ptr()->y;

                parent->forceMouseToCoord_OriginBottom(iParentPos_bottom);
            }
        }

        void RenderWindowRegion::forceMouseToCoord_OriginBottom(const MathCore::vec2i &iPos_origin_bottom) const
        {
            auto parent = ToShared(parentRef);

            if (parent == nullptr && force_viewport_from_real_window_size)
            {

                AppKit::GLEngine::AppBase *app = AppKit::GLEngine::Engine::Instance()->app;
                if (app == nullptr)
                    return;

                MathCore::vec2i iParentPos_top = invertYCoordi(iPos_origin_bottom);

                AppKit::Window::Devices::Mouse::setPosition(iParentPos_top, app->window);
            }
            else
            {
                MathCore::vec2i iParentPos_bottom;

                iParentPos_bottom.x = iPos_origin_bottom.x + WindowViewport.c_ptr()->x;
                iParentPos_bottom.y = iPos_origin_bottom.y + WindowViewport.c_ptr()->y;

                parent->forceMouseToCoord_OriginBottom(iParentPos_bottom);
            }
        }

        int RenderWindowRegion::getChildCount() const
        {
            return (int)innerWindowList.size();
        }
        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::getChild(int at) const
        {
            return innerWindowList[at];
        }
        std::shared_ptr<RenderWindowRegion> RenderWindowRegion::addChild(std::shared_ptr<RenderWindowRegion> child)
        {
            auto child_parent = ToShared(child->parentRef);
            ITK_ABORT(child_parent != nullptr, "This child has been added as child of another render window");

            innerWindowList.push_back(child);
            child->parentRef = this->self();
            return child;
        }
        void RenderWindowRegion::removeChild(std::shared_ptr<RenderWindowRegion> child)
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                if (innerWindowList[i] == child)
                {
                    child->parentRef.reset(); // = nullptr;
                    innerWindowList.erase(innerWindowList.begin() + i);
                    return;
                }
            }
        }
        void RenderWindowRegion::clearChildren()
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
                innerWindowList[i]->parentRef.reset(); // = nullptr;
            innerWindowList.clear();
        }

        void RenderWindowRegion::moveMouseToScreenCenter()
        {
            MousePos_OriginBottom = screenCenterF_OriginBottom; // set app state do cursor center
            MousePosRelatedToCenter_OriginBottom = MathCore::vec2f(0, 0);
            MousePosRelatedToCenterNormalized_OriginBottom = MathCore::vec2f(0, 0);
            forceMouseToCoord_OriginBottom(screenCenterI_OriginBottom);
        }

        MathCore::vec2i RenderWindowRegion::windowOriginTop_To_LocalOriginTop(const MathCore::vec2i &mouse_origin_top) const
        {
            int offset_x = 0;
            int offset_y = 0;

            std::shared_ptr<RenderWindowRegion> base = this->self();

            std::shared_ptr<RenderWindowRegion> parent_walker = ToShared(parentRef);
            while (parent_walker != nullptr)
            {
                base = parent_walker;

                offset_x += parent_walker->WindowViewport.c_ptr()->x;
                offset_y += parent_walker->WindowViewport.c_ptr()->y;

                parent_walker = ToShared(parent_walker->parentRef);
            }

            MathCore::vec2i global_origin_bottom = base->invertYCoordi(mouse_origin_top);

            MathCore::vec2i result_origin_bottom;
            result_origin_bottom.x = global_origin_bottom.x - offset_x;
            result_origin_bottom.y = global_origin_bottom.y - offset_y;

            MathCore::vec2i result_origin_top = this->invertYCoordi(result_origin_bottom);

            return result_origin_top;
        }

        MathCore::vec2i RenderWindowRegion::invertYCoordi(const MathCore::vec2i &mouse_origin_top) const
        {
            return MathCore::vec2i(mouse_origin_top.x, WindowViewport.c_val().h - 1 - mouse_origin_top.y);
        }

        MathCore::vec2f RenderWindowRegion::invertYCoordf(const MathCore::vec2f &mouse_origin_top) const
        {
            return MathCore::vec2f(mouse_origin_top.x, WindowViewport.c_val().h - 1 - mouse_origin_top.y);
        }

        MathCore::vec2f RenderWindowRegion::appCoordRelativeToCenter_OriginBottom(const MathCore::vec2f &appCoord_origin_bottom) const
        {
            return appCoord_origin_bottom - screenCenterF_OriginBottom;
        }

        MathCore::vec2f RenderWindowRegion::normalizeAppCoord(const MathCore::vec2f &appCoord) const
        {
            return appCoord * normalization_factor;
        }

        MathCore::vec2f RenderWindowRegion::mouse_OriginTop_To_NormalizedAppCoordRelativeToCenter_OriginBottom(const MathCore::vec2i &mouse_coord_origin_top) const
        {
            MathCore::vec2i local_coord_origin_top = windowOriginTop_To_LocalOriginTop(mouse_coord_origin_top);
            MathCore::vec2i local_coord_origin_bottom = invertYCoordi(local_coord_origin_top);

            return ((MathCore::vec2f)local_coord_origin_bottom - screenCenterF_OriginBottom) * normalization_factor;
        }

        bool RenderWindowRegion::isLocalInsideViewport_OriginBottom(const MathCore::vec2i &coord_origin_bottom) const
        {
            return coord_origin_bottom.x >= 0 && coord_origin_bottom.y >= 0 &&
                   coord_origin_bottom.x < WindowViewport.c_ptr()->w &&
                   coord_origin_bottom.y < WindowViewport.c_ptr()->h;
        }

    }

}