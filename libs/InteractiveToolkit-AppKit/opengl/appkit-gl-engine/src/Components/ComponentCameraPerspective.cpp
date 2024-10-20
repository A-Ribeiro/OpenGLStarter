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

                renderWindowRegionRef.reset();

                fovDegrees.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);
                nearPlane.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);
                farPlane.OnChange.add(&ComponentCameraPerspective::OnUpdateCameraFloatParameter, this);

                leftHanded.OnChange.add(&ComponentCameraPerspective::OnLeftHandedChanged, this);
                rightHanded.OnChange.add(&ComponentCameraPerspective::OnRightHandedChanged, this);
            }

            ComponentCameraPerspective::~ComponentCameraPerspective()
            {
                // AppBase* appBase = Engine::Instance()->app;
                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                {
                    renderWindowRegion->CameraViewport.OnChange.remove(&ComponentCameraPerspective::OnViewportChanged, this);
                }
            }

            void ComponentCameraPerspective::start()
            {
                // renderWindowRegion = transform[0]->renderWindowRegion;

                // AppBase* appBase = Engine::Instance()->app;

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                renderWindowRegion->CameraViewport.OnChange.add(&ComponentCameraPerspective::OnViewportChanged, this);

                // call the first setup
                OnViewportChanged(renderWindowRegion->CameraViewport, renderWindowRegion->CameraViewport);
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

                auto renderWindowRegion = ToShared(renderWindowRegionRef);

                if (renderWindowRegion != nullptr)
                    size = MathCore::vec2i(renderWindowRegion->CameraViewport.c_ptr()->w, renderWindowRegion->CameraViewport.c_ptr()->h);
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

            void ComponentCameraPerspective::attachToTransform(std::shared_ptr<Transform> t)
            {
                renderWindowRegionRef = t->renderWindowRegion;
            }
            void ComponentCameraPerspective::detachFromTransform(std::shared_ptr<Transform> t)
            {
                auto renderWindowRegion = ToShared(renderWindowRegionRef);
                if (renderWindowRegion != nullptr)
                    renderWindowRegion->CameraViewport.OnChange.remove(&ComponentCameraPerspective::OnViewportChanged, this);
                renderWindowRegionRef.reset();
            }

            // always clone
            std::shared_ptr<Component> ComponentCameraPerspective::duplicate_ref_or_clone(bool force_clone){
                auto result = Component::CreateShared<ComponentCameraPerspective>();

                result->nearPlane.setValueNoCallback(this->nearPlane);
                result->farPlane.setValueNoCallback(this->farPlane);

                result->leftHanded.setValueNoCallback(this->leftHanded);
                result->rightHanded.setValueNoCallback(this->rightHanded);

                result->fovDegrees.setValueNoCallback(this->fovDegrees);

                // configureProjection() is called once the start command is called

                return result;
            }

            void ComponentCameraPerspective::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer){
                writer.StartObject();
                writer.String("type");
                writer.String(ComponentCameraPerspective::Type);
                writer.String("id");
                writer.Uint64((intptr_t)self().get());
                writer.EndObject();
                
            }
            void ComponentCameraPerspective::Deserialize(rapidjson::Value &_value, std::unordered_map<intptr_t, std::shared_ptr<Transform>> &transform_map, std::unordered_map<intptr_t, std::shared_ptr<Component>> &component_map){
                if (!_value.HasMember("type") || !_value["type"].IsString())
                    return;
                if (!strcmp(_value["type"].GetString(), ComponentCameraPerspective::Type) == 0)
                    return;
                
            }
        }
    }

}