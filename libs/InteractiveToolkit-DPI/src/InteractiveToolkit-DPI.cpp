#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#if defined(__linux__)

// https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_Changing_the_Screen_Resolution

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

namespace DPI
{

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

        return NULL;
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
        return NULL;
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
        return NULL;
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

    std::vector<Monitor> Display::QueryMonitors(int *monitorDefaultIndex)
    {
        *monitorDefaultIndex = 0;

        std::vector<Monitor> result;

        auto dpy = XOpenDisplay(NULL);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        auto rootWindowID = XDefaultRootWindow(dpy);

        auto screenResources = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
        ITK_ABORT(!screenResources, "XRandr screen resources not found.\n");

        int nmonitors;
        auto monitor_info_array = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);

        for (int i = 0; i < nmonitors; i++)
        {
            auto monitor_info = &monitor_info_array[i];

            Monitor monitor;

            monitor.name = XGetAtomName(dpy, monitor_info->name);

            monitor.scaleFactor = 1.0f;
            monitor.primary = monitor_info->primary != 0;

            if (monitor.primary)
                *monitorDefaultIndex = i;

            monitor.x = monitor_info->x;
            monitor.y = monitor_info->y;

            monitor.width = monitor_info->width;
            monitor.height = monitor_info->height;

            monitor.mwidth = monitor_info->mwidth;
            monitor.mheight = monitor_info->mheight;

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

                    if (k < output_info->npreferred)
                    {
                        monitor.prefered_freq_index.push_back(mode.freqs.size() - 1);
                        monitor.prefered_mode_index.push_back(monitor.getModeIndex(mode_info->width, mode_info->height));
                    }
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

        XCloseDisplay(dpy);
        return result;
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

        const Atom netWmState = XInternAtom(dpy, "_NET_WM_STATE", true);
        const Atom netWmStateFullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", true);

        auto event = XEvent();
        event.type = ClientMessage;
        event.xclient.window = nativeWindow;
        event.xclient.format = 32;
        event.xclient.message_type = netWmState;
        event.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
        event.xclient.data.l[1] = static_cast<long>(netWmStateFullscreen);
        event.xclient.data.l[2] = 0; // No second property
        event.xclient.data.l[3] = 1; // Normal window

        const int result = XSendEvent(dpy,
                                      DefaultRootWindow(dpy),
                                      False,
                                      SubstructureNotifyMask | SubstructureRedirectMask,
                                      &event);

        // XEvent x_event;
        // Atom wm_fullscreen;

        // x_event.type = ClientMessage;
        // x_event.xclient.window = nativeWindow;
        // x_event.xclient.message_type = XInternAtom(dpy, "_NET_WM_STATE", False);
        // x_event.xclient.format = 32;
        // x_event.xclient.data.l[0] = i_state;
        // wm_fullscreen = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
        // x_event.xclient.data.l[1] = wm_fullscreen;
        // x_event.xclient.data.l[2] = 0;

        // XSendEvent(dpy,
        // //RootWindow(dpy, DefaultScreen(dpy)),
        // DefaultRootWindow(dpy),
        // False, ClientMessage, &x_event);
    }

    void Display::setFullscreenAttribute(const NativeWindowHandleType &nativeWindow)
    {
        auto dpy = XOpenDisplay(NULL);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        XMapWindow(dpy, nativeWindow);

        change_window_fullscreen(dpy, nativeWindow);
        change_compositor_fullscreen(dpy, nativeWindow);
        send_change_fullscreen(dpy, nativeWindow);

        XFlush(dpy);

        XCloseDisplay(dpy);
    }

    int Display::MonitorCount()
    {
        ::Display *pdsp = NULL;

        pdsp = XOpenDisplay(NULL);
        ITK_ABORT(!pdsp, "Failed to open default display.\n");

        int result = ScreenCount(pdsp);

        XCloseDisplay(pdsp);
        return result;
    }

    int Display::MonitorDefault()
    {
        ::Display *pdsp = NULL;

        pdsp = XOpenDisplay(NULL);
        ITK_ABORT(!pdsp, "Failed to open default display.\n");

        int result = DefaultScreen(pdsp);

        XCloseDisplay(pdsp);
        return result;
    }

    MathCore::vec2i Display::MonitorPositionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        // ::Display *pdsp = NULL;
        // ::Window pRootWindow;
        // ::XWindowAttributes xwa;

        // pdsp = XOpenDisplay(NULL);
        // ITK_ABORT(!pdsp, "Failed to open default display.\n");

        // pRootWindow = RootWindow(pdsp, monitor_num);

        // XGetWindowAttributes(pdsp, pRootWindow, &xwa);

        // MathCore::vec2i result(xwa.x, xwa.y);

        // XCloseDisplay(pdsp);

        auto dpy = XOpenDisplay(NULL);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        auto rootWindowID = RootWindow(dpy, monitor_num);

        MathCore::vec2i result;

        // using get monitors
        int nmonitors;
        auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
        ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
        result = MathCore::vec2i(monitor_info[0].x, monitor_info[0].y);
        XRRFreeMonitors(monitor_info);

        XCloseDisplay(dpy);

        return result;

        return result;
    }

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        // ::Display *pdsp = NULL;
        // ::Screen *pscr = NULL;

        // pdsp = XOpenDisplay(NULL);
        // ITK_ABORT(!pdsp, "Failed to open default display.\n");

        // pscr = ScreenOfDisplay(pdsp, monitor_num);
        // ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");

        // MathCore::vec2i result(pscr->width, pscr->height);

        // XCloseDisplay(pdsp);

        auto dpy = XOpenDisplay(NULL);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        auto rootWindowID = RootWindow(dpy, monitor_num);

        MathCore::vec2i result;

        // using get screen resources
        // auto screen_info = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
        // ITK_ABORT(!screen_info, "XRandr screen info not found.\n");
        // for (int i = 0; i < screen_info->noutput; i++)
        // {
        //     bool connected = false;
        //     XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
        //                                                   screen_info,
        //                                                   screen_info->outputs[i]);

        //     connected = (output_info->connection == RR_Connected);
        //     if (connected){

        //         for(int j=0;j<screen_info->nmode;j++){
        //             XRRModeInfo *mode_info = &screen_info->modes[j];
        //             mode_info->

        //         }

        //         //RRMode currentMode = output_info->modes[output_info->nmode];

        //         result = MathCore::vec2i(output_info->mm_width, output_info->mm_height);
        //     }
        //     XRRFreeOutputInfo(output_info);
        //     if (connected)
        //         break;
        // }
        // XRRFreeScreenResources(screen_info);

        // using get monitors
        int nmonitors;
        auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
        ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
        result = MathCore::vec2i(monitor_info[0].width, monitor_info[0].height);
        XRRFreeMonitors(monitor_info);

        XCloseDisplay(dpy);

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        auto dpy = XOpenDisplay(NULL);
        ITK_ABORT(!dpy, "Failed to open default display.\n");

        // auto pscr = ScreenOfDisplay(dpy, monitor_num);
        // ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");
        // MathCore::vec2f result(pscr->mwidth, pscr->mheight);

        auto rootWindowID = RootWindow(dpy, monitor_num);

        // auto screenConfiguration = XRRGetScreenInfo (dpy, rootWindowID);
        // ITK_ABORT(!screenConfiguration, "Failed to obtain the default screen of given display.\n");

        // Rotation original_rotation;
        // auto current_size_index = XRRConfigCurrentConfiguration (screenConfiguration, &original_rotation);
        // int nsize;
        // auto sizes = XRRConfigSizes(screenConfiguration, &nsize);

        // MathCore::vec2f result =
        //     MathCore::vec2f(sizes[current_size_index].mwidth, sizes[current_size_index].mheight) ;

        // using get monitors
        // int nmonitors;
        // auto monitor_info = XRRGetMonitors( dpy, rootWindowID, true, &nmonitors );
        // MathCore::vec2f result =
        //     MathCore::vec2f(monitor_info[0].mwidth, monitor_info[0].mheight) ;

        // for (i = 0; i < nsize; i++) {
        //     printf ("%c%-2d %5d x %-5d  (%4dmm x%4dmm )",
        //         i == current_size ? '*' : ' ',
        //         i, sizes[i].width, sizes[i].height,
        //         sizes[i].mwidth, sizes[i].mheight);
        //     // ...
        // }

        MathCore::vec2f result;

        int nmonitors;
        auto monitor_info = XRRGetMonitors(dpy, rootWindowID, true, &nmonitors);
        ITK_ABORT(!monitor_info || nmonitors <= 0, "XRandr monitor info not found.\n");
        result = MathCore::vec2i(monitor_info[0].mwidth, monitor_info[0].mheight);

        // // using get screen resources
        // auto screen_info = XRRGetScreenResourcesCurrent(dpy, rootWindowID);
        // ITK_ABORT(!screen_info, "XRandr screen info not found.\n");
        // printf("%i\n", screen_info->noutput);
        // for (int i = 0; i < screen_info->noutput; i++)
        // {
        //     XRROutputInfo *output_info = XRRGetOutputInfo(dpy,
        //                                                   screen_info,
        //                                                   screen_info->outputs[i]);

        //     bool connected = (output_info->connection == RR_Connected);
        //     if (connected)
        //     {
        //         result = MathCore::vec2f(output_info->mm_width, output_info->mm_height);
        //     }
        //     XRRFreeOutputInfo(output_info);
        //     if (connected)
        //         break;
        // }
        // XRRFreeScreenResources(screen_info);

        XRRFreeMonitors(monitor_info);

        XCloseDisplay(dpy);

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeInches(int monitor_num)
    {
        return (MathCore::vec2f)MonitorRealSizeMillimeters(monitor_num) / 25.4f;
    }

    MathCore::vec2f Display::MonitorDPIf(int monitor_num)
    {
        return ComputeDPIf(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    }

    MathCore::vec2i Display::MonitorDPIi(int monitor_num)
    {
        return ComputeDPIi(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    }

    MathCore::vec2f Display::ComputeDPIf(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2f)sizePixels / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches)
    {
        return (MathCore::vec2i)(ComputeDPIf(sizePixels, realSizeInches) + 0.5f);
    }
}

#elif defined(_WIN32)

#include <shellscalingapi.h>

namespace DPI
{
    struct _MonitorInfo
    {
        // HMONITOR hMonitor;
        // HDC hdcMonitor;
        // MONITORINFOEX monitorInfoEx;
        // uint32_t dpi_x;
        // uint32_t dpi_y;
        // DEVICE_SCALE_FACTOR deviceScaleFactor;
        // float scaleFactor;
        // DEVMODE devMode;

        int x_pixels;
        int y_pixels;

        int width_virtual_pixels;
        int height_virtual_pixels;

        int width_pixels;
        int height_pixels;

        int width_mm;
        int height_mm;

        float scale_factor;

        bool is_primary;
    };

    BOOL CALLBACK _FillMonitorVector(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
    {
        std::vector<_MonitorInfo> &allMonitors = *(std::vector<_MonitorInfo> *)dwData;

        MONITORINFOEX miex;
        memset(&miex, 0, sizeof(MONITORINFOEX));
        miex.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &miex))
        {

            _MonitorInfo this_monitor;
            memset(&this_monitor, 0, sizeof(_MonitorInfo));
            // this_monitor.hMonitor = hMonitor;
            // this_monitor.hdcMonitor = hdcMonitor;
            // this_monitor.monitorInfoEx = miex;

            HDC dc = CreateDC(("DISPLAY"), miex.szDevice, NULL, NULL);
            if (dc)
            {

                // SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);

                this_monitor.width_virtual_pixels = GetDeviceCaps(dc, HORZRES);  // miex.rcMonitor.right - miex.rcMonitor.left;//GetDeviceCaps(dc, HORZRES);//
                this_monitor.height_virtual_pixels = GetDeviceCaps(dc, VERTRES); // miex.rcMonitor.bottom - miex.rcMonitor.top;//GetDeviceCaps(dc, VERTRES);//

                this_monitor.width_mm = GetDeviceCaps(dc, HORZSIZE);
                this_monitor.height_mm = GetDeviceCaps(dc, VERTSIZE);

                this_monitor.is_primary = (miex.dwFlags == MONITORINFOF_PRIMARY);

                DEVMODE devMode;
                memset(&devMode, 0, sizeof(DEVMODE));
                devMode.dmSize = sizeof(DEVMODE);
                if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &devMode))
                {
                    return FALSE;
                }

                this_monitor.x_pixels = devMode.dmPosition.x;
                this_monitor.y_pixels = devMode.dmPosition.y;

                this_monitor.width_pixels = devMode.dmPelsWidth;
                this_monitor.height_pixels = devMode.dmPelsHeight;

                this_monitor.scale_factor = (float)this_monitor.width_pixels / (float)(this_monitor.width_virtual_pixels);
                // printf("%f\n", this_monitor.scale_factor);

                ReleaseDC(NULL, dc);
            }
            else
                return FALSE;

            // if (GetDpiForMonitor(hMonitor, MDT_RAW_DPI, &this_monitor.dpi_x, &this_monitor.dpi_y) != S_OK) {
            //     /*HDC screen = GetDC(0);
            //     this_monitor.dpi_x = GetDeviceCaps(screen, LOGPIXELSX);
            //     this_monitor.dpi_y = GetDeviceCaps(screen, LOGPIXELSY);
            //     ReleaseDC(0, screen);*/
            //     return FALSE;
            // }

            // this_monitor.devMode.dmSize = sizeof(DEVMODE);
            // if (EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &this_monitor.devMode) == 0) {
            //     return FALSE;
            // }

            // if (GetScaleFactorForMonitor(hMonitor, &this_monitor.deviceScaleFactor) != S_OK) {
            //     return FALSE;
            // }

            // this_monitor.scaleFactor = (float)this_monitor.devMode.dmPelsWidth / (float)(this_monitor.monitorInfoEx.rcMonitor.right - this_monitor.monitorInfoEx.rcMonitor.left);

            allMonitors.push_back(this_monitor);
        }
        return TRUE;
    }

    int Display::MonitorCount()
    {
        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);
        return (int)allMonitors.size();
    }

    int Display::MonitorDefault()
    {
        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        for (size_t i = 0; i < allMonitors.size(); i++)
        {
            if (allMonitors[i].is_primary)
                return (int)i;
        }

        return 0;
    }

    MathCore::vec2i Display::MonitorPositionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2i result = MathCore::vec2i(
            selectedMonitor.x_pixels,
            selectedMonitor.y_pixels);

        return result;
    }

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2i result = MathCore::vec2i(
            selectedMonitor.width_pixels,
            selectedMonitor.height_pixels);

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2f result = MathCore::vec2f(
            selectedMonitor.width_mm,
            selectedMonitor.height_mm);

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeInches(int monitor_num)
    {
        return MonitorRealSizeMillimeters(monitor_num) / 25.4f;
    }

    MathCore::vec2f Display::MonitorDPIf(int monitor_num)
    {
        return ComputeDPIf(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    }

    MathCore::vec2i Display::MonitorDPIi(int monitor_num)
    {
        return ComputeDPIi(MonitorCurrentResolutionPixels(monitor_num), MonitorRealSizeInches(monitor_num));
    }

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