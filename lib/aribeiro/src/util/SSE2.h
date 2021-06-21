#ifndef SSE2__H
#define SSE2__H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
//#include <malloc.h>


#if defined(ARIBEIRO_SSE2)

    #include <xmmintrin.h> // SSE1
    #include <emmintrin.h> // SSE2


    #ifdef _MSC_VER //_WIN32
        //  Windows
        //#ifdef _MSC_VER
        #include <intrin.h>
        //#else
        //#define cpuid(info, x)    __cpuidex(info, x, 0)
        //#endif
    #else
        //  GCC Intrinsics
        #include <cpuid.h>
        void cpuid(int info[4], int InfoType);
        unsigned long long custom_xgetbv(unsigned int index);
    #endif

    class CPUInfo {

        CPUInfo();

    public:

        static const CPUInfo &Instance();

        //  Misc.
        bool HW_MMX;
        bool HW_x64;
        bool HW_ABM;      // Advanced Bit Manipulation
        bool HW_RDRAND;
        bool HW_BMI1;
        bool HW_BMI2;
        bool HW_ADX;
        bool HW_PREFETCHWT1;

        //  SIMD: 128-bit
        bool HW_SSE;
        bool HW_SSE2;
        bool HW_SSE3;
        bool HW_SSSE3;
        bool HW_SSE41;
        bool HW_SSE42;
        bool HW_SSE4a;
        bool HW_AES;
        bool HW_SHA;

        //  SIMD: 256-bit
        bool HW_AVX;
        bool HW_XOP; // SSE5
        bool HW_FMA3;
        bool HW_FMA4;
        bool HW_AVX2;

        //  SIMD: 512-bit
        bool HW_AVX512F;    //  AVX512 Foundation
        bool HW_AVX512CD;   //  AVX512 Conflict Detection
        bool HW_AVX512PF;   //  AVX512 Prefetch
        bool HW_AVX512ER;   //  AVX512 Exponential + Reciprocal
        bool HW_AVX512VL;   //  AVX512 Vector Length Extensions
        bool HW_AVX512BW;   //  AVX512 Byte + Word
        bool HW_AVX512DQ;   //  AVX512 Doubleword + Quadword
        bool HW_AVX512IFMA; //  AVX512 Integer 52-bit Fused Multiply-Add
        bool HW_AVX512VBMI; //  AVX512 Vector Byte Manipulation Instructions

        bool OS_AVX_SUPPORTED;
    };

    /*
    #pragma pack(push, 16)

    class op{
    public:
        float a[4] _MM_ALIGN16;
        float b[4] _MM_ALIGN16;
        float c[4] _MM_ALIGN16;
    };

    #pragma pack(pop)
    */

    #include <stdio.h>

    class CheckSSE2 {
    public:
        CheckSSE2();
    };

    extern const CheckSSE2 ___CheckSSE2;

    #if _MSC_VER

        //windows compatible
        #define _SSE2_ALIGN_PRE _MM_ALIGN16

        //other platforms compatible
        #define _SSE2_ALIGN_POS
        #define ARIBEIRO_INLINE __forceinline


        #define _mm_f32_(v,i) v.m128_f32[i]
        #define _mm_load_(x,y,z,w) _mm_set_ps(w,z,y,x)

    #else

        #define _SSE2_ALIGN_PRE

        #ifndef _MM_ALIGN16
            #define _MM_ALIGN16 __attribute__ (( __aligned__ (16)))
        #endif

        #define _SSE2_ALIGN_POS _MM_ALIGN16
        #define ARIBEIRO_INLINE inline __attribute__((always_inline))

        #define _mm_f32_(v,i) v[i]
        #define _mm_load_(x,y,z,w) _mm_set_ps(w,z,y,x)

    #endif


    //
    // Need to inherits this to objects allocated in the heap, avoiding missaligned data access when using SSE2
    //
    /*
    class _SSE2_ALIGN_PRE SSE2Object {

    public:

        SSE2_CLASS_NEW_OPERATOR

    } _SSE2_ALIGN_POS;
    */

    //
    // In Visual Studio the function parameter passing cannot be larger than 8 bytes... you cannot pass an object as parameter by value
    //
    //
    // Another way to write sse2 compatible classes, is use SSE2_CLASS_NEW_OPERATOR
    //
    //
    //  these operators guarantees the 16 byte alignment of the class
    //
    #define SSE2_CLASS_NEW_OPERATOR \
    ARIBEIRO_INLINE void* operator new(size_t size) {\
        size_t complete_16bytes = ( 16 - size % 16 ) % 16;\
        return _mm_malloc(size + complete_16bytes, 16);\
    }\
    ARIBEIRO_INLINE void operator delete(void* p) { \
        _mm_free(p);\
    }\
    ARIBEIRO_INLINE void* operator new[](size_t size) {\
        size_t complete_16bytes = ( 16 - size % 16 ) % 16;\
        return _mm_malloc(size + complete_16bytes, 16);\
    }\
    ARIBEIRO_INLINE void operator delete[](void* p) {\
        _mm_free(p);\
    }\
    ARIBEIRO_INLINE void* operator new (std::size_t n, void* ptr)\
    {\
        return ptr;\
    }\
    ARIBEIRO_INLINE void operator delete(void *objectAllocated, void* ptr) {\
    }

    /*
    is possible to overload the global new operator...

    void* operator new     ( size_t size ) { return myAlloc( size ); }
    void* operator new[]   ( size_t size ) { return myAlloc( size ); }
    void  operator delete  ( void* ptr   ) { myFree( ptr ); }
    void  operator delete[]( void* ptr   ) { myFree( ptr ); }

    */

#elif defined(ARIBEIRO_NEON)

    #include <arm_neon.h>

    #define _SSE2_ALIGN_PRE
    #define _SSE2_ALIGN_POS __attribute__ (( __aligned__ (16)))
    #define ARIBEIRO_INLINE inline __attribute__((always_inline))

    #define SSE2_CLASS_NEW_OPERATOR \
    ARIBEIRO_INLINE void* operator new(size_t size) {\
        size_t complete_16bytes = ( 16 - size % 16 ) % 16;\
        return aligned_alloc(16, size+complete_16bytes);\
    }\
    ARIBEIRO_INLINE void operator delete(void* p) { \
        free(p);\
    }\
    ARIBEIRO_INLINE void* operator new[](size_t size) {\
        size_t complete_16bytes = ( 16 - size % 16 ) % 16;\
        return aligned_alloc(16, size+complete_16bytes);\
    }\
    ARIBEIRO_INLINE void operator delete[](void* p) {\
        free(p);\
    }\
    ARIBEIRO_INLINE void* operator new (std::size_t n, void* ptr)\
    {\
        return ptr;\
    }\
    ARIBEIRO_INLINE void operator delete(void *objectAllocated, void* ptr) {\
    }

#else

    //
    // Not SSE Code
    //

    #ifdef ARIBEIRO_RPI

        #define _SSE2_ALIGN_PRE
        #define _SSE2_ALIGN_POS __attribute__ (( __aligned__ (16)))
        #define ARIBEIRO_INLINE inline __attribute__((always_inline))

    #else

        #define _SSE2_ALIGN_PRE
        #define _SSE2_ALIGN_POS

        #if _MSC_VER
            #define ARIBEIRO_INLINE __forceinline
        #else
            #define ARIBEIRO_INLINE inline __attribute__((always_inline))
        #endif

    #endif


    /*
    class SSE2Object {

    public:

    };
    */

    #define SSE2_CLASS_NEW_OPERATOR


#endif


#include <new>

namespace aRibeiro {

    /// \brief SSE Align is a template definition of an aligned STL allocator.
    ///
    /// It can be used to force the STL internal structures to be aligned with the parameter.
    ///
    /// It is required to align the memory to 16 bytes to use the SSE2 instruction set.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// // defining an aligned vector
    /// std::vector< vec3, ssealign<vec3> > _16_bytes_vector_aligned;
    ///
    /// ...
    ///
    /// // defining an aligned map
    /// std::map<std::string, vec3, std::less<std::string>,ssealign<std::pair<const std::string, vec3> > > _16_bytes_map_aligned;
    ///
    /// ...
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T, size_t N = 16>
    class ssealign {
    public:
        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef T * pointer;
        typedef const T * const_pointer;

        typedef T & reference;
        typedef const T & const_reference;

    public:
        inline ssealign() throw () { }

        template <typename T2>
        inline ssealign(const ssealign<T2, N> &) throw () { }

        inline ~ssealign() throw () { }

        inline pointer adress(reference r) {
            return &r;
        }

        inline const_pointer adress(const_reference r) const {
            return &r;
        }

        inline pointer allocate(size_type n) {
            //return (pointer)_aligned_malloc(n * sizeof(value_type), N);
            size_t size = n * sizeof(value_type);
            size_t complete_16bytes = (N - size % N) % N;
#if defined(ARIBEIRO_SSE2)
            return (pointer)_mm_malloc(size + complete_16bytes, N);
#elif defined(ARIBEIRO_RPI)
            return (pointer)aligned_alloc(N, size + complete_16bytes);
#else
            return (pointer)malloc(size + complete_16bytes);
#endif
        }

        inline void deallocate(pointer p, size_type) {
            //_aligned_free(p);
#if defined(ARIBEIRO_SSE2)
            _mm_free(p);
#elif defined(ARIBEIRO_RPI)
            free(p);
#else
            free(p);
#endif
        }

        inline void construct(pointer p, const value_type & wert) {
            new (p) value_type(wert);
            //if (p == 0)
                //return;
            //*p = wert;
        }

        inline void destroy(pointer p) {
            p->~value_type();
        }

        inline size_type max_size() const throw () {
            return size_type(-1) / sizeof(value_type);
        }

        template <typename T2>
        struct rebind {
            typedef ssealign<T2, N> other;
        };

        bool operator!=(const ssealign<T, N>& other) const {
            return !(*this == other);
        }

        // Returns true if and only if storage allocated from *this
        // can be deallocated from other, and vice versa.
        // Always returns true for stateless allocators.
        bool operator==(const ssealign<T, N>& other) const {
            return true;
        }
    };
}

#include <vector>
#include <map>

namespace aRibeiro {
    /// \brief Template class specialization of the STL std::map
    ///
    /// It allows to define a 16 bytes aligned std::map structure in a short way.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// // defining an aligned map
    /// aligned_map<std::string, vec3> _16_bytes_map_aligned;
    ///
    /// ...
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <class __map_Key_, class _map_Tp_>
    class aligned_map : public std::map<__map_Key_, _map_Tp_, std::less<__map_Key_>, ssealign<std::pair<const __map_Key_, _map_Tp_> > > {
    public:
        typedef std::map<__map_Key_, _map_Tp_, std::less<__map_Key_>, ssealign<std::pair<const __map_Key_, _map_Tp_> > > __map__base;
        typedef typename __map__base::key_compare                                           __my_key_compare;

        ARIBEIRO_INLINE aligned_map() :__map__base() {}
        ARIBEIRO_INLINE aligned_map(const aligned_map& __m) : __map__base(__m) {}
        ARIBEIRO_INLINE aligned_map(const __my_key_compare& __comp) : __map__base(__comp) {}

    };

    /// \brief Template class specialization of the STL std::vector
    ///
    /// It allows to define a 16 bytes aligned std::vector structure in a short way.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// // defining an aligned vector
    /// aligned_vector<vec3> _16_bytes_vector_aligned;
    ///
    /// ...
    ///
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <class _vector_Tp_>
    class aligned_vector : public std::vector<_vector_Tp_, ssealign<_vector_Tp_> > {
    public:
        typedef std::vector<_vector_Tp_, ssealign<_vector_Tp_> >           __vector__base;
        //typedef typename __vector__base::allocator_type    __my_allocator_type;
        typedef typename __vector__base::size_type         __my_size_type;

        ARIBEIRO_INLINE aligned_vector() :__vector__base() {}
        ARIBEIRO_INLINE aligned_vector(__my_size_type __n) : __vector__base(__n) {}
        ARIBEIRO_INLINE aligned_vector(const aligned_vector& __v) : __vector__base(__v) {}

    };
}

#if DOXYGEN
#undef ARIBEIRO_INLINE
#define ARIBEIRO_INLINE

#undef _SSE2_ALIGN_PRE
#define _SSE2_ALIGN_PRE

#undef _SSE2_ALIGN_POS
#define _SSE2_ALIGN_POS
#endif

#endif
