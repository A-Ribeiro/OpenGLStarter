#include "ComponentCameraRotateOnTarget.h"

namespace GLEngine {

    namespace Components {


        const ComponentType ComponentCameraRotateOnTarget::Type = "ComponentCameraRotateOnTarget";

        Transform* ComponentCameraRotateOnTarget::getTarget() {
            return target;
        }

        void ComponentCameraRotateOnTarget::setTarget(Transform* _transform){
            target = _transform;
            if (target == NULL) {
                mouseMoving = false;
                return;
            }

            Transform *toLookNode = target;


            distanceRotation = aRibeiro::distance(transform[0]->Position, toLookNode->Position);


            //convert the transform camera to camera move euler angles...
            aRibeiro::vec3 forward = aRibeiro::toVec3(transform[0]->getMatrix()[2]);
            aRibeiro::vec3 proj_y = aRibeiro::vec3(forward.x, 0, forward.z);
            float length_proj_y = aRibeiro::length(proj_y);
            proj_y = aRibeiro::normalize(proj_y);
            aRibeiro::vec3 cone_proj_x = aRibeiro::normalize(aRibeiro::vec3(length_proj_y, forward.y, 0));

            euler.x = -atan2(cone_proj_x.y, cone_proj_x.x);
            euler.y = atan2(proj_y.x, proj_y.z);
            euler.z = 0;

            while (euler.x < -DEG2RAD(90.0f))
                euler.x += DEG2RAD(360.0f);
            while (euler.x > DEG2RAD(90.0f))
                euler.x -= DEG2RAD(360.0f);

            /*
            if (mouseMoving) {
                AppBase* app = Engine::Instance()->app;
                OnMouseDown(sf::Mouse::Button::Left, app->MousePos);
            }*/

            //time->timeScale = 1.0f;
            mouseMoving = false;
        }

        void ComponentCameraRotateOnTarget::start(){
            AppBase* app = Engine::Instance()->app;
            
            app->MousePosRelatedToCenterNormalized.OnChange.add(this, &ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged);
            app->OnMouseDown.add(this, &ComponentCameraRotateOnTarget::OnMouseDown);
            app->OnMouseUp.add(this, &ComponentCameraRotateOnTarget::OnMouseUp);

        }
        
        void ComponentCameraRotateOnTarget::OnMouseUp(sf::Mouse::Button button, const aRibeiro::vec2 &pos){
            //time->timeScale = 1.0f;
            mouseMoving = false;

        }

        void ComponentCameraRotateOnTarget::OnMouseDown(sf::Mouse::Button button, const aRibeiro::vec2 &pos){
            if (target == NULL)
                return;

            AppBase* app = Engine::Instance()->app;

            lastPosition = app->WindowCoordToNormalizedCoord(pos);
            //time->timeScale = 0.0f;
            mouseMoving = true;
        }

        void ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged(aRibeiro::Property<aRibeiro::vec2> *prop){
            if (!mouseMoving || target == NULL) 
                return;
            
            AppBase* app = Engine::Instance()->app;

            if (!enabled) {
                lastPosition = app->MousePosRelatedToCenterNormalized.value;
                return;
            }

            aRibeiro::vec2 delta = app->MousePosRelatedToCenterNormalized.value - lastPosition;
            lastPosition = app->MousePosRelatedToCenterNormalized.value;

            Transform *toLookNode = target;

            {
                euler.y = fmod(euler.y + delta.x * speedAngle, 2.0f*aRibeiro::PI);
                euler.x = euler.x - delta.y * speedAngle;
            }

            //euler.x = clamp(euler.x, -DEG2RAD(90.0f), DEG2RAD(90.0f));
            euler.x = aRibeiro::clamp(euler.x, -DEG2RAD(22.0f), DEG2RAD(47.0f));

            transform[0]->Rotation = aRibeiro::quatFromEuler(euler.x, euler.y, euler.z);

            aRibeiro::vec3 backward = transform[0]->Rotation * aRibeiro::vec3(0, 0, -1);
            transform[0]->Position = toLookNode->getPosition() + backward * distanceRotation;
        }

        ComponentCameraRotateOnTarget::ComponentCameraRotateOnTarget():
            Component(ComponentCameraRotateOnTarget::Type),
            Target(this,&ComponentCameraRotateOnTarget::getTarget, &ComponentCameraRotateOnTarget::setTarget) {
            
            speedAngle = DEG2RAD( 128.0f );
            mouseMoving = false;
            enabled = true;
            
        }

        ComponentCameraRotateOnTarget::~ComponentCameraRotateOnTarget() {
            AppBase* app = Engine::Instance()->app;

            app->MousePosRelatedToCenterNormalized.OnChange.remove(this, &ComponentCameraRotateOnTarget::OnNormalizedMousePosChanged);
            app->OnMouseDown.remove(this, &ComponentCameraRotateOnTarget::OnMouseDown);
            app->OnMouseUp.remove(this, &ComponentCameraRotateOnTarget::OnMouseUp);
        }

    }
}
