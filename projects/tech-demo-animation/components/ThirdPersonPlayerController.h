#ifndef ThirdPersonPlayerController__H__
#define ThirdPersonPlayerController__H__

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include "AnimationMotion.h"

namespace GLEngine {
    namespace Components {

        class ThirdPersonPlayerController:public Component {
        public:
            static const ComponentType Type;

            SharedPointer<AnimationMotion> animationMotion;

            aRibeiro::PressReleaseDetector walk;
            aRibeiro::PressReleaseDetector run;

            aRibeiro::PressReleaseDetector right;
            aRibeiro::PressReleaseDetector left;

            float turn_speed;

            ThirdPersonPlayerController():Component(ThirdPersonPlayerController::Type) {
                turn_speed = 100.0f;
            }

            void start() {
                AppBase* app = Engine::Instance()->app;
                app->OnUpdate.add(this, &ThirdPersonPlayerController::OnUpdate);

                animationMotion = (AnimationMotion*)transform[0]->findComponent(AnimationMotion::Type);
                ARIBEIRO_ABORT(animationMotion == NULL, "Failed to query skinned mesh\n.");

                animationMotion->OnMove.add(this, &ThirdPersonPlayerController::OnMoveAnimation);

            }

            virtual ~ThirdPersonPlayerController() {
                AppBase* app = Engine::Instance()->app;
                app->OnUpdate.remove(this, &ThirdPersonPlayerController::OnUpdate);

                if (animationMotion != NULL) {
                    animationMotion = NULL;
                }
            }

            void OnMoveAnimation(float right, float up, float forward, const aRibeiro::vec3 &final_local_position) {
                transform[0]->LocalPosition = final_local_position;
            }

            void OnUpdate(aRibeiro::PlatformTime *time) {
                
                right.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::A) );
                left.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::D) );
                walk.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::W) || (right.pressed ^ left.pressed) );
                run.setState( sf::Keyboard::isKeyPressed(sf::Keyboard::W) && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) );

                if (run.down)
                    animationMotion->TriggerClip("run");
                else if (walk.down || (walk.pressed && run.up)) {
                    animationMotion->TriggerClip("walk");
                }
                else if (walk.up || run.up)
                    animationMotion->TriggerClip("idle");

                if (right.pressed ^ left.pressed)
                {
                    if (right.pressed)
                        transform[0]->LocalRotation = transform[0]->LocalRotation * aRibeiro::quatFromEuler(0, -DEG2RAD(turn_speed) * time->deltaTime, 0);
                    else if (left.pressed)
                        transform[0]->LocalRotation = transform[0]->LocalRotation * aRibeiro::quatFromEuler(0, DEG2RAD(turn_speed) * time->deltaTime, 0);
                }

                /*
                if (walk.pressed) {
                    transform[0]->LocalPosition = transform[0]->LocalPosition + transform[0]->LocalRotation * aRibeiro::vec3(0, 0, time->deltaTime * 5.0f);
                }
                */
                
            }

        };


    }

}

#endif