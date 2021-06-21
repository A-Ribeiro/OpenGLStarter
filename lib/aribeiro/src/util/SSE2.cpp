#include "SSE2.h"

#include "aribeiro/aribeiro.h"

#if defined(ARIBEIRO_SSE2)

#include <stdio.h>
#include <stdlib.h> // exit

#ifdef _MSC_VER //_WIN32

    #define cpuid(info, x)    __cpuidex(info, x, 0)


#else

void cpuid(int info[4], int InfoType){
    __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}

unsigned long long custom_xgetbv(unsigned int index)
{
    unsigned int eax, edx;
    __asm__ __volatile__(
                         "xgetbv;"
                         : "=a" (eax), "=d"(edx)
                         : "c" (index)
                         );
    return ((unsigned long long)edx << 32) | eax;
}

#endif


    CPUInfo::CPUInfo(){
        int info[4];
        cpuid(info, 0);
        int nIds = info[0];

        cpuid(info, 0x80000000);
        unsigned nExIds = info[0];

        //  Detect Features
        if (nIds >= 0x00000001){
            cpuid(info,0x00000001);
            HW_MMX    = (info[3] & ((int)1 << 23)) != 0;
            HW_SSE    = (info[3] & ((int)1 << 25)) != 0;
            HW_SSE2   = (info[3] & ((int)1 << 26)) != 0;
            HW_SSE3   = (info[2] & ((int)1 <<  0)) != 0;

            HW_SSSE3  = (info[2] & ((int)1 <<  9)) != 0;
            HW_SSE41  = (info[2] & ((int)1 << 19)) != 0;
            HW_SSE42  = (info[2] & ((int)1 << 20)) != 0;
            HW_AES    = (info[2] & ((int)1 << 25)) != 0;

            HW_AVX    = (info[2] & ((int)1 << 28)) != 0;
            HW_FMA3   = (info[2] & ((int)1 << 12)) != 0;

            HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
        }
        if (nIds >= 0x00000007){
            cpuid(info,0x00000007);
            HW_AVX2   = (info[1] & ((int)1 <<  5)) != 0;

            HW_BMI1        = (info[1] & ((int)1 <<  3)) != 0;
            HW_BMI2        = (info[1] & ((int)1 <<  8)) != 0;
            HW_ADX         = (info[1] & ((int)1 << 19)) != 0;
            HW_SHA         = (info[1] & ((int)1 << 29)) != 0;
            HW_PREFETCHWT1 = (info[2] & ((int)1 <<  0)) != 0;

            HW_AVX512F     = (info[1] & ((int)1 << 16)) != 0;
            HW_AVX512CD    = (info[1] & ((int)1 << 28)) != 0;
            HW_AVX512PF    = (info[1] & ((int)1 << 26)) != 0;
            HW_AVX512ER    = (info[1] & ((int)1 << 27)) != 0;
        HW_AVX512VL    = (info[1] & ((int)1 << 31)) != 0;
        HW_AVX512BW    = (info[1] & ((int)1 << 30)) != 0;
        HW_AVX512DQ    = (info[1] & ((int)1 << 17)) != 0;
        HW_AVX512IFMA  = (info[1] & ((int)1 << 21)) != 0;
        HW_AVX512VBMI  = (info[2] & ((int)1 <<  1)) != 0;
    }
    if (nExIds >= 0x80000001){
        cpuid(info,0x80000001);
        HW_x64   = (info[3] & ((int)1 << 29)) != 0;
        HW_ABM   = (info[2] & ((int)1 <<  5)) != 0;
        HW_SSE4a = (info[2] & ((int)1 <<  6)) != 0;
        HW_FMA4  = (info[2] & ((int)1 << 16)) != 0;
        HW_XOP   = (info[2] & ((int)1 << 11)) != 0;
    }

    OS_AVX_SUPPORTED = false;
    if (HW_AVX512ER && HW_AVX512CD) {
        // _XCR_XFEATURE_ENABLED_MASK = 0

        #ifdef _MSC_VER
            unsigned long long xcrFeatureMask = _xgetbv(0);
        #else
            unsigned long long xcrFeatureMask = custom_xgetbv(0);
        #endif
        OS_AVX_SUPPORTED = (xcrFeatureMask & 0x6) == 0x6;

    }

}

const CPUInfo &CPUInfo::Instance(){
    static CPUInfo cpuInfo;
    return cpuInfo;
}

CheckSSE2::CheckSSE2() {

    ARIBEIRO_ABORT(!CPUInfo::Instance().HW_SSE2, "SSE2 not available in this CPU/OS.\n");

    //if (!CPUInfo::Instance().HW_SSE2){
    //    fprintf(stderr,"SSE2 not available in this CPU/OS.\n");
    //    exit(-1);
    //} else {
    //    //printf("SSE2 available !\n");
    //}
}

const CheckSSE2 ___CheckSSE2;


#endif
