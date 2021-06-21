#include <aribeiro/aribeiro.h>

using namespace aRibeiro;



#include <stdio.h>

void test_MoveFunctions() {

    printf("---------------------------------------\n");
    printf("  test_MoveFunctions\n");
    printf("---------------------------------------\n\n");

    vec3 p(100,0,0), b(0,100,0);

    printf("move\n");
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = move(p,b,60);
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = move(p, b, 60);
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = move(p, b, 60);
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = move(p, b, 60);
    printf(" p(%f, %f, %f) \n\n", p.x, p.y, p.z);


    printf("moveSlerp\n");
    p = vec3(1, 0, 0);
    b = vec3(0, 1, 0);
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = moveSlerp(p, b, DEG2RAD(90.0f/4.0f));
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = moveSlerp(p, b, DEG2RAD(90.0f / 4.0f));
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = moveSlerp(p, b, DEG2RAD(90.0f / 4.0f));
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);
    p = moveSlerp(p, b, DEG2RAD(90.0f / 4.0f));
    printf(" p(%f, %f, %f) \n", p.x, p.y, p.z);

}

//
// Test: Quaternions Operations
//
void test_Quaternions() {
    printf("---------------------------------------\n");
    printf("  test_Quaternions\n");
    printf("---------------------------------------\n");

    vec3 p = vec3(1,1,1);
    printf(" p vec3(%f, %f, %f) \n", p.x, p.y, p.z);

    printf("quatFromEuler and extractEuler test...\n"
          "   If you don't see any error message, so the implementation is OK.\n\n");

    float anglex = 0.0f;
    for (anglex = 0; anglex <= 360.1f; anglex += 360.0f / 20.0f ) {

        float angley = 0.0f;
        for (angley = 0; angley <= 360.1f; angley += 360.0f / 20.0f ) {

            float anglez = 0.0f;
            for (anglez = 0; anglez <= 360.1f; anglez += 360.0f / 20.0f) {

                quat quatRotation = quatFromEuler(DEG2RAD(anglex), DEG2RAD(angley), DEG2RAD(anglez));
                mat4 matRotation = eulerRotate(DEG2RAD(anglex), DEG2RAD(angley), DEG2RAD(anglez));

                vec3 result = toVec3( matRotation * toVec4(p) );
                vec3 resultQuat = quatRotation * p;
                if (distance(result, resultQuat) > 0.02f) {
                    printf("  ERROR!!!\n ");
                    printf("  r (%f, %f, %f) ", result.x, result.y, result.z);
                    printf(" q (%f, %f, %f) -> ", resultQuat.x, resultQuat.y, resultQuat.z);
                    printf(" dst: %f\n", distance(result, resultQuat));
                }

                //convert back the quaternion to a transformation matrix, and multiply...
                resultQuat = toVec3( toMat4( quatRotation ) * toVec4(p));
                if (distance(result, resultQuat) > 0.02f) {
                    printf("  ERROR!!!\n ");
                    printf("  r (%f, %f, %f) ", result.x, result.y, result.z);
                    printf(" q (%f, %f, %f) -> ", resultQuat.x, resultQuat.y, resultQuat.z);
                    printf(" dst: %f\n", distance(result, resultQuat));
                }

                //convert the matrix4 to quaternion and multiply
                resultQuat = extractQuat(matRotation) * p;
                if (distance(result, resultQuat) > 0.02f) {
                    printf("  ERROR!!!\n ");
                    printf("  r (%f, %f, %f) ", result.x, result.y, result.z);
                    printf(" q (%f, %f, %f) -> ", resultQuat.x, resultQuat.y, resultQuat.z);
                    printf(" dst: %f\n", distance(result, resultQuat));
                }


                // none of the algorithms tested with euler extract from quaternion works when applied the same point to the angles calculated...
                vec3 euler = vec3(0);
                extractEuler(quatRotation, &euler.x, &euler.y, &euler.z);
                //
                // test transform using the extracted euler angles from quaternion
                //
                resultQuat = quatFromEuler(euler.x, euler.y, euler.z) * p;
                if (distance(result, resultQuat) > 0.02f) {
                    printf("  error on extracting euler angles!!!\n ");
                    printf("    matrix rotation (%f, %f, %f) \n", result.x, result.y, result.z);
                    printf("    quaternion rotation (%f, %f, %f) -> \n", resultQuat.x, resultQuat.y, resultQuat.z);
                    printf("      dst: %f\n\n", distance(result, resultQuat));

                    printf("    Angles: %f %f %f\n", anglex, angley, anglez);
                    printf("    ExtractedAngles: %f %f %f\n", RAD2DEG(euler.x), RAD2DEG(euler.y), RAD2DEG(euler.z));
                    //fgetc(stdin);
                }

                //
                // test transform using the extracted euler angles from mat4
                //
                euler = vec3(0);
                extractEuler(matRotation, &euler.x, &euler.y, &euler.z);

                //test transform using the extracted euler angles
                resultQuat = quatFromEuler(euler.x, euler.y, euler.z) * p;
                if (distance(result, resultQuat) > 0.02f) {
                    printf("  error on extracting euler angles!!!\n ");
                    printf("    matrix rotation (%f, %f, %f) \n", result.x, result.y, result.z);
                    printf("    quaternion rotation (%f, %f, %f) -> \n", resultQuat.x, resultQuat.y, resultQuat.z);
                    printf("      dst: %f\n\n", distance(result, resultQuat));

                    printf("    Angles: %f %f %f\n", anglex, angley, anglez);
                    printf("    ExtractedAngles: %f %f %f\n", RAD2DEG(euler.x), RAD2DEG(euler.y), RAD2DEG(euler.z));
                    //fgetc(stdin);
                }
            }
        }
    }

    printf(" Slerp test.\n\n");

    //slerp test
    for (anglex = 0; anglex <= 170.1f; anglex += 170.0f / 5.0f) {
        float lrp = anglex / 170.0f;
        quat result = slerp( quat() ,quatFromEuler(DEG2RAD(170.0f), 0.0f, 0.0f), lrp);

        vec3 euler = vec3(0);
        extractEuler(result, &euler.x, &euler.y, &euler.z);
        euler *= RAD2DEG(1);

        if (euler.x < -0)
            euler.x += 360.0f;
        if (euler.y < -0)
            euler.y += 360.0f;
        if (euler.z < -0)
            euler.z += 360.0f;

        printf("    lrp: %f\n", lrp);
        printf("    anglex: %f\n", anglex);
        printf("    AnglesFromQuaternion: %f %f %f\n", euler.x, euler.y, euler.z);
        printf("\n");
    }

    printf(" Slerp test On Vec3.\n\n");

    for (anglex = 0; anglex <= 170.1f; anglex += 170.0f / 5.0f) {
        float lrp = anglex / 170.0f;
        vec3 src = quat() * vec3(1.0f, 0.0f, 0.0f);
        vec3 target = quatFromEuler(0.0f, DEG2RAD(170.0f), 0.0f) * vec3(1.0f,0.0f,0.0f);

        vec3 computed = slerp(src, target, lrp);

        float angledeg = RAD2DEG( angleBetween(src, computed) );
        float angledegTotal = RAD2DEG(angleBetween(src, target));

        float angleBetweenQuat = RAD2DEG(angleBetween(quat(), quatFromEuler(0.0f, DEG2RAD(anglex), 0.0f)));

        printf("    lrp: %f\n", lrp);
        printf("    anglex: %f\n", anglex);
        printf("    angleBetween: %f\n", angledeg);
        printf("    angleBetween(quat): %f\n", angleBetweenQuat);
        printf("    angleBetween(total): %f\n", angledegTotal);
        printf("\n");
    }


    
    printf("Done.\n\n");
    

}


//
// Test: PlatformTime PlatformSleep
//
void test_PlatformTime_and_PlatformSleep() {

    printf("---------------------------------------\n");
    printf("  test_PlatformTime_and_PlatformSleep\n");
    printf("---------------------------------------\n");

    PlatformTime time;

    time.timeScale = 0.5f;
    printf("time.timeScale = 0.5f\n");

    time.update();
    PlatformSleep::sleepMillis(5);
    time.update();

#if !defined(OS_TARGET_win)

    printf("PlatformSleep::sleepMillis(5): %lld\n", time.deltaTimeMicro);

#else

    printf("PlatformSleep::sleepMillis(5): %I64d\n", time.deltaTimeMicro);

#endif




    time.update();
    PlatformSleep::busySleepMicro(300);
    time.update();

#if !defined(OS_TARGET_win)
    printf("PlatformSleep::busySleepMicro(300): %lld\n", time.deltaTimeMicro);
#else
    printf("PlatformSleep::busySleepMicro(300): %I64d\n", time.deltaTimeMicro);
#endif


    time.update();
    PlatformSleep::sleepMillis(2000);
    time.update();

    printf("time.deltaTime (2 secs): %f\n", time.deltaTime);
    printf("time.unscaledDeltaTime (2 secs): %f\n", time.unscaledDeltaTime);

    printf("\n");
}
//
// Test: PlatformThread PlatformMutex
//
volatile int sharedCounter = 0;

void Thread1_IncrementCounter() {
    sharedCounter = 0;
    printf("Thread 1 Start...\n");
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 SharedCounter Increment...\n");
    sharedCounter++;
    PlatformSleep::sleepMillis(3000);
    printf("Thread 1 End...\n");
}

void Thread2_CheckCounter() {
    int localCounter = 0;

    printf("  Thread 2 Start...\n");

    while (localCounter < 3) {
        if (localCounter < sharedCounter) {
            printf("  Thread 2 Detected SharedCounter modification...\n");
            printf("  Thread 2 LocalCounter Increment...\n");
            localCounter++;
        }
        //avoid 100% CPU using by this thread
        PlatformSleep::sleepMillis(1);
    }
    printf("  Thread 2 End...\n");
}

void Thread3_Interrupt() {

    printf("Thread3_Interrupt Start...\n");
    printf("Thread3_Interrupt waiting interrupt from another thread...\n");

    while (!PlatformThread::isCurrentThreadInterrupted()) {
        //avoid 100% CPU using by this thread
        PlatformSleep::sleepMillis(3000);
    }

    printf("Thread3_Interrupt Interrupt Detected !!!\n");
    printf("Thread3_Interrupt End...\n");
}

PlatformMutex mutex;

void Thread4_Mutex() {
    printf("Thread4_Mutex Start...\n");

    mutex.lock();

    printf("Thread4_Mutex Critical Section Enter Success !!!\n");

    mutex.unlock();

    printf("Thread4_Mutex End...\n");
}

void test_PlatformThread_and_PlatformMutex() {

    printf("---------------------------------------\n");
    printf("  test_PlatformThread\n");
    printf("---------------------------------------\n");

    {
        PlatformThread t1(&Thread1_IncrementCounter);
        PlatformThread t2(&Thread2_CheckCounter);

        t1.start();
        t2.start();
    }
    printf("\n");

    printf("  InterruptTest\n");
    printf("----------------\n");
    {
        PlatformThread t3(&Thread3_Interrupt);
        t3.start();


        PlatformSleep::sleepMillis(4000);
        printf("  MainThread: Interrupt and Waiting thread To Finish!!!\n");
        t3.interrupt();
        t3.wait();
        printf("  MainThread: Thread Finish Detected!!!\n");

    }


    printf("\n");

    printf("  MutexTest\n");
    printf("----------------\n");
    {
        PlatformThread t3(&Thread4_Mutex);

        printf("  MainThread: Mutex lock before thread start\n");
        mutex.lock();

        t3.start();

        PlatformSleep::sleepSec(3);

        printf("  MainThread: Mutex unlock\n");
        mutex.unlock();

    }

    printf("\n");
}

//
// Test: PlatformPath
//
void test_PlatformPath(char* argv0) {
    printf("---------------------------------------\n");
    printf("  test_PlatformPath\n");
    printf("---------------------------------------\n");

    std::string saveGamePath = PlatformPath::getSaveGamePath("CompanyName", "GameName");
    printf("  SaveGamePath: %s\n", saveGamePath.c_str());

    std::string exePath = PlatformPath::getExecutablePath(argv0);
    std::string workPath = PlatformPath::getWorkingPath();

    printf("  exePath: %s\n", exePath.c_str());
    printf("  workPath: %s\n", workPath.c_str());

    printf(" changeWorkPath!!!\n");
    PlatformPath::setWorkingPath(PlatformPath::getExecutablePath(argv0));

    workPath = PlatformPath::getWorkingPath();
    printf("  new workPath: %s\n", workPath.c_str());

    printf("\n");
}

//
// Test: Util test
//

void DelegateFunctionOutside(int v) {
    printf("  executed function DelegateFunctionOutside: %i\n", v);
}
class DelegateTest {
public:
    BEGIN_DECLARE_DELEGATE(MethodDelegateWithIntParameter, int v) CALL_PATTERN(v) END_DECLARE_DELEGATE;

    MethodDelegateWithIntParameter OnInt;

    DelegateTest() {

        printf(" DelegateTest\n");

        OnInt.add(this, &DelegateTest::int1);
        OnInt.add(this, &DelegateTest::int2);
        OnInt.add(&DelegateFunctionOutside);

        OnInt(10);
    }

    void int1(int v) {
        printf("  executed method int1: %i\n", v);
    }

    void int2(int v) {
        printf("  executed method int2: %i\n", v);
    }
};

void test_Util() {
    printf("---------------------------------------\n");
    printf("  test_Util\n");
    printf("---------------------------------------\n");
    {
        DelegateTest();
    }

    printf("\n");

    printf(" getFirstMacAddress\n");
    getFirstMacAddress();

    printf("\n");
    printf(" setNullAndDelete\n");
    int* test = new int;

    
#if defined(ARCH_TARGET_x86) || defined(ARCH_TARGET_arm32)
    printf(" address before: 0x%x\n", (unsigned int)test);
#elif defined(ARCH_TARGET_x64) || defined(ARCH_TARGET_arm64)

#if !defined(OS_TARGET_win)
    printf(" address before: 0x%llx\n", (uint64_t)test);
#else
    printf(" address before: 0x%I64x\n", (uint64_t)test);
#endif

#else
    printf(" ARCH DETECT ERROR...\n");
#endif

    setNullAndDelete(test);

#if defined(ARCH_TARGET_x86) || defined(ARCH_TARGET_arm32)
    printf(" address after: 0x%x\n", (unsigned int)test);
#elif defined(ARCH_TARGET_x64) || defined(ARCH_TARGET_arm64)
#if !defined(OS_TARGET_win)
    printf(" address after: 0x%llx\n", (uint64_t)test);
#else
    printf(" address after: 0x%I64x\n", (uint64_t)test);
#endif
#else
    printf(" ARCH DETECT ERROR...\n");
#endif

    printf("\n");
}


int main(int argc, char* argv[]) {


    printf("  press any key to do move functions test...\n");fgetc(stdin);
    test_MoveFunctions();
    printf("  press any key to do quaternion test...\n"); fgetc(stdin);
    test_Quaternions();
    printf("  press any key to do path test...\n"); fgetc(stdin);
    test_PlatformPath(argv[0]);
    printf("  press any key to do time and sleep test...\n"); fgetc(stdin);
    test_PlatformTime_and_PlatformSleep();
    printf("  press any key to do thread and mutex test...\n"); fgetc(stdin);
    test_PlatformThread_and_PlatformMutex();
    printf("  press any key to do util test...\n"); fgetc(stdin);
    test_Util();


    printf("  press any key to exit...\n");
    fgetc(stdin);

    return 0;
}
