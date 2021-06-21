#include "ComponentCameraPerspective.h"
#include <mini-gl-engine/ComponentColorLine.h>
#include <mini-gl-engine/ComponentColorMesh.h>

using namespace aRibeiro;

namespace GLEngine {

    namespace Components {

        ComponentType ComponentCameraPerspective::Type = "ComponentCameraPerspective";

        ComponentCameraPerspective::ComponentCameraPerspective():ComponentCamera(ComponentCameraPerspective::Type) {
            
            fovDegrees = 60.0f;
            nearPlane = 0.01f;
            farPlane = 1000.0f;
            
            leftHanded = true;
            rightHanded = false;
            
            AppBase* appBase = Engine::Instance()->app;

            appBase->WindowSize.OnChange.add(this, &ComponentCameraPerspective::OnWindowSizeChanged);
            
            //call the first setup
            OnWindowSizeChanged(&appBase->WindowSize);
            
            fovDegrees.OnChange.add(this, &ComponentCameraPerspective::OnUpdateCameraFloatParameter);
            nearPlane.OnChange.add(this, &ComponentCameraPerspective::OnUpdateCameraFloatParameter);
            farPlane.OnChange.add(this, &ComponentCameraPerspective::OnUpdateCameraFloatParameter);
            
            leftHanded.OnChange.add(this, &ComponentCameraPerspective::OnLeftHandedChanged);
            rightHanded.OnChange.add(this, &ComponentCameraPerspective::OnRightHandedChanged);
        }

        ComponentCameraPerspective::~ComponentCameraPerspective() {
            AppBase* appBase = Engine::Instance()->app;
            appBase->WindowSize.OnChange.remove(this, &ComponentCameraPerspective::OnWindowSizeChanged);
        }

        void ComponentCameraPerspective::OnUpdateCameraFloatParameter(Property<float> *prop) {
            configureProjection();
        }

        void ComponentCameraPerspective::OnWindowSizeChanged(Property<sf::Vector2i> *prop) {
            configureProjection();
        }

        void ComponentCameraPerspective::OnLeftHandedChanged(Property<bool> *prop) {
            rightHanded.value = ! prop->value;
            configureProjection();
        }

        void ComponentCameraPerspective::OnRightHandedChanged(Property<bool> *prop) {
            leftHanded.value = ! prop->value;
            configureProjection();
        }

        void ComponentCameraPerspective::configureProjection() {
            AppBase* appBase = Engine::Instance()->app;
            sf::Vector2i size = appBase->WindowSize;
            viewport = GLEngine::iRect(size.x,size.y);
            if (rightHanded)
                projection = projection_perspective_rh_negative_one(fovDegrees, (float)size.x / (float)size.y, nearPlane, farPlane);
            else
                projection = projection_perspective_lh_negative_one(fovDegrees, (float)size.x / (float)size.y, nearPlane, farPlane);
        }

/*
        void ComponentCameraPerspective::precomputeViewProjection(bool useVisitedFlag) {
            viewProjection = projection * transform->getMatrixInverse(useVisitedFlag);
            
            view = transform->getMatrixInverse(useVisitedFlag);
            viewInv = transform->getMatrix(useVisitedFlag);
            viewIT = transpose(viewInv);
            
        }*/

        void ComponentCameraPerspective::addLinesComponent() {


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
    }
}
