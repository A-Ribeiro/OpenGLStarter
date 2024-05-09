#include <InteractiveToolkit-DPI/InteractiveToolkit-DPI.h>

#if defined(__linux__)

// https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux:_Changing_the_Screen_Resolution

#include <X11/Xlib.h>

namespace DPI
{

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

        ::Display *pdsp = NULL;
        ::Window pRootWindow;
        ::XWindowAttributes xwa;

        pdsp = XOpenDisplay(NULL);
        ITK_ABORT(!pdsp, "Failed to open default display.\n");

        pRootWindow = RootWindow(pdsp, monitor_num);

        XGetWindowAttributes( pdsp, pRootWindow, &xwa );

        MathCore::vec2i result(xwa.x, xwa.y);

        XCloseDisplay(pdsp);

        return result;
    }

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        ::Display *pdsp = NULL;
        ::Screen* pscr = NULL;

        pdsp = XOpenDisplay(NULL);
        ITK_ABORT(!pdsp, "Failed to open default display.\n");

        pscr = ScreenOfDisplay(pdsp, monitor_num);
        ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");

        MathCore::vec2i result(pscr->width, pscr->height);

        XCloseDisplay(pdsp);

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        ::Display* pdsp = NULL;
        ::Screen* pscr = NULL;

        pdsp = XOpenDisplay(NULL);
        ITK_ABORT(!pdsp, "Failed to open default display.\n");

        pscr = ScreenOfDisplay(pdsp, monitor_num);
        ITK_ABORT(!pscr, "Failed to obtain the default screen of given display.\n");

        MathCore::vec2f result(pscr->mwidth, pscr->mheight);

        XCloseDisplay(pdsp);

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

    MathCore::vec2f Display::ComputeDPIf(const MathCore::vec2i& resolution, const MathCore::vec2f& realSizeInches) {
        return (MathCore::vec2f)resolution / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi(const MathCore::vec2i& resolution, const MathCore::vec2f& realSizeInches) {
        return (MathCore::vec2i)(ComputeDPIf(resolution, realSizeInches) + 0.5f);
    }
}

#elif defined(_WIN32)

#include<shellscalingapi.h>

namespace DPI
{
    struct _MonitorInfo {
        //HMONITOR hMonitor;
        //HDC hdcMonitor;
        //MONITORINFOEX monitorInfoEx;
        //uint32_t dpi_x;
        //uint32_t dpi_y;
        //DEVICE_SCALE_FACTOR deviceScaleFactor;
        //float scaleFactor;
        //DEVMODE devMode;

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

    BOOL CALLBACK _FillMonitorVector(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
        std::vector<_MonitorInfo>& allMonitors = *(std::vector<_MonitorInfo>*)dwData;

        MONITORINFOEX miex;
        memset(&miex, 0, sizeof(MONITORINFOEX));
        miex.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &miex)) {

            _MonitorInfo this_monitor;
            memset(&this_monitor, 0, sizeof(_MonitorInfo));
            //this_monitor.hMonitor = hMonitor;
            //this_monitor.hdcMonitor = hdcMonitor;
            //this_monitor.monitorInfoEx = miex;

            HDC dc = CreateDC(("DISPLAY"), miex.szDevice, NULL, NULL);
            if (dc) {

                //SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);

                this_monitor.width_virtual_pixels = GetDeviceCaps(dc, HORZRES);//miex.rcMonitor.right - miex.rcMonitor.left;//GetDeviceCaps(dc, HORZRES);//
                this_monitor.height_virtual_pixels = GetDeviceCaps(dc, VERTRES);//miex.rcMonitor.bottom - miex.rcMonitor.top;//GetDeviceCaps(dc, VERTRES);//

                this_monitor.width_mm = GetDeviceCaps(dc, HORZSIZE);
                this_monitor.height_mm = GetDeviceCaps(dc, VERTSIZE);

                this_monitor.is_primary = (miex.dwFlags == MONITORINFOF_PRIMARY);

                DEVMODE devMode;
                memset(&devMode, 0, sizeof(DEVMODE));
                devMode.dmSize = sizeof(DEVMODE);
				if (!EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &devMode)) {
                    return FALSE;
                }

                this_monitor.x_pixels = devMode.dmPosition.x;
                this_monitor.y_pixels = devMode.dmPosition.y;

                this_monitor.width_pixels = devMode.dmPelsWidth;
                this_monitor.height_pixels = devMode.dmPelsHeight;

                this_monitor.scale_factor = (float)this_monitor.width_pixels / (float)(this_monitor.width_virtual_pixels);
                //printf("%f\n", this_monitor.scale_factor);

                ReleaseDC(NULL, dc);
            }
            else
                return FALSE;

            //if (GetDpiForMonitor(hMonitor, MDT_RAW_DPI, &this_monitor.dpi_x, &this_monitor.dpi_y) != S_OK) {
            //    /*HDC screen = GetDC(0);
            //    this_monitor.dpi_x = GetDeviceCaps(screen, LOGPIXELSX);
            //    this_monitor.dpi_y = GetDeviceCaps(screen, LOGPIXELSY);
            //    ReleaseDC(0, screen);*/
            //    return FALSE;
            //}

            //this_monitor.devMode.dmSize = sizeof(DEVMODE);
            //if (EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &this_monitor.devMode) == 0) {
            //    return FALSE;
            //}

            //if (GetScaleFactorForMonitor(hMonitor, &this_monitor.deviceScaleFactor) != S_OK) {
            //    return FALSE;
            //}

            //this_monitor.scaleFactor = (float)this_monitor.devMode.dmPelsWidth / (float)(this_monitor.monitorInfoEx.rcMonitor.right - this_monitor.monitorInfoEx.rcMonitor.left);

            allMonitors.push_back(this_monitor);
        }
        return TRUE;
    }

    int Display::MonitorCount()
    {
        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM) & allMonitors);
        return (int)allMonitors.size();
    }

    int Display::MonitorDefault()
    {
        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        for (size_t i = 0; i < allMonitors.size();i++) {
            if (allMonitors[i].is_primary)
                return (int)i;
        }

        return 0;
    }

    MathCore::vec2i Display::MonitorPositionPixels(int monitor_num) {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo& selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2i result = MathCore::vec2i(
            selectedMonitor.x_pixels,
            selectedMonitor.y_pixels
        );

        return result;
    }

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2i result =  MathCore::vec2i(
            selectedMonitor.width_pixels,
            selectedMonitor.height_pixels
        );
        
        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo& selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2f result = MathCore::vec2f(
            selectedMonitor.width_mm,
            selectedMonitor.height_mm
        );

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

    MathCore::vec2f Display::ComputeDPIf(const MathCore::vec2i& resolution, const MathCore::vec2f& realSizeInches) {
        return (MathCore::vec2f)resolution / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi(const MathCore::vec2i& resolution, const MathCore::vec2f& realSizeInches) {
        return (MathCore::vec2i)(ComputeDPIf(resolution, realSizeInches) + 0.5f);
    }
}

#elif defined(__APPLE__)

#endif