#include <string.h> //memcmp

#include <InteractiveToolkit/MathCore/MathCore.h>
#include <appkit-gl-engine/Engine.h>

namespace AppKit
{
    namespace GLEngine
    {

        iRect iRect::convert_y_coord_to_opengl_using_app_screen() const
        {
            const iRect &screenViewport = AppKit::GLEngine::Engine::Instance()->app->getFullWindowViewport();
            iRect result = *this;
            // y coordinate is inverted in OpenGL, so we need to convert it
            result.y = screenViewport.h - 1 - (h - 1 + y);
            return result;
        }

    }

}