#pragma once

// #include <aRibeiroCore/aRibeiroCore.h>
// #include <appkit-gl-engine/SharedPointer.h>
// #include "StartEventManager.h"
#include <vector>
#include <InteractiveToolkit/EventCore/HandleCallback.h>
#include "ToShared.h"

#include <unordered_map>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>

#include <appkit-gl-engine/Serializer/SerializerUtil.h>
// #include <appkit-gl-engine/ResourceMap.h>

namespace AppKit
{
    namespace GLEngine
    {

        class Component;
        class Transform;
        struct ResourceSet;
        class ResourceMap;

        // BEGIN_DECLARE_DELEGATE(VoidEvent)
        // CALL_PATTERN() END_DECLARE_DELEGATE;
        // BEGIN_DECLARE_DELEGATE(TwoComponentsEvent, Component *a, Component *b)
        // CALL_PATTERN(a, b) END_DECLARE_DELEGATE;

        typedef const char *ComponentType;

        class Component : public EventCore::HandleCallback
        {
        private:
            // bool mStartCalled;
        protected:

            struct Entry {
                std::weak_ptr<Transform> weak_ptr;
                Transform* ptr;
            };

            // all subclasses need to provide a const char* ref in the constructor
            ComponentType type_const_ref;
            Component(ComponentType type);

            std::weak_ptr<Component> mSelf;
            std::vector<Entry> mTransform;

            bool start_registered;

            void registerStart();
            void unregisterStart();

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            Component(const Component &v) = delete;
            Component& operator=(const Component &v) = delete;
        
            std::shared_ptr<Transform> getTransform(int i = 0)
            {
                if (i >= (int)mTransform.size())
                    return nullptr;
                return ToShared(mTransform[i].weak_ptr);
            }
            int getTransformCount() const
            {
                return (int)mTransform.size();
            }

            ComponentType getType() const;
            bool compareType(ComponentType t) const;

            virtual ~Component();

            virtual void start();
            // void callStartOnce();

            virtual void attachToTransform(std::shared_ptr<Transform> t);
            virtual void detachFromTransform(std::shared_ptr<Transform> t);

            using TransformMapT = std::unordered_map<std::shared_ptr<Transform>, std::shared_ptr<Transform>>;
            using ComponentMapT = std::unordered_map<std::shared_ptr<Component>, std::shared_ptr<Component>>;

        protected:
            // clone a component by using the same reference when possible,
            // or making a new object from the original
            virtual std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone) = 0;
            // after a full clone, you need to fix the internal component references
            virtual void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap) = 0;

        public:
            inline std::shared_ptr<Component> self()
            {
                return std::shared_ptr<Component>(mSelf);
            }

            template <typename _ComponentType,
                      typename std::enable_if<
                          std::is_base_of<Component, _ComponentType>::value,
                          bool>::type = true>
            inline std::shared_ptr<_ComponentType> self()
            {
                return std::dynamic_pointer_cast<_ComponentType>(self());
            }

            template <typename _ComponentType, typename... _param_args,
                      typename std::enable_if<
                          std::is_base_of<Component, _ComponentType>::value,
                          bool>::type = true>
            static inline std::shared_ptr<_ComponentType> CreateShared(_param_args &&...args)
            {
                auto result = std::make_shared<_ComponentType>(std::forward<_param_args>(args)...);
                result->mSelf = std::weak_ptr<Component>(result);
                return result;
            }

            virtual void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) = 0;
            virtual void Deserialize(rapidjson::Value &_value, 
                std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map, 
                std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                ResourceSet &resourceSet
            ) = 0;

            friend class Transform;
        };

    }

}

#include <appkit-gl-engine/ResourceMap.h>