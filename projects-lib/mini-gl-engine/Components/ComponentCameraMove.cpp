#include "ComponentCameraMove.h"
#include <stdio.h>
using namespace aRibeiro;

namespace GLEngine {

    namespace Components {

        const ComponentType ComponentCameraMove::Type = "ComponentCameraMove";

        void ComponentCameraMove::start(){
            camera = (ComponentCameraPerspective*)transform[0]->findComponent(ComponentCameraPerspective::Type);
            
            AppBase* app = Engine::Instance()->app;
            
            app->MousePos = app->MousePosCenter;//set app state do cursor center
            app->moveMouseToScreenCenter();//queue update to screen center
            
            app->OnLateUpdate.add(this, &ComponentCameraMove::OnLateUpdate);
            app->MousePos.OnChange.add(this, &ComponentCameraMove::OnMousePosChanged);
            app->WindowSize.OnChange.add(this, &ComponentCameraMove::OnWindowSizeChanged);
            
            OnWindowSizeChanged(&app->WindowSize);
            OnMousePosChanged(&app->MousePos);
            
            //convert the transform camera to camera move euler angles...
            vec3 forward = toVec3(transform[0]->getMatrix()[2]);
            vec3 proj_y = vec3(forward.x,0,forward.z);
            float length_proj_y = length(proj_y);
            proj_y = normalize(proj_y);
            vec3 cone_proj_x = normalize(vec3(length_proj_y,forward.y,0));
            
            euler.x = -atan2(cone_proj_x.y,cone_proj_x.x);
            euler.y = atan2(proj_y.x, proj_y.z);
            euler.z = 0;
            
            while (euler.x < - DEG2RAD(90.0f))
                euler.x += DEG2RAD(360.0f);
            while (euler.x > DEG2RAD(90.0f))
                euler.x -= DEG2RAD(360.0f);
            
            transform[0]->Rotation = quatFromEuler(euler.x, euler.y, euler.z);
        }

        void ComponentCameraMove::OnLateUpdate(PlatformTime *time) {

            if (camera == NULL)
                return;

            left.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A));
            right.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D));
            up.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W));
            down.setState(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S));
            
            vec3 forwardVec;
            
            if (camera->rightHanded)
                forwardVec = transform[0]->Rotation * vec3(0, 0, -1);
            else
                forwardVec = transform[0]->Rotation * vec3(0, 0, 1);
                
            vec3 rightVec = transform[0]->Rotation * vec3(1, 0, 0);

            if (left.pressed)
                transform[0]->Position = transform[0]->Position - rightVec * strafeSpeed * time->deltaTime;
            if (right.pressed)
                transform[0]->Position = transform[0]->Position + rightVec * strafeSpeed * time->deltaTime;
            if (up.pressed)
                transform[0]->Position = transform[0]->Position + forwardVec * strafeSpeed * time->deltaTime;
            if (down.pressed)
                transform[0]->Position = transform[0]->Position - forwardVec * strafeSpeed * time->deltaTime;
        }

        void ComponentCameraMove::OnWindowSizeChanged(Property<sf::Vector2i> *prop) {
            AppBase* app = Engine::Instance()->app;
            app->moveMouseToScreenCenter();
        }

        void ComponentCameraMove::OnMousePosChanged(Property<vec2> *prop) {
            AppBase* app = Engine::Instance()->app;

            vec2 pos = prop->value;
            
            vec2 delta = pos - app->MousePosCenter;
            
            if (sqrLength(delta) > EPSILON) {
                if (camera->rightHanded) {
                    euler.y = fmod(euler.y - delta.x * angleSpeed, 2.0f*PI);
                    euler.x = euler.x + delta.y * angleSpeed;
                }else{
                    euler.y = fmod(euler.y + delta.x * angleSpeed, 2.0f*PI);
                    euler.x = euler.x - delta.y * angleSpeed;
                }
                
                euler.x = clamp(euler.x, -DEG2RAD(90.0f), DEG2RAD(90.0f));
                
                transform[0]->Rotation = quatFromEuler(euler.x, euler.y, euler.z);
                
                app->moveMouseToScreenCenter();
            }
        }


        ComponentCameraMove::ComponentCameraMove():Component(ComponentCameraMove::Type){    
            forwardSpeed = 1.0f;
            strafeSpeed = 1.0f;
            angleSpeed = DEG2RAD(0.10f);
        }

        ComponentCameraMove::~ComponentCameraMove(){
            AppBase* app = Engine::Instance()->app;

            app->OnLateUpdate.remove(this, &ComponentCameraMove::OnLateUpdate);
            app->MousePos.OnChange.remove(this, &ComponentCameraMove::OnMousePosChanged);
            app->WindowSize.OnChange.remove(this, &ComponentCameraMove::OnWindowSizeChanged);
        }
    
    }
}
