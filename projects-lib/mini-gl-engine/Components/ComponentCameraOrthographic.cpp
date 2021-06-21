#include "ComponentCameraOrthographic.h"
#include <mini-gl-engine/ComponentColorLine.h>

using namespace aRibeiro;

namespace GLEngine {

    namespace Components {

        ComponentType ComponentCameraOrthographic::Type = "ComponentCameraOrthographic";

        ComponentCameraOrthographic::ComponentCameraOrthographic():ComponentCamera(ComponentCameraOrthographic::Type) {
            
            nearPlane = -1000.0f;
            farPlane = 1000.0f;
            
            leftHanded = true;
            rightHanded = false;

            useSizeY = false;
            sizeY = 1.0f;

            useSizeX = false;
            sizeX = 1.0f;
            
            AppBase* appBase = Engine::Instance()->app;

            appBase->WindowSize.OnChange.add(this, &ComponentCameraOrthographic::OnWindowSizeChanged);
            
            //call the first setup
            OnWindowSizeChanged(&appBase->WindowSize);
            
            nearPlane.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraFloatParameter);
            farPlane.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraFloatParameter);

            sizeY.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraFloatParameter);
            sizeX.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraFloatParameter);

            useSizeY.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraBoolParameter);
            useSizeX.OnChange.add(this, &ComponentCameraOrthographic::OnUpdateCameraBoolParameter);
            
            leftHanded.OnChange.add(this, &ComponentCameraOrthographic::OnLeftHandedChanged);
            rightHanded.OnChange.add(this, &ComponentCameraOrthographic::OnRightHandedChanged);
        }

        ComponentCameraOrthographic::~ComponentCameraOrthographic() {
            AppBase* appBase = Engine::Instance()->app;
            appBase->WindowSize.OnChange.remove(this, &ComponentCameraOrthographic::OnWindowSizeChanged);
        }

        void ComponentCameraOrthographic::OnUpdateCameraFloatParameter(Property<float> *prop) {
            configureProjection();
        }

        void ComponentCameraOrthographic::OnUpdateCameraBoolParameter(aRibeiro::Property<bool> *prop) {
            configureProjection();
        }

        void ComponentCameraOrthographic::OnWindowSizeChanged(Property<sf::Vector2i> *prop) {
            configureProjection();
        }

        void ComponentCameraOrthographic::OnLeftHandedChanged(Property<bool> *prop) {
            rightHanded.value = ! prop->value;
            configureProjection();
        }

        void ComponentCameraOrthographic::OnRightHandedChanged(Property<bool> *prop) {
            leftHanded.value = ! prop->value;
            configureProjection();
        }

        void ComponentCameraOrthographic::configureProjection() {
            AppBase* appBase = Engine::Instance()->app;
            sf::Vector2i size = appBase->WindowSize;
            viewport = GLEngine::iRect(size.x,size.y);

            float x = (float)viewport.w;
            float y = (float)viewport.h;

            if (useSizeY.value || useSizeX.value){
                x = sizeX.value;
                y = sizeY.value;

                if (!useSizeY.value) {
                    float aspectY = (float)viewport.h/(float)viewport.w;
                    y = x * aspectY;
                } else if (!useSizeX.value) {
                    float aspectX = (float)viewport.w/(float)viewport.h;
                    x = y * aspectX;
                }
            }

            if (rightHanded)
                projection = projection_ortho_rh_negative_one(
                    - x / 2.0f, 
                    x / 2.0f, 
                    - y / 2.0f, 
                    y / 2.0f,
                    nearPlane, farPlane);
            else
                projection = projection_ortho_lh_negative_one(
                    - x / 2.0f, 
                    x / 2.0f, 
                    - y / 2.0f, 
                    y / 2.0f, 
                    nearPlane, farPlane);
        }

        /*
        void ComponentCameraOrthographic::precomputeViewProjection(bool useVisitedFlag) {
            viewProjection = projection * transform->getMatrixInverse(useVisitedFlag);
            
            view = transform->getMatrixInverse(useVisitedFlag);
            viewInv = transform->getMatrix(useVisitedFlag);
            viewIT = transpose(viewInv);
            
        }
        */

        void ComponentCameraOrthographic::addLinesComponent() {

            ComponentColorLine *lines = (ComponentColorLine *)transform[0]->findComponent(ComponentColorLine::Type);
            if (lines == NULL)
                lines = (ComponentColorLine *)transform[0]->addComponent(new ComponentColorLine());
            
            lines->vertices.clear();
            
            collision::Frustum frustum(projection);
            
            vec3 ptnInPlane =
            (
            frustum.vertices[0]+frustum.vertices[1]+
            frustum.vertices[4]+frustum.vertices[5]
            )*0.25f;
            lines->vertices.push_back(ptnInPlane);
            lines->vertices.push_back(ptnInPlane + frustum.rightPlane.normal * 0.2f);
            
            ptnInPlane =
            (
            frustum.vertices[0]+frustum.vertices[3]+
            frustum.vertices[4]+frustum.vertices[7]
            )*0.25f;
            lines->vertices.push_back(ptnInPlane);
            lines->vertices.push_back(ptnInPlane + frustum.topPlane.normal * 0.2f);
            
            ptnInPlane =
            (
            frustum.vertices[0]+frustum.vertices[1]+
            frustum.vertices[2]+frustum.vertices[3]
            )*0.25f;
            lines->vertices.push_back(ptnInPlane);
            lines->vertices.push_back(ptnInPlane + frustum.nearPlane.normal * 0.2f);
            
            lines->vertices.push_back(frustum.vertices[0]);
            lines->vertices.push_back(frustum.vertices[1]);
            
            lines->vertices.push_back(frustum.vertices[1]);
            lines->vertices.push_back(frustum.vertices[2]);
            
            lines->vertices.push_back(frustum.vertices[2]);
            lines->vertices.push_back(frustum.vertices[3]);
            
            lines->vertices.push_back(frustum.vertices[3]);
            lines->vertices.push_back(frustum.vertices[0]);
            
            lines->vertices.push_back(frustum.vertices[0+4]);
            lines->vertices.push_back(frustum.vertices[1+4]);
            
            lines->vertices.push_back(frustum.vertices[1+4]);
            lines->vertices.push_back(frustum.vertices[2+4]);
            
            lines->vertices.push_back(frustum.vertices[2+4]);
            lines->vertices.push_back(frustum.vertices[3+4]);
            
            lines->vertices.push_back(frustum.vertices[3+4]);
            lines->vertices.push_back(frustum.vertices[0+4]);
            
            lines->vertices.push_back(frustum.vertices[0]);
            lines->vertices.push_back(frustum.vertices[0+4]);
            
            lines->vertices.push_back(frustum.vertices[1]);
            lines->vertices.push_back(frustum.vertices[1+4]);
            
            lines->vertices.push_back(frustum.vertices[2]);
            lines->vertices.push_back(frustum.vertices[2+4]);
            
            lines->vertices.push_back(frustum.vertices[3]);
            lines->vertices.push_back(frustum.vertices[3+4]);
            
            vec3 top = (frustum.vertices[0]+frustum.vertices[3]) * 0.5f;
            top.y *= 2.0f;
            
            lines->vertices.push_back(frustum.vertices[0]);
            lines->vertices.push_back(top);
            
            lines->vertices.push_back(frustum.vertices[3]);
            lines->vertices.push_back(top);
            
            vec3 lp = transform[0]->LocalPosition;
            quat lr = transform[0]->LocalRotation;
            
            transform[0]->Position = vec3(0);
            transform[0]->Rotation = quat();
            
            mat4 & m = transform[0]->worldToLocalMatrix();
            for(int i=0;i<lines->vertices.size();i++){
                lines->vertices[i] = toVec3( m * toPtn4(lines->vertices[i]) );
            }
            
            transform[0]->LocalPosition = lp;
            transform[0]->LocalRotation = lr;
            
            lines->syncVBODynamic();
            
        }


        aRibeiro::vec3 ComponentCameraOrthographic::App2MousePosition() {
            GLEngine::AppBase * app = GLEngine::Engine::Instance()->app;
            aRibeiro::vec2 windowSize_2 = aRibeiro::vec2(app->WindowSize.value.x,app->WindowSize.value.y) * 0.5f;
            aRibeiro::vec3 mousePosition = aRibeiro::vec3(app->MousePos - windowSize_2,0.0f);
            return mousePosition;
        }
    }
}
