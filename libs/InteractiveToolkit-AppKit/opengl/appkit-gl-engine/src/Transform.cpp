#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Engine.h>
#include <appkit-gl-engine/AppBase/RenderWindowRegion.h>

#include <appkit-gl-engine/SharedPointer/SharedPointerDatabase.h>

// int stat_num_visited;
// int stat_num_recalculated;
// int stat_draw_recalculated;

namespace AppKit
{
    namespace GLEngine
    {

        Transform::Transform(const Transform &v) : Parent(
                                                       EventCore::CallbackWrapper(&Transform::getParent, this),
                                                       EventCore::CallbackWrapper(&Transform::setParent, this)),

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
                                                       //EventCore::CallbackWrapper(&Transform::getPosition, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getPosition, this),
                                                       EventCore::CallbackWrapper(&Transform::setPosition, this)),
                                                   // Euler(this, &Transform::getEuler, &Transform::setEuler),
                                                   Rotation(
                                                       //EventCore::CallbackWrapper(&Transform::getRotation, this),
                                                       EventCore::Callback<MathCore::quatf()>(&Transform::getRotation, this),
                                                       EventCore::CallbackWrapper(&Transform::setRotation, this)),
                                                   Scale(
                                                       //EventCore::CallbackWrapper(&Transform::getScale, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getScale, this),
                                                       EventCore::CallbackWrapper(&Transform::setScale, this)),

                                                   Name(
                                                       EventCore::CallbackWrapper(&Transform::getName, this),
                                                       EventCore::CallbackWrapper(&Transform::setName, this))
        {

            userData = NULL;
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

        Transform *Transform::removeChild(int index)
        {

            ITK_ABORT((index >= children.size() || index < 0), "Trying to remove a child that is not in the list...\n");
            // if (index >= children.size() || index < 0)
            //{
            //     fprintf(stderr, "Trying to remove a child that is not in the list...\n");
            //     exit(-1);
            //     return NULL;
            // }

            Transform *node = children[index];
            children.erase(children.begin() + index);

            // removeMapName(node);

            return node;
        }
        Transform *Transform::removeChild(Transform *transform)
        {
            for (int i = 0; i < children.size(); i++)
                if (children[i] == transform)
                {
                    children.erase(children.begin() + i);
                    transform->parent = NULL;
                    transform->visited = false;

                    // removeMapName(transform);

                    return transform;
                }

            ITK_ABORT(true, "Trying to remove a child that is not in the scene...\n");

            // fprintf(stderr,"Trying to remove a child that is not in the scene...\n");
            // exit(-1);
            return NULL;
        }

        Transform *Transform::addChild(Transform *transform)
        {
            if (transform->parent != NULL)
                transform->parent->removeChild(transform);
            transform->parent = this;
            transform->visited = false;
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
            return children.size();
        }

        Transform *Transform::getChildAt(int i)
        {
            if (i >= 0 && i < children.size())
                return children[i];
            return NULL;
        }

        Transform *Transform::getParent()
        {
            return parent;
        }

        void Transform::setParent(Transform *const &prnt)
        {

            // ITK_ABORT(prnt == NULL, "Trying to set parent to NULL...\n");
            // if (prnt == NULL) {
            //     fprintf(stderr,"Trying to set parent to NULL...\n");
            //     exit(-1);
            // }
            if (parent != NULL)
                parent->removeChild(this);
            // parent = prnt;
            if (prnt != NULL)
                prnt->addChild(this);
        }

        bool Transform::isRoot()
        {
            return parent == NULL;
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

            //MathCore::OP<MathCore::mat4f>::extractEuler(localRotation, &localEuler.x, &localEuler.y, &localEuler.z);

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
            Transform *parent = getParent();
            if (parent != NULL && !parent->isRoot())
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
            if (parent != NULL && !parent->isRoot()) {

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
            Transform *parent = getParent();
            if (parent != NULL && !parent->isRoot())
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
            if (parent != NULL && !parent->isRoot())
                setLocalPosition(
                    MathCore::CVT<MathCore::vec4f>::toVec3(
                        parent->getMatrixInverse() * MathCore::CVT<MathCore::vec3f>::toPtn4(pos)
                        )
                    );
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
        //     if (parent != NULL && parent->isRoot())
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
        //     if (parent != NULL && parent->isRoot())
        //         return localEuler;

        //     MathCore::vec3f euler;
        //     extractEuler(getMatrix(useVisitedFlag), &euler.x, &euler.y, &euler.z);
        //     return euler;
        // }

        void Transform::setRotation(const MathCore::quatf &rot)
        {
            if (parent != NULL && parent->isRoot())
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
            if (parent != NULL && parent->isRoot())
                return localRotation;

            MathCore::quatf q = MathCore::GEN<MathCore::quatf>::fromMat4(getMatrix(useVisitedFlag));
            return q;
        }

        void Transform::setScale(const MathCore::vec3f &s)
        {
            if (parent != NULL && parent->isRoot())
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
            if (parent != NULL && parent->isRoot())
                return localScale;

            MathCore::mat4f &m = getMatrix(useVisitedFlag);
            return MathCore::vec3f(
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[0])),
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[1])),
                MathCore::OP<MathCore::vec3f>::length(MathCore::CVT<MathCore::vec4f>::toVec3(m[2]))
            );
        }

        void Transform::lookAtRightHanded(const Transform *to, const MathCore::vec3f &worldUp)
        {
            MathCore::vec3f lookVector = to->Position - Position;
            Rotation = MathCore::GEN<MathCore::quatf>::lookAtRotationRH(lookVector, worldUp);
        }

        void Transform::lookAtLeftHanded(const Transform *to, const MathCore::vec3f &worldUp)
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

                OnVisited(this);
            }
            for (int i = 0; i < children.size(); i++)
            {
                children[i]->preComputeTransforms();
            }
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
            std::map<std::string,std::vector<Transform*> >::iterator it;
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

        void Transform::makeFirstComponent(Component *c)
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

        void Transform::makeLastComponent(Component *c)
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i] == c)
                {
                    int lastIndex = components.size() - 1;
                    for (int j = i; j < lastIndex; j++)
                        components[j] = components[j + 1];
                    components[lastIndex] = c;
                    break;
                }
            }
        }

        Component *Transform::addComponent(Component *c)
        {
            // ITK_ABORT(c->transform!=NULL,"cannot add same component to two or more transforms\n.");
            components.push_back(c);

            c->transform.push_back(this);
            // c->transform = this;

            c->attachToTransform(this);
            return c;
        }

        Component *Transform::removeComponent(Component *c)
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i] == c)
                {
                    components.erase(components.begin() + i);

                    for (size_t j = c->transform.size() - 1; j >= 0; j--)
                    {
                        if (c->transform[j] == this)
                        {
                            c->transform.erase(c->transform.begin() + j);
                            c->detachFromTransform(this);
                            break;
                        }
                    }

                    // Transform *t = c->transform;
                    // c->transform = NULL;
                    // if (t != NULL)
                    // c->detachFromTransform(t);

                    return c;
                }
            }
            return NULL;
        }

        Component *Transform::removeComponentAt(int i)
        {
            if (i >= 0 && i < components.size())
            {
                Component *result = components[i];
                components.erase(components.begin() + i);

                for (size_t j = result->transform.size() - 1; j >= 0; j--)
                {
                    if (result->transform[j] == this)
                    {
                        result->transform.erase(result->transform.begin() + j);
                        result->detachFromTransform(this);
                        break;
                    }
                }

                // Transform *t = result->transform;
                // result->transform = NULL;
                // if (t != NULL)
                // result->detachFromTransform(t);
                return result;
            }
            return NULL;
        }

        Component *Transform::findComponent(ComponentType t) const
        {
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i]->compareType(t))
                {
                    return components[i];
                }
            }
            return NULL;
        }

        std::vector<Component *> Transform::findComponents(ComponentType t) const
        {
            std::vector<Component *> result;
            for (int i = 0; i < components.size(); i++)
            {
                if (components[i]->compareType(t))
                {
                    result.push_back(components[i]);
                }
            }
            return result;
        }

        int Transform::getComponentCount() const
        {
            return components.size();
        }
        Component *Transform::getComponentAt(int i)
        {
            if (i >= 0 && i < components.size())
                return components[i];
            return NULL;
        }

        Component *Transform::findComponentInChildren(ComponentType t) const
        {
            Component *result = NULL;
            for (int i = 0; i < children.size(); i++)
            {
                result = children[i]->findComponent(t);
                if (result != NULL)
                    return result;
                result = children[i]->findComponentInChildren(t);
                if (result != NULL)
                    return result;
            }
            return result;
        }

        std::vector<Component *> Transform::findComponentsInChildren(ComponentType t) const
        {
            std::vector<Component *> result;
            std::vector<Component *> parcialResult;
            for (int i = 0; i < children.size(); i++)
            {
                parcialResult = children[i]->findComponents(t);
                if (parcialResult.size() > 0)
                    result.insert(result.end(), parcialResult.begin(), parcialResult.end());
                parcialResult = children[i]->findComponentsInChildren(t);
                if (parcialResult.size() > 0)
                    result.insert(result.end(), parcialResult.begin(), parcialResult.end());
            }
            return result;
        }

        void Transform::setName(const std::string &p)
        {
            if (name == p)
                return;

            // if (this->parent != NULL)
            // this->parent->removeMapName(this);
            name = p;
            // if (this->parent != NULL)
            // this->parent->insertMapName(this);
        }
        const std::string &Transform::getName() const
        {
            return name;
        }

        // VirtualProperty<std::string> Name;

        Transform *Transform::findTransformByName(const std::string &name, int maxLevel)
        {
            if (this->name == name)
                return this;
            if (maxLevel > 0)
            {
                Transform *result;
                for (int i = 0; i < children.size(); i++)
                {
                    result = children[i]->findTransformByName(name, maxLevel - 1);
                    if (result != NULL)
                        return result;
                }
            }
            return NULL;
        }

        std::vector<Transform *> Transform::findTransformsByName(const std::string &name, int maxLevel)
        {
            std::vector<Transform *> result;
            std::vector<Transform *> parcialResult;

            if (this->name == name)
                result.push_back(this);

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

        Transform::Transform() : Parent(
                                                       EventCore::CallbackWrapper(&Transform::getParent, this),
                                                       EventCore::CallbackWrapper(&Transform::setParent, this)),

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
                                                       //EventCore::CallbackWrapper(&Transform::getPosition, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getPosition, this),
                                                       EventCore::CallbackWrapper(&Transform::setPosition, this)),
                                                   // Euler(this, &Transform::getEuler, &Transform::setEuler),
                                                   Rotation(
                                                       //EventCore::CallbackWrapper(&Transform::getRotation, this),
                                                       EventCore::Callback<MathCore::quatf()>(&Transform::getRotation, this),
                                                       EventCore::CallbackWrapper(&Transform::setRotation, this)),
                                                   Scale(
                                                       //EventCore::CallbackWrapper(&Transform::getScale, this),
                                                       EventCore::Callback<MathCore::vec3f()>(&Transform::getScale, this),
                                                       EventCore::CallbackWrapper(&Transform::setScale, this)),

                                                   Name(
                                                       EventCore::CallbackWrapper(&Transform::getName, this),
                                                       EventCore::CallbackWrapper(&Transform::setName, this))
        {
            // hierarchy ops
            parent = NULL;
            // transform
            localPosition = MathCore::vec3f(0);
            //localEuler = MathCore::vec3f(0);
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

            // model = NULL;

            visited = true;

            renderMVP = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMV = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMVIT = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderMVInv = MathCore::CONSTANT<MathCore::mat4f>::Identity();
            renderViewProjection = MathCore::CONSTANT<MathCore::mat4f>::Identity();

            renderDirty = true;

            renderWindowRegion = &AppKit::GLEngine::Engine::Instance()->app->screenRenderWindow;
        }

        Transform::~Transform()
        {
            SharedPointerDatabase::Instance()->notifyDeletion(this);
            renderWindowRegion = NULL;
        }

        Transform *Transform::setRenderWindowRegion(RenderWindowRegion *renderWindowRegion)
        {
            this->renderWindowRegion = renderWindowRegion;
            return this;
        }

        bool Transform::traversePreOrder_DepthFirst(
            const EventCore::Callback<bool(Transform *t, void *userData)> &OnNode, 
            void *userData, int maxLevel)
        {
            if (!OnNode(this, userData))
                return false;
            maxLevel--;
            if (maxLevel > 0)
            {
                for (int i = 0; i < children.size(); i++)
                {
                    bool should_continue_traversing = children[i]->traversePreOrder_DepthFirst(OnNode, userData, maxLevel);
                    if (!should_continue_traversing)
                        return false;
                }
            }
            return true;
        }

        bool Transform::traversePostOrder_DepthFirst(
            const EventCore::Callback<bool(Transform *t, void *userData)> &OnNode,
            void *userData, int maxLevel)
        {
            maxLevel--;
            if (maxLevel > 0)
            {
                for (int i = children.size() - 1; i >= 0; i--)
                {
                    bool should_continue_traversing = children[i]->traversePostOrder_DepthFirst(OnNode, userData, maxLevel);
                    if (!should_continue_traversing)
                        return false;
                }
            }
            return OnNode(this, userData);
        }

        bool Transform::traversePreOrder_DepthFirst(
            const EventCore::Callback<bool(Transform *t, const void *userData)> &OnNode, 
            const void *userData, int maxLevel)
        {
            if (!OnNode(this, userData))
                return false;
            maxLevel--;
            if (maxLevel > 0)
            {
                for (int i = 0; i < children.size(); i++)
                {
                    bool should_continue_traversing = children[i]->traversePreOrder_DepthFirst(OnNode, userData, maxLevel);
                    if (!should_continue_traversing)
                        return false;
                }
            }
            return true;
        }

        bool Transform::traversePostOrder_DepthFirst(
            const EventCore::Callback<bool(Transform *t, const void *userData)> &OnNode,
            const void *userData, int maxLevel)
        {
            maxLevel--;
            if (maxLevel > 0)
            {
                for (int i = children.size() - 1; i >= 0; i--)
                {
                    bool should_continue_traversing = children[i]->traversePostOrder_DepthFirst(OnNode, userData, maxLevel);
                    if (!should_continue_traversing)
                        return false;
                }
            }
            return OnNode(this, userData);
        }

    }
}
