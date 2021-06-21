#include "RotatingCross.h"

namespace GLEngine {
    namespace Components {

        const ComponentType RotatingCross::Type = "RotatingCross";

        RotatingCross::RotatingCross():Component(RotatingCross::Type){
            speed_degrees = -100.0f;
        }

        void RotatingCross::start(){
            AppBase* app = Engine::Instance()->app;
            app->OnUpdate.add(this, &RotatingCross::OnUpdate);
        }

        void RotatingCross::OnUpdate(aRibeiro::PlatformTime *time) {
            transform[0]->LocalRotation = transform[0]->LocalRotation * aRibeiro::quatFromEuler(0, DEG2RAD(speed_degrees) * time->deltaTime, 0);
        }

        RotatingCross::~RotatingCross(){
            AppBase* app = Engine::Instance()->app;
            app->OnUpdate.remove(this, &RotatingCross::OnUpdate);
        }

    }

};
