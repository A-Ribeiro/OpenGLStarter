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
                //if (engine->isIntelCard) 
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
        void RenderWindowRegion::onMouseEvent(const AppKit::Window::MouseEvent &evt)
        {
            AppKit::Window::MouseEvent local_evt = evt;
            local_evt.position.x -= WindowViewport.c_ptr()->x;

            // local_evt.position.y = parent->Viewport.c_ptr()->h -1 - Viewport.c_ptr()->h - local_evt.position.y,
            local_evt.position.y -= WindowViewport.c_ptr()->y;

            if (evt.type == AppKit::Window::MouseEventType::ButtonPressed &&
                !isLocalInsideViewport(local_evt.position))
            {
                return;
            }

            switch (evt.type)
            {
            case AppKit::Window::MouseEventType::ButtonPressed:
            case AppKit::Window::MouseEventType::ButtonReleased:
            case AppKit::Window::MouseEventType::Moved:
            {
                iMousePosLocal = local_evt.position;
                MathCore::vec2f appCoord = localCoordToAppCoord(local_evt.position);
                MousePos = appCoord;
                appCoord -= screenCenterF;
                MousePosRelatedToCenter = appCoord;
                // appCoord = app->normalizeAppCoord(appCoord);
                appCoord *= normalization_factor;
                MousePosRelatedToCenterNormalized = appCoord;
                break;
            }
            default:
            {
                break;
            }
            }

            if (!forward_events)
                return;
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                innerWindowList[i]->inputManager.onMouseEvent(local_evt);
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
            CameraViewport = AppKit::GLEngine::iRect(0, 0, 0, 0);
            normalization_factor = 1.0f;
            fbo = nullptr;
            color_buffer = nullptr;
            // z_buffer = nullptr;
            z_render_buffer = nullptr;
            screenCenterI = MathCore::vec2i(0, 0);

            parentRef.reset();// = nullptr;

            inputManager.onMouseEvent.add(&RenderWindowRegion::onMouseEvent, this);
            inputManager.onWindowEvent.add(&RenderWindowRegion::onWindowEvent, this);
            WindowViewport.OnChange.add(&RenderWindowRegion::OnWindowViewportChanged, this);
        }
        RenderWindowRegion::~RenderWindowRegion()
        {
            //this->setEventForwardingEnabled(false);

            inputManager.onMouseEvent.remove(&RenderWindowRegion::onMouseEvent, this);
            inputManager.onWindowEvent.remove(&RenderWindowRegion::onWindowEvent, this);
            WindowViewport.OnChange.remove(&RenderWindowRegion::OnWindowViewportChanged, this);
            

            // handle_window_close = false;
            // force_viewport_from_real_window_size = false;
            // viewport = AppKit::GLEngine::iRect(0,0,0,0);
            // normalization_factor = 1.0f;

            if (fbo != nullptr){
                delete fbo;
                fbo = nullptr;
            }
            if (color_buffer != nullptr){
                delete color_buffer;
                color_buffer = nullptr;
            }
        // delete z_buffer);
            if (z_render_buffer != nullptr){
                delete z_render_buffer;
                z_render_buffer = nullptr;
            }
        // screenCenterI = MathCore::vec2i::Create(0,0);
        }

        void RenderWindowRegion::OnWindowViewportChanged(const AppKit::GLEngine::iRect &value,const AppKit::GLEngine::iRect &oldValue)
        {

            AppKit::GLEngine::iRect viewport = value;

            screenCenterI.x = viewport.w / 2;
            screenCenterI.y = viewport.h / 2;

            screenCenterF.x = screenCenterI.x;
            screenCenterF.y = viewport.h - 1 - screenCenterI.y;

            if (screenCenterI.x > screenCenterI.y)
                normalization_factor = 1.0f / (float)screenCenterI.y;
            else
                normalization_factor = 1.0f / (float)screenCenterI.x;

            MathCore::vec2f size = MathCore::vec2f(viewport.w,viewport.h) / viewportScaleFactor;
            MathCore::vec2i sizei = (MathCore::vec2i)(MathCore::OP<MathCore::vec2f>::round(size) + 0.5f);

            if (fbo != nullptr){
                fbo->setSize(sizei.width, sizei.height);
            }

            CameraViewport = AppKit::GLEngine::iRect(
                //viewport.x, viewport.y,
                sizei.width,
                sizei.height
            );

            windowToCameraScale.width = (float)CameraViewport.c_ptr()->w / (float)viewport.w;
            windowToCameraScale.height = (float)CameraViewport.c_ptr()->h / (float)viewport.h;

            cameraToWindowScale.width = (float)viewport.w / (float)CameraViewport.c_ptr()->w;
            cameraToWindowScale.height = (float)viewport.h / (float)CameraViewport.c_ptr()->h;

            //cameraToWindowScale
        }

        AppKit::GLEngine::iRect RenderWindowRegion::getWindowViewport() const
        {
            return *WindowViewport.c_ptr();
        }

        // void RenderWindowRegion::viewport_set_internal(const AppKit::GLEngine::iRect &viewport)
        // {
        //     setViewport(viewport);
        // }
        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::setWindowViewport(const AppKit::GLEngine::iRect &viewport)
        {
            // this->viewport = viewport;

            WindowViewport = viewport;

            return this->self();
        }

        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::createFBO()
        {
            if (fbo != nullptr)
                return this->self();
            
            int w = CameraViewport.c_ptr()->w;
            int h = CameraViewport.c_ptr()->h;
            
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
        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::setEventForwardingEnabled(bool v)
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

        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::setHandleWindowCloseButtonEnabled(bool v)
        {
            this->handle_window_close = v;
            return this->self();
        }

        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::setViewportFromRealWindowSizeEnabled(bool v)
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

        void RenderWindowRegion::forceMouseToCoord(const MathCore::vec2i &iPos) const
        {
            auto parent = ToShared(parentRef);

            if (parent == nullptr && force_viewport_from_real_window_size)
            {

                AppKit::GLEngine::AppBase *app = AppKit::GLEngine::Engine::Instance()->app;
                if (app == nullptr)
                    return;

                AppKit::Window::Devices::Mouse::setPosition(iPos, app->window);
            }
            else
            {

                MathCore::vec2i iParentPos;
                iParentPos.x = iPos.x + WindowViewport.c_ptr()->x;
                iParentPos.y = iPos.y + WindowViewport.c_ptr()->y;

                parent->forceMouseToCoord(iParentPos);
            }
        }

        int RenderWindowRegion::getChildCount() const
        {
            return (int)innerWindowList.size();
        }
        std::shared_ptr<RenderWindowRegion>RenderWindowRegion::getChild(int at) const
        {
            return innerWindowList[at];
        }
        void RenderWindowRegion::addChild(std::shared_ptr<RenderWindowRegion>child)
        {
            auto child_parent = ToShared(child->parentRef);
            ITK_ABORT(child_parent != nullptr, "This child has been added as child of another render window");

            innerWindowList.push_back(child);
            child->parentRef = this->self();
        }
        void RenderWindowRegion::removeChild(std::shared_ptr<RenderWindowRegion>child)
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
            {
                if (innerWindowList[i] == child)
                {
                    child->parentRef.reset();// = nullptr;
                    innerWindowList.erase(innerWindowList.begin() + i);
                    return;
                }
            }
        }
        void RenderWindowRegion::clearChildren()
        {
            for (size_t i = 0; i < innerWindowList.size(); i++)
                innerWindowList[i]->parentRef.reset();// = nullptr;
            innerWindowList.clear();
        }

        void RenderWindowRegion::moveMouseToScreenCenter() const
        {
            forceMouseToCoord(screenCenterI);
        }

        MathCore::vec2i RenderWindowRegion::windowToLocal(const MathCore::vec2i &input) const
        {

            int offset_x = 0;
            int offset_y = 0;

            std::shared_ptr<RenderWindowRegion> parent_walker = ToShared(parentRef);
            while (parent_walker != nullptr)
            {

                offset_x += parent_walker->WindowViewport.c_ptr()->x;
                offset_y += parent_walker->WindowViewport.c_ptr()->y;

                parent_walker = ToShared(parent_walker->parentRef);
            }

            MathCore::vec2i result;
            result.x = input.x - offset_x;
            result.y = input.y - offset_y;

            return result;
        }

        MathCore::vec2f RenderWindowRegion::localCoordToAppCoord(const MathCore::vec2i &input) const
        {
            return MathCore::vec2f(input.x, WindowViewport.c_ptr()->h - 1 - input.y);
        }

        MathCore::vec2f RenderWindowRegion::appCoordRelativeToCenter(const MathCore::vec2f &appCoord) const
        {
            return appCoord - screenCenterF;
        }

        MathCore::vec2f RenderWindowRegion::normalizeAppCoord(const MathCore::vec2f &appCoord) const
        {
            return appCoord * normalization_factor;
        }

        MathCore::vec2f RenderWindowRegion::localCoordToNormalizedAppCoordRelativeToCenter(const MathCore::vec2i &input) const
        {
            return (MathCore::vec2f(input.x, WindowViewport.c_ptr()->h - 1 - input.y) - screenCenterF) * normalization_factor;
        }

        bool RenderWindowRegion::isLocalInsideViewport(const MathCore::vec2i &input) const
        {
            return input.x < WindowViewport.c_ptr()->w && input.y < WindowViewport.c_ptr()->h;
        }

    }

}