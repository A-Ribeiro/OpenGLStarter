#pragma once

#include <InteractiveToolkit/MathCore/MathCore.h>

//#include <appkit-gl-base/opengl-wrapper.h>

#include <appkit-gl-engine/Component.h>
#include <appkit-gl-engine/Transform.h>
#include <appkit-gl-engine/Engine.h>

#include <InteractiveToolkit/CollisionCore/CollisionCore.h>

#include <appkit-gl-engine/util/Interpolators.h>

namespace AppKit
{
    namespace GLEngine
    {

        namespace Components
        {

            class ComponentParticleSystem;

            class Particle
            {
            public:
                MathCore::vec3f pos;
                MathCore::vec3f dir;

                float elapsed_sec;

                ComponentParticleSystem *system;

                float lifetime_sec;
                float lifetime_sec_inv;

                float speed;
                float size;
                MathCore::vec3f color;
                float alpha;

                float distance_to_camera;

                bool canBeRemoved;

                Particle(
                    ComponentParticleSystem *_system,
                    const MathCore::vec3f &_pos,
                    const MathCore::vec3f &_dir);
                void updateParameters(float lrp);
                void update(float _elapsed_sec);
            };

            class ComponentParticleSystem : public Component
            {

            protected:
                std::weak_ptr<RenderWindowRegion> renderWindowRegionRef;

                float lifetime_sec;
                float lifetime_sec_inv;

                float rateOverTime;
                float rateOverTime_inv;
                float rateElapsed_sec;

                void OnAfterGraphPrecompute(Platform::Time *time);

            public:
                static const ComponentType Type;

                ITKCommon::Random32 random32;
                MathCore::MathRandomExt<ITKCommon::Random32> mathRandom;

                std::vector<Particle> particles;

                bool loop;
                float elapsed_sec;
                float duration_sec;

                LinearInterpolator<float> Speed;
                LinearInterpolator<float> Size;
                LinearInterpolator<MathCore::vec3f> Color;
                LinearInterpolator<float> Alpha;

                std::shared_ptr<Transform> boxEmmiter;
                MathCore::mat4f emissionMatrix;
                MathCore::vec3f emissionDir;

                CollisionCore::AABB<MathCore::vec3f> aabb;
                MathCore::vec3f aabb_center;
                // float distance_to_camera;

                MathCore::vec4f textureColor;
                std::shared_ptr<AppKit::OpenGL::GLTexture> texture;

                bool soft;

                ComponentParticleSystem();

                ~ComponentParticleSystem();

                void setLifetime(float v_sec);

                float getLifetime() const;

                float getLifetime_inv() const;

                void setRateOverTime(float v);

                Particle *emmitParticle();

                void emmitStart();

                void prewarmStart();

                void sortPositions(const MathCore::vec3f &cameraPos, const MathCore::vec3f &cameraDirection);

                void computeAABB();

                // always clone 
                std::shared_ptr<Component> duplicate_ref_or_clone(bool force_clone);
                void fix_internal_references(TransformMapT &transformMap, ComponentMapT &componentMap);

                void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
                void Deserialize(rapidjson::Value &_value,
                                 std::unordered_map<uint64_t, std::shared_ptr<Transform>> &transform_map,
                                 std::unordered_map<uint64_t, std::shared_ptr<Component>> &component_map,
                                 ResourceSet &resourceSet);

            };

        }
    }

}
