#ifndef __Transform__H__
#define __Transform__H__

#include <aribeiro/aribeiro.h>
#include "Component.h"

namespace GLEngine {

    class Transform;

    BEGIN_DECLARE_DELEGATE(TransformVisitedEvent, Transform* t) CALL_PATTERN(t) END_DECLARE_DELEGATE;
    DefineMethodPointer(TransformTraverseMethod, bool, Transform* t, void* userData) ReturnMethodCall(t, userData);
    DefineMethodPointer(TransformTraverseMethod_const, bool, Transform* t, const void* userData) ReturnMethodCall(t, userData);

    class Transform {
        Transform(const Transform& v);
        void operator=(const Transform& v);

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Node Hierarchy structure and operations
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:
        std::vector<Transform*> children;
        Transform* parent;

    public:

        Transform* removeChild(int index);
        Transform* removeChild(Transform * transform);
        Transform* addChild(Transform * transform);
        //std::vector<Transform*> &getChildren();
        int getChildCount();
        Transform* getChildAt(int);

        Transform *getParent();
        void setParent(Transform *prnt);
        bool isRoot();

        aRibeiro::VirtualProperty<Transform *> Parent;
        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform default graph operations
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:
        aRibeiro::vec3 localPosition;
        aRibeiro::vec3 localEuler;
        aRibeiro::quat localRotation;
        aRibeiro::vec3 localScale;

        bool localRotationBaseDirty;
        aRibeiro::mat4 localRotationBase;

        bool localMatrixDirty;
        aRibeiro::mat4 localMatrix;
        bool localMatrixInverseTransposeDirty;
        aRibeiro::mat4 localMatrixInverseTranspose;
        bool localMatrixInverseDirty;
        aRibeiro::mat4 localMatrixInverse;


        void updateLocalRotationBase();
    public:

        aRibeiro::vec3 getLocalPosition()const;
        aRibeiro::vec3 getLocalEuler()const;
        aRibeiro::quat getLocalRotation()const;
        aRibeiro::vec3 getLocalScale()const;

        void setLocalPosition(const aRibeiro::vec3 &p);
        void setLocalEuler(const aRibeiro::vec3 &e);
        void setLocalRotation(const aRibeiro::quat &q);
        void setLocalScale(const aRibeiro::vec3 &s);

        aRibeiro::mat4 & getLocalMatrix();
        aRibeiro::mat4 & getLocalMatrixInverseTranspose();
        aRibeiro::mat4 & getLocalMatrixInverse();

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform Global Ops...
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:

        bool matrixDirty;
        aRibeiro::mat4 matrix, matrixParent;
        bool matrixInverseTransposeDirty;
        aRibeiro::mat4 matrixInverseTranspose;//, matrixInverseTransposeParent;
        bool matrixInverseDirty;
        aRibeiro::mat4 matrixInverse, matrixInverseParent;

    public:
        aRibeiro::mat4& getMatrix(bool useVisitedFlag = false);
        aRibeiro::mat4& getMatrixInverseTranspose(bool useVisitedFlag = false);
        aRibeiro::mat4& getMatrixInverse(bool useVisitedFlag = false);

        void setPosition(const aRibeiro::vec3 &pos);
        aRibeiro::vec3 getPosition();
        aRibeiro::vec3 getPosition(bool useVisitedFlag);
        void setEuler(const aRibeiro::vec3 &rot);
        aRibeiro::vec3 getEuler();
        aRibeiro::vec3 getEuler(bool useVisitedFlag);
        void setRotation(const aRibeiro::quat &rot);
        aRibeiro::quat getRotation();
        aRibeiro::quat getRotation(bool useVisitedFlag);
        void setScale(const aRibeiro::vec3 &s);
        aRibeiro::vec3 getScale();
        aRibeiro::vec3 getScale(bool useVisitedFlag);

        void lookAtRightHanded(const Transform* to, const aRibeiro::vec3 &worldUp = aRibeiro::vec3(0, 1, 0));
        void lookAtLeftHanded(const Transform* to, const aRibeiro::vec3 &worldUp = aRibeiro::vec3(0, 1, 0));

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform Render Ops...
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:

        aRibeiro::mat4 renderMVP, renderMV, renderMVIT, renderMVInv, renderViewProjection;
        bool renderDirty;
    public:
        bool visited;

        void resetVisited(bool forceMarkFalse = false);
        void preComputeTransforms();
        void computeRenderMatrix(
            const aRibeiro::mat4 &viewProjection,
            const aRibeiro::mat4 &view,
            const aRibeiro::mat4 &viewIT,
            const aRibeiro::mat4 &viewInv,
            aRibeiro::mat4 **mvp,
            aRibeiro::mat4 **mv,
            aRibeiro::mat4 **mvIT,
            aRibeiro::mat4 **mvInv);

        TransformVisitedEvent OnVisited;

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Transform Properties
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:
    public:
        aRibeiro::VirtualProperty<aRibeiro::vec3> LocalPosition;
        aRibeiro::VirtualProperty<aRibeiro::vec3> LocalEuler;
        aRibeiro::VirtualProperty<aRibeiro::quat> LocalRotation;
        aRibeiro::VirtualProperty<aRibeiro::vec3> LocalScale;

        aRibeiro::VirtualProperty<aRibeiro::vec3> Position;
        aRibeiro::VirtualProperty<aRibeiro::vec3> Euler;
        aRibeiro::VirtualProperty<aRibeiro::quat> Rotation;
        aRibeiro::VirtualProperty<aRibeiro::vec3> Scale;

        aRibeiro::mat4& worldToLocalMatrix(bool useVisitedFlag = false);
        aRibeiro::mat4& localToWorldMatrix(bool useVisitedFlag = false);


        ///////////////////////////////////////////////////////
        //
        //
        //
        // Object References...
        //
        //
        //
        ///////////////////////////////////////////////////////
    private:
        std::vector<Component*> components;
        std::string name;
        //std::map< std::string, std::vector<Transform*> > name2children;

        //void insertMapName(Transform *t);
        //void removeMapName(Transform *t);
    public:

        void makeFirstComponent(Component*);
        void makeLastComponent(Component*);

        Component* addComponent(Component*);
        Component* removeComponent(Component*);
        Component* removeComponentAt(int);
        Component* findComponent(ComponentType)const;
        std::vector<Component*> findComponents(ComponentType)const;
        int getComponentCount()const;
        Component* getComponentAt(int);

        Component* findComponentInChildren(ComponentType)const;
        std::vector<Component*> findComponentsInChildren(ComponentType)const;

        void setName(const std::string &p);
        const std::string& getName()const;
        aRibeiro::VirtualProperty<std::string> Name;

        Transform * findTransformByName(const std::string &name, int maxLevel = INT_MAX);
        std::vector<Transform*> findTransformsByName(const std::string &name, int maxLevel = INT_MAX);

        Transform();
        virtual ~Transform();

        ///////////////////////////////////////////////////////
        //
        //
        //
        // Traverse Methods
        //
        //
        //
        ///////////////////////////////////////////////////////

        bool traversePreOrder_DepthFirst(const TransformTraverseMethod &OnNode, void* userData = NULL, int maxLevel = INT_MAX);
        bool traversePostOrder_DepthFirst(const TransformTraverseMethod &OnNode, void* userData = NULL, int maxLevel = INT_MAX);

        bool traversePreOrder_DepthFirst(const TransformTraverseMethod_const &OnNode, const void* userData = NULL, int maxLevel = INT_MAX);
        bool traversePostOrder_DepthFirst(const TransformTraverseMethod_const &OnNode, const void* userData = NULL, int maxLevel = INT_MAX);

        ///////////////////////////////////////////////////////
        //
        //
        //
        // User Data Information
        //
        //
        //
        ///////////////////////////////////////////////////////

        void* userData;

        SSE2_CLASS_NEW_OPERATOR

    };

}

#endif
