#include <appkit-gl-engine/Components/ComponentCameraPerspective.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>
#include <appkit-gl-engine/Components/deprecated/ComponentColorMesh.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            ComponentType ComponentCameraPerspective::Type = "ComponentCameraPerspective";

            ComponentCameraPerspective::ComponentCameraPerspective() : ComponentCamera(ComponentCameraPerspective::Type)
            {

                fovDegrees = 60.0f;
                nearPlane = 0.1f;
                farPlane = 1000.0f;

                leftHanded = true;
                rightHanded = false;

                renderWindowRegion = NULL;

                fovDegrees.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);
                nearPlane.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);
                farPlane.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);

                leftHanded.OnChange.add(&ComponentCameraPerspective::OnLeftHandedChanged, this);
                rightHanded.OnChange.add(&ComponentCameraPerspective::OnRightHandedChanged, this);
            }

            ComponentCameraPerspective::~ComponentCameraPerspective()
            {
                // AppBase* appBase = Engine::Instance()->app;
                if (renderWindowRegion != NULL)
                {
                    renderWindowRegion->Viewport.OnChange.remove(&ComponentCameraPerspective::OnViewportChanged, this);
                }
            }

            void ComponentCameraPerspective::start()
            {
                // renderWindowRegion = transform[0]->renderWindowRegion;

                // AppBase* appBase = Engine::Instance()->app;

                renderWindowRegion->Viewport.OnChange.add(&ComponentCameraPerspective::OnViewportChanged, this);

                // call the first setup
                OnViewportChanged(renderWindowRegion->Viewport, renderWindowRegion->Viewport);
            }

            void ComponentCameraPerspective::OnUpdateCameraFloatParameter(const float &value, const float &oldValue)
            {
                configureProjection();
            }

            void ComponentCameraPerspective::OnViewportChanged(const iRect &value, const iRect &oldValue)
            {
                configureProjection();
            }

            void ComponentCameraPerspective::OnLeftHandedChanged(const bool &value, const bool &oldValue)
            {
                rightHanded.setValueNoCallback(!value);
                configureProjection();
            }

            void ComponentCameraPerspective::OnRightHandedChanged(const bool &value, const bool &oldValue)
            {
                leftHanded.setValueNoCallback(!value);
                configureProjection();
            }

            void ComponentCameraPerspective::configureProjection()
            {
                // AppBase* appBase = Engine::Instance()->app;
                MathCore::vec2i size;

                if (renderWindowRegion != NULL)
                    size = MathCore::vec2i(renderWindowRegion->Viewport.c_ptr()->w, renderWindowRegion->Viewport.c_ptr()->h);
                else
                    size = Engine::Instance()->app->window->getSize();

                viewport = AppKit::GLEngine::iRect(size.width, size.height);
                if (rightHanded)
                    projection = MathCore::GEN<MathCore::mat4f>::projection_perspective_rh_negative_one(fovDegrees, (float)size.x / (float)size.y, nearPlane, farPlane);
                else
                    projection = MathCore::GEN<MathCore::mat4f>::projection_perspective_lh_negative_one(fovDegrees, (float)size.x / (float)size.y, nearPlane, farPlane);
            }

            /*
                    void ComponentCameraPerspective::precomputeViewProjection(bool useVisitedFlag) {
                        viewProjection = projection * transform->getMatrixInverse(useVisitedFlag);

                        view = transform->getMatrixInverse(useVisitedFlag);
                        viewInv = transform->getMatrix(useVisitedFlag);
                        viewIT = transpose(viewInv);

                    }*/

            void ComponentCameraPerspective::addLinesComponent()
            {

                ComponentColorLine *lines = (ComponentColorLine *)transform[0]->findComponent(ComponentColorLine::Type);
                if (lines == NULL)
                    lines = (ComponentColorLine *)transform[0]->addComponent(new ComponentColorLine());

                lines->vertices.clear();

                CollisionCore::Frustum<MathCore::vec3f> frustum(projection);

                MathCore::vec3f ptnInPlane =
                    (frustum.vertices[0] + frustum.vertices[1] +
                     frustum.vertices[4] + frustum.vertices[5]) *
                    0.25f;
                lines->vertices.push_back(ptnInPlane);
                lines->vertices.push_back(ptnInPlane + frustum.planes[CollisionCore::FrustumPlane::Right].normal * 0.2f);

                ptnInPlane =
                    (frustum.vertices[0] + frustum.vertices[3] +
                     frustum.vertices[4] + frustum.vertices[7]) *
                    0.25f;
                lines->vertices.push_back(ptnInPlane);
                lines->vertices.push_back(ptnInPlane + frustum.planes[CollisionCore::FrustumPlane::Top].normal * 0.2f);

                ptnInPlane =
                    (frustum.vertices[0] + frustum.vertices[1] +
                     frustum.vertices[2] + frustum.vertices[3]) *
                    0.25f;
                lines->vertices.push_back(ptnInPlane);
                lines->vertices.push_back(ptnInPlane + frustum.planes[CollisionCore::FrustumPlane::Near].normal * 0.2f);

                lines->vertices.push_back(frustum.vertices[0]);
                lines->vertices.push_back(frustum.vertices[1]);

                lines->vertices.push_back(frustum.vertices[1]);
                lines->vertices.push_back(frustum.vertices[2]);

                lines->vertices.push_back(frustum.vertices[2]);
                lines->vertices.push_back(frustum.vertices[3]);

                lines->vertices.push_back(frustum.vertices[3]);
                lines->vertices.push_back(frustum.vertices[0]);

                lines->vertices.push_back(frustum.vertices[0 + 4]);
                lines->vertices.push_back(frustum.vertices[1 + 4]);

                lines->vertices.push_back(frustum.vertices[1 + 4]);
                lines->vertices.push_back(frustum.vertices[2 + 4]);

                lines->vertices.push_back(frustum.vertices[2 + 4]);
                lines->vertices.push_back(frustum.vertices[3 + 4]);

                lines->vertices.push_back(frustum.vertices[3 + 4]);
                lines->vertices.push_back(frustum.vertices[0 + 4]);

                lines->vertices.push_back(frustum.vertices[0]);
                lines->vertices.push_back(frustum.vertices[0 + 4]);

                lines->vertices.push_back(frustum.vertices[1]);
                lines->vertices.push_back(frustum.vertices[1 + 4]);

                lines->vertices.push_back(frustum.vertices[2]);
                lines->vertices.push_back(frustum.vertices[2 + 4]);

                lines->vertices.push_back(frustum.vertices[3]);
                lines->vertices.push_back(frustum.vertices[3 + 4]);

                MathCore::vec3f top = (frustum.vertices[0] + frustum.vertices[3]) * 0.5f;
                top.y *= 2.0f;

                lines->vertices.push_back(frustum.vertices[0]);
                lines->vertices.push_back(top);

                lines->vertices.push_back(frustum.vertices[3]);
                lines->vertices.push_back(top);

                MathCore::vec3f lp = transform[0]->LocalPosition;
                MathCore::quatf lr = transform[0]->LocalRotation;

                transform[0]->Position = MathCore::vec3f(0);
                transform[0]->Rotation = MathCore::quatf();

                MathCore::mat4f &m = transform[0]->worldToLocalMatrix();
                for (int i = 0; i < lines->vertices.size(); i++)
                {
                    lines->vertices[i] = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(lines->vertices[i]));
                }

                transform[0]->LocalPosition = lp;
                transform[0]->LocalRotation = lr;

                lines->syncVBODynamic();
            }
        }
    }

}