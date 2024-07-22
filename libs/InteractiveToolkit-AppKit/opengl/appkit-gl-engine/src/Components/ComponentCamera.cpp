#include <appkit-gl-engine/Components/ComponentCamera.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            ComponentCamera::ComponentCamera(ComponentType type) : Component(type)
            {
            }

            ComponentCamera::~ComponentCamera()
            {
            }

            void ComponentCamera::precomputeViewProjection(bool useVisitedFlag)
            {
                auto transform = getTransform();

                viewProjection = projection * transform->getMatrixInverse(useVisitedFlag);

                view = transform->getMatrixInverse(useVisitedFlag);
                viewInv = transform->getMatrix(useVisitedFlag);
                viewIT = MathCore::OP<MathCore::mat4f>::transpose(viewInv);
            }

        }
    }
}