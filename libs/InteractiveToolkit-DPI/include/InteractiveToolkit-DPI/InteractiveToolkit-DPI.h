#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

namespace DPI
{

    class Mode
    {
    public:
        int width, height;
        std::vector<float> freqs;

        void setFrequency(float freq)
        {
            freqs.clear();
            freqs.push_back(freq);
        }

        void setFrequencyByIndex(int freq_idx)
        {
            if (freq_idx >= (int)freqs.size())
                return;
            float freq = freqs[freq_idx];
            freqs.clear();
            freqs.push_back(freq);
        }
    
    };


    class Monitor
    {
    public:
        std::string name;
        std::string port;
        int x, y, width, height, mwidth, mheight;
        float scaleFactor;
        bool primary;

        std::vector<int> prefered_mode_index;
        std::vector<int> prefered_freq_index;

        int current_mode_index;
        int current_freq_index;

        std::vector<Mode> modes;

        MathCore::vec2i Position() const
        {
            return MathCore::vec2i(x, y);
        }

        MathCore::vec2i SizePixels() const
        {
            return MathCore::vec2i(width, height);
        }

        MathCore::vec2f SizeMillimeters() const
        {
            return MathCore::vec2f((float)mwidth, (float)mheight);
        }

        MathCore::vec2f SizeInches() const
        {
            return SizeMillimeters() / 25.4f;
        }

        int getModeIndex(int width, int height) const
        {
            for (int i = 0; i < (int)modes.size(); i++)
            {
                auto &mode = modes[i];
                if (mode.width == width && mode.height == height)
                    return i;
            }
            return 0;
        }

        Mode &getOrCreateMode(int width, int height)
        {
            for (auto &mode : modes)
            {
                if (mode.width == width && mode.height == height)
                    return mode;
            }
            Mode newMode;
            newMode.width = width;
            newMode.height = height;
            modes.push_back(newMode);
            return modes[modes.size() - 1];
        }

        Mode &getMode(int width, int height)
        {
            return modes[getModeIndex(width, height)];
        }

        void setMode(const Mode &mode);

        Mode getCurrentMode() const
        {
            Mode result = modes[current_mode_index];
            float aux = result.freqs[current_freq_index];
            result.freqs.clear();
            result.freqs.push_back(aux);
            return result;
        }

        std::vector<Mode> getPreferedModes() const
        {
            std::vector<Mode> result;
            for (int i = 0; i < (int)prefered_mode_index.size(); i++)
            {
                Mode aux_mode = modes[prefered_mode_index[i]];
                float aux = aux_mode.freqs[prefered_freq_index[i]];
                aux_mode.freqs.clear();
                aux_mode.freqs.push_back(aux);
                result.push_back(aux_mode);
            }
            return result;
        }
    };

#if defined(_WIN32)
    // Window handle is HWND (HWND__*) or (HWND) on Windows
    typedef HWND__ *NativeWindowHandleType;
#elif defined(__linux__)
    // Window handle is Window (unsigned long) on Unix - X11
    typedef unsigned long NativeWindowHandleType;
#elif defined(__APPLE__)
    // Window handle is NSWindow or NSView (void*) on Mac OS X - Cocoa
    typedef void *NativeWindowHandleType;
#else
    typedef void *NativeWindowHandleType;
#endif

    struct Display
    {

        static std::vector<Monitor> QueryMonitors(bool only_default, int *monitorDefaultIndex = nullptr);

        static void setFullscreenAttribute(const NativeWindowHandleType &nativeWindow, const Monitor *monitor);

        // static int MonitorCount();

        // static int MonitorDefault() ;

        // static MathCore::vec2i MonitorPositionPixels(int monitor_num = -1);
        // static MathCore::vec2i MonitorCurrentResolutionPixels(int monitor_num = -1);
        // static MathCore::vec2f MonitorRealSizeMillimeters(int monitor_num = -1);
        // static MathCore::vec2f MonitorRealSizeInches(int monitor_num = -1);
        // static MathCore::vec2f MonitorDPIf(int monitor_num = -1);
        // static MathCore::vec2i MonitorDPIi(int monitor_num = -1);

        static MathCore::vec2f ComputeDPIf(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches);
        static MathCore::vec2i ComputeDPIi(const MathCore::vec2i &sizePixels, const MathCore::vec2f &realSizeInches);

        static void setMonitorResolution(const std::string &monitor_port, int width, int height, float freq);
        // static void setMonitorFractionalScaling(const std::string &monitor_port, float scaleFactor);
    };
}
