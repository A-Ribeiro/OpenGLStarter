#ifndef ThirdPersonCamera__H__
#define ThirdPersonCamera__H__

#include <aribeiro/aribeiro.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/ComponentMaterial.h>
#include <mini-gl-engine/ComponentMesh.h>



namespace GLEngine {
    namespace Components {

        class ThirdPersonCamera:public Component {
        public:
            static const ComponentType Type;

            float current_x_angle;
            float distance;
            float distance_offset;
            float pos_speed;
            float rotacional_speed_y;
            float rotacional_speed_x;

            SharedPointer<Transform> Target;
            aRibeiro::vec3 transform_position_target;

            //Transform *debugSphere;
            //Transform *debugSphere2;

            ThirdPersonCamera():Component(ThirdPersonCamera::Type) {
                current_x_angle = -1;
                distance = -1;
                distance_offset = 1;
                pos_speed = 2.0f;
                rotacional_speed_y = 50.0f;
                rotacional_speed_x = 10.0f;
            }

            void start() {
                AppBase* app = Engine::Instance()->app;
                app->OnLateUpdate.add(this, &ThirdPersonCamera::OnLateUpdate);

                transform_position_target = transform[0]->Position;

                if (Target == NULL)
                    return;

                if (distance <= 0)
                    distance = aRibeiro::distance(Target->Position, transform[0]->Position);

                if (current_x_angle < 0)
                {
                    aRibeiro::vec3 cam2target = aRibeiro::normalize(Target->Position - transform_position_target);
                    aRibeiro::vec2 angle_x_aux = aRibeiro::vec2(sqrt(cam2target.x * cam2target.x + cam2target.z * cam2target.z), cam2target.y);
                    current_x_angle =  RAD2DEG( -atan2(angle_x_aux.y, angle_x_aux.x) );
                }

                /*
                debugSphere = new Transform();
                debugSphere2 = new Transform();
                transform[0]->getParent()->addChild(debugSphere);
                transform[0]->getParent()->addChild(debugSphere2);
                {
                    ComponentMaterial * newMaterial;

                    newMaterial = new ComponentMaterial();
                    newMaterial->type = Components::MaterialPBR;
                    newMaterial->pbr.albedoColor = aRibeiro::vec4(1, 0, 0, 1);
                    newMaterial->pbr.metallic = 0.0f;
                    newMaterial->pbr.roughness = 1.0f;
                    newMaterial->pbr.texAlbedo = NULL;
                    newMaterial->pbr.texNormal = NULL;
                    newMaterial->pbr.texSpecular = NULL;

                    debugSphere->addComponent(newMaterial);

                    debugSphere->addComponent(ComponentMesh::createSphere(0.1, 8, 8));


                    newMaterial = new ComponentMaterial();
                    newMaterial->type = Components::MaterialPBR;
                    newMaterial->pbr.albedoColor = aRibeiro::vec4(0, 0, 1, 1);
                    newMaterial->pbr.metallic = 0.0f;
                    newMaterial->pbr.roughness = 1.0f;
                    newMaterial->pbr.texAlbedo = NULL;
                    newMaterial->pbr.texNormal = NULL;
                    newMaterial->pbr.texSpecular = NULL;

                    debugSphere2->addComponent(newMaterial);
                    debugSphere2->addComponent(ComponentMesh::createSphere(0.1, 8, 8));
                }
                */

            }

            virtual ~ThirdPersonCamera() {
                AppBase* app = Engine::Instance()->app;
                app->OnLateUpdate.remove(this, &ThirdPersonCamera::OnLateUpdate);

                if (Target != NULL) {
                    Target = NULL;
                }
            }

            void OnLateUpdate(aRibeiro::PlatformTime *time) {
                if (Target == NULL)
                    return;

                Position_1st_Quadratic_Clamp(time);
                Position_2nd_Quadratic_Clamp(time);
                X_Rotate_Smooth(time);
                Y_Rotate_Smooth(time);
                //RaycastPlayerToScene(time);
            }

            void Position_1st_Quadratic_Clamp(aRibeiro::PlatformTime *time)
            {
                //quadratic clamp
                float new_distance = aRibeiro::distance(Target->Position, transform_position_target);
                if (new_distance > distance)
                {
                    aRibeiro::vec3 cam2target = aRibeiro::normalize(Target->Position - transform_position_target);
                    transform_position_target = Target->Position - cam2target * distance;
                }

                //move towards
                transform[0]->Position = aRibeiro::move(transform[0]->Position, transform_position_target, pos_speed * time->deltaTime);
            }

            void Position_2nd_Quadratic_Clamp(aRibeiro::PlatformTime *time)
            {
                aRibeiro::vec3 cam2target = aRibeiro::normalize(Target->Position - transform[0]->Position);
                float new_distance = aRibeiro::distance(Target->Position, transform[0]->Position);
                
                //quadratic clamp
                if (new_distance > distance + distance_offset)
                {
                    //force set without interpolation...
                    transform_position_target = Target->Position - cam2target * (distance + distance_offset);
                    transform[0]->Position = transform_position_target;
                }

                transform[0]->Rotation = aRibeiro::quatLookAtRotationLH(cam2target, aRibeiro::vec3(0,1,0));
            }

            void X_Rotate_Smooth(aRibeiro::PlatformTime *time)
            {
                aRibeiro::vec3 cam2target = Target->Position - transform[0]->Position;
                if ( aRibeiro::sqrLength(cam2target) <= 0.0002f)
                    return;
                cam2target = aRibeiro::normalize(cam2target);
                float new_distance = aRibeiro::distance(Target->Position, transform[0]->Position);

                aRibeiro::vec3 camera_forward = transform[0]->Rotation * aRibeiro::vec3(0,0,1);
                aRibeiro::vec3 camera_projected_on_y = (aRibeiro::vec3(camera_forward.x, 0, camera_forward.z));
                if (aRibeiro::sqrLength(camera_projected_on_y) > 0.0002f)
                {
                    camera_projected_on_y = normalize(camera_projected_on_y);
                    aRibeiro::vec3 camera_left = transform[0]->Rotation * aRibeiro::vec3(-1,0,0);

                    aRibeiro::vec3 target_rotation_back = aRibeiro::quatFromAxisAngle(camera_left, DEG2RAD(-current_x_angle)) * camera_projected_on_y;
                    target_rotation_back = normalize(target_rotation_back);

                    target_rotation_back = aRibeiro::moveSlerp(
                       aRibeiro::quatLookAtRotationLH(cam2target, aRibeiro::vec3(0,1,0)),
                       aRibeiro::quatLookAtRotationLH(target_rotation_back, aRibeiro::vec3(0,1,0)),
                        DEG2RAD(rotacional_speed_x) * time->deltaTime
                    ) * aRibeiro::vec3(0,0,1);
                    target_rotation_back = normalize(target_rotation_back);

                    transform[0]->Position = Target->Position - target_rotation_back * new_distance;
                    //the target just need to be rotated as the main position... but the distance keeps the same
                    transform_position_target = Target->Position - target_rotation_back * aRibeiro::distance(Target->Position, transform_position_target);

                    transform[0]->Rotation = aRibeiro::quatLookAtRotationLH(target_rotation_back, aRibeiro::vec3(0,1,0));
                }
            }

            void Y_Rotate_Smooth(aRibeiro::PlatformTime *time)
            //rotate to be in the back
            {
                aRibeiro::vec3 cam2target = Target->Position - transform[0]->Position;
                if ( aRibeiro::sqrLength(cam2target) <= 0.0002f)
                    return;
                cam2target = aRibeiro::normalize(cam2target);
                float new_distance = aRibeiro::distance(Target->Position, transform[0]->Position);
                aRibeiro::vec2 angle_x_aux = aRibeiro::vec2(sqrt(cam2target.x * cam2target.x + cam2target.z * cam2target.z), cam2target.y);
                float current_x_angle = RAD2DEG( -atan2(angle_x_aux.y, angle_x_aux.x) );

                //aRibeiro::vec3 player_forward = Target.parent.forward;
                aRibeiro::vec3 player_forward = Target->getParent()->Rotation * aRibeiro::vec3(0,0,1);

                aRibeiro::vec3 player_projected_on_y = (aRibeiro::vec3(player_forward.x, 0, player_forward.z));
                if ( aRibeiro::sqrLength( player_projected_on_y ) > 0.0002f)
                {
                    player_projected_on_y = normalize(player_projected_on_y);
                    aRibeiro::vec3 player_left = Target->getParent()->Rotation * aRibeiro::vec3(-1,0,0);

                    aRibeiro::vec3 target_rotation_back = aRibeiro::quatFromAxisAngle(player_left, DEG2RAD(-current_x_angle)) * player_projected_on_y;
                    target_rotation_back = normalize(target_rotation_back);

                    //debugSphere->setPosition(Target->getPosition() - cam2target);
                    //debugSphere2->setPosition(Target->getPosition() - target_rotation_back);

                    target_rotation_back = aRibeiro::moveSlerp(
                       aRibeiro::quatLookAtRotationLH(cam2target, aRibeiro::vec3(0,1,0)),
                       aRibeiro::quatLookAtRotationLH(target_rotation_back, aRibeiro::vec3(0,1,0)),
                        DEG2RAD(rotacional_speed_y) * time->deltaTime
                    ) * aRibeiro::vec3(0,0,1);
                    target_rotation_back = normalize(target_rotation_back);

                    transform[0]->Position = Target->Position - target_rotation_back * new_distance;
                    //the target just need to be rotated as the main position... but the distance keeps the same
                    transform_position_target = Target->Position - target_rotation_back * aRibeiro::distance(Target->Position, transform_position_target);

                    transform[0]->Rotation = aRibeiro::quatLookAtRotationLH(target_rotation_back, aRibeiro::vec3(0,1,0));
                }
            }

            void RaycastPlayerToScene(aRibeiro::PlatformTime *time)
            {
                aRibeiro::vec3 target2cam = transform[0]->Position - Target->Position;
                if ( aRibeiro::sqrLength(target2cam) <= 0.0002f)
                    return;
                target2cam = aRibeiro::normalize(target2cam);

                float distance = aRibeiro::distance(transform[0]->Position, Target->Position);

                aRibeiro::collision::Ray ray = aRibeiro::collision::Ray(Target->Position, target2cam);
                /*
                //
                // physics not implemented...
                //
                RaycastHit[] result = Physics.RaycastAll(ray, distance, 1 << LayerMask.NameToLayer("col.scene"));
                if (result.Length > 0)
                {
                    transform_position_target = Target->Position + target2cam * result[0].distance;
                    transform[0]->Position = transform_position_target;
                }
                */
            }

        };


    }

}

#endif