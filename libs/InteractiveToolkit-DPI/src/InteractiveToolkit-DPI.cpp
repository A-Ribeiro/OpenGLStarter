#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>
#include <thread>
#include <exception>

#define USE_XRANDR_CMDLINE_HELPER 0

#if USE_XRANDR_CMDLINE_HELPER == 1
int main_xrandr(int argc, char **argv);
#endif

namespace DPI
{
    void Monitor::setMode(const Mode &mode)
    {
#if defined(__linux__)
        Display::setMonitorResolution(port, mode.width, mode.height, mode.freqs[0]);
#elif defined(_WIN32)
        // windows implementation
        // ITK_ABORT(true, "Not implemented yet on Windows...\n");
#endif
    }

}

#if defined(__linux__)

// https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_Changing_the_Screen_Resolution

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <signal.h>

namespace DPI
{
    // X11 error handlers for child process
    static int x11_error_handler(::Display *dpy, XErrorEvent *error)
    {
        char error_text[256];
        XGetErrorText(dpy, error->error_code, error_text, sizeof(error_text));
        fprintf(stderr, "X11 Error: %s (code: %d, request: %d, minor: %d)\n",
                error_text, error->error_code, error->request_code, error->minor_code);
        return 0; // Continue execution instead of exiting
    }

    static int x11_io_error_handler(::Display *dpy)
    {
        fprintf(stderr, "X11 IO Error: Connection to X server lost\n");
        _exit(1); // IO errors are fatal
        return 0;
    }

    static XRROutputInfo *getOutputFromIDX(::Display *dpy, XRRScreenResources *res, const RROutput &output_idx)
    {
        for (int k = 0; k < res->noutput; k++)
        {
            if (output_idx == res->outputs[k])
            {
                XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
                                                              res,
                                                              res->outputs[k]);
                // XRRFreeOutputInfo(output_info);
                return output_info;
            }
        }

        return nullptr;
    }

    static XRRModeInfo *getModeFromIDX(::Display *dpy, XRRScreenResources *res, const RRMode &mode_idx)
    {
        for (int k = 0; k < res->nmode; k++)
        {
            if (mode_idx == res->modes[k].id)
            {
                return &res->modes[k];
            }
        }
        return nullptr;
    }

    static XRRCrtcInfo *getCrtcFromIDX(::Display *dpy, XRRScreenResources *res, const RRCrtc &crtc_idx)
    {
        for (int k = 0; k < res->ncrtc; k++)
        {
            if (crtc_idx == res->crtcs[k])
            {
                XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy, res, res->crtcs[k]);
                return crtc_info;
                // XRRFreeCrtcInfo(crtc_info);
            }
        }
        return nullptr;
    }

    static float getRefreshRateFromMode(const XRRModeInfo *mode_info)
    {
        float rate = 0.0f;
        if (mode_info->hTotal && mode_info->vTotal)
            rate = ((float)mode_info->dotClock /
                    ((float)mode_info->hTotal * (float)mode_info->vTotal));

        if (mode_info->modeFlags & RR_DoubleScan)
        {
            rate *= 0.5f;
        }

        if (mode_info->modeFlags & RR_Interlace)
        {
            rate *= 2.0f;
        }
        return rate;
    }

    std::vector<Monitor> Display::QueryMonitors(bool only_default, int *monitorDefaultIndex)
    {
        if (monitorDefaultIndex)
            *monitorDefaultIndex = 0;

        std::vector<Monitor> result;

        auto dpy = XOpenDisplay(nullptr);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        auto rootWindowID = XDefaultRootWindow(dpy);

        auto screenResources = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
        ITK_ABORT(!screenResources, "XRandr screen resources not found.\n");

        int nmonitors;
        auto monitor_info_array = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);

        for (int i = 0; i < nmonitors; i++)
        {
            if (only_default && monitor_info_array[i].primary == 0)
                continue;
            auto monitor_info = &monitor_info_array[i];

            Monitor monitor;

            {
                auto atom_name = XGetAtomName(dpy, monitor_info->name);
                monitor.name = atom_name;
                XFree(atom_name);
            }

            monitor.scaleFactor = 1.0f;
            monitor.primary = monitor_info->primary != 0;

            if (monitorDefaultIndex && monitor.primary)
                *monitorDefaultIndex = (int)result.size();

            monitor.x = monitor_info->x;
            monitor.y = monitor_info->y;

            monitor.width = monitor_info->width;
            monitor.height = monitor_info->height;

            monitor.mwidth = monitor_info->mwidth;
            monitor.mheight = monitor_info->mheight;
            {
                const char *gdk_dpi_scale_str = std::getenv("GDK_DPI_SCALE");
                if (!gdk_dpi_scale_str)
                    gdk_dpi_scale_str = std::getenv("GTK_SCALE");
                if (!gdk_dpi_scale_str)
                    gdk_dpi_scale_str = std::getenv("QT_SCALE_FACTOR");
                if (!gdk_dpi_scale_str)
                    gdk_dpi_scale_str = std::getenv("GDK_SCALE");

                if (gdk_dpi_scale_str)
                {
                    double gdk_dpi_scale = atof(gdk_dpi_scale_str);

                    // gdk_dpi_scale *= 96.0 / 25.4;
                    // monitor.mwidth = (int)((double)monitor.width / gdk_dpi_scale);
                    // monitor.mheight = (int)((double)monitor.height / gdk_dpi_scale);

                    monitor.mwidth = (int)((double)monitor.mwidth / gdk_dpi_scale);
                    monitor.mheight = (int)((double)monitor.mheight / gdk_dpi_scale);
                }
            }

            monitor.current_freq_index = 0;
            monitor.current_mode_index = 0;

            for (int j = 0; j < monitor_info->noutput; j++)
            {
                auto output_idx = monitor_info->outputs[j];
                auto output_info = getOutputFromIDX(dpy, screenResources, output_idx);
                if (!output_info)
                    continue;

                monitor.port = output_info->name;

                for (int k = 0; k < output_info->nmode; k++)
                {
                    auto mode_idx = output_info->modes[k];
                    auto mode_info = getModeFromIDX(dpy, screenResources, mode_idx);
                    if (!mode_info)
                        continue;
                    if (mode_info->modeFlags & RR_Interlace)
                    {
                        continue; // skip interlace modes...
                    }

                    float rate = getRefreshRateFromMode(mode_info);

                    Mode &mode = monitor.getOrCreateMode(mode_info->width, mode_info->height);
                    mode.freqs.push_back(rate);

                    // if (k < output_info->npreferred)
                    //{
                    //     monitor.prefered_freq_index.push_back(mode.freqs.size() - 1);
                    //     monitor.prefered_mode_index.push_back(monitor.getModeIndex(mode_info->width, mode_info->height));
                    // }
                }

                // sort modes and freqs
                {
                    std::sort(monitor.modes.begin(),
                              monitor.modes.end(),
                              [](const Mode &a, const Mode &b)
                              {
                                  return b.width * b.height < a.width * a.height;
                              });

                    for (auto &mode : monitor.modes)
                    {
                        std::sort(mode.freqs.begin(),
                                  mode.freqs.end(),
                                  [](const float &a, const float &b)
                                  {
                                      return b < a;
                                  });
                    }
                }

                for (int k = 0; k < output_info->npreferred; k++)
                {
                    auto mode_idx = output_info->modes[k];
                    auto mode_info = getModeFromIDX(dpy, screenResources, mode_idx);
                    if (!mode_info)
                        continue;
                    if (mode_info->modeFlags & RR_Interlace)
                    {
                        continue; // skip interlace modes...
                    }

                    float rate = getRefreshRateFromMode(mode_info);

                    int freq_index = 0;
                    float nearest = MathCore::FloatTypeInfo<float>::max;
                    Mode &mode = monitor.getOrCreateMode(mode_info->width, mode_info->height);
                    for (int k = 0; k < (int)mode.freqs.size(); k++)
                    {
                        float dst = MathCore::OP<float>::abs(rate - mode.freqs[k]);
                        if (dst < nearest)
                        {
                            nearest = dst;
                            freq_index = k;
                        }
                    }

                    monitor.prefered_freq_index.push_back(freq_index);
                    monitor.prefered_mode_index.push_back(monitor.getModeIndex(mode_info->width, mode_info->height));
                }

                {
                    auto crtc_info = getCrtcFromIDX(dpy, screenResources, output_info->crtc);
                    ITK_ABORT(!crtc_info, "XRandr crtc not found.\n");

                    auto mode_info = getModeFromIDX(dpy, screenResources, crtc_info->mode);
                    ITK_ABORT(!mode_info, "XRandr crtc mode not found.\n");

                    float rate = getRefreshRateFromMode(mode_info);

                    Mode &mode = monitor.getOrCreateMode(mode_info->width, mode_info->height);

                    monitor.current_mode_index = monitor.getModeIndex(mode_info->width, mode_info->height);
                    monitor.current_freq_index = 0;

                    float nearest = MathCore::FloatTypeInfo<float>::max;

                    for (int k = 0; k < (int)mode.freqs.size(); k++)
                    {
                        float dst = MathCore::OP<float>::abs(rate - mode.freqs[k]);
                        if (dst < nearest)
                        {
                            nearest = dst;
                            monitor.current_freq_index = k;
                        }
                    }

                    XRRFreeCrtcInfo(crtc_info);
                }

                XRRFreeOutputInfo(output_info);
            }

            // compute scale factor
            // monitor.scaleFactor = (float) monitor.height / (float) monitor.getCurrentMode().height;

            result.push_back(monitor);
        }

        XRRFreeMonitors(monitor_info_array);

        XRRFreeScreenResources(screenResources);

        // compute scale factors using xlib
        {
            XWindowAttributes xwa;
            for (auto &monitor : result)
            {
                // search display / screen match
                for (int dpyScreen = 0; dpyScreen < ScreenCount(dpy); dpyScreen++)
                {
                    Screen *screen = ScreenOfDisplay(dpy, dpyScreen);
                    Window rootWindow = RootWindow(dpy, dpyScreen);
                    if (!XGetWindowAttributes(dpy, rootWindow, &xwa))
                        continue;
                    if (monitor.x != xwa.x || monitor.y != xwa.y)
                        continue;

                    double scale_x = (double)screen->width / (double)screen->mwidth * 25.4;
                    double scale_y = (double)screen->height / (double)screen->mheight * 25.4;
                    double scale = (scale_x + scale_y) * 0.5;
                    scale = MathCore::OP<double>::round(scale);
                    scale = scale / 96.0;

                    monitor.scaleFactor = scale;

                    break;
                }
            }
        }

        XCloseDisplay(dpy);
        return result;
    }

    static void grad_window_focus(::Display *dpy, const NativeWindowHandleType &nativeWindow)
    {
        Atom netActiveWindow = XInternAtom(dpy, "_NET_WM_BYPASS_COMPOSITOR", False);

        // Only try to grab focus if the window is mapped
        XWindowAttributes attr;
        XGetWindowAttributes(dpy, nativeWindow, &attr);

        if (attr.map_state == IsUnmapped)
            return;

        if (netActiveWindow)
        {

            ::Time time = 0;

            const Atom netWmUserTime = XInternAtom(dpy, "_NET_WM_USER_TIME", False);
            if (netWmUserTime)
            {
                Atom real_type;
                int real_format;
                unsigned long items_read, items_left;
                int *data = nullptr;
                if (XGetWindowProperty(dpy, nativeWindow,
                                       netWmUserTime,
                                       0L, 2L, False,
                                       XA_CARDINAL,
                                       &real_type,
                                       &real_format,
                                       &items_read,
                                       &items_left,
                                       (unsigned char **)&data) == Success &&
                    items_read)
                    time = *data;

                if (data)
                    XFree(data);
            }

            auto event = XEvent();
            event.type = ClientMessage;
            event.xclient.window = nativeWindow;
            event.xclient.format = 32;
            event.xclient.message_type = netActiveWindow;
            event.xclient.data.l[0] = 1; // Normal application
            event.xclient.data.l[1] = static_cast<long>(time);
            event.xclient.data.l[2] = 0; // We don't know the currently active window

            const int result = XSendEvent(dpy,
                                          DefaultRootWindow(dpy),
                                          False,
                                          SubstructureNotifyMask | SubstructureRedirectMask,
                                          &event);

            XFlush(dpy);
        }
        else
        {
            XRaiseWindow(dpy, nativeWindow);
            XSetInputFocus(dpy, nativeWindow, RevertToPointerRoot, CurrentTime);
            XFlush(dpy);
        }
    }

    static void update_window_hints(::Display *dpy, const NativeWindowHandleType &nativeWindow, const MathCore::vec2i &monitorPosition, const MathCore::vec2i &monitorSize)
    {
        XSizeHints sizeHints{};
        long flags = 0;
        XGetWMNormalHints(dpy, nativeWindow, &sizeHints, &flags);
        sizeHints.flags &= ~(PMinSize | PMaxSize);

        sizeHints.flags |= USPosition;

        // sizeHints.flags |= PMinSize | PMaxSize | USPosition;

        sizeHints.min_width = sizeHints.max_width = monitorSize.width;
        sizeHints.min_height = sizeHints.max_height = monitorSize.height;

        sizeHints.x = monitorPosition.x;
        sizeHints.y = monitorPosition.y;

        XSetWMNormalHints(dpy, nativeWindow, &sizeHints);

        // {
        //     XSizeHints sizeHints{};
        //     sizeHints.flags     = PMinSize | PMaxSize | USPosition;
        //     sizeHints.min_width = sizeHints.max_width = static_cast<int>(width);
        //     sizeHints.min_height = sizeHints.max_height = static_cast<int>(height);
        //     sizeHints.x                                 = windowPosition.x;
        //     sizeHints.y                                 = windowPosition.y;
        //     XSetWMNormalHints(m_display.get(), m_window, &sizeHints);
        // }
    }

    static void change_compositor_fullscreen(::Display *dpy, const NativeWindowHandleType &nativeWindow)
    {
        const Atom netWmBypassCompositor = XInternAtom(dpy, "_NET_WM_BYPASS_COMPOSITOR", False);
        if (netWmBypassCompositor)
        {
            constexpr unsigned long bypassCompositor = 1;

            XChangeProperty(dpy,
                            nativeWindow,
                            netWmBypassCompositor,
                            XA_CARDINAL,
                            32,
                            PropModeReplace,
                            reinterpret_cast<const unsigned char *>(&bypassCompositor),
                            1);
        }
    }

    static void change_window_fullscreen(::Display *dpy, const NativeWindowHandleType &nativeWindow)
    {
        Atom wm_state = XInternAtom(dpy, "_NET_WM_STATE", True);
        Atom wm_fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", True);
        XChangeProperty(dpy, nativeWindow, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_fullscreen, 1);
    }

    static void send_change_fullscreen(::Display *dpy, const NativeWindowHandleType &nativeWindow)
    {

        const int _NET_WM_STATE_REMOVE = 0; /* remove/unset property */
        const int _NET_WM_STATE_ADD = 1;    /* add/set property */
        const int _NET_WM_STATE_TOGGLE = 2; /* toggle property  */

        auto event = XEvent();
        event.type = ClientMessage;
        event.xclient.window = nativeWindow;
        event.xclient.format = 32;
        event.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
        event.xclient.data.l[0] = _NET_WM_STATE_ADD;
        event.xclient.data.l[1] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
        event.xclient.data.l[2] = 0; // No second property
        event.xclient.data.l[3] = 1; // Normal window
        event.xclient.data.l[4] = 0; // unused

        const int result = XSendEvent(dpy,
                                      DefaultRootWindow(dpy),
                                      False,
                                      SubstructureNotifyMask | SubstructureRedirectMask,
                                      // ClientMessage,
                                      &event);
    }

    void Display::setFullscreenAttribute(const NativeWindowHandleType &nativeWindow, const Monitor *monitor)
    {

        auto dpy = XOpenDisplay(nullptr);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        // {

        //     // XWindowAttributes wa;
        //     // XGetWindowAttributes(dpy, nativeWindow, &wa);
        //     // constexpr unsigned long eventMask = FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask |
        //     //                         PointerMotionMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask |
        //     //                         EnterWindowMask | LeaveWindowMask | VisibilityChangeMask | PropertyChangeMask;

        //     XSetWindowAttributes attributes;
        //     //attributes.colormap   = wa.colormap;
        //     //attributes.event_mask = eventMask;//wa.your_event_mask;
        //     attributes.override_redirect = false;//wa.override_redirect;

        //     XChangeWindowAttributes(
        //         dpy, nativeWindow,
        //         CWOverrideRedirect, //, CWEventMask | CWOverrideRedirect | CWColormap,
        //         &attributes);

        //     XFlush(dpy);
        // }

        auto pos = monitor->Position();
        auto res = monitor->SizePixels();

        update_window_hints(dpy, nativeWindow, pos, res);
        XFlush(dpy);

        // grad_window_focus(dpy, nativeWindow);
        // change_window_fullscreen(dpy, nativeWindow);
        // send_change_fullscreen(dpy, nativeWindow);

        XMoveWindow(dpy, nativeWindow, pos.x, pos.y);
        XFlush(dpy);
        // change_compositor_fullscreen(dpy,nativeWindow);
        send_change_fullscreen(dpy, nativeWindow);
        XFlush(dpy);

        XCloseDisplay(dpy);
    }

    // int Display::MonitorCount()
    // {
    //     ::Display *pdsp = nullptr;

    //     pdsp = XOpenDisplay(nullptr);
    //     ITK_ABORT(!pdsp, "Failed to open default display.\n");

    //     int result = ScreenCount(pdsp);

    //     XCloseDisplay(pdsp);
    //     return result;
    // }

    // int Display::MonitorDefault()
    // {
    //     ::Display *pdsp = nullptr;

    //     pdsp = XOpenDisplay(nullptr);
    //     ITK_ABORT(!pdsp, "Failed to open default display.\n");

    //     int result = DefaultScreen(pdsp);

    //     XCloseDisplay(pdsp);
    //     return result;
    // }

    // MathCore::vec2i Display::MonitorPositionPixels(int monitor_num)
    // {
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //     // ::Display *pdsp = nullptr;
    //     // ::Window pRootWindow;
    //     // ::XWindowAttributes xwa;

    //     // pdsp = XOpenDisplay(nullptr);
    //     // ITK_ABORT(!pdsp, "Failed to open default display.\n");

    //     // pRootWindow = RootWindow(pdsp, monitor_num);

    //     // XGetWindowAttributes(pdsp, pRootWindow, &xwa);

    //     // MathCore::vec2i result(xwa.x, xwa.y);

    //     // XCloseDisplay(pdsp);

    //     auto dpy = XOpenDisplay(nullptr);
    //     ITK_ABORT(!dpy, "Failed to open default display.\n");

    //     auto rootWindowID = RootWindow(dpy, monitor_num);

    //     MathCore::vec2i result;

    //     // using get monitors
    //     int nmonitors;
    //     auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
    //     ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
    //     result = MathCore::vec2i(monitor_info[0].x, monitor_info[0].y);
    //     XRRFreeMonitors(monitor_info);

    //     XCloseDisplay(dpy);

    //     return result;

    //     return result;
    // }

    // MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    // {
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //     // ::Display *pdsp = nullptr;
    //     // ::Screen *pscr = nullptr;

    //     // pdsp = XOpenDisplay(nullptr);
    //     // ITK_ABORT(!pdsp, "Failed to open default display.\n");

    //     // pscr = ScreenOfDisplay(pdsp, monitor_num);
    //     // ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");

    //     // MathCore::vec2i result(pscr->width, pscr->height);

    //     // XCloseDisplay(pdsp);

    //     auto dpy = XOpenDisplay(nullptr);
    //     ITK_ABORT(!dpy, "Failed to open default display.\n");

    //     auto rootWindowID = RootWindow(dpy, monitor_num);

    //     MathCore::vec2i result;

    //     // using get screen resources
    //     // auto screen_info = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
    //     // ITK_ABORT(!screen_info, "XRandr screen info not found.\n");
    //     // for (int i = 0; i < screen_info->noutput; i++)
    //     // {
    //     //     bool connected = false;
    //     //     XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
    //     //                                                   screen_info,
    //     //                                                   screen_info->outputs[i]);

    //     //     connected = (output_info->connection == RR_Connected);
    //     //     if (connected){

    //     //         for(int j=0;j<screen_info->nmode;j++){
    //     //             XRRModeInfo *mode_info = &screen_info->modes[j];
    //     //             mode_info->

    //     //         }

    //     //         //RRMode currentMode = output_info->modes[output_info->nmode];

    //     //         result = MathCore::vec2i(output_info->mm_width, output_info->mm_height);
    //     //     }
    //     //     XRRFreeOutputInfo(output_info);
    //     //     if (connected)
    //     //         break;
    //     // }
    //     // XRRFreeScreenResources(screen_info);

    //     // using get monitors
    //     int nmonitors;
    //     auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
    //     ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
    //     result = MathCore::vec2i(monitor_info[0].width, monitor_info[0].height);
    //     XRRFreeMonitors(monitor_info);

    //     XCloseDisplay(dpy);

    //     return result;
    // }

    // MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    // {
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //     auto dpy = XOpenDisplay(nullptr);
    //     ITK_ABORT(!dpy, "Failed to open default display.\n");

    //     // auto pscr = ScreenOfDisplay(dpy, monitor_num);
    //     // ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");
    //     // MathCore::vec2f result(pscr->mwidth, pscr->mheight);

    //     auto rootWindowID = RootWindow(dpy, monitor_num);

    //     // auto screenConfiguration = XRRGetScreenInfo (dpy, rootWindowID);
    //     // ITK_ABORT(!screenConfiguration, "Failed to obtain the default screen of given display.\n");

    //     // Rotation original_rotation;
    //     // auto current_size_index = XRRConfigCurrentConfiguration (screenConfiguration, &original_rotation);
    //     // int nsize;
    //     // auto sizes = XRRConfigSizes(screenConfiguration, &nsize);

    //     // MathCore::vec2f result =
    //     //     MathCore::vec2f(sizes[current_size_index].mwidth, sizes[current_size_index].mheight) ;

    //     // using get monitors
    //     // int nmonitors;
    //     // auto monitor_info = XRRGetMonitors( dpy, rootWindowID, true, &nmonitors );
    //     // MathCore::vec2f result =
    //     //     MathCore::vec2f(monitor_info[0].mwidth, monitor_info[0].mheight) ;

    //     // for (i = 0; i < nsize; i++) {
    //     //     printf ("%c%-2d %5d x %-5d  (%4dmm x%4dmm )",
    //     //         i == current_size ? '*' : ' ',
    //     //         i, sizes[i].width, sizes[i].height,
    //     //         sizes[i].mwidth, sizes[i].mheight);
    //     //     // ...
    //     // }

    //     MathCore::vec2f result;

    //     int nmonitors;
    //     auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
    //     ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
    //     result = MathCore::vec2i(monitor_info[0].mwidth, monitor_info[0].mheight);

    //     // // using get screen resources
    //     // auto screen_info = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
    //     // ITK_ABORT(!screen_info, "XRandr screen info not found.\n");
    //     // printf("%i\n", screen_info->noutput);
    //     // for (int i = 0; i < screen_info->noutput; i++)
    //     // {
    //     //     XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
    //     //                                                   screen_info,
    //     //                                                   screen_info->outputs[i]);

    //     //     bool connected = (output_info->connection == RR_Connected);
    //     //     if (connected)
    //     //     {
    //     //         result = MathCore::vec2f(output_info->mm_width, output_info->mm_height);
    //     //     }
    //     //     XRRFreeOutputInfo(output_info);
    //     //     if (connected)
    //     //         break;
    //     // }
    //     // XRRFreeScreenResources(screen_info);

    //     XRRFreeMonitors(monitor_info);

    //     XCloseDisplay(dpy);

    //     return result;
    // }

    // MathCore::vec2f Display::MonitorRealSizeInches(int monitor_num)
    // {
    //     return (MathCore::vec2f)MonitorRealSizeMillimeters(monitor_num) / 25.4f;
    // }

    // MathCore::vec2f Display::MonitorDPIf(int monitor_num)
    // {
    //     return ComputeDPIf(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    // }

    // MathCore::vec2i Display::MonitorDPIi(int monitor_num)
    // {
    //     return ComputeDPIi(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    // }

    MathCore::vec2f Display::ComputeDPIf(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2f)sizePixels / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2i)(ComputeDPIf(sizePixels, realSizeInches) + 0.5f);
    }

    void Display::setMonitorResolution(const std::string &monitor_port, int width, int height, float freq)
    {
#if USE_XRANDR_CMDLINE_HELPER == 0
        ::Display *dpy = XOpenDisplay(nullptr);
        ITK_ABORT(!dpy, "Failed to open default display.\n");
        // ITK_ABORT(!XGrabServer(dpy), "Failed to grab server.\n");

        EventCore::ExecuteOnScopeEnd _release_dpy([dpy]()
                                                  {
            // XUngrabServer(dpy);
            XCloseDisplay(dpy); });

        int screen = DefaultScreen(dpy);
        Window root = RootWindow(dpy, screen);

        int event_base, error_base;
        int major, minor;
        ITK_ABORT(!XRRQueryExtension(dpy, &event_base, &error_base) ||
                      !XRRQueryVersion(dpy, &major, &minor),
                  "Failed to query XRandR extension.\n");

        // version 1.2 or higher is required for setting screen size
        if (major > 1 || (major == 1 && minor >= 2))
        {

            static int minWidth, maxWidth, minHeight, maxHeight;
            XRRGetScreenSizeRange(dpy, root, &minWidth, &minHeight,
                                  &maxWidth, &maxHeight);

            // XRRScreenResources *res = XRRGetScreenResources(dpy, root);
            XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);
            ITK_ABORT(!res, "Failed to get XRandR screen resources.\n");
            EventCore::ExecuteOnScopeEnd _release_screen_resources([dpy, res]()
                                                                   { XRRFreeScreenResources(res); });

            for (int i = 0; i < res->noutput; i++)
            {
                XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
                                                              res,
                                                              res->outputs[i]);
                if (!output_info)
                    continue;
                EventCore::ExecuteOnScopeEnd _release_output_info([output_info]()
                                                                  { XRRFreeOutputInfo(output_info); });

                if (output_info->connection == RR_Disconnected)
                    continue;

                if (monitor_port != output_info->name)
                    continue;

                RRCrtc crtc = output_info->crtc;
                XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy, res, crtc);
                if (!crtc_info)
                    continue;

                EventCore::ExecuteOnScopeEnd _release_crtc_info([crtc_info]()
                                                                { XRRFreeCrtcInfo(crtc_info); });

                if (crtc_info->mode == None)
                    continue;

                RRMode new_mode = 0;
                float freq_diff_dst = FLT_MAX;
                // search for mode
                for (int m = 0; m < output_info->nmode; m++)
                {
                    RRMode mode_idx = output_info->modes[m];
                    XRRModeInfo *mode_info = getModeFromIDX(dpy, res, mode_idx);
                    if (!mode_info)
                        continue;
                    if (mode_info->width == width &&
                        mode_info->height == height)
                    {
                        float mode_freq = getRefreshRateFromMode(mode_info);
                        float freq_diff = MathCore::OP<float>::abs(mode_freq - freq);
                        if (freq_diff < freq_diff_dst)
                        {
                            new_mode = mode_idx;
                            freq_diff_dst = freq_diff;
                        }
                    }
                }

                ITK_ABORT(new_mode == 0, "Requested mode not found on given monitor.\n");

                if (new_mode == crtc_info->mode)
                {
                    printf("Requested mode is already set on given monitor.\n");
                    return;
                }

                struct _transform
                {
                    XTransform transform;
                    std::string filter;
                    int nparams;
                    std::vector<XFixed> params;
                };

                _transform currTransform;
                // get current transform
                {
                    memset(&currTransform.transform, 0, sizeof(currTransform.transform));
                    for (int x = 0; x < 3; x++)
                        currTransform.transform.matrix[x][x] = XDoubleToFixed(1.0);
                    currTransform.filter = "";
                    currTransform.nparams = 0;
                    currTransform.params = {};

                    XRRCrtcTransformAttributes *attr = nullptr;
                    if (XRRGetCrtcTransform(dpy, crtc, &attr) && attr)
                    {
                        currTransform.transform = attr->currentTransform;
                        currTransform.filter = attr->currentFilter ? attr->currentFilter : "";
                        currTransform.nparams = attr->currentNparams;
                        for (int j = 0; j < attr->currentNparams; j++)
                            currTransform.params.push_back(attr->currentParams[j]);
                        XFree(attr);
                    }
                }

                // XRRPanning *panning_info = nullptr;

                // if (major > 1 || (major == 1 && minor >= 3))
                // {
                //     XRRPanning zero;
                //     memset(&zero, 0, sizeof(zero));
                //     panning_info = XRRGetPanning(dpy, res, crtc);
                //     zero.timestamp = panning_info->timestamp;
                //     if (!memcmp(panning_info, &zero, sizeof(zero)))
                //     {
                //         XRRFreePanning(panning_info);
                //         panning_info = nullptr;
                //     }
                // }

                // disable crtc before changing mode
                XRRSetCrtcConfig(dpy, res, crtc, CurrentTime,
                                 0, 0, None, RR_Rotate_0, NULL, 0);

                int x, y, w, h;
                // transform and compute box values based on the new resolution...
                {

                    XRRModeInfo *new_mode_info = getModeFromIDX(dpy, res, new_mode);

                    int new_mode_screen_width, new_mode_screen_height;

                    switch (crtc_info->rotation & 0xf)
                    {
                    case RR_Rotate_0:
                    case RR_Rotate_180:
                        new_mode_screen_width = new_mode_info->width;
                        new_mode_screen_height = new_mode_info->height;
                        break;
                    case RR_Rotate_90:
                    case RR_Rotate_270:
                        new_mode_screen_width = new_mode_info->height;
                        new_mode_screen_height = new_mode_info->width;
                        break;
                    default:
                        new_mode_screen_width = 0;
                        new_mode_screen_height = 0;
                    }

                    MathCore::vec3f points[4] = {
                        MathCore::vec3f(0, 0, 1),
                        MathCore::vec3f(new_mode_screen_width, 0, 1),
                        MathCore::vec3f(0, new_mode_screen_height, 1),
                        MathCore::vec3f(new_mode_screen_width, new_mode_screen_height, 1)};

                    MathCore::mat3f transformMat;
                    for (int x = 0; x < 3; x++)
                        for (int y = 0; y < 3; y++)
                            transformMat[x][y] = XFixedToDouble(currTransform.transform.matrix[x][y]);

                    MathCore::vec3f box_min, box_max;
                    for (int p = 0; p < 4; p++)
                    {
                        MathCore::vec3f tp = transformMat * points[p];
                        MathCore::vec3f tp_floor = MathCore::OP<MathCore::vec3f>::floor(tp);
                        MathCore::vec3f tp_ceil = MathCore::OP<MathCore::vec3f>::ceil(tp);
                        if (p == 0)
                        {
                            box_min = tp_floor;
                            box_max = tp_ceil;
                        }
                        else
                        {
                            box_min = MathCore::OP<MathCore::vec3f>::minimum(box_min, tp_floor);
                            box_max = MathCore::OP<MathCore::vec3f>::maximum(box_max, tp_ceil);
                        }
                    }

                    MathCore::vec3i box_min_i = (MathCore::vec3i)(box_min + 0.5f);
                    MathCore::vec3i box_max_i = (MathCore::vec3i)(box_max + 0.5f);

                    x = crtc_info->x + box_min_i.x;
                    y = crtc_info->y + box_min_i.y;
                    w = box_max_i.x - box_min_i.x;
                    h = box_max_i.y - box_min_i.y;
                }

                int fb_width = MathCore::OP<int>::clamp(x + w, minWidth, maxWidth);
                int fb_height = MathCore::OP<int>::clamp(y + h, minHeight, maxHeight);
                int fb_width_mm = DisplayWidthMM(dpy, screen);
                int fb_height_mm = DisplayHeightMM(dpy, screen);

                if (fb_width != DisplayWidth(dpy, screen) ||
                    fb_height != DisplayHeight(dpy, screen) ||
                    fb_width_mm != DisplayWidthMM(dpy, screen) ||
                    fb_height_mm != DisplayHeightMM(dpy, screen))
                    XRRSetScreenSize(dpy, root, fb_width, fb_height,
                                     fb_width_mm, fb_height_mm);

                // set new mode
                std::vector<RROutput> outputs;
                for (int o = 0; o < crtc_info->noutput; o++)
                    outputs.push_back(crtc_info->outputs[o]);

                int set_result = XRRSetCrtcConfig(dpy, res, crtc, CurrentTime,
                                                  crtc_info->x,
                                                  crtc_info->y,
                                                  new_mode,
                                                  crtc_info->rotation,
                                                  outputs.data(),
                                                  (int)outputs.size());
                // need fallback in case of the monitor is with a black screen
                if (set_result != Success)
                {
                    XRRSetCrtcConfig(dpy, res, crtc, CurrentTime,
                                     crtc_info->x, crtc_info->y,
                                     crtc_info->mode, crtc_info->rotation,
                                     crtc_info->outputs, crtc_info->noutput);
                    XSync(dpy, False);
                }
                ITK_ABORT(set_result != Success, "Failed to set new mode on given monitor.\n");

                XSync(dpy, False);
                break;
            }
        }

#endif

#if USE_XRANDR_CMDLINE_HELPER == 1

        // char command[512];
        // snprintf(command, sizeof(command),
        //          "xrandr --output %s --mode %dx%d --rate %.2f 2>/dev/null",
        //          monitor_port.c_str(), width, height, freq);
        // int system_result = system(command);
        // if (system_result == 0)
        // {
        //     printf("Successfully changed resolution using xrandr command after reset\n");
        // }

        {
            auto size_str = ITKCommon::PrintfToStdString("%dx%d", width, height);
            auto freq_str = ITKCommon::PrintfToStdString("%.2f", freq);

            char *params[] = {
                (char *)"xrandr",
                (char *)"--output",
                (char *)monitor_port.c_str(),
                (char *)"--mode",
                (char *)size_str.c_str(),
                (char *)"--rate",
                (char *)freq_str.c_str(),
                (char *)"--nograb",
                nullptr};

            int system_result = main_xrandr(sizeof(params) / sizeof(params[0]) - 1, params);
            if (system_result == 0)
            {
                printf("Successfully changed resolution using xrandr command after reset\n");
            }
        }

#endif

        printf("Resolution change operation completed.\n");
    }

    // void Display::setMonitorFractionalScaling(const std::string &monitor_port, float scaleFactor)
    // {
    //     // is not possible to set fractional scaling with xrandr
    //     // needs wayland integration
    // }
}

#elif defined(_WIN32)

#include <shellscalingapi.h>

namespace DPI
{
    // struct _MonitorInfo
    //{
    //     // HMONITOR hMonitor;
    //     // HDC hdcMonitor;
    //     // MONITORINFOEX monitorInfoEx;
    //     // uint32_t dpi_x;
    //     // uint32_t dpi_y;
    //     // DEVICE_SCALE_FACTOR deviceScaleFactor;
    //     // float scaleFactor;
    //     // DEVMODE devMode;

    //    int x_pixels;
    //    int y_pixels;

    //    int width_virtual_pixels;
    //    int height_virtual_pixels;

    //    int width_pixels;
    //    int height_pixels;

    //    int width_mm;
    //    int height_mm;

    //    float scale_factor;

    //    bool is_primary;
    //};

    BOOL CALLBACK _FillMonitorVector(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
        std::vector<Monitor> &allMonitors = *(std::vector<Monitor> *)dwData;

        MONITORINFOEX miex;
        memset(&miex, 0, sizeof(MONITORINFOEX));
        miex.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &miex))
        {

            Monitor this_monitor;

            // this_monitor.hMonitor = hMonitor;
            // this_monitor.hdcMonitor = hdcMonitor;
            // this_monitor.monitorInfoEx = miex;

            HDC dc = CreateDC(("DISPLAY"), miex.szDevice, nullptr, nullptr);
            if (dc)
            {

                int width_virtual_pixels = GetDeviceCaps(dc, HORZRES);  // miex.rcMonitor.right - miex.rcMonitor.left;//GetDeviceCaps(dc, HORZRES);//
                int height_virtual_pixels = GetDeviceCaps(dc, VERTRES); // miex.rcMonitor.bottom - miex.rcMonitor.top;//GetDeviceCaps(dc, VERTRES);//

                this_monitor.mwidth = GetDeviceCaps(dc, HORZSIZE);
                this_monitor.mheight = GetDeviceCaps(dc, VERTSIZE);

                this_monitor.primary = (miex.dwFlags == MONITORINFOF_PRIMARY);

                DEVMODE devMode;
                memset(&devMode, 0, sizeof(DEVMODE));
                devMode.dmSize = sizeof(DEVMODE);
                if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
                {
                    return FALSE;
                }

                this_monitor.x = devMode.dmPosition.x;
                this_monitor.y = devMode.dmPosition.y;

                this_monitor.width = devMode.dmPelsWidth;
                this_monitor.height = devMode.dmPelsHeight;

                // compute scale factor
                {
                    this_monitor.scaleFactor =
                        ((float)this_monitor.width / (float)(width_virtual_pixels) +
                         (float)this_monitor.height / (float)(height_virtual_pixels)) *
                        0.5f;

                    this_monitor.scaleFactor = MathCore::OP<float>::round(this_monitor.scaleFactor * 100.0f) / 100.0f;

                    PROCESS_DPI_AWARENESS processDpiAwareness;
                    if (SUCCEEDED(GetProcessDpiAwareness(GetCurrentProcess(), &processDpiAwareness)))
                    {
                        if (processDpiAwareness == PROCESS_PER_MONITOR_DPI_AWARE)
                        {
                            UINT dpiX, dpiY;
                            if (SUCCEEDED(GetDpiForMonitor(hMonitor, (MONITOR_DPI_TYPE)MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
                            {
                                double scale_x = (double)dpiX;
                                double scale_y = (double)dpiY;
                                double scale = (scale_x + scale_y) * 0.5;
                                scale = MathCore::OP<double>::round(scale);
                                scale = scale / 96.0;
                                this_monitor.scaleFactor = (float)scale;
                            }
                        }
                        else if (processDpiAwareness == PROCESS_SYSTEM_DPI_AWARE)
                        {
                            int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
                            int dpiY = GetDeviceCaps(dc, LOGPIXELSY);
                            double scale_x = (double)dpiX;
                            double scale_y = (double)dpiY;
                            double scale = (scale_x + scale_y) * 0.5;
                            scale = MathCore::OP<double>::round(scale);
                            scale = scale / 96.0;
                            this_monitor.scaleFactor = (float)scale;
                        }
                    }
                }

                this_monitor.name = miex.szDevice;
                this_monitor.port = (char *)devMode.dmDeviceName;
                // printf("%f\n", this_monitor.scale_factor);

                // fill modes
                {
                    DWORD mode_idx = 0;
                    DEVMODE devMode_aux = {};
                    memset(&devMode_aux, 0, sizeof(DEVMODE));
                    devMode_aux.dmSize = sizeof(DEVMODE);
                    while (EnumDisplaySettings(miex.szDevice, mode_idx, &devMode_aux))
                    {
                        if (devMode_aux.dmBitsPerPel != devMode.dmBitsPerPel
                            //||(devMode_aux.dmDisplayFlags & DM_INTERLACED) != 0
                            || devMode_aux.dmDisplayFixedOutput != DMDFO_DEFAULT)
                        {
                            mode_idx++;
                            memset(&devMode_aux, 0, sizeof(DEVMODE));
                            devMode_aux.dmSize = sizeof(DEVMODE);
                            continue;
                        }

                        Mode &mode = this_monitor.getOrCreateMode(devMode_aux.dmPelsWidth, devMode_aux.dmPelsHeight);
                        mode.freqs.push_back((float)devMode_aux.dmDisplayFrequency);

                        mode_idx++;
                        memset(&devMode_aux, 0, sizeof(DEVMODE));
                        devMode_aux.dmSize = sizeof(DEVMODE);
                    }

                    // sort modes and freqs
                    {
                        std::sort(this_monitor.modes.begin(),
                                  this_monitor.modes.end(),
                                  [](const Mode &a, const Mode &b)
                                  {
                                      return b.width * b.height < a.width * a.height;
                                  });

                        for (auto &mode : this_monitor.modes)
                        {
                            std::sort(mode.freqs.begin(),
                                      mode.freqs.end(),
                                      [](const float &a, const float &b)
                                      {
                                          return b < a;
                                      });
                        }
                    }

                    {
                        Mode &mode = this_monitor.getOrCreateMode(devMode.dmPelsWidth, devMode.dmPelsHeight);

                        this_monitor.current_mode_index = this_monitor.getModeIndex(devMode.dmPelsWidth, devMode.dmPelsHeight);
                        this_monitor.current_freq_index = 0;

                        float nearest = MathCore::FloatTypeInfo<float>::max;

                        for (int k = 0; k < (int)mode.freqs.size(); k++)
                        {
                            float dst = MathCore::OP<float>::abs((float)devMode.dmDisplayFrequency - mode.freqs[k]);
                            if (dst < nearest)
                            {
                                nearest = dst;
                                this_monitor.current_freq_index = k;
                            }
                        }
                    }

                    /*Mode mode;
                    mode.width = devMode.dmPelsWidth;
                    mode.height = devMode.dmPelsHeight;
                    mode.freqs.push_back((float)devMode.dmDisplayFrequency);
                    this_monitor.modes.push_back(mode);


                    this_monitor.current_mode_index = 0;
                    this_monitor.current_freq_index = 0;

                    this_monitor.prefered_mode_index.push_back(0);
                    this_monitor.prefered_freq_index.push_back(0);*/
                }

                allMonitors.push_back(this_monitor);

                ReleaseDC(nullptr, dc);
            }
            else
                return FALSE;
        }

        return TRUE;

        // std::vector<_MonitorInfo> &allMonitors = *(std::vector<_MonitorInfo> *)dwData;

        // MONITORINFOEX miex;
        // memset(&miex, 0, sizeof(MONITORINFOEX));
        // miex.cbSize = sizeof(MONITORINFOEX);
        // if (GetMonitorInfo(hMonitor, &miex))
        //{

        //    _MonitorInfo this_monitor;
        //    memset(&this_monitor, 0, sizeof(_MonitorInfo));
        //    // this_monitor.hMonitor = hMonitor;
        //    // this_monitor.hdcMonitor = hdcMonitor;
        //    // this_monitor.monitorInfoEx = miex;

        //    HDC dc = CreateDC(("DISPLAY"), miex.szDevice, nullptr, nullptr);
        //    if (dc)
        //    {

        //        // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);

        //        this_monitor.width_virtual_pixels = GetDeviceCaps(dc, HORZRES);  // miex.rcMonitor.right - miex.rcMonitor.left;//GetDeviceCaps(dc, HORZRES);//
        //        this_monitor.height_virtual_pixels = GetDeviceCaps(dc, VERTRES); // miex.rcMonitor.bottom - miex.rcMonitor.top;//GetDeviceCaps(dc, VERTRES);//

        //        this_monitor.width_mm = GetDeviceCaps(dc, HORZSIZE);
        //        this_monitor.height_mm = GetDeviceCaps(dc, VERTSIZE);

        //        this_monitor.is_primary = (miex.dwFlags == MONITORINFOF_PRIMARY);

        //        DEVMODE devMode;
        //        memset(&devMode, 0, sizeof(DEVMODE));
        //        devMode.dmSize = sizeof(DEVMODE);
        //        if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
        //        {
        //            return FALSE;
        //        }

        //        this_monitor.x_pixels = devMode.dmPosition.x;
        //        this_monitor.y_pixels = devMode.dmPosition.y;

        //        this_monitor.width_pixels = devMode.dmPelsWidth;
        //        this_monitor.height_pixels = devMode.dmPelsHeight;

        //        this_monitor.scale_factor = (float)this_monitor.width_pixels / (float)(this_monitor.width_virtual_pixels);
        //        // printf("%f\n", this_monitor.scale_factor);

        //        ReleaseDC(nullptr, dc);
        //    }
        //    else
        //        return FALSE;

        //    // if (GetDpiForMonitor(hMonitor, MDT_RAW_DPI, &this_monitor.dpi_x, &this_monitor.dpi_y) != S_OK) {
        //    //     /*HDC screen = GetDC(0);
        //    //     this_monitor.dpi_x = GetDeviceCaps(screen, LOGPIXELSX);
        //    //     this_monitor.dpi_y = GetDeviceCaps(screen, LOGPIXELSY);
        //    //     ReleaseDC(0, screen);*/
        //    //     return FALSE;
        //    // }

        //    // this_monitor.devMode.dmSize = sizeof(DEVMODE);
        //    // if (EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &this_monitor.devMode) == 0) {
        //    //     return FALSE;
        //    // }

        //    // if (GetScaleFactorForMonitor(hMonitor, &this_monitor.deviceScaleFactor) != S_OK) {
        //    //     return FALSE;
        //    // }

        //    // this_monitor.scaleFactor = (float)this_monitor.devMode.dmPelsWidth / (float)(this_monitor.monitorInfoEx.rcMonitor.right - this_monitor.monitorInfoEx.rcMonitor.left);

        //    allMonitors.push_back(this_monitor);
        //}
        // return TRUE;
    }

    BOOL CALLBACK _FillMonitorVectorOnlyPrimary(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
        std::vector<Monitor> &allMonitors = *(std::vector<Monitor> *)dwData;

        MONITORINFOEX miex;
        memset(&miex, 0, sizeof(MONITORINFOEX));
        miex.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &miex))
        {
            if (miex.dwFlags != MONITORINFOF_PRIMARY || allMonitors.size() > 0)
                return TRUE;

            Monitor this_monitor;

            HDC dc = CreateDC(("DISPLAY"), miex.szDevice, nullptr, nullptr);
            if (dc)
            {

                int width_virtual_pixels = GetDeviceCaps(dc, HORZRES);  // miex.rcMonitor.right - miex.rcMonitor.left;//GetDeviceCaps(dc, HORZRES);//
                int height_virtual_pixels = GetDeviceCaps(dc, VERTRES); // miex.rcMonitor.bottom - miex.rcMonitor.top;//GetDeviceCaps(dc, VERTRES);//

                this_monitor.mwidth = GetDeviceCaps(dc, HORZSIZE);
                this_monitor.mheight = GetDeviceCaps(dc, VERTSIZE);

                this_monitor.primary = (miex.dwFlags == MONITORINFOF_PRIMARY);

                DEVMODE devMode;
                memset(&devMode, 0, sizeof(DEVMODE));
                devMode.dmSize = sizeof(DEVMODE);
                if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
                {
                    return FALSE;
                }

                this_monitor.x = devMode.dmPosition.x;
                this_monitor.y = devMode.dmPosition.y;

                this_monitor.width = devMode.dmPelsWidth;
                this_monitor.height = devMode.dmPelsHeight;

                // compute scale factor
                {
                    this_monitor.scaleFactor =
                        ((float)this_monitor.width / (float)(width_virtual_pixels) +
                         (float)this_monitor.height / (float)(height_virtual_pixels)) *
                        0.5f;

                    this_monitor.scaleFactor = MathCore::OP<float>::round(this_monitor.scaleFactor * 100.0f) / 100.0f;

                    PROCESS_DPI_AWARENESS processDpiAwareness;
                    if (SUCCEEDED(GetProcessDpiAwareness(GetCurrentProcess(), &processDpiAwareness)))
                    {
                        if (processDpiAwareness == PROCESS_PER_MONITOR_DPI_AWARE)
                        {
                            UINT dpiX, dpiY;
                            if (SUCCEEDED(GetDpiForMonitor(hMonitor, (MONITOR_DPI_TYPE)MDT_EFFECTIVE_DPI, &dpiX, &dpiY)))
                            {
                                double scale_x = (double)dpiX;
                                double scale_y = (double)dpiY;
                                double scale = (scale_x + scale_y) * 0.5;
                                scale = MathCore::OP<double>::round(scale);
                                scale = scale / 96.0;
                                this_monitor.scaleFactor = (float)scale;
                            }
                        }
                        else if (processDpiAwareness == PROCESS_SYSTEM_DPI_AWARE)
                        {
                            int dpiX = GetDeviceCaps(dc, LOGPIXELSX);
                            int dpiY = GetDeviceCaps(dc, LOGPIXELSY);
                            double scale_x = (double)dpiX;
                            double scale_y = (double)dpiY;
                            double scale = (scale_x + scale_y) * 0.5;
                            scale = MathCore::OP<double>::round(scale);
                            scale = scale / 96.0;
                            this_monitor.scaleFactor = (float)scale;
                        }
                    }
                }

                this_monitor.name = miex.szDevice;
                this_monitor.port = (char *)devMode.dmDeviceName;
                // printf("%f\n", this_monitor.scale_factor);

                // fill modes
                {
                    DWORD mode_idx = 0;
                    DEVMODE devMode_aux = {};
                    memset(&devMode_aux, 0, sizeof(DEVMODE));
                    devMode_aux.dmSize = sizeof(DEVMODE);
                    while (EnumDisplaySettings(miex.szDevice, mode_idx, &devMode_aux))
                    {
                        if (devMode_aux.dmBitsPerPel != devMode.dmBitsPerPel
                            //||(devMode_aux.dmDisplayFlags & DM_INTERLACED) != 0
                            || devMode_aux.dmDisplayFixedOutput != DMDFO_DEFAULT)
                        {
                            mode_idx++;
                            memset(&devMode_aux, 0, sizeof(DEVMODE));
                            devMode_aux.dmSize = sizeof(DEVMODE);
                            continue;
                        }

                        Mode &mode = this_monitor.getOrCreateMode(devMode_aux.dmPelsWidth, devMode_aux.dmPelsHeight);
                        mode.freqs.push_back((float)devMode_aux.dmDisplayFrequency);

                        mode_idx++;
                        memset(&devMode_aux, 0, sizeof(DEVMODE));
                        devMode_aux.dmSize = sizeof(DEVMODE);
                    }

                    // sort modes and freqs
                    {
                        std::sort(this_monitor.modes.begin(),
                                  this_monitor.modes.end(),
                                  [](const Mode &a, const Mode &b)
                                  {
                                      return b.width * b.height < a.width * a.height;
                                  });

                        for (auto &mode : this_monitor.modes)
                        {
                            std::sort(mode.freqs.begin(),
                                      mode.freqs.end(),
                                      [](const float &a, const float &b)
                                      {
                                          return b < a;
                                      });
                        }
                    }

                    {
                        Mode &mode = this_monitor.getOrCreateMode(devMode.dmPelsWidth, devMode.dmPelsHeight);

                        this_monitor.current_mode_index = this_monitor.getModeIndex(devMode.dmPelsWidth, devMode.dmPelsHeight);
                        this_monitor.current_freq_index = 0;

                        float nearest = MathCore::FloatTypeInfo<float>::max;

                        for (int k = 0; k < (int)mode.freqs.size(); k++)
                        {
                            float dst = MathCore::OP<float>::abs((float)devMode.dmDisplayFrequency - mode.freqs[k]);
                            if (dst < nearest)
                            {
                                nearest = dst;
                                this_monitor.current_freq_index = k;
                            }
                        }
                    }
                }

                allMonitors.push_back(this_monitor);

                ReleaseDC(nullptr, dc);
            }
            else
                return FALSE;
        }

        return TRUE;
    }

    std::vector<Monitor> Display::QueryMonitors(bool only_default, int *monitorDefaultIndex)
    {
        // GetProcessDpiAwareness()
        // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
        // SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        // SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

        std::vector<Monitor> allMonitors;

        if (only_default)
            EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVectorOnlyPrimary, (LPARAM)&allMonitors);
        else
            EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);

        if (monitorDefaultIndex != nullptr)
            *monitorDefaultIndex = 0;
        for (int i = 0; i < (int)allMonitors.size(); i++)
        {
            if (allMonitors[i].primary)
            {
                if (monitorDefaultIndex != nullptr)
                    *monitorDefaultIndex = i;
                break;
            }
        }

        return allMonitors;
    }

    void Display::setFullscreenAttribute(const NativeWindowHandleType &nativeWindow, const Monitor *monitor)
    {

        // make borderless window occupy the entire monitor area

        DWORD style = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtr(nativeWindow, GWL_STYLE, style);
        SetWindowPos(nativeWindow, HWND_NOTOPMOST, monitor->x, monitor->y, monitor->width, monitor->height, SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_NOZORDER);

        //SetWindowPos(nativeWindow, HWND_NOTOPMOST, monitor->x, monitor->y, monitor->width, monitor->height, 0);//SWP_NOZORDER | SWP_SHOWWINDOW
        //SetWindowPos(nativeWindow, HWND_TOP, monitor->x, monitor->y, monitor->width, monitor->height, SWP_NOZORDER);

        // if (m_cursorGrabbed)
        // grabCursor(true);
    }

    // int Display::MonitorCount()
    //{
    //     std::vector<_MonitorInfo> allMonitors;
    //     EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);
    //     return (int)allMonitors.size();
    // }

    // int Display::MonitorDefault()
    //{
    //     std::vector<_MonitorInfo> allMonitors;
    //     EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);

    //    for (size_t i = 0; i < allMonitors.size(); i++)
    //    {
    //        if (allMonitors[i].is_primary)
    //            return (int)i;
    //    }

    //    return 0;
    //}

    // MathCore::vec2i Display::MonitorPositionPixels(int monitor_num)
    //{
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //    std::vector<_MonitorInfo> allMonitors;
    //    EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);

    //    _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

    //    MathCore::vec2i result = MathCore::vec2i(
    //        selectedMonitor.x_pixels,
    //        selectedMonitor.y_pixels);

    //    return result;
    //}

    // MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    //{
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //    std::vector<_MonitorInfo> allMonitors;
    //    EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);

    //    _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

    //    MathCore::vec2i result = MathCore::vec2i(
    //        selectedMonitor.width_pixels,
    //        selectedMonitor.height_pixels);

    //    return result;
    //}

    // MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    //{
    //     if (monitor_num == -1)
    //         monitor_num = MonitorDefault();

    //    std::vector<_MonitorInfo> allMonitors;
    //    EnumDisplayMonitors(nullptr, nullptr, _FillMonitorVector, (LPARAM)&allMonitors);

    //    _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

    //    MathCore::vec2f result = MathCore::vec2f(
    //        selectedMonitor.width_mm,
    //        selectedMonitor.height_mm);

    //    return result;
    //}

    // MathCore::vec2f Display::MonitorRealSizeInches(int monitor_num)
    //{
    //     return MonitorRealSizeMillimeters(monitor_num) / 25.4f;
    // }

    // MathCore::vec2f Display::MonitorDPIf(int monitor_num)
    //{
    //     return ComputeDPIf(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    // }

    // MathCore::vec2i Display::MonitorDPIi(int monitor_num)
    //{
    //     return ComputeDPIi(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    // }

    MathCore::vec2f Display::ComputeDPIf(const MathCore::vec2i &resolution, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2f)resolution / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi(const MathCore::vec2i &resolution, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2i)(ComputeDPIf(resolution, realSizeInches) + 0.5f);
    }
}

#elif defined(__APPLE__)

#endif