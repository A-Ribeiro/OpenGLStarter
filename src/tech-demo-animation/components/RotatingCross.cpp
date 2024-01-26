#include "RotatingCross.h"

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/Transform.h>

namespace AppKit
{
    namespace GLEngine
    {
        namespace Components
        {

            const ComponentType RotatingCross::Type = "RotatingCross";

            RotatingCross::RotatingCross() : Component(RotatingCross::Type)
            {
                speed_degrees = -100.0f;
            }

            void RotatingCross::start()
            {
                AppBase *app = Engine::Instance()->app;
                app->screenRenderWindow.OnUpdate.add(&RotatingCross::OnUpdate, this);
            }

            void RotatingCross::OnUpdate(Platform::Time *time)
            {
                transform[0]->LocalRotation =
                    transform[0]->LocalRotation *
                    MathCore::GEN<MathCore::quatf>::fromEuler(
                        0,
                        MathCore::OP<float>::deg_2_rad(speed_degrees) * time->deltaTime,
                        0);
            }

            RotatingCross::~RotatingCross()
            {
                AppBase *app = Engine::Instance()->app;
                app->screenRenderWindow.OnUpdate.remove(&RotatingCross::OnUpdate, this);
            }

        }

    }
}
