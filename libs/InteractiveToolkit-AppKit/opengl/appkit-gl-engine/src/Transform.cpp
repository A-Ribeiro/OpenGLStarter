#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>
#include <InteractiveToolkit/ITKCommon/STL_Tools.h>

// #include <appkit-gl-engine/SharedPointer/SharedPointerDatabase.h>

// int stat_num_visited;
// int stat_num_recalculated;
// int stat_draw_recalculated;

namespace AppKit
{
    namespace GLEngine
    {

        Transform::Transform(const Transform &v) :

                                                   // Parent(
                                                   //    EventCore::CallbackWrapper(&Transform::getParent, this),
                                                   //    EventCore::CallbackWrapper(&Transform::setParent, this)),

                                                   LocalPosition(
                                                       EventCore::CallbackWrapper(&Transform::getLocalPosition, this),
                                                       EventCore::CallbackWrapper(&Transform::setLocalPosition, this)),
                                                   // LocalEuler(this, &Transform::getLocalEuler, &Transform::setLocalEuler),
                                                   LocalRotation(
                                                       EventCore::CallbackWrapper(&Transform::getLocalRotation, this),
                                                       EventCore::CallbackWrapper(&Transform::setLocalRotation, this)),
                                                   LocalScale(
                                                       EventCore::CallbackWrapper(&Transform::getLocalScale, this),
                                                       EventCore::CallbackWrapper(&Transform::setLocalScale, this)),

                                                   Position(
                                                       // EventCore::CallbackWrapper(&Transform::getPosition, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getPosition, this),
                                                       EventCore::CallbackWrapper(&Transform::setPosition, this)),
                                                   // Euler(this, &Transform::getEuler, &Transform::setEuler),
                                                   Rotation(
                                                       // EventCore::CallbackWrapper(&Transform::getRotation, this),
                                                       EventCore::Callback<MathCore::quatf()>(&Transform::getRotation, this),
                                                       EventCore::CallbackWrapper(&Transform::setRotation, this)),
                                                   Scale(
                                                       // EventCore::CallbackWrapper(&Transform::getScale, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getScale, this),
                                                       EventCore::CallbackWrapper(&Transform::setScale, this)),

                                                   Name(
                                                       EventCore::CallbackWrapper(&Transform::getName, this),
                                                       EventCore::CallbackWrapper(&Transform::setName, this))
        {

            userData = nullptr;
        }
        void Transform::operator=(const Transform &v) {}

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Node Hierarchy structure and operations
        //
        //
        //
        ///////////////////////////////////////////////////////

        std::shared_ptr<Transform> Transform::removeChild(int index)
        {

            // ITK_ABORT((index >= children.size() || index < 0), "Trying to remove a child that is not in the list...\n");
            // if (index >= children.size() || index < 0)
            //{
            //     fprintf(stderr, "Trying to remove a child that is not in the list...\n");
            //     exit(-1);
            //     return nullptr;
            // }

            auto node = children[index];
            children.erase(children.begin() + index);

            // node->setParent(nullptr);
            node->mParent.reset(); // = std::shared_ptr<Transform>(nullptr);
            node->visited = false;

            // removeMapName(node);

            return node;
        }
        std::shared_ptr<Transform> Transform::removeChild(std::shared_ptr<Transform> transform)
        {
            for (int i = 0; i < children.size(); i++)
            {
                if (children[i] == transform)
                {
                    children.erase(children.begin() + i);

                    // transform->setParent(nullptr);
                    transform->mParent.reset();

                    transform->visited = false;

                    // removeMapName(transform);

                    return transform;
                }
            }

            // ITK_ABORT(true, "Trying to remove a child that is not in the scene...\n");

            // fprintf(stderr,"Trying to remove a child that is not in the scene...\n");
            // exit(-1);
            return nullptr;
        }

        std::shared_ptr<Transform> Transform::addChild(std::shared_ptr<Transform> transform, std::shared_ptr<Transform> before_transform)
        {

            auto _currentParent = transform->getParent();
            if (_currentParent != nullptr)
                _currentParent->removeChild(transform);

            auto _self = this->self();
            // transform->setParent(_self);
            transform->mParent = _self;

            transform->visited = false;

            if (before_transform != nullptr)
            {
                children.insert(
                    std::find(children.begin(), children.end(), before_transform),
                    transform);
            }
            else
                children.push_back(transform);

            transform->renderWindowRegion = this->renderWindowRegion;

            // insertMapName(transform);

            return transform;
        }

        // std::vector<Transform*> &Transform::getChildren() {
        //     return children;
        // }

        int Transform::getChildCount()
        {
            return (int)children.size();
        }

        std::shared_ptr<Transform> Transform::getChildAt(int i)
        {
            if (i >= 0 && i < children.size())
                return children[i];
            return nullptr;
        }

        void Transform::clearChildren()
        {
            for (int i = getChildCount() - 1; i >= 0; i--)
                removeChild(i);
            // for(int i= getComponentCount()-1;i>=0;i--)
            //     removeComponentAt(i);

            // renderWindowRegion.reset();// = nullptr;
        }

        std::shared_ptr<Transform> Transform::getParent()
        {
            return ToShared(mParent);
        }

        // void Transform::setParent(Transform *const &prnt)
        void Transform::setParent(std::shared_ptr<Transform> new_parent)
        {
            auto self = this->self();
            auto currentParent = this->getParent();
            if (currentParent != nullptr)
                currentParent->removeChild(self);
            if (new_parent != nullptr)
                new_parent->addChild(self);
        }

        bool Transform::isRoot()
        {
            return this->getParent() == nullptr;
        }

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform default graph operations
        //
        //
        //
        ///////////////////////////////////////////////////////
        void Transform::updateLocalRotationBase()
        {
            if (!localRotationBaseDirty)
                return;
            localRotationBase = MathCore::GEN<MathCore::mat4f>::fromQuat(localRotation);
            localRotationBaseDirty = false;
        }

        MathCore::vec3f Transform::getLocalPosition() const
        {
            return localPosition;
        }
        // MathCore::vec3f Transform::getLocalEuler() const
        // {
        //     return localEuler;
        // }
        MathCore::quatf Transform::getLocalRotation() const
        {
            return localRotation;
        }
        MathCore::vec3f Transform::getLocalScale() const
        {
            return localScale;
        }

        void Transform::setLocalPosition(const MathCore::vec3f &p)
        {
            if (localPosition == p)
                return;

            localPosition = p;

            // if (!localRotationBaseDirty) localRotationBaseDirty = true;
            if (!localMatrixDirty)
                localMatrixDirty = true;
            if (!localMatrixInverseTransposeDirty)
                localMatrixInverseTransposeDirty = true;
            if (!localMatrixInverseDirty)
                localMatrixInverseDirty = true;
        }
        // void Transform::setLocalEuler(const MathCore::vec3f &e)
        // {
        //     if (localEuler == e)
        //         return;

        //     localEuler = e;
        //     if (localEuler.x < 0)
        //         localEuler.x += ceil(-localEuler.x / MathCore::OP<float>::deg_2_rad(360.0f)) * MathCore::OP<float>::deg_2_rad(360.0f);
        //     if (localEuler.x > MathCore::OP<float>::deg_2_rad(360.0f))
        //         localEuler.x = MathCore::OP<float>::fmod(localEuler.x, MathCore::OP<float>::deg_2_rad(360.0f));

        //     if (localEuler.y < 0)
        //         localEuler.y += ceil(-localEuler.y / MathCore::OP<float>::deg_2_rad(360.0f)) * MathCore::OP<float>::deg_2_rad(360.0f);
        //     if (localEuler.y > MathCore::OP<float>::deg_2_rad(360.0f))
        //         localEuler.y = MathCore::OP<float>::fmod(localEuler.y, MathCore::OP<float>::deg_2_rad(360.0f));

        //     if (localEuler.z < 0)
        //         localEuler.z += ceil(-localEuler.z / MathCore::OP<float>::deg_2_rad(360.0f)) * MathCore::OP<float>::deg_2_rad(360.0f);
        //     if (localEuler.z > MathCore::OP<float>::deg_2_rad(360.0f))
        //         localEuler.z = MathCore::OP<float>::fmod(localEuler.z, MathCore::OP<float>::deg_2_rad(360.0f));

        //     localRotation = quatFromEuler(localEuler.x, localEuler.y, localEuler.z);

        //     /*
        //     Unity Euler angles are: yxz

        //     localRotation =
        //     quatFromAxisAngle(MathCore::vec3f(0.0, 1.0, 0.0), localEuler.y) *
        //     quatFromAxisAngle(MathCore::vec3f(1.0, 0.0, 0.0), localEuler.x) *
        //     quatFromAxisAngle(MathCore::vec3f(0.0, 0.0, 1.0), localEuler.z);
        //     */

        //     if (!localRotationBaseDirty)
        //         localRotationBaseDirty = true;
        //     if (!localMatrixDirty)
        //         localMatrixDirty = true;
        //     if (!localMatrixInverseTransposeDirty)
        //         localMatrixInverseTransposeDirty = true;
        //     if (!localMatrixInverseDirty)
        //         localMatrixInverseDirty = true;
        // }
        void Transform::setLocalRotation(const MathCore::quatf &q)
        {
            if (localRotation == q)
                return;

            localRotation = q;

            // MathCore::OP<MathCore::mat4f>::extractEuler(localRotation, &localEuler.x, &localEuler.y, &localEuler.z);

            if (!localRotationBaseDirty)
                localRotationBaseDirty = true;
            if (!localMatrixDirty)
                localMatrixDirty = true;
            if (!localMatrixInverseTransposeDirty)
                localMatrixInverseTransposeDirty = true;
            if (!localMatrixInverseDirty)
                localMatrixInverseDirty = true;
        }
        void Transform::setLocalScale(const MathCore::vec3f &s)
        {
            if (localScale == s)
                return;

            localScale = s;

            // if (!localRotationBaseDirty) localRotationBaseDirty = true;
            if (!localMatrixDirty)
                localMatrixDirty = true;
            if (!localMatrixInverseTransposeDirty)
                localMatrixInverseTransposeDirty = true;
            if (!localMatrixInverseDirty)
                localMatrixInverseDirty = true;
        }

        MathCore::mat4f &Transform::getLocalMatrix()
        {
            if (localMatrixDirty)
            {
                //
                // compute the matrix transform
                //
                updateLocalRotationBase();

                localMatrix = localRotationBase;

                localMatrix.a1 *= localScale.x;
                localMatrix.a2 *= localScale.x;
                localMatrix.a3 *= localScale.x;

                localMatrix.b1 *= localScale.y;
                localMatrix.b2 *= localScale.y;
                localMatrix.b3 *= localScale.y;

                localMatrix.c1 *= localScale.z;
                localMatrix.c2 *= localScale.z;
                localMatrix.c3 *= localScale.z;

                localMatrix.d1 = localPosition.x;
                localMatrix.d2 = localPosition.y;
                localMatrix.d3 = localPosition.z;

                localMatrixDirty = false;
                matrixDirty = true;
                // renderDirty = true;
            }
            return localMatrix;
        }

        MathCore::mat4f &Transform::getLocalMatrixInverseTranspose()
        {
            if (localMatrixInverseTransposeDirty)
            {

                localMatrixInverseTranspose = MathCore::OP<MathCore::mat4f>::transpose(getLocalMatrixInverse());

                /*
                updateLocalRotationBase();

                localMatrixInverseTranspose = localRotationBase;

                MathCore::vec3f t_inv = -localPosition;
                localMatrixInverseTranspose.a4 = localMatrixInverseTranspose.a1 * t_inv.x + localMatrixInverseTranspose.a2 * t_inv.y + localMatrixInverseTranspose.a3 * t_inv.z;
                localMatrixInverseTranspose.b4 = localMatrixInverseTranspose.b1 * t_inv.x + localMatrixInverseTranspose.b2 * t_inv.y + localMatrixInverseTranspose.b3 * t_inv.z;
                localMatrixInverseTranspose.c4 = localMatrixInverseTranspose.c1 * t_inv.x + localMatrixInverseTranspose.c2 * t_inv.y + localMatrixInverseTranspose.c3 * t_inv.z;

                localMatrixInverseTranspose[0] *= 1.0f / localScale.x;
                localMatrixInverseTranspose[1] *= 1.0f / localScale.y;
                localMatrixInverseTranspose[2] *= 1.0f / localScale.z;

                */

                localMatrixInverseTransposeDirty = false;
                matrixInverseTransposeDirty = true;
                // renderDirty = true;
            }
            return localMatrixInverseTranspose;
        }

        MathCore::mat4f &Transform::getLocalMatrixInverse()
        {
            if (localMatrixInverseDirty)
            {
                updateLocalRotationBase();

                localMatrixInverse = MathCore::OP<MathCore::mat4f>::transpose(localRotationBase);

                MathCore::vec3f s_inv = 1.0f / localScale;

                localMatrixInverse.a1 *= s_inv.x;
                localMatrixInverse.a2 *= s_inv.y;
                localMatrixInverse.a3 *= s_inv.z;

                localMatrixInverse.b1 *= s_inv.x;
                localMatrixInverse.b2 *= s_inv.y;
                localMatrixInverse.b3 *= s_inv.z;

                localMatrixInverse.c1 *= s_inv.x;
                localMatrixInverse.c2 *= s_inv.y;
                localMatrixInverse.c3 *= s_inv.z;

                MathCore::vec3f t_inv = -localPosition;

                localMatrixInverse.d1 = localMatrixInverse.a1 * t_inv.x + localMatrixInverse.b1 * t_inv.y + localMatrixInverse.c1 * t_inv.z;
                localMatrixInverse.d2 = localMatrixInverse.a2 * t_inv.x + localMatrixInverse.b2 * t_inv.y + localMatrixInverse.c2 * t_inv.z;
                localMatrixInverse.d3 = localMatrixInverse.a3 * t_inv.x + localMatrixInverse.b3 * t_inv.y + localMatrixInverse.c3 * t_inv.z;

                localMatrixInverseDirty = false;
                matrixInverseDirty = true;
                // renderDirty = true;

                // localMatrixInverse = inv( getLocalMatrix() );

                //[optimization] ->
                //   the inverse transpose dont need to be recalculated...
                //   just need to transpose the inverse
                getLocalMatrixInverseTranspose();
            }
            return localMatrixInverse;
        }

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform Global Ops...
        //
        //
        //
        ///////////////////////////////////////////////////////
        MathCore::mat4f &Transform::getMatrix(bool useVisitedFlag)
        {

            if (useVisitedFlag && visited)
                return matrix;

            // stat_num_recalculated++;

            MathCore::mat4f &localM = getLocalMatrix();
            auto parent = getParent();
            if (parent != nullptr && !parent->isRoot())
            {
                MathCore::mat4f &aux = parent->getMatrix(useVisitedFlag);
                if (matrixParent != aux)
                {
                    matrixParent = aux;
                    matrixDirty = true;
                }
                if (matrixDirty)
                {
                    matrix = matrixParent * localM;
                    matrixDirty = false;
                    // renderDirty = true;
                }
                return matrix;
            }
            else
            {
                if (matrixDirty)
                {
                    matrix = localM;
                    matrixDirty = false;
                    // renderDirty = true;
                }
                return matrix;
            }
        }

        MathCore::mat4f &Transform::getMatrixInverseTranspose(bool useVisitedFlag)
        {

            if (useVisitedFlag && visited)
                return matrixInverseTranspose;

            MathCore::mat4f &inverse = getMatrixInverse(useVisitedFlag);
            if (matrixInverseTransposeDirty)
            {
                matrixInverseTranspose = MathCore::OP<MathCore::mat4f>::transpose(inverse);
                matrixInverseTransposeDirty = false;
            }

            return matrixInverseTranspose;

            /*
            MathCore::mat4f &localM = getLocalMatrixInverseTranspose();
            Transform* parent = getParent();
            if (parent != nullptr && !parent->isRoot()) {

                MathCore::mat4f &aux = parent->getMatrixInverseTranspose(useVisitedFlag);
                if (matrixInverseTransposeParent != aux) {
                    matrixInverseTransposeParent = aux;
                    matrixInverseTransposeDirty = true;
                }
                if (matrixInverseTransposeDirty) {
                    matrixInverseTranspose = matrixInverseTransposeParent * localM;
                    matrixInverseTransposeDirty = false;
                    //renderDirty = true;
                }
                return matrixInverseTranspose;
            } else{
                if (matrixInverseTransposeDirty) {
                    matrixInverseTranspose = localM;
                    matrixInverseTransposeDirty = false;
                    //renderDirty = true;
                }
                return matrixInverseTranspose;
            }
            */
        }

        MathCore::mat4f &Transform::getMatrixInverse(bool useVisitedFlag)
        {

            if (useVisitedFlag && visited)
                return matrixInverse;

            MathCore::mat4f &localM = getLocalMatrixInverse();
            auto parent = getParent();
            if (parent != nullptr && !parent->isRoot())
            {
                MathCore::mat4f &aux = parent->getMatrixInverse(useVisitedFlag);
                if (matrixInverseParent != aux)
                {
                    matrixInverseParent = aux;
                    matrixInverseDirty = true;
                }
                if (matrixInverseDirty)
                {
                    matrixInverse = localM * matrixInverseParent;
                    matrixInverseDirty = false;

                    matrixInverseTransposeDirty = true;
                    // renderDirty = true;
                }
                return matrixInverse;
            }
            else
            {
                if (matrixInverseDirty)
                {
                    matrixInverse = localM;
                    matrixInverseDirty = false;

                    matrixInverseTransposeDirty = true;
                    // renderDirty = true;
                }
                return matrixInverse;
            }
        }

        void Transform::setPosition(const MathCore::vec3f &pos)
        {
            auto parent = getParent();
            if (parent != nullptr && !parent->isRoot())
                setLocalPosition(
                    MathCore::CVT<MathCore::vec4f>::toVec3(
                        parent->getMatrixInverse() * MathCore::CVT<MathCore::vec3f>::toPtn4(pos)));
            else
                setLocalPosition(pos);
        }

        MathCore::vec3f Transform::getPosition(bool useVisitedFlag)
        {
            return MathCore::CVT<MathCore::vec4f>::toVec3(getMatrix(useVisitedFlag)[3]);
        }

        MathCore::vec3f Transform::getPosition()
        {
            return getPosition(false);
        }

        // void Transform::setEuler(const MathCore::vec3f &rot)
        // {
        //     if (parent != nullptr && parent->isRoot())
        //         setLocalEuler(rot);

        //     MathCore::quatf q = extractQuat(parent->getMatrixInverse());
        //     setLocalRotation(q * quatFromEuler(rot.x, rot.y, rot.z));
        // }

        // MathCore::vec3f Transform::getEuler()
        // {
        //     return getEuler(false);
        // }

        // MathCore::vec3f Transform::getEuler(bool useVisitedFlag = false)
        // {
        //     if (parent != nullptr && parent->isRoot())
        //         return localEuler;

        //     MathCore::vec3f euler;
        //     extractEuler(getMatrix(useVisitedFlag), &euler.x, &euler.y, &euler.z);
        //     return euler;
        // }

        void Transform::setRotation(const MathCore::quatf &rot)
        {
            auto parent = getParent();
            if (parent != nullptr && parent->isRoot())
                setLocalRotation(rot);

            MathCore::quatf q = MathCore::GEN<MathCore::quatf>::fromMat4(parent->getMatrixInverse());
            setLocalRotation(q * rot);
        }

        MathCore::quatf Transform::getRotation()
        {
            return getRotation(false);
        }

        MathCore::quatf Transform::getRotation(bool useVisitedFlag)
        {
            auto parent = getParent();

            if (parent != nullptr && parent->isRoot())
                return localRotation;

            MathCore::quatf q = MathCore::GEN<MathCore::quatf>::fromMat4(getMatrix(useVisitedFlag));
            return q;
        }

        void Transform::setScale(const MathCore::vec3f &s)
        {
            auto parent = getParent();

            if (parent != nullptr && parent->isRoot())
                setLocalScale(s);

            MathCore::mat4f &m = parent->getMatrixInverse();
            setLocalScale(MathCore::vec3f(
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[0])) * s.x,
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[1])) * s.y,
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[2])) * s.z));
        }

        MathCore::vec3f Transform::getScale()
        {
            return getScale(false);
        }

        MathCore::vec3f Transform::getScale(bool useVisitedFlag)
        {
            auto parent = getParent();

            if (parent != nullptr && parent->isRoot())
                return localScale;

            MathCore::mat4f &m = getMatrix(useVisitedFlag);
            return MathCore::vec3f(
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[0])),
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[1])),
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[2])));
        }

        void Transform::lookAtRightHanded(std::shared_ptr<Transform> to, const MathCore::vec3f &worldUp)
        {
            MathCore::vec3f lookVector = to->Position - Position;
            Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationRH(lookVector, worldUp);
        }

        void Transform::lookAtLeftHanded(std::shared_ptr<Transform> to, const MathCore::vec3f &worldUp)
        {
            MathCore::vec3f lookVector = to->Position - Position;
            Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationLH(lookVector, worldUp);
        }

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform Render Ops...
        //
        //
        //
        ///////////////////////////////////////////////////////
        void Transform::resetVisited(bool forceMarkFalse)
        {
            if (forceMarkFalse ||
                localMatrixDirty ||
                localMatrixInverseTransposeDirty ||
                localMatrixInverseDirty ||
                matrixDirty ||
                matrixInverseTransposeDirty ||
                matrixInverseDirty)
            {
                visited = false;
                forceMarkFalse = true;
                renderDirty = true;
            }
            for (int i = 0; i < children.size(); i++)
            {
                children[i]->resetVisited(forceMarkFalse);
            }
        }

        void Transform::preComputeTransforms()
        {
            if (!visited)
            {
                if (!isRoot())
                {
                    getMatrix(true);
                    // getMatrixInverse(true);
                    getMatrixInverseTranspose(true); // already process the inverse...
                }
                visited = true;
                // stat_num_visited++;

                OnVisited(this->self());
            }
            for (int i = 0; i < children.size(); i++)
                children[i]->preComputeTransforms();
        }

        void Transform::computeRenderMatrix(const MathCore::mat4f &viewProjection,
                                            const MathCore::mat4f &view,
                                            const MathCore::mat4f &viewIT,
                                            const MathCore::mat4f &viewInv,
                                            MathCore::mat4f **mvp,
                                            MathCore::mat4f **mv,
                                            MathCore::mat4f **mvIT,
                                            MathCore::mat4f **mvInv)
        {
            if (renderDirty || viewProjection != renderViewProjection)
            {

                // stat_draw_recalculated++;

                renderViewProjection = viewProjection;
                renderMVP = viewProjection * getMatrix(true);
                renderMV = view * getMatrix(true);
                renderMVIT = viewIT * getMatrixInverseTranspose(true);
                renderMVInv = getMatrixInverse(true) * viewInv;

                renderDirty = false;
            }

            *mvp = &renderMVP;
            *mv = &renderMV;
            *mvIT = &renderMVIT;
            *mvInv = &renderMVInv;
        }

        MathCore::mat4f &Transform::worldToLocalMatrix(bool useVisitedFlag)
        {
            return getMatrixInverse(useVisitedFlag);
        }

        MathCore::mat4f &Transform::localToWorldMatrix(bool useVisitedFlag)
        {
            return getMatrix(useVisitedFlag);
        }

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Object References...
        //
        //
        //
        ///////////////////////////////////////////////////////

        /*
        void Transform::insertMapName(Transform *node){

            name2children[node->name].push_back(node);

        }

        void Transform::removeMapName(Transform *node){
            std::unordered_map<std::string,std::vector<Transform*> >::iterator it;
            it = name2children.find(node->name);
            if (it != name2children.end()){
                std::vector<Transform*> &vector = it->second;
                for(int j=0;j<vector.size();j++){
                    if (vector[j] == node){
                        vector.erase(vector.begin()+j);
                        break;
                    }
                }
                if (vector.size() == 0)
                    name2children.erase(it);
            }
        }
        */

        void Transform::makeFirstComponent(std::shared_ptr<Component> c)
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i] == c)
                {
                    for (int j = i; j >= 1; j--)
                        components[j] = components[j - 1];
                    components[0] = c;
                    break;
                }
            }
        }

        void Transform::makeLastComponent(std::shared_ptr<Component> c)
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i] == c)
                {
                    int lastIndex = (int)components.size() - 1;
                    for (int j = i; j < lastIndex; j++)
                        components[j] = components[j + 1];
                    components[lastIndex] = c;
                    break;
                }
            }
        }

        std::shared_ptr<Component> Transform::addComponent(std::shared_ptr<Component> c)
        {
            // ITK_ABORT(c->transform!=nullptr,"cannot add same component to two or more transforms\n.");
            components.push_back(c);

            auto this_self = this->self();

            c->mTransform.push_back(this_self);
            // c->transform = this;

            c->attachToTransform(this_self);
            return c;
        }

        std::shared_ptr<Component> Transform::removeComponent(std::shared_ptr<Component> c)
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i] == c)
                {
                    components.erase(components.begin() + i);

                    for (int j = (int)c->mTransform.size() - 1; j >= 0; j--)
                    {
                        if (ToShared(c->mTransform[j]).get() == this)
                        {
                            c->mTransform.erase(c->mTransform.begin() + j);
                            c->detachFromTransform(this->self());
                            break;
                        }
                    }

                    // Transform *t = c->transform;
                    // c->transform = nullptr;
                    // if (t != nullptr)
                    // c->detachFromTransform(t);

                    return c;
                }
            }
            return nullptr;
        }

        std::shared_ptr<Component> Transform::removeComponentAt(int i)
        {
            if (i >= 0 && i < components.size())
            {
                std::shared_ptr<Component> result = components[i];
                components.erase(components.begin() + i);

                for (int j = result->getTransformCount() - 1; j >= 0; j--)
                {
                    if (result->getTransform(j).get() == this)
                    {
                        result->mTransform.erase(result->mTransform.begin() + j);
                        result->detachFromTransform(this->self());
                        break;
                    }
                }

                // Transform *t = result->transform;
                // result->transform = nullptr;
                // if (t != nullptr)
                // result->detachFromTransform(t);
                return result;
            }
            return nullptr;
        }

        // std::shared_ptr<Component> Transform::findComponent(ComponentType t)
        // {
        //     for (int i = 0; i < components.size(); i++)
        //     {
        //         if (components[i]->compareType(t))
        //             return components[i];
        //     }
        //     return nullptr;
        // }

        // std::vector<std::shared_ptr<Component>> Transform::findComponents(ComponentType t)
        // {
        //     std::vector<std::shared_ptr<Component>> result;
        //     for (int i = 0; i < components.size(); i++)
        //     {
        //         if (components[i]->compareType(t))
        //             result.push_back(components[i]);
        //     }
        //     return result;
        // }

        int Transform::getComponentCount() const
        {
            return (int)components.size();
        }
        std::shared_ptr<Component> Transform::getComponentAt(int i)
        {
            if (i >= 0 && i < components.size())
                return components[i];
            return nullptr;
        }

        // std::shared_ptr<Component> Transform::findComponentInChildren(ComponentType t)
        // {
        //     std::shared_ptr<Component> result;
        //     for (int i = 0; i < children.size(); i++)
        //     {
        //         result = children[i]->findComponent(t);
        //         if (result != nullptr)
        //             return result;
        //         result = children[i]->findComponentInChildren(t);
        //         if (result != nullptr)
        //             return result;
        //     }
        //     return result;
        // }

        // std::vector<std::shared_ptr<Component>> Transform::findComponentsInChildren(ComponentType t)
        // {
        //     std::vector<std::shared_ptr<Component>> result;
        //     std::vector<std::shared_ptr<Component>> parcialResult;
        //     for (int i = 0; i < children.size(); i++)
        //     {
        //         parcialResult = children[i]->findComponents(t);
        //         if (parcialResult.size() > 0)
        //             result.insert(result.end(), parcialResult.begin(), parcialResult.end());
        //         parcialResult = children[i]->findComponentsInChildren(t);
        //         if (parcialResult.size() > 0)
        //             result.insert(result.end(), parcialResult.begin(), parcialResult.end());
        //     }
        //     return result;
        // }

        void Transform::clearComponents()
        {
            // for(int i= getChildCount()-1;i>=0;i--)
            //     removeChild(i);
            for (int i = getComponentCount() - 1; i >= 0; i--)
                removeComponentAt(i);

            // renderWindowRegion.reset();// = nullptr;
        }

        void Transform::setName(const std::string &p)
        {
            if (name == p)
                return;

            // if (this->parent != nullptr)
            // this->parent->removeMapName(this);
            name = p;
            // if (this->parent != nullptr)
            // this->parent->insertMapName(this);
        }
        const std::string &Transform::getName() const
        {
            return name;
        }

        // VirtualProperty<std::string> Name;

        std::shared_ptr<Transform> Transform::findTransformByName(const std::string &name, int maxLevel)
        {
            if (this->name == name)
                return this->self();
            if (maxLevel > 0)
            {
                std::shared_ptr<Transform> result;
                for (int i = 0; i < children.size(); i++)
                {
                    result = children[i]->findTransformByName(name, maxLevel - 1);
                    if (result != nullptr)
                        return result;
                }
            }
            return nullptr;
        }

        std::vector<std::shared_ptr<Transform>> Transform::findTransformsByName(const std::string &name, int maxLevel)
        {
            std::vector<std::shared_ptr<Transform>> result;
            std::vector<std::shared_ptr<Transform>> parcialResult;

            if (this->name == name)
                result.push_back(this->self());

            if (maxLevel > 0)
            {
                for (int i = 0; i < children.size(); i++)
                {
                    parcialResult = children[i]->findTransformsByName(name, maxLevel - 1);
                    if (parcialResult.size() > 0)
                        result.insert(result.end(), parcialResult.begin(), parcialResult.end());
                }
            }

            return result;
        }

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Default Constructor
        //
        //
        //
        ///////////////////////////////////////////////////////

        Transform::Transform() : // Parent(
                                 //      EventCore::CallbackWrapper(&Transform::getParent, this),
                                 //      EventCore::CallbackWrapper(&Transform::setParent, this)),

                                 LocalPosition(
                                     EventCore::CallbackWrapper(&Transform::getLocalPosition, this),
                                     EventCore::CallbackWrapper(&Transform::setLocalPosition, this)),
                                 // LocalEuler(this, &Transform::getLocalEuler, &Transform::setLocalEuler),
                                 LocalRotation(
                                     EventCore::CallbackWrapper(&Transform::getLocalRotation, this),
                                     EventCore::CallbackWrapper(&Transform::setLocalRotation, this)),
                                 LocalScale(
                                     EventCore::CallbackWrapper(&Transform::getLocalScale, this),
                                     EventCore::CallbackWrapper(&Transform::setLocalScale, this)),

                                 Position(
                                     // EventCore::CallbackWrapper(&Transform::getPosition, this),
                                     EventCore::Callback<MathCore::vec3f()>(&Transform::getPosition, this),
                                     EventCore::CallbackWrapper(&Transform::setPosition, this)),
                                 // Euler(this, &Transform::getEuler, &Transform::setEuler),
                                 Rotation(
                                     // EventCore::CallbackWrapper(&Transform::getRotation, this),
                                     EventCore::Callback<MathCore::quatf()>(&Transform::getRotation, this),
                                     EventCore::CallbackWrapper(&Transform::setRotation, this)),
                                 Scale(
                                     // EventCore::CallbackWrapper(&Transform::getScale, this),
                                     EventCore::Callback<MathCore::vec3f()>(&Transform::getScale, this),
                                     EventCore::CallbackWrapper(&Transform::setScale, this)),

                                 Name(
                                     EventCore::CallbackWrapper(&Transform::getName, this),
                                     EventCore::CallbackWrapper(&Transform::setName, this))
        {
            // hierarchy ops
            // parent = nullptr;
            // transform
            localPosition = MathCore::vec3f(0);
            // localEuler = MathCore::vec3f(0);
            localRotation = MathCore::quatf();
            localScale = MathCore::vec3f(1.0f);

            localRotationBase = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            localMatrix = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            localMatrixInverseTranspose = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            localMatrixInverse = MathCore::CONSTANT<MathCore::mat4f>::Identity();

            localRotationBaseDirty = false;
            localMatrixDirty = false;
            localMatrixInverseTransposeDirty = false;
            localMatrixInverseDirty = false;

            matrixDirty = false;
            matrix = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            matrixParent = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            matrixInverseTransposeDirty = false;
            matrixInverseTranspose = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            // matrixInverseTransposeParent = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            matrixInverseDirty = false;
            matrixInverse = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            matrixInverseParent = MathCore::CONSTANT<MathCore::mat4f>::Identity();

            // model = nullptr;

            visited = true;

            renderMVP = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMV = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMVIT = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMVInv = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderViewProjection = MathCore::CONSTANT<MathCore::mat4f>::Identity();

            renderDirty = true;

            // renderWindowRegion = AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;
            auto app = AppKit::GLEngine::Engine::Instance()->app;
            if (app != nullptr)
                renderWindowRegion = app->screenRenderWindow;
            
            skip_traversing = false;
        }

        Transform::~Transform()
        {
            // SharedPointerDatabase::Instance()->notifyDeletion(this);
            // for(int i= getChildCount()-1;i>=0;i--)
            //     removeChild(i);
            // for(int i= getComponentCount()-1;i>=0;i--)
            //     removeComponentAt(i);

            clearChildren();
            clearComponents();

            renderWindowRegion.reset(); // = nullptr;
        }

        std::shared_ptr<Transform> Transform::setRenderWindowRegion(std::shared_ptr<RenderWindowRegion> renderWindowRegion)
        {
            this->renderWindowRegion = renderWindowRegion;
            return this->self();
        }

        bool Transform::traversePreOrder_DepthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, void *userData)> &OnNode,
            void *userData, int maxLevel)
        {
            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
            };

            std::vector<itemT> to_traverse;
            // std::list<itemT> to_traverse;

            to_traverse.push_back({self(), maxLevel});

            while (to_traverse.size() > 0)
            {
                auto item = to_traverse.back();
                to_traverse.pop_back();

                if (item.transform->skip_traversing)
                    continue;

                if (!OnNode(item.transform, userData))
                    return false;

                item.level--;
                if (item.level > 0)
                    for (auto &child : STL_Tools::Reversal(item.transform->children))
                        to_traverse.push_back({child, item.level});
            }

            return true;

            // if (!OnNode(this->self(), userData))
            //     return false;
            // maxLevel--;
            // if (maxLevel > 0)
            // {
            //     for (auto &chld : children)
            //     {
            //         if (!chld->traversePreOrder_DepthFirst(OnNode, userData, maxLevel))
            //             return false;
            //     }
            // }
            // return true;
        }

        bool Transform::traversePostOrder_DepthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, void *userData)> &OnNode,
            void *userData, int maxLevel)
        {

            // implementation with two stacks:

            // struct itemT
            // {
            //     std::shared_ptr<Transform> transform;
            //     int level;
            // };

            // std::vector<itemT> to_traverse;
            // // std::list<itemT> to_traverse;
            // std::vector<itemT> stack;

            // to_traverse.push_back({self(), maxLevel});

            // while (to_traverse.size() > 0)
            // {
            //     auto item = to_traverse.back();
            //     to_traverse.pop_back();

            //     stack.push_back(item);

            //     int new_lvl = item.level-1;
            //     if (new_lvl > 0)
            //         for (auto &child : item.transform->children)
            //             to_traverse.push_back({child, new_lvl});
            // }

            // while (stack.size() > 0){
            //     auto item = stack.back();
            //     stack.pop_back();

            //     if (!OnNode(item.transform, userData))
            //         return false;
            // }

            // return true;


            // implementation with one stack:

            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
                int child_idx;
            };
            std::vector<itemT> stack;

            itemT root = {self(), maxLevel, 0};

            while (root.transform != nullptr || stack.size() > 0)
            {
                // walk down the max possible nodes on left side
                // after that, the stack will hold the parent -> child relationship only
                // between each level.
                // Each place on stack is a parent element.
                while (root.transform != nullptr)
                {
                    stack.push_back(root);
                    int new_lvl = root.level - 1;
                    if (new_lvl > 0 && root.transform->children.size() > 0 && !root.transform->children[0]->skip_traversing)
                        root = {root.transform->children[0], new_lvl, 0};
                    else
                        root = {nullptr, 0, 0};
                }

                // remove the lowest child node value from stack and
                // save the current info from it
                auto item = stack.back();
                stack.pop_back();

                if (!item.transform->skip_traversing)
                if (!OnNode(item.transform, userData))
                    return false;

                // while the removed element is the lastest children,
                // walk to parent, making it the new child
                while (stack.size() > 0 &&
                       // stack.back().transform->children.size() > 0 &&
                       item.transform == stack.back().transform->children.back())
                {
                    item = stack.back();
                    stack.pop_back();
                    if (!item.transform->skip_traversing)
                    if (!OnNode(item.transform, userData))
                        return false;
                }

                // if there is any element in the stack, it means that
                // this is a parent with more children to compute
                if (stack.size() > 0)
                {
                    int next_child_idx = item.child_idx + 1;
                    //if (next_child_idx < (int)stack.back().transform->children.size() - 1)
                    root = {stack.back().transform->children[next_child_idx], item.level, next_child_idx};
                }
            }

            return true;

            // maxLevel--;
            // if (maxLevel > 0)
            // {
            //     for (auto &chld : children)
            //     {
            //         if (!chld->traversePostOrder_DepthFirst(OnNode, userData, maxLevel))
            //             return false;
            //     }
            // }
            // return OnNode(this->self(), userData);
        }

        bool Transform::traversePreOrder_DepthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, const void *userData)> &OnNode,
            const void *userData, int maxLevel)
        {
            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
            };

            std::vector<itemT> to_traverse;
            // std::list<itemT> to_traverse;

            to_traverse.push_back({self(), maxLevel});

            while (to_traverse.size() > 0)
            {
                auto item = to_traverse.back();
                to_traverse.pop_back();

                if (item.transform->skip_traversing)
                    continue;

                if (!OnNode(item.transform, userData))
                    return false;

                item.level--;
                if (item.level > 0)
                    for (auto &child : STL_Tools::Reversal(item.transform->children))
                        to_traverse.push_back({child, item.level});
            }

            return true;

            // if (!OnNode(this->self(), userData))
            //     return false;
            // maxLevel--;
            // if (maxLevel > 0)
            // {
            //     for (auto &chld : children)
            //     {
            //         if (!chld->traversePreOrder_DepthFirst(OnNode, userData, maxLevel))
            //             return false;
            //     }
            // }
            // return true;
        }

        bool Transform::traversePostOrder_DepthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, const void *userData)> &OnNode,
            const void *userData, int maxLevel)
        {
            // implementation with two stacks:

            // struct itemT
            // {
            //     std::shared_ptr<Transform> transform;
            //     int level;
            // };

            // std::vector<itemT> to_traverse;
            // // std::list<itemT> to_traverse;
            // std::vector<itemT> stack;

            // to_traverse.push_back({self(), maxLevel});

            // while (to_traverse.size() > 0)
            // {
            //     auto item = to_traverse.back();
            //     to_traverse.pop_back();

            //     stack.push_back(item);

            //     int new_lvl = item.level-1;
            //     if (new_lvl > 0)
            //         for (auto &child : item.transform->children)
            //             to_traverse.push_back({child, new_lvl});
            // }

            // while (stack.size() > 0){
            //     auto item = stack.back();
            //     stack.pop_back();

            //     if (!OnNode(item.transform, userData))
            //         return false;
            // }

            // return true;

            // implementation with one stack:

            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
                int child_idx;
            };
            std::vector<itemT> stack;

            itemT root = {self(), maxLevel, 0};

            while (root.transform != nullptr || stack.size() > 0)
            {
                // walk down the max possible nodes on left side
                // after that, the stack will hold the parent -> child relationship only
                // between each level.
                // Each place on stack is a parent element.
                while (root.transform != nullptr)
                {
                    stack.push_back(root);
                    int new_lvl = root.level - 1;
                    if (new_lvl > 0 && root.transform->children.size() > 0 && !root.transform->children[0]->skip_traversing)
                        root = {root.transform->children[0], new_lvl, 0};
                    else
                        root = {nullptr, 0, 0};
                }

                // remove the lowest child node value from stack and
                // save the current info from it
                auto item = stack.back();
                stack.pop_back();
                if (!item.transform->skip_traversing)
                if (!OnNode(item.transform, userData))
                    return false;

                // while the removed element is the lastest children,
                // walk to parent, making it the new child
                while (stack.size() > 0 &&
                       // stack.back().transform->children.size() > 0 &&
                       item.transform == stack.back().transform->children.back())
                {
                    item = stack.back();
                    stack.pop_back();
                    if (!item.transform->skip_traversing)
                    if (!OnNode(item.transform, userData))
                        return false;
                }

                // if there is any element in the stack, it means that
                // this is a parent with more children to compute
                if (stack.size() > 0)
                {
                    int next_child_idx = item.child_idx + 1;
                    //if (next_child_idx < (int)stack.back().transform->children.size() - 1)
                    root = {stack.back().transform->children[next_child_idx], item.level, next_child_idx};
                }
            }

            return true;

            // // recursive
            // maxLevel--;
            // if (maxLevel > 0)
            // {
            //     for (auto &chld : children)
            //     {
            //         if (!chld->traversePostOrder_DepthFirst(OnNode, userData, maxLevel))
            //             return false;
            //     }
            // }
            // return OnNode(this->self(), userData);
        }

        bool Transform::traverse_BreadthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, void *userData)> &OnNode,
            void *userData, int maxLevel)
        {
            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
            };

            // std::vector<itemT> to_traverse;
            std::list<itemT> to_traverse;

            to_traverse.push_back({self(), maxLevel});

            while (to_traverse.size() > 0)
            {
                auto item = to_traverse.back();
                to_traverse.pop_back();

                if (item.transform->skip_traversing)
                    continue;

                if (!OnNode(item.transform, userData))
                    return false;

                item.level--;
                if (item.level > 0)
                    for (auto &child : item.transform->children)
                        to_traverse.push_front({child, item.level});
            }

            return true;
        }

        bool Transform::traverse_BreadthFirst(
            const EventCore::Callback<bool(std::shared_ptr<Transform> t, const void *userData)> &OnNode,
            const void *userData, int maxLevel)
        {
            struct itemT
            {
                std::shared_ptr<Transform> transform;
                int level;
            };

            // std::vector<itemT> to_traverse;
            std::list<itemT> to_traverse;

            to_traverse.push_back({self(), maxLevel});

            while (to_traverse.size() > 0)
            {
                auto item = to_traverse.back();
                to_traverse.pop_back();

                if (item.transform->skip_traversing)
                    continue;

                if (!OnNode(item.transform, userData))
                    return false;

                item.level--;
                if (item.level > 0)
                    for (auto &child : item.transform->children)
                        to_traverse.push_front({child, item.level});
            }

            return true;
        }

        std::shared_ptr<Transform> Transform::clone(
            bool force_make_component_copy,
            TransformMapT *_transformMap,
            ComponentMapT *_componentMap)
        {
            std::shared_ptr<TransformMapT> transformMap;
            if (!_transformMap)
                transformMap = std::make_shared<TransformMapT>();
            else
                transformMap = std::shared_ptr<TransformMapT>(_transformMap, [](TransformMapT *v) {});

            std::shared_ptr<ComponentMapT> componentMap;
            if (!_componentMap)
                componentMap = std::make_shared<ComponentMapT>();
            else
                componentMap = std::shared_ptr<ComponentMapT>(_componentMap, [](ComponentMapT *v) {});

            // transformMap->clear();
            // componentMap->clear();

            auto result = Transform::CreateShared();

            struct _clone_structT
            {
                std::shared_ptr<Transform> cloneSrc;
                std::shared_ptr<Transform> cloneDst;
            };
            std::list<_clone_structT> to_clone;
            {
                auto _self = self();
                to_clone.push_back(_clone_structT{_self, result});
                transformMap->operator[](_self) = result;
            }

            while (to_clone.size() > 0)
            {
                auto entry = to_clone.front();
                to_clone.pop_front();

                entry.cloneDst->setName(entry.cloneSrc->getName());
                entry.cloneDst->setLocalPosition(entry.cloneSrc->getLocalPosition());
                entry.cloneDst->setLocalRotation(entry.cloneSrc->getLocalRotation());
                entry.cloneDst->setLocalScale(entry.cloneSrc->getLocalScale());
                entry.cloneDst->skip_traversing = entry.cloneSrc->skip_traversing;

                for (auto &src_transform : entry.cloneSrc->getChildren())
                {
                    auto new_transform = entry.cloneDst->addChild(Transform::CreateShared());
                    to_clone.push_back(_clone_structT{src_transform, new_transform});
                    transformMap->operator[](src_transform) = new_transform;
                }
            }

            // clone components
            for (auto &pair : *transformMap)
            {
                auto &src = pair.first;
                auto &dst = pair.second;
                for (auto &src_component : src->getComponents())
                {
                    auto new_component = dst->addComponent(src_component->duplicate_ref_or_clone(force_make_component_copy));
                    if (src_component != new_component)
                    {
                        // the component was fully cloned, so it needs
                        // to update its references
                        componentMap->operator[](src_component) = new_component;
                    }
                }
            }

            // fix fully cloned components references
            for (auto &pair : *componentMap)
            {
                // auto &src = pair.first;
                auto &dst = pair.second;
                dst->fix_internal_references(*transformMap, *componentMap);
            }

            return result;
        }

    }
}
