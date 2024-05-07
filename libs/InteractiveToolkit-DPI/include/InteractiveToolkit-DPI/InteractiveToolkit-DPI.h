#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/MathCore/MathCore.h>

namespace DPI {

    struct Display {

        static int MonitorCount();

        static int MonitorDefault() ;

        static MathCore::vec2i MonitorCurrentResolutionPixels(int monitor_num = -1);
        static MathCore::vec2f MonitorRealSizeMillimeters(int monitor_num = -1);
        static MathCore::vec2f MonitorRealSizeInches(int monitor_num = -1);
        static MathCore::vec2f MonitorDPIf(int monitor_num = -1);
        static MathCore::vec2i MonitorDPIi(int monitor_num = -1);

        static MathCore::vec2f ComputeDPIf( const MathCore::vec2i & resolution, const MathCore::vec2f & realSizeInches );
        static MathCore::vec2i ComputeDPIi( const MathCore::vec2i & resolution, const MathCore::vec2f & realSizeInches );

    };
}
