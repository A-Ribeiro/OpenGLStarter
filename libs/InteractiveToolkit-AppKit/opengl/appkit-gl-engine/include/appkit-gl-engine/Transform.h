#pragma once

//#include <aRibeiroCore/aRibeiroCore.h>
#include <InteractiveToolkit/MathCore/MathCore.h>
#include <InteractiveToolkit/EventCore/Event.h>
#include <InteractiveToolkit/EventCore/Callback.h>
#include <InteractiveToolkit/EventCore/VirtualProperty.h>
#include "Component.h"

namespace AppKit
{
    namespace GLEngine
    {

        class Transform;
        class RenderWindowRegion;

        // BEGIN_DECLARE_DELEGATE(TransformVisitedEvent, Transform *t)
        // CALL_PATTERN(t) END_DECLARE_DELEGATE;
        // DefineMethodPointer(EventCore::CallbackWrapper, bool, Transform *t, void *userData) ReturnMethodCall(t, userData);
        // DefineMethodPointer(EventCore::CallbackWrapper_const, bool, Transform *t, const void *userData) ReturnMethodCall(t, userData);

        class Transform: public EventCore::HandleCallback
        {
            Transform(const Transform &v);
            void operator=(const Transform &v);

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
            std::vector<Transform *> children;
            Transform *parent;

        public:
            Transform *removeChild(int index);
            Transform *removeChild(Transform *transform);
            Transform *addChild(Transform *transform);
            // std::vector<Transform*> &getChildren();
            int getChildCount();
            Transform *getChildAt(int);

            Transform *getParent();
            void setParent(Transform * const &prnt);
            bool isRoot();

            
            EventCore::VirtualProperty<Transform *> Parent;
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
            MathCore::vec3f localPosition;
            //MathCore::vec3f localEuler;
            MathCore::quatf localRotation;
            MathCore::vec3f localScale;

            bool localRotationBaseDirty;
            MathCore::mat4f localRotationBase;

            bool localMatrixDirty;
            MathCore::mat4f localMatrix;
            bool localMatrixInverseTransposeDirty;
            MathCore::mat4f localMatrixInverseTranspose;
            bool localMatrixInverseDirty;
            MathCore::mat4f localMatrixInverse;

            void updateLocalRotationBase();

        public:
            MathCore::vec3f getLocalPosition() const;
            //MathCore::vec3f getLocalEuler() const;
            MathCore::quatf getLocalRotation() const;
            MathCore::vec3f getLocalScale() const;

            void setLocalPosition(const MathCore::vec3f &p);
            //void setLocalEuler(const MathCore::vec3f &e);
            void setLocalRotation(const MathCore::quatf &q);
            void setLocalScale(const MathCore::vec3f &s);

            MathCore::mat4f &getLocalMatrix();
            MathCore::mat4f &getLocalMatrixInverseTranspose();
            MathCore::mat4f &getLocalMatrixInverse();

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
            MathCore::mat4f matrix, matrixParent;
            bool matrixInverseTransposeDirty;
            MathCore::mat4f matrixInverseTranspose; //, matrixInverseTransposeParent;
            bool matrixInverseDirty;
            MathCore::mat4f matrixInverse, matrixInverseParent;

        public:
            MathCore::mat4f &getMatrix(bool useVisitedFlag = false);
            MathCore::mat4f &getMatrixInverseTranspose(bool useVisitedFlag = false);
            MathCore::mat4f &getMatrixInverse(bool useVisitedFlag = false);

            void setPosition(const MathCore::vec3f &pos);
            MathCore::vec3f getPosition();
            MathCore::vec3f getPosition(bool useVisitedFlag);
            // void setEuler(const MathCore::vec3f &rot);
            // MathCore::vec3f getEuler();
            // MathCore::vec3f getEuler(bool useVisitedFlag);
            void setRotation(const MathCore::quatf &rot);
            MathCore::quatf getRotation();
            MathCore::quatf getRotation(bool useVisitedFlag);
            void setScale(const MathCore::vec3f &s);
            MathCore::vec3f getScale();
            MathCore::vec3f getScale(bool useVisitedFlag);

            void lookAtRightHanded(const Transform *to, const MathCore::vec3f &worldUp = MathCore::vec3f(0, 1, 0));
            void lookAtLeftHanded(const Transform *to, const MathCore::vec3f &worldUp = MathCore::vec3f(0, 1, 0));

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
            MathCore::mat4f renderMVP, renderMV, renderMVIT, renderMVInv, renderViewProjection;
            bool renderDirty;

        public:
            bool visited;

            void resetVisited(bool forceMarkFalse = false);
            void preComputeTransforms();
            void computeRenderMatrix(
                const MathCore::mat4f &viewProjection,
                const MathCore::mat4f &view,
                const MathCore::mat4f &viewIT,
                const MathCore::mat4f &viewInv,
                MathCore::mat4f **mvp,
                MathCore::mat4f **mv,
                MathCore::mat4f **mvIT,
                MathCore::mat4f **mvInv);

            EventCore::Event<void(Transform*)> OnVisited;

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
            EventCore::VirtualProperty<MathCore::vec3f> LocalPosition;
            //EventCore::VirtualProperty<MathCore::vec3f> LocalEuler;
            EventCore::VirtualProperty<MathCore::quatf> LocalRotation;
            EventCore::VirtualProperty<MathCore::vec3f> LocalScale;

            EventCore::VirtualProperty<MathCore::vec3f> Position;
            //EventCore::VirtualProperty<MathCore::vec3f> Euler;
            EventCore::VirtualProperty<MathCore::quatf> Rotation;
            EventCore::VirtualProperty<MathCore::vec3f> Scale;

            MathCore::mat4f &worldToLocalMatrix(bool useVisitedFlag = false);
            MathCore::mat4f &localToWorldMatrix(bool useVisitedFlag = false);

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
            std::vector<Component *> components;
            std::string name;
            // std::map< std::string, std::vector<Transform*> > name2children;

            // void insertMapName(Transform *t);
            // void removeMapName(Transform *t);
        public:
            void makeFirstComponent(Component *);
            void makeLastComponent(Component *);

            Component *addComponent(Component *);
            Component *removeComponent(Component *);
            Component *removeComponentAt(int);
            Component *findComponent(ComponentType) const;
            std::vector<Component *> findComponents(ComponentType) const;
            int getComponentCount() const;
            Component *getComponentAt(int);

            Component *findComponentInChildren(ComponentType) const;
            std::vector<Component *> findComponentsInChildren(ComponentType) const;

            void setName(const std::string &p);
            const std::string &getName() const;
            EventCore::VirtualProperty<std::string> Name;

            Transform *findTransformByName(const std::string &name, int maxLevel = INT_MAX);
            std::vector<Transform *> findTransformsByName(const std::string &name, int maxLevel = INT_MAX);

            Transform();
            ~Transform();

            ///////////////////////////////////////////////////////
            //
            //
            //
            // Abstract Window This Transform Belongs To
            //
            //
            //
            ///////////////////////////////////////////////////////

            RenderWindowRegion *renderWindowRegion;
            Transform *setRenderWindowRegion(RenderWindowRegion *renderWindowRegion);

            ///////////////////////////////////////////////////////
            //
            //
            //
            // Traverse Methods
            //
            //
            //
            ///////////////////////////////////////////////////////

            bool traversePreOrder_DepthFirst(
                const EventCore::Callback<bool(Transform *t, void *userData)> &OnNode, 
                void *userData = NULL, int maxLevel = INT_MAX);
            bool traversePostOrder_DepthFirst(
                const EventCore::Callback<bool(Transform *t, void *userData)> &OnNode, 
                void *userData = NULL, int maxLevel = INT_MAX);

            bool traversePreOrder_DepthFirst(
                const EventCore::Callback<bool(Transform *t, const void *userData)> &OnNode, 
                const void *userData = NULL, int maxLevel = INT_MAX);
            bool traversePostOrder_DepthFirst(
                const EventCore::Callback<bool(Transform *t, const void *userData)> &OnNode,
                const void *userData = NULL, int maxLevel = INT_MAX);

            ///////////////////////////////////////////////////////
            //
            //
            //
            // User Data Information
            //
            //
            //
            ///////////////////////////////////////////////////////

            void *userData;
        };

    }

}