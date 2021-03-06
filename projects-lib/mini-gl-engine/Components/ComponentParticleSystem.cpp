#include "ComponentParticleSystem.h"

#include <algorithm> // std::sort

namespace GLEngine {

    namespace Components {


        Particle::Particle(
            ComponentParticleSystem *_system,
            const aRibeiro::vec3 &_pos,
            const aRibeiro::vec3 &_dir
        ) {
            pos = _pos;
            dir = _dir;

            system = _system;

            lifetime_sec = system->getLifetime();
            lifetime_sec_inv = system->getLifetime_inv();

            canBeRemoved = false;
            elapsed_sec = 0.0f;

            updateParameters(0.0f);
        }

        void Particle::updateParameters(float lrp) {
            speed = system->Speed.getValue(lrp);
            size = system->Size.getValue(lrp);
            color = system->Color.getValue(lrp);
            alpha = system->Alpha.getValue(lrp);

            //printf("\nspeed: %f\n", speed);
            //printf("\nsize: %f\n", size);
            //printf("\nalpha: %f\n", alpha);
        }

        void Particle::update(float _elapsed_sec) {
            float elapsed_modified_by_speed = _elapsed_sec * speed;
            elapsed_sec += elapsed_modified_by_speed;
            canBeRemoved = elapsed_sec > lifetime_sec;

            //printf("elapsed_sec: %f\n", elapsed_sec);
            //printf("lifetime_sec: %f\n", lifetime_sec);

            if (!canBeRemoved){

                pos = pos + dir * elapsed_modified_by_speed;

                float lrp = elapsed_sec * lifetime_sec_inv;
                updateParameters(lrp);
            }

        }


        const ComponentType ComponentParticleSystem::Type = "ComponentParticleSystem";


        void ComponentParticleSystem::OnAfterGraphPrecompute(aRibeiro::PlatformTime *time){

            if (boxEmmiter != NULL){
                emissionMatrix = boxEmmiter->localToWorldMatrix(true);
                emissionDir = boxEmmiter->getRotation(true) * aRibeiro::vec3(0,0,1);
            }

            elapsed_sec += time->deltaTime;

            //printf( "elapsed: %f particles count: %lu \n", elapsed_sec, particles.size() );

            for (int i=particles.size()-1;i>=0;i--)
                particles[i].update(time->deltaTime);
            
            for (int i=particles.size()-1;i>=0;i--) {
                if (particles[i].canBeRemoved)
                    particles.erase(particles.begin() + i);
            }

            if (!loop && elapsed_sec > duration_sec) {

                if (particles.size() == 0){
                    AppBase* app = Engine::Instance()->app;
                    app->OnAfterGraphPrecompute.remove( this, &ComponentParticleSystem::OnAfterGraphPrecompute );
                }

                return;
            }

            rateElapsed_sec += time->deltaTime;
            int emmissionToDo = (int)floor( rateElapsed_sec / rateOverTime_inv );
            rateElapsed_sec = rateElapsed_sec - (float)emmissionToDo * rateOverTime_inv;

            for(int i=emmissionToDo-1;i>=0;i--){
                Particle *p = emmitParticle();
                p->update(rateElapsed_sec + (float)i * rateOverTime_inv);
            }
            // rateElapsed_sec = 3.25
            // emissionToDo = 3
            //          p        p        p  t(0.25)
            // |--------|--------|--------|--------|--------|


            // 1 2 3 4 a b c
            for (int i=particles.size()-1;i>= particles.size() - emmissionToDo ;i--) {
                if (particles[i].canBeRemoved)
                    particles.erase(particles.begin() + i);
            }

            computeAABB();
        }

        ComponentParticleSystem::ComponentParticleSystem():Component(ComponentParticleSystem::Type) {
            loop = false;

            elapsed_sec = 0.0f;
            duration_sec = 1.0f;
            lifetime_sec = 1.0f;
            lifetime_sec_inv = 1.0f;

            rateOverTime = 1.0f;
            rateOverTime_inv = 1.0f;

            rateElapsed_sec = 0.0f;

            boxEmmiter = NULL;

            textureColor = aRibeiro::vec4(1,1,1,1);
            texture = NULL;

            soft = true;
        }
        
        ComponentParticleSystem::~ComponentParticleSystem() {
            AppBase* app = Engine::Instance()->app;

            app->OnAfterGraphPrecompute.remove( this, &ComponentParticleSystem::OnAfterGraphPrecompute );

            ReferenceCounter<openglWrapper::GLTexture*> *texRefCounter = &Engine::Instance()->textureReferenceCounter;

            texRefCounter->remove(texture);
        }

        void ComponentParticleSystem::setLifetime(float v_sec){
            lifetime_sec = v_sec;
            lifetime_sec_inv = 1.0f/lifetime_sec;
        }

        float ComponentParticleSystem::getLifetime()const {
            return lifetime_sec;
        }

        float ComponentParticleSystem::getLifetime_inv()const {
            return lifetime_sec_inv;
        }

        void ComponentParticleSystem::setRateOverTime(float v){
            rateOverTime = v;
            rateOverTime_inv = 1.0f/rateOverTime;
        }

        Particle * ComponentParticleSystem::emmitParticle()
        {
            aRibeiro::vec3 pos, 
            dir = aRibeiro::vec3(0,0,1);

            if (boxEmmiter != NULL){
                pos = aRibeiro::Random::getVec3() - aRibeiro::vec3(0.5f);
                pos = aRibeiro::toVec3( emissionMatrix * aRibeiro::toPtn4(pos) );

                dir = emissionDir;
            }

            Particle p = Particle(this,pos,dir);

            particles.push_back(p);

            return &particles[particles.size()-1];
        }

        void ComponentParticleSystem::emmitStart() {
            AppBase* app = Engine::Instance()->app;

            app->OnAfterGraphPrecompute.remove( this, &ComponentParticleSystem::OnAfterGraphPrecompute );
            app->OnAfterGraphPrecompute.add( this, &ComponentParticleSystem::OnAfterGraphPrecompute );

            elapsed_sec = 0.0f;
            rateElapsed_sec = 0.0f;

            if (boxEmmiter != NULL){
                emissionMatrix = boxEmmiter->localToWorldMatrix();
                emissionDir = boxEmmiter->getRotation() * aRibeiro::vec3(0,0,1);
            }

            emmitParticle();//emmit the first particle

            computeAABB();
        }

        void ComponentParticleSystem::prewarmStart() {

            if (boxEmmiter != NULL){
                emissionMatrix = boxEmmiter->localToWorldMatrix();
                emissionDir = boxEmmiter->getRotation() * aRibeiro::vec3(0,0,1);
            }
            
            AppBase* app = Engine::Instance()->app;

            app->OnAfterGraphPrecompute.remove( this, &ComponentParticleSystem::OnAfterGraphPrecompute );
            app->OnAfterGraphPrecompute.add( this, &ComponentParticleSystem::OnAfterGraphPrecompute );

            elapsed_sec = 0.0f;
            rateElapsed_sec = 0.0f;

            elapsed_sec = duration_sec;
            rateElapsed_sec = duration_sec;
            int emmissionToDo = (int)floor( rateElapsed_sec / rateOverTime_inv );
            rateElapsed_sec = rateElapsed_sec - (float)emmissionToDo * rateOverTime_inv;

            for(int i=emmissionToDo-1;i>=0;i--){
                Particle *p = emmitParticle();
                p->update(rateElapsed_sec + (float)i * rateOverTime_inv);
            }

            for (int i=particles.size()-1;i>=0;i--) {
                if (particles[i].canBeRemoved)
                    particles.erase(particles.begin() + i);
            }

            computeAABB();

        }


        bool __compare__particle__reverse__( const Particle &a, const Particle &b ){
            return (a.distance_to_camera > b.distance_to_camera);
        }

        void ComponentParticleSystem::sortPositions(const aRibeiro::vec3 &cameraPos, const aRibeiro::vec3 &cameraDirection) {

            aRibeiro::vec3 aux;
            for (int i=particles.size()-1;i>=0;i--){
                //particles[i].distance_to_camera = aRibeiro::sqrDistance(particles[i].pos,cameraPos);
                aux = particles[i].pos - cameraPos;
                particles[i].distance_to_camera = dot(aux, cameraDirection);
            }

            std::sort(particles.begin(),particles.end(),__compare__particle__reverse__);

            //printf("------------------PARTICLES DISTANCES------------------\n");
            //for(int i=0;i<particles.size();i++)
                //printf("%f \n", particles[i].distance_to_camera);
        }


        void ComponentParticleSystem::computeAABB() {
            //compute box
            if (particles.size() > 0){
                Particle *particle = &particles[particles.size()-1];
                aRibeiro::vec3 size = aRibeiro::vec3(particle->size * 0.5f);

                aabb = aRibeiro::collision::AABB( particle->pos - size, particle->pos + size);

                for (int i=particles.size()-2;i>=0;i--)
                {
                    particle = &particles[i];
                    size = aRibeiro::vec3(particle->size * 0.5f);
                    aabb = aRibeiro::collision::AABB::joinAABB( aabb, aRibeiro::collision::AABB( particle->pos - size, particle->pos + size) );
                }

                aabb_center = (aabb.min_box + aabb.max_box) * 0.5f;
            }
        }
            

    }
}