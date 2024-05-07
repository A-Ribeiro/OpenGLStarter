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

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        ::Display *pdsp = NULL;
        ::Screen *pscr = NULL;

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

        ::Display *pdsp = NULL;
        ::Screen *pscr = NULL;

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

    MathCore::vec2f Display::ComputeDPIf( const MathCore::vec2i & resolution, const MathCore::vec2f & realSizeInches ) {
        return (MathCore::vec2f) resolution / realSizeInches;
    }

    MathCore::vec2i Display::ComputeDPIi( const MathCore::vec2i & resolution, const MathCore::vec2f & realSizeInches ) {
        return (MathCore::vec2i)( ComputeDPIf(resolution, realSizeInches)  + 0.5f );
    }
}

#elif defined(_WIN32)

#include<shellscalingapi.h>

namespace DPI
{
    struct _MonitorInfo{
        HMONITOR hMonitor;
        HDC hdcMonitor;
        MONITORINFOEX monitorInfoEx;
        uint32_t dpi_x;
        uint32_t dpi_y;
    };

    BOOL CALLBACK _FillMonitorVector(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
        std::vector<_MonitorInfo> &allMonitors = *(std::vector<_MonitorInfo>*)dwData;

        MONITORINFOEX miex;
        memset(&miex, 0, sizeof(MONITORINFOEX));
        miex.cbSize = sizeof(MONITORINFOEX);
        if (GetMonitorInfo(hMonitor, &miex)) {

            _MonitorInfo this_monitor;
            this_monitor.hMonitor = hMonitor;
            this_monitor.hdcMonitor = hdcMonitor;
            this_monitor.monitorInfoEx = miex;

            if (GetDpiForMonitor(hMonitor, MDT_DEFAULT, &this_monitor.dpi_x, &this_monitor.dpi_y) != S_OK) {
                HDC screen = GetDC(0);
                this_monitor.dpi_x = GetDeviceCaps(screen, LOGPIXELSX);
                this_monitor.dpi_y = GetDeviceCaps(screen, LOGPIXELSY);
                ReleaseDC(0, screen);
            }

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
            if (allMonitors[i].monitorInfoEx.dwFlags == MONITORINFOF_PRIMARY)
                return (int)i;
        }

        return 0;
    }

    MathCore::vec2i Display::MonitorCurrentResolutionPixels(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo &selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2i result(
            selectedMonitor.monitorInfoEx.rcMonitor.right - selectedMonitor.monitorInfoEx.rcMonitor.left, 
            selectedMonitor.monitorInfoEx.rcMonitor.bottom - selectedMonitor.monitorInfoEx.rcMonitor.top
            );

        return result;
    }

    MathCore::vec2f Display::MonitorRealSizeMillimeters(int monitor_num)
    {
        return (MathCore::vec2f)MonitorRealSizeInches(monitor_num) * 25.4f;
    }

    MathCore::vec2f Display::MonitorRealSizeInches(int monitor_num)
    {
        if (monitor_num == -1)
            monitor_num = MonitorDefault();

        std::vector<_MonitorInfo> allMonitors;
        EnumDisplayMonitors(NULL, NULL, _FillMonitorVector, (LPARAM)&allMonitors);

        _MonitorInfo& selectedMonitor = allMonitors[monitor_num];

        MathCore::vec2f size_pixels(
            selectedMonitor.monitorInfoEx.rcMonitor.right - selectedMonitor.monitorInfoEx.rcMonitor.left,
            selectedMonitor.monitorInfoEx.rcMonitor.bottom - selectedMonitor.monitorInfoEx.rcMonitor.top
        );

        MathCore::vec2f size_in = size_pixels / MathCore::vec2f(selectedMonitor.dpi_x, selectedMonitor.dpi_y);
        return size_in;
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