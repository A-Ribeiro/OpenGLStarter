#include "MaskCommon.h"

#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/ResourceMap.h>

#include "../components/ui/ComponentRectangle.h"

#include <appkit-gl-engine/Components/Core/ComponentCameraPerspective.h>
#include <appkit-gl-engine/Components/Core/ComponentCameraOrthographic.h>


namespace AppKit
{
    namespace GLEngine
    {

        void AddShaderRectangleMask::mask_query_uniform_locations_and_set_default_values()
        {
            u_transform_to_mask = getUniformLocation("uTransformToMask");
            u_mask_corner = getUniformLocation("uMask_corner");
            u_mask_radius = getUniformLocation("uMask_radius");

            setUniform(u_transform_to_mask, uTransformToMask);
            setUniform(u_mask_radius, uMask_radius);
            // array uniform upload
            if (u_mask_corner >= 0)
                OPENGL_CMD(glUniform2fv(u_mask_corner, (GLsizei)4, uMask_corner[0].array));

        }

        void AddShaderRectangleMask::setMaskFromPropertyBag(const AppKit::GLEngine::Utils::ShaderPropertyBag &materialBag)
        {
            auto componentRectangleRaw = materialBag.getProperty<std::weak_ptr<Component>>("ComponentRectangle").lock();
            auto componentCameraRaw = materialBag.getProperty<std::weak_ptr<Component>>("ComponentCamera").lock();

            if (componentRectangleRaw != nullptr && componentCameraRaw != nullptr)
            {
                auto componentRectangle = (Components::ComponentRectangle *)(componentRectangleRaw.get());
                auto componentRectangle_transform = componentRectangle->getTransform().get();

                setMask_Corner_Radius(componentRectangle->mask_radius);
                setMask_Corner_Centers(componentRectangle->mask_corner);

                // mask screen to local space mapping
                if (componentRectangle_transform != nullptr)
                {
                    auto camera = (Components::ComponentCamera *)componentCameraRaw.get();

                    MathCore::mat4f *mvp;
                    MathCore::mat4f *mv;
                    MathCore::mat4f *mvIT;
                    MathCore::mat4f *mvInv;
                    componentRectangle_transform->computeRenderMatrix(camera->viewProjection, camera->view, camera->viewIT, camera->viewInv,
                                                                      &mvp, &mv, &mvIT, &mvInv);
                    MathCore::vec3f scale = 2.0f / MathCore::vec3f(camera->viewport.w, camera->viewport.h, 2.0f);
                    MathCore::mat4f transform_mask = mvp->inverse() *
                                                     MathCore::GEN<MathCore::mat4f>::translateHomogeneous(-1.0f, -1.0f, 0.0f) *
                                                     MathCore::GEN<MathCore::mat4f>::scaleHomogeneous(scale);

                    setMask_ScreenToLocalTransform(transform_mask);
                }
            }
        }

        void AddShaderRectangleMask::setMask_ScreenToLocalTransform(const MathCore::mat4f &uTransformToMask_p)
        {
            if (uTransformToMask != uTransformToMask_p)
            {
                uTransformToMask = uTransformToMask_p;
                setUniform(u_transform_to_mask, uTransformToMask);
            }
        }
        void AddShaderRectangleMask::setMask_Corner_Centers(const MathCore::vec2f uMask_corner_p[4])
        {
            bool needs_set = false;
            for (int i = 0; i < 4; i++)
            {
                if (uMask_corner[i] != uMask_corner_p[i])
                {
                    uMask_corner[i] = uMask_corner_p[i];
                    needs_set = true;
                }
            }

            // array uniform upload
            if (u_mask_corner >= 0 && needs_set)
                OPENGL_CMD(glUniform2fv(u_mask_corner, (GLsizei)4, uMask_corner[0].array));
        }
        void AddShaderRectangleMask::setMask_Corner_Radius(const MathCore::vec4f &uMask_radius_p)
        {
            if (this->uMask_radius != uMask_radius_p)
            {
                this->uMask_radius = uMask_radius_p;
                setUniform(u_mask_radius, this->uMask_radius);
            }
        }

        Utils::ShaderPropertyBag AddShaderRectangleMask::mask_default_bag() const
        {
            Utils::ShaderPropertyBag bag;

            bag.addProperty("ComponentRectangle", std::weak_ptr<Component>());
            bag.addProperty("ComponentCamera", std::weak_ptr<Component>());

            return bag;
        }


}
}
