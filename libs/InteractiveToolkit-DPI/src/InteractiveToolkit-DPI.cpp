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

#elif defined(__APPLE__)

#endif