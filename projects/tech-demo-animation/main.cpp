#include <mini-gl-engine/mini-gl-engine.h>
#include "App.h"

static AppBase * CreateAppInstance() {
    App *app = new App();
    app->load();
    return app;
}

int main(int argc, char* argv[]) {

    __m128 a = _mm_load_(1,2,3,4), 
           b = _mm_load_(5,6,7,8);
    __m128 c = dot_sse_4(a,b);

    printf(" %f %f %f %f \n", _mm_f32_(c,0), _mm_f32_(c,1), _mm_f32_(c,2), _mm_f32_(c,3) );

    c = dot_sse_3(a,b);

    printf(" %f %f %f %f \n", _mm_f32_(c,0), _mm_f32_(c,1), _mm_f32_(c,2), _mm_f32_(c,3) );

    c = _mm_dp_ps( a,b, 0xff );

    printf(" %f %f %f %f \n", _mm_f32_(c,0), _mm_f32_(c,1), _mm_f32_(c,2), _mm_f32_(c,3) );

    c = _mm_dp_ps( a,b, 0x77 );

    printf(" %f %f %f %f \n", _mm_f32_(c,0), _mm_f32_(c,1), _mm_f32_(c,2), _mm_f32_(c,3) );

    const float _float_bitsign = -.0f; // -0.f = 1 << 31
    const uint32_t _float_bitsign_uint32_t = (*(uint32_t*)(&_float_bitsign));
    const float _float_one = 1.0f;
    const uint32_t _float_one_uint32_t = (*(uint32_t*)(&_float_one));

    float value = 99.0f;
    uint32_t &value_int = *(uint32_t*)(&value);
    uint32_t sign_int = (value_int & _float_bitsign_uint32_t) | _float_one_uint32_t;
    float &sign_result = *(float*)(&sign_int);

    printf("Sign: %f\n", sign_result);

    const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
    const __m128 _vec4_one_mask = _mm_set1_ps(1.0f); // -0.f = 1 << 31
    
    __m128 sse_value = _mm_setr_ps(-123.123f , 0.0f, 10.02f, 456.24f);
    __m128 sign_aux = _mm_and_ps(sse_value, _vec4_sign_mask);
    __m128 sign = _mm_or_ps(sign_aux, _vec4_one_mask);

    printf("%f %f %f %f\n", sign.m128_f32[0], sign.m128_f32[1], sign.m128_f32[2], sign.m128_f32[3]);

    {
        __m128 a = _mm_setr_ps(1, 2, 3, 4);
        __m128 b = _mm_setr_ps(5, 6, 7, 8);
        __m128 c = _mm_blend_ps(a, b, 0x8);

        c.m128_f32[1] = 0.0f;

        printf("%f %f %f %f\n", c.m128_f32[0], c.m128_f32[1], c.m128_f32[2], c.m128_f32[3]);
    }


    //return 0;
    
    /*

    FILE* fp;
    AllocConsole();
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    for (int i = 0; i < 5; i++) {
        printf("*************************** epoca %i\n", i);

        int count = 100000 * 5;

        PlatformTime timer;

        mat4 m = eulerRotate(0, 0, DEG2RAD(30.0f)) * eulerRotate(0, DEG2RAD(30.0f), 0) * eulerRotate(DEG2RAD(30.0f), 0, 0) * translate(10, 11, 12);

        timer.update();
        for (int i = 0; i < count; i++) {
            m = inv(m);
        }
        timer.update();
        printf("elapsed (inv): %f\n", timer.deltaTime);
        float old_time = timer.deltaTime;

        GLEngine::Transform *transform = new GLEngine::Transform();
        quat quaternion = quatFromEuler(0, 0, DEG2RAD(30.0f)) *  quatFromEuler(0, DEG2RAD(30.0f), 0) *  quatFromEuler(DEG2RAD(30.0f), 0, 0);
        vec3 position = vec3(10, 11, 12);

        timer.update();
        for (int i = 0; i < count; i++) {
            transform->setLocalRotation(quaternion);
            transform->setPosition(position);

            transform->getLocalMatrixInverse();
        }
        timer.update();
        printf("elapsed (transform): %f\n", timer.deltaTime);


        printf("speedup: %f\n", old_time / timer.deltaTime);

        
    }

    fgetc(stdin);

    return 0;
    //*/

    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv[0]));

    GLEngine::Engine *engine = GLEngine::Engine::Instance();

    engine->initialSetup("Milky Way Studio", "Tech Demo: Animation", &CreateAppInstance);

#ifdef ARIBEIRO_RPI
    engine->setResolution( sf::VideoMode(1280,720), true, true, 0);
#else
#if defined(NDEBUG)
    //fullscreen
    engine->setResolution(engine->getResolutionList()[0], true, true, 1);
#else
    engine->setResolution( sf::VideoMode(852,480), false, true, 1);
#endif
#endif


    engine->mainLoop();

    return 0;
}
