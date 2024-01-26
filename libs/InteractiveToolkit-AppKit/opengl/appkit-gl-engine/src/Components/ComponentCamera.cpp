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
                viewProjection = projection * transform[0]->getMatrixInverse(useVisitedFlag);

                view = transform[0]->getMatrixInverse(useVisitedFlag);
                viewInv = transform[0]->getMatrix(useVisitedFlag);
                viewIT = MathCore::OP<MathCore::mat4f>::transpose(viewInv);
            }

        }
    }
}