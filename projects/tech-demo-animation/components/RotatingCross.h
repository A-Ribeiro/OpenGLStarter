#ifndef RotatingCross__H__
#define RotatingCross__H__

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

namespace GLEngine {
    namespace Components {

        class RotatingCross:public Component {
        public:
            static const ComponentType Type;

            float speed_degrees;

            RotatingCross();
            virtual ~RotatingCross();

            void start();
            void OnUpdate(aRibeiro::PlatformTime *time);



        };


    }

}

#endif