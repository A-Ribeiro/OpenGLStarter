#ifndef _ComponentParticleSystem_h_
#define _ComponentParticleSystem_h_

#include <aribeiro/aribeiro.h>
#include <opengl-wrapper/opengl-wrapper.h>
#include <mini-gl-engine/Component.h>
#include <mini-gl-engine/Transform.h>
#include <mini-gl-engine/Engine.h>

#include <mini-gl-engine/Interpolators.h>

namespace GLEngine {

    namespace Components {

        class ComponentParticleSystem;


        class _SSE2_ALIGN_PRE Particle {
        public:
            aRibeiro::vec3 pos;
            aRibeiro::vec3 dir;

            float elapsed_sec;

            ComponentParticleSystem *system;

            float lifetime_sec;
            float lifetime_sec_inv;
            
            float speed;
            float size;
            aRibeiro::vec3 color;
            float alpha;

            float distance_to_camera;

            bool canBeRemoved;

            Particle(
                ComponentParticleSystem *_system,
                const aRibeiro::vec3 &_pos,
                const aRibeiro::vec3 &_dir
            );
            void updateParameters(float lrp);
            void update(float _elapsed_sec);

            SSE2_CLASS_NEW_OPERATOR
        }_SSE2_ALIGN_POS;


        class ComponentParticleSystem: public Component {

        protected:

            float lifetime_sec;
            float lifetime_sec_inv;

            float rateOverTime;
            float rateOverTime_inv;
            float rateElapsed_sec;

            void OnAfterGraphPrecompute(aRibeiro::PlatformTime *time);

        public:
            
            static const ComponentType Type;

            aRibeiro::aligned_vector<Particle> particles;

            bool loop;
            float elapsed_sec;
            float duration_sec;
            
            LinearInterpolator<float> Speed;
            LinearInterpolator<float> Size;
            LinearInterpolator<aRibeiro::vec3> Color;
            LinearInterpolator<float> Alpha;

            Transform *boxEmmiter;
            aRibeiro::mat4 emissionMatrix;
            aRibeiro::vec3 emissionDir;

            aRibeiro::collision::AABB aabb;
            aRibeiro::vec3 aabb_center;
            float distance_to_camera;

            aRibeiro::vec4 textureColor;
            openglWrapper::GLTexture *texture;

            bool soft;

            ComponentParticleSystem();
            
            virtual ~ComponentParticleSystem();

            void setLifetime(float v_sec);

            float getLifetime()const ;

            float getLifetime_inv()const ;

            void setRateOverTime(float v);

            Particle * emmitParticle();

            void emmitStart();

            void prewarmStart();

            void sortPositions(const aRibeiro::vec3 &cameraPos, const aRibeiro::vec3 &cameraDirection);

            void computeAABB();
            
        };

    }
}

#endif
