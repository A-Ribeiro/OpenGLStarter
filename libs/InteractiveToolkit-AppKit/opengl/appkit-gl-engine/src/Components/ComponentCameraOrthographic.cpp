#include <appkit-gl-engine/Components/ComponentCameraOrthographic.h>

#include <appkit-gl-engine/Components/deprecated/ComponentColorLine.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            ComponentType ComponentCameraOrthographic::Type = "ComponentCameraOrthographic";

            ComponentCameraOrthographic::ComponentCameraOrthographic() : ComponentCamera(ComponentCameraOrthographic::Type)
            {

                nearPlane = -1000.0f;
                farPlane = 1000.0f;

                leftHanded = true;
                rightHanded = false;

                useSizeY = false;
                sizeY = 1.0f;

                useSizeX = false;
                sizeX = 1.0f;

                renderWindowRegionRef.reset();// = nullptr;

                // renderWindowRegion = transform[0]->renderWindowRegion;

                // //AppBase* appBase = Engine::Instance()->app;

                // //appBase->WindowSize.OnChange.add(this, &ComponentCameraOrthographic::OnWindowSizeChanged);

                // //call the first setup
                // //OnWindowSizeChanged(&appBase->WindowSize);

                // renderWindowRegion->Viewport.OnChange.add(this, &ComponentCameraOrthographic::OnViewportChanged);

                // //call the first setup
                // OnViewportChanged(&renderWindowRegion->Viewport);

                nearPlane.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraFloatParameter, this);
                farPlane.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraFloatParameter, this);

                sizeY.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraFloatParameter, this);
                sizeX.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraFloatParameter, this);

                useSizeY.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraBoolParameter, this);
                useSizeX.OnChange.add(&ComponentCameraOrthographic::OnUpdateCameraBoolParameter, this);

                leftHanded.OnChange.add(&ComponentCameraOrthographic::OnLeftHandedChanged, this);
                rightHanded.OnChange.add(&ComponentCameraOrthographic::OnRightHandedChanged, this);
            }

            ComponentCameraOrthographic::~ComponentCameraOrthographic()
            {
                // AppBase* appBase = Engine::Instance()->app;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                {
                    renderWindowRegion->CameraViewport.OnChange.remove(&ComponentCameraOrthographic::OnViewportChanged, this);
                }
            }

            void ComponentCameraOrthographic::start()
            {
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                //renderWindowRegion = transform[0]->renderWindowRegion;
                renderWindowRegion->CameraViewport.OnChange.add(&ComponentCameraOrthographic::OnViewportChanged, this);
                // call the first setup
                OnViewportChanged(renderWindowRegion->CameraViewport, renderWindowRegion->CameraViewport);
            }

            void ComponentCameraOrthographic::OnUpdateCameraFloatParameter(const float &value, const float &oldValue)
            {
                configureProjection();
            }

            void ComponentCameraOrthographic::OnUpdateCameraBoolParameter(const bool &value, const bool &oldValue)
            {
                configureProjection();
            }

            void ComponentCameraOrthographic::OnViewportChanged(const iRect &value, const iRect &oldValue)
            {
                configureProjection();
            }

            void ComponentCameraOrthographic::OnLeftHandedChanged(const bool &value, const bool &oldValue)
            {
                rightHanded.setValueNoCallback(!value);
                configureProjection();
            }

            void ComponentCameraOrthographic::OnRightHandedChanged(const bool &value, const bool &oldValue)
            {
                leftHanded.setValueNoCallback(!value);
                configureProjection();
            }

            void ComponentCameraOrthographic::configureProjection()
            {
                // AppBase* appBase = Engine::Instance()->app;
                MathCore::vec2i size;

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                    size = MathCore::vec2i(renderWindowRegion->CameraViewport.c_ptr()->w, renderWindowRegion->CameraViewport.c_ptr()->h);
                else
                    size = Engine::Instance()->app->window->getSize();

                viewport = AppKit::GLEngine::iRect(size.width, size.height);

                float x = (float)viewport.w;
                float y = (float)viewport.h;

                if (useSizeY.c_val() || useSizeX.c_val())
                {
                    x = sizeX.c_val();
                    y = sizeY.c_val();

                    if (!useSizeY.c_val())
                    {
                        float aspectY = (float)viewport.h / (float)viewport.w;
                        y = x * aspectY;
                    }
                    else if (!useSizeX.c_val())
                    {
                        float aspectX = (float)viewport.w / (float)viewport.h;
                        x = y * aspectX;
                    }
                }

                if (rightHanded)
                    projection = MathCore::GEN<MathCore::mat4f>::projection_ortho_rh_negative_one(
                        -x / 2.0f,
                        x / 2.0f,
                        -y / 2.0f,
                        y / 2.0f,
                        nearPlane, farPlane);
                else
                    projection = MathCore::GEN<MathCore::mat4f>::projection_ortho_lh_negative_one(
                        -x / 2.0f,
                        x / 2.0f,
                        -y / 2.0f,
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

            void ComponentCameraOrthographic::addLinesComponent()
            {
                auto transform = getTransform();

                auto lines = transform->findComponent<ComponentColorLine>();
                if (lines == nullptr)
                    lines = transform->addNewComponent<ComponentColorLine>();

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

                MathCore::vec3f lp = transform->LocalPosition;
                MathCore::quatf lr = transform->LocalRotation;

                transform->Position = MathCore::vec3f(0);
                transform->Rotation = MathCore::quatf();

                MathCore::mat4f &m = transform->worldToLocalMatrix();
                for (int i = 0; i < lines->vertices.size(); i++)
                {
                    lines->vertices[i] = MathCore::CVT<MathCore::vec4f>::toVec3(m * MathCore::CVT<MathCore::vec3f>::toPtn4(lines->vertices[i]));
                }

                transform->LocalPosition = lp;
                transform->LocalRotation = lr;

                lines->syncVBODynamic();
            }

            // MathCore::vec3f ComponentCameraOrthographic::App2MousePosition() {
            //     AppKit::GLEngine::AppBase * app = AppKit::GLEngine::Engine::Instance()->app;
            //     MathCore::vec2f windowSize_2 = MathCore::vec2f(app->WindowSize.value.x,app->WindowSize.value.y) * 0.5f;
            //     MathCore::vec3f mousePosition = MathCore::vec3f(app->MousePos - windowSize_2,0.0f);
            //     return mousePosition;
            // }
        }
    }

}