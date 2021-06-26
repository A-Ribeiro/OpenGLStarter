#ifndef mat4_h
#define mat4_h

#include <aribeiro/common.h>
#include <aribeiro/vec4.h>
#include <aribeiro/SSE2.h>
#include <aribeiro/floatOPs.h>

namespace aRibeiro {

#if defined(ARIBEIRO_SSE2)
#pragma pack(push, 16)
#endif

    /// \brief Matrix with 4x4 components
    ///
    /// Matrix definition to work with rigid transformations
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with mat4 and float combinations.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aribeiro/aribeiro.h>
    /// using namespace aRibeiro;
    ///
    /// mat4 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    ///
    /// \author Alessandro Ribeiro
    ///
    class _SSE2_ALIGN_PRE mat4 {
    public:
        union _SSE2_ALIGN_PRE {
            struct _SSE2_ALIGN_PRE {
                float _11, _21, _31, _41,
                    _12, _22, _32, _42,
                    _13, _23, _33, _43,
                    _14, _24, _34, _44;
            }_SSE2_ALIGN_POS;
            struct _SSE2_ALIGN_PRE {
                float a1, a2, a3, a4,
                    b1, b2, b3, b4,
                    c1, c2, c3, c4,
                    d1, d2, d3, d4;
            }_SSE2_ALIGN_POS;
            float _SSE2_ALIGN_PRE array[16]_SSE2_ALIGN_POS;
            // column-major (OpenGL like matrix byte order)
            //  x  y  z  w
            //  0  4  8 12
            //  1  5  9 13
            //  2  6 10 14
            //  3  7 11 15
#if defined(ARIBEIRO_SSE2)
            __m128 _SSE2_ALIGN_PRE array_sse[4] _SSE2_ALIGN_POS;
#endif

#if defined(ARIBEIRO_NEON)
            float32x4_t _SSE2_ALIGN_PRE array_neon[4] _SSE2_ALIGN_POS;
#endif


        }_SSE2_ALIGN_POS;

#if defined(ARIBEIRO_SSE2)
    //special SSE2 constructor
        ARIBEIRO_INLINE mat4(const __m128 &a, const __m128 &b, const __m128 &c, const __m128 &d) {
            array_sse[0] = a;
            array_sse[1] = b;
            array_sse[2] = c;
            array_sse[3] = d;
        }
#endif

#if defined(ARIBEIRO_NEON)
        ARIBEIRO_INLINE mat4(const float32x4_t &a, const float32x4_t &b, const float32x4_t &c, const float32x4_t &d) {
            array_neon[0] = a;
            array_neon[1] = b;
            array_neon[2] = c;
            array_neon[3] = d;
        }
#endif

        //---------------------------------------------------------------------------
        /// \brief Constructs an identity matrix 4x4
        ///
        /// This construct an identity matrix
        ///
        /// <pre>
        /// | 1 0 0 0 |
        /// | 0 1 0 0 |
        /// | 0 0 1 0 |
        /// | 0 0 0 1 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix = mat4();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ARIBEIRO_INLINE mat4() {
            const mat4 mat4_IdentityMatrix(
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
            *this = mat4_IdentityMatrix;
        }
        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize all components of the matrix with the same value
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix = mat4( 10.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value Value to initialize the components
        ///
        ARIBEIRO_INLINE mat4(const float &value) {
#if defined(ARIBEIRO_SSE2)
            __m128 data = _mm_set1_ps(value);
            array_sse[0] = data;
            array_sse[1] = data;
            array_sse[2] = data;
            array_sse[3] = data;
#elif defined(ARIBEIRO_NEON)
            float32x4_t data = vset1(value);
            array_neon[0] = data;
            array_neon[1] = data;
            array_neon[2] = data;
            array_neon[3] = data;
#else
            _11 = _12 = _13 = _14 = _21 = _22 = _23 = _24 =
                _31 = _32 = _33 = _34 = _41 = _42 = _43 = _44 = value;
#endif
        }
        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat4 components from the parameters
        ///
        /// The visual is related to the matrix column major order.
        ///
        /// <pre>
        /// | a1 b1 c1 d1 |
        /// | a2 b2 c2 d2 |
        /// | a3 b3 c3 d3 |
        /// | a4 b4 c4 d4 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix = mat4( 1.0f, 0.0f, 0.0f, 0.0f,
        ///                     0.0f, 1.0f, 0.0f, 0.0f,
        ///                     0.0f, 0.0f, 1.0f, 0.0f,
        ///                     0.0f, 0.0f, 0.0f, 1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ARIBEIRO_INLINE mat4(
            const float &a1, const float &b1, const float &c1, const float &d1,
            const float &a2, const float &b2, const float &c2, const float &d2,
            const float &a3, const float &b3, const float &c3, const float &d3,
            const float &a4, const float &b4, const float &c4, const float &d4) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = _mm_load_(a1, a2, a3, a4);//_mm_set_ps(a4, a1, a2, a3);
            array_sse[1] = _mm_load_(b1, b2, b3, b4);
            array_sse[2] = _mm_load_(c1, c2, c3, c4);
            array_sse[3] = _mm_load_(d1, d2, d3, d4);
#elif defined(ARIBEIRO_NEON)
            array_neon[0] = (float32x4_t) { a1, a2, a3, a4 };//_mm_set_ps(a4, a1, a2, a3);
            array_neon[1] = (float32x4_t) { b1, b2, b3, b4 };
            array_neon[2] = (float32x4_t) { c1, c2, c3, c4 };
            array_neon[3] = (float32x4_t) { d1, d2, d3, d4 };
#else
            _11 = a1; _12 = b1; _13 = c1; _14 = d1;
            _21 = a2; _22 = b2; _23 = c2; _24 = d2;
            _31 = a3; _32 = b3; _33 = c3; _34 = d3;
            _41 = a4; _42 = b4; _43 = c4; _44 = d4;
#endif
        }
        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat4 components by copying other mat4 instance
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix_src = mat4( 1.0f, 0.0f, 0.0f, 0.0f,
        ///                         0.0f, 1.0f, 0.0f, 0.0f,
        ///                         0.0f, 0.0f, 1.0f, 0.0f,
        ///                         0.0f, 0.0f, 0.0f, 1.0f);
        ///
        /// mat4 matrix = mat4( matrix_src );
        ///
        /// mat4 matrix_a = matrix_src;
        ///
        /// mat4 matrix_b;
        /// matrix_b = matrix_src;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to assign to the instance
        ///
        ARIBEIRO_INLINE mat4(const mat4 &m) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = m.array_sse[0];
            array_sse[1] = m.array_sse[1];
            array_sse[2] = m.array_sse[2];
            array_sse[3] = m.array_sse[3];
#elif defined(ARIBEIRO_NEON)
            array_neon[0] = m.array_neon[0];
            array_neon[1] = m.array_neon[1];
            array_neon[2] = m.array_neon[2];
            array_neon[3] = m.array_neon[3];
#else
            *this = m;
#endif
            //memcpy(array,m.array,sizeof(float)*4*4);
        }

        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat4 components from vec4 parameters
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to assign to the instance
        ///
        ARIBEIRO_INLINE mat4(const vec4 &a, const vec4 &b, const vec4 &c, const vec4 &d) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = a.array_sse;
            array_sse[1] = b.array_sse;
            array_sse[2] = c.array_sse;
            array_sse[3] = d.array_sse;
#elif defined(ARIBEIRO_NEON)
            array_neon[0] = a.array_neon;
            array_neon[1] = b.array_neon;
            array_neon[2] = c.array_neon;
            array_neon[3] = d.array_neon;
#else
            _11 = a.x; _12 = b.x; _13 = c.x; _14 = d.x;
            _21 = a.y; _22 = b.y; _23 = c.y; _24 = d.y;
            _31 = a.z; _32 = b.z; _33 = c.z; _34 = d.z;
            _41 = a.w; _42 = b.w; _43 = c.w; _44 = d.w;
#endif
            //memcpy(array,m.array,sizeof(float)*4*4);
        }

        //---------------------------------------------------------------------------
        /// \brief Matrix multiplication
        ///
        /// Makes the full 4x4 matrix multiplication
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix, other_matrix;
        ///
        /// matrix *= other_matrix;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param M the matrix to be multiplied by the current instance
        /// \return A reference to the multiplied matrix current instance
        ///
        ARIBEIRO_INLINE mat4& operator*=(const mat4 &M) {

#if defined(ARIBEIRO_SSE2)
            __m128 array_sse_result[4];
            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[0], M.array_sse[0], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[0] = a2;
            }

            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[1], M.array_sse[1], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[1] = a2;
            }

            {
                __m128 e0 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[2], M.array_sse[2], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[2] = a2;
            }

            {
                //(__m128&)_mm_shuffle_epi32(__m128i&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
                __m128 e0 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(0, 0, 0, 0));
                __m128 e1 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(1, 1, 1, 1));
                __m128 e2 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(2, 2, 2, 2));
                __m128 e3 = _mm_shuffle_ps(M.array_sse[3], M.array_sse[3], _MM_SHUFFLE(3, 3, 3, 3));

                __m128 m0 = _mm_mul_ps(array_sse[0], e0);
                __m128 m1 = _mm_mul_ps(array_sse[1], e1);
                __m128 m2 = _mm_mul_ps(array_sse[2], e2);
                __m128 m3 = _mm_mul_ps(array_sse[3], e3);

                __m128 a0 = _mm_add_ps(m0, m1);
                __m128 a1 = _mm_add_ps(m2, m3);
                __m128 a2 = _mm_add_ps(a0, a1);

                array_sse_result[3] = a2;
            }

            array_sse[0] = array_sse_result[0];
            array_sse[1] = array_sse_result[1];
            array_sse[2] = array_sse_result[2];
            array_sse[3] = array_sse_result[3];

#elif defined(ARIBEIRO_NEON)

            float32x4_t array_neon_result[4];
            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[0]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[0]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[0]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[0]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[0] = a2;
            }

            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[1]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[1]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[1]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[1]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[1] = a2;
            }

            {
                float32x4_t e0 = vshuffle_0000(M.array_neon[2]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[2]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[2]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[2]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[2] = a2;
            }

            {
                //(float32x4_t&)_mm_shuffle_epi32(float32x4_ti&)in2[0], _MM_SHUFFLE(3, 3, 3, 3))
                float32x4_t e0 = vshuffle_0000(M.array_neon[3]);
                float32x4_t e1 = vshuffle_1111(M.array_neon[3]);
                float32x4_t e2 = vshuffle_2222(M.array_neon[3]);
                float32x4_t e3 = vshuffle_3333(M.array_neon[3]);

                float32x4_t m0 = vmulq_f32(array_neon[0], e0);
                float32x4_t m1 = vmulq_f32(array_neon[1], e1);
                float32x4_t m2 = vmulq_f32(array_neon[2], e2);
                float32x4_t m3 = vmulq_f32(array_neon[3], e3);

                float32x4_t a0 = vaddq_f32(m0, m1);
                float32x4_t a1 = vaddq_f32(m2, m3);
                float32x4_t a2 = vaddq_f32(a0, a1);

                array_neon_result[3] = a2;
            }

            array_neon[0] = array_neon_result[0];
            array_neon[1] = array_neon_result[1];
            array_neon[2] = array_neon_result[2];
            array_neon[3] = array_neon_result[3];

#else

            float a, b, c, d;
            a = _11; b = _12; c = _13; d = _14;
            _11 = (a*M._11 + b * M._21 + c * M._31 + d * M._41);
            _12 = (a*M._12 + b * M._22 + c * M._32 + d * M._42);
            _13 = (a*M._13 + b * M._23 + c * M._33 + d * M._43);
            _14 = (a*M._14 + b * M._24 + c * M._34 + d * M._44);

            a = _21; b = _22; c = _23; d = _24;
            _21 = (a*M._11 + b * M._21 + c * M._31 + d * M._41);
            _22 = (a*M._12 + b * M._22 + c * M._32 + d * M._42);
            _23 = (a*M._13 + b * M._23 + c * M._33 + d * M._43);
            _24 = (a*M._14 + b * M._24 + c * M._34 + d * M._44);

            a = _31; b = _32; c = _33; d = _34;
            _31 = (a*M._11 + b * M._21 + c * M._31 + d * M._41);
            _32 = (a*M._12 + b * M._22 + c * M._32 + d * M._42);
            _33 = (a*M._13 + b * M._23 + c * M._33 + d * M._43);
            _34 = (a*M._14 + b * M._24 + c * M._34 + d * M._44);

            a = _41; b = _42; c = _43; d = _44;
            _41 = (a*M._11 + b * M._21 + c * M._31 + d * M._41);
            _42 = (a*M._12 + b * M._22 + c * M._32 + d * M._42);
            _43 = (a*M._13 + b * M._23 + c * M._33 + d * M._43);
            _44 = (a*M._14 + b * M._24 + c * M._34 + d * M._44);
#endif
            return *this;
        }
        //---------------------------------------------------------------------------
        /// \brief Matrix access based on X (col) and Y (row)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix(3,0) = 1.0f;
        ///
        /// float v = matrix(3,3);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x The column to get the element at index
        /// \param y The row to get the element at index
        /// \return A reference to the matrix element
        ///
        ARIBEIRO_INLINE float& operator()(const int x, const int y) {
            return array[y * 4 + x];
        }
        //---------------------------------------------------------------------------
        /// \brief Matrix row access based
        ///
        /// Acess one of the 4 rows of the matrix as a vec4 type
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        /// vec3 translate_vec;
        ///
        /// vec4 forward = matrix[2];
        ///
        /// matrix[3] = toPtn4( translate_vec );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x The row to get
        /// \return A reference to the matrix row as vec4
        ///
        ARIBEIRO_INLINE vec4& operator[](const int x) {
            return *((vec4*)&array[x * 4]);
        }

        /// \brief Matrix row access based
        ///
        /// Acess one of the 4 rows of the matrix as a vec4 type
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// void process_matrix( const mat4 &matrix ) {
        ///     vec4 forward = matrix[2];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x The row to get
        /// \return A reference to the matrix row as vec4
        ///
        ARIBEIRO_INLINE const vec4 &operator[](const int v)const {
            return *((vec4*)&array[v * 4]);
        }
        //---------------------------------------------------------------------------
        /// \brief Compare two matrix using the #EPSILON constant
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix_a, matrix_b;
        ///
        /// if ( matrix_a == matrix_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix to compare with
        /// \return true: the matrix is equal, considering the #EPSILON
        ///
        ARIBEIRO_INLINE bool operator==(const mat4&v) const {

#if defined(ARIBEIRO_SSE2)

            //const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

            __m128 diff_abs[4];
            diff_abs[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
            diff_abs[0] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[0]); //abs

            diff_abs[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
            diff_abs[1] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[1]); //abs

            diff_abs[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);
            diff_abs[2] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[2]); //abs

            diff_abs[3] = _mm_sub_ps(array_sse[3], v.array_sse[3]);
            diff_abs[3] = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs[3]); //abs

            __m128 accumulator_a = _mm_add_ps(diff_abs[0], diff_abs[1]);
            __m128 accumulator_b = _mm_add_ps(diff_abs[2], diff_abs[3]);

            __m128 accumulator = _mm_add_ps(accumulator_a, accumulator_b);

#if true //defined(_MSC_VER)

            accumulator = _mm_hadd_ps(accumulator, accumulator);
            accumulator = _mm_hadd_ps(accumulator, accumulator);

#else
            //swp0 = [1,0,3,2]
            __m128 swp0 = _mm_shuffle_ps(accumulator, accumulator, _MM_SHUFFLE(2, 3, 0, 1));
            //add0 = [0+1,1+0,2+3,3+2]
            __m128 add0 = _mm_add_ps(accumulator, swp0);
            //swp1 = [3+2,2+3,1+0,0+1]
            __m128 swp1 = _mm_shuffle_ps(add0, add0, _MM_SHUFFLE(0, 1, 2, 3));
            //add1 = [0+1+3+2,1+0+2+3,2+3+1+0,3+2+0+1]
            accumulator = _mm_add_ps(add0, swp1);
#endif

            if (_mm_f32_(accumulator, 0) > EPSILON2)
                return false;

            /*
            //const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
            //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
            for(int j=0;j<4;j++){
                for(int i=0;i<4;i++){
                    if (_mm_f32_(diff_abs[j],i) > EPSILON2)
                        return false;
                }
            }
            */

            return true;

#elif defined(ARIBEIRO_NEON)


            float32x4_t diff_abs[4];
            diff_abs[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
            diff_abs[0] = vabsq_f32(diff_abs[0]); //abs

            diff_abs[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
            diff_abs[1] = vabsq_f32(diff_abs[1]); //abs

            diff_abs[2] = vsubq_f32(array_neon[2], v.array_neon[2]);
            diff_abs[2] = vabsq_f32(diff_abs[2]); //abs

            diff_abs[3] = vsubq_f32(array_neon[3], v.array_neon[3]);
            diff_abs[3] = vabsq_f32(diff_abs[3]); //abs

            float32x4_t accumulator_a = vaddq_f32(diff_abs[0], diff_abs[1]);
            float32x4_t accumulator_b = vaddq_f32(diff_abs[2], diff_abs[3]);

            float32x4_t acc_4_elements = vaddq_f32(accumulator_a, accumulator_b);

            float32x2_t acc_2_elements = vadd_f32(vget_high_f32(acc_4_elements), vget_low_f32(acc_4_elements));
            acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

            if (acc_2_elements[0] > EPSILON2)
                return false;

            /*
            //const __m128 epsilon = _mm_set1_ps(1e-4f); // -0.f = 1 << 31
            //_mm_shuffle_ps(mul0, mul0, _MM_SHUFFLE(2, 3, 0, 1));
            for(int j=0;j<4;j++){
                for(int i=0;i<4;i++){
                    if (diff_abs[j][i] > EPSILON2)
                        return false;
                }
            }
            */

            return true;

#else
            float accumulator = 0.0f;
            for (int i = 0; i < 16; i++) {
                accumulator += absv(array[i] - v.array[i]);
            }
            if (accumulator >= EPSILON2)//EPSILON
                return false;
            return true;
            //return memcmp(array, v.array, sizeof(float) * 16) == 0;
#endif
        }

        /// \brief Compare two matrix using the #EPSILON constant
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix_a, matrix_b;
        ///
        /// if ( matrix_a != matrix_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix to compare with
        /// \return true: the matrix is not equal, considering the #EPSILON
        ///
        ARIBEIRO_INLINE bool operator!=(const mat4&v) const {
            return !((*this) == v);
        }

        /// \brief Component-wise add elements of the matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix, matrix_b;
        ///
        /// matrix += matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix used to add values
        /// \return The matrix with the sum result
        ///
        ARIBEIRO_INLINE mat4& operator+=(const mat4& v) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = _mm_add_ps(array_sse[0], v.array_sse[0]);
            array_sse[1] = _mm_add_ps(array_sse[1], v.array_sse[1]);
            array_sse[2] = _mm_add_ps(array_sse[2], v.array_sse[2]);
            array_sse[3] = _mm_add_ps(array_sse[3], v.array_sse[3]);


#elif defined(ARIBEIRO_NEON)

            array_neon[0] = vaddq_f32(array_neon[0], v.array_neon[0]);
            array_neon[1] = vaddq_f32(array_neon[1], v.array_neon[1]);
            array_neon[2] = vaddq_f32(array_neon[2], v.array_neon[2]);
            array_neon[3] = vaddq_f32(array_neon[3], v.array_neon[3]);

#else
            _11 += v._11; _12 += v._12; _13 += v._13; _14 += v._14;
            _21 += v._21; _22 += v._22; _23 += v._23; _24 += v._24;
            _31 += v._31; _32 += v._32; _33 += v._33; _34 += v._34;
            _41 += v._41; _42 += v._42; _43 += v._43; _44 += v._44;
#endif
            return *this;
        }

        /// \brief Component-wise subtract elements of the matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix, matrix_b;
        ///
        /// matrix -= matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix used to subtract values
        /// \return The matrix with the subtract result
        ///
        ARIBEIRO_INLINE mat4& operator-=(const mat4& v) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = _mm_sub_ps(array_sse[0], v.array_sse[0]);
            array_sse[1] = _mm_sub_ps(array_sse[1], v.array_sse[1]);
            array_sse[2] = _mm_sub_ps(array_sse[2], v.array_sse[2]);
            array_sse[3] = _mm_sub_ps(array_sse[3], v.array_sse[3]);


#elif defined(ARIBEIRO_NEON)

            array_neon[0] = vsubq_f32(array_neon[0], v.array_neon[0]);
            array_neon[1] = vsubq_f32(array_neon[1], v.array_neon[1]);
            array_neon[2] = vsubq_f32(array_neon[2], v.array_neon[2]);
            array_neon[3] = vsubq_f32(array_neon[3], v.array_neon[3]);

#else
            _11 -= v._11; _12 -= v._12; _13 -= v._13; _14 -= v._14;
            _21 -= v._21; _22 -= v._22; _23 -= v._23; _24 -= v._24;
            _31 -= v._31; _32 -= v._32; _33 -= v._33; _34 -= v._34;
            _41 -= v._41; _42 -= v._42; _43 -= v._43; _44 -= v._44;
#endif
            return *this;
        }

        /// \brief Component-wise change signal
        ///
        /// Change the signal of each element in the matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix = -matrix;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the signal changed
        ///
        ARIBEIRO_INLINE mat4 operator-() const {
#if defined(ARIBEIRO_SSE2)
            mat4 result;

            //const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31

            result.array_sse[0] = _mm_xor_ps(_vec4_sign_mask_sse, array_sse[0]);
            result.array_sse[1] = _mm_xor_ps(_vec4_sign_mask_sse, array_sse[1]);
            result.array_sse[2] = _mm_xor_ps(_vec4_sign_mask_sse, array_sse[2]);
            result.array_sse[3] = _mm_xor_ps(_vec4_sign_mask_sse, array_sse[3]);

            return result;

#elif defined(ARIBEIRO_NEON)

            const float32x4_t neg = vset1(-1.0f);

            return mat4(
                vmulq_f32(array_neon[0], neg),
                vmulq_f32(array_neon[1], neg),
                vmulq_f32(array_neon[2], neg),
                vmulq_f32(array_neon[3], neg)
            );

#else
            return mat4(-_11, -_21, -_31, -_41,
                -_12, -_22, -_32, -_42,
                -_13, -_23, -_33, -_43,
                -_14, -_24, -_34, -_44);
#endif
        }

        /// \brief Component-wise divide element
        ///
        /// Make the division operation on each element of the matrix
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix, matrix_b;
        ///
        /// matrix /= matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the division result
        ///
        ARIBEIRO_INLINE mat4& operator/=(const mat4& v) {
#if defined(ARIBEIRO_SSE2)
            array_sse[0] = _mm_div_ps(array_sse[0], v.array_sse[0]);
            array_sse[1] = _mm_div_ps(array_sse[1], v.array_sse[1]);
            array_sse[2] = _mm_div_ps(array_sse[2], v.array_sse[2]);
            array_sse[3] = _mm_div_ps(array_sse[3], v.array_sse[3]);
#elif defined(ARIBEIRO_NEON)
            array_neon[0] = vdivq_f32(array_neon[0], v.array_neon[0]);
            array_neon[1] = vdivq_f32(array_neon[0], v.array_neon[1]);
            array_neon[2] = vdivq_f32(array_neon[0], v.array_neon[2]);
            array_neon[3] = vdivq_f32(array_neon[0], v.array_neon[3]);
#else
            mat4 operant(
                1.0f / v.a1, 1.0f / v.b1, 1.0f / v.c1, 1.0f / v.d1,
                1.0f / v.a2, 1.0f / v.b2, 1.0f / v.c2, 1.0f / v.d2,
                1.0f / v.a3, 1.0f / v.b3, 1.0f / v.c3, 1.0f / v.d3,
                1.0f / v.a4, 1.0f / v.b4, 1.0f / v.c4, 1.0f / v.d4
            );
            (*this) *= operant;
#endif
            return *this;
        }

        /// \brief Add (sum) matrix with a scalar
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix += 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the sum of the elements 
        ///
        ARIBEIRO_INLINE mat4& operator+=(const float &v) {
#if defined(ARIBEIRO_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_add_ps(array_sse[0], tmp);
            array_sse[1] = _mm_add_ps(array_sse[1], tmp);
            array_sse[2] = _mm_add_ps(array_sse[2], tmp);
            array_sse[3] = _mm_add_ps(array_sse[3], tmp);

#elif defined(ARIBEIRO_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vaddq_f32(array_neon[0], tmp);
            array_neon[1] = vaddq_f32(array_neon[1], tmp);
            array_neon[2] = vaddq_f32(array_neon[2], tmp);
            array_neon[3] = vaddq_f32(array_neon[3], tmp);

#else
            _11 += v; _12 += v; _13 += v; _14 += v;
            _21 += v; _22 += v; _23 += v; _24 += v;
            _31 += v; _32 += v; _33 += v; _34 += v;
            _41 += v; _42 += v; _43 += v; _44 += v;
#endif
            return *this;
        }

        /// \brief Subtract matrix with a scalar
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix -= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the subtract of the elements 
        ///
        ARIBEIRO_INLINE mat4& operator-=(const float &v) {
#if defined(ARIBEIRO_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_sub_ps(array_sse[0], tmp);
            array_sse[1] = _mm_sub_ps(array_sse[1], tmp);
            array_sse[2] = _mm_sub_ps(array_sse[2], tmp);
            array_sse[3] = _mm_sub_ps(array_sse[3], tmp);

#elif defined(ARIBEIRO_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vsubq_f32(array_neon[0], tmp);
            array_neon[1] = vsubq_f32(array_neon[1], tmp);
            array_neon[2] = vsubq_f32(array_neon[2], tmp);
            array_neon[3] = vsubq_f32(array_neon[3], tmp);

#else
            _11 -= v; _12 -= v; _13 -= v; _14 -= v;
            _21 -= v; _22 -= v; _23 -= v; _24 -= v;
            _31 -= v; _32 -= v; _33 -= v; _34 -= v;
            _41 -= v; _42 -= v; _43 -= v; _44 -= v;
#endif
            return *this;
        }

        /// \brief Multiply matrix elements with a scalar
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix *= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the multiplication of the elements 
        ///
        ARIBEIRO_INLINE mat4& operator*=(const float &v) {
#if defined(ARIBEIRO_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_mul_ps(array_sse[0], tmp);
            array_sse[1] = _mm_mul_ps(array_sse[1], tmp);
            array_sse[2] = _mm_mul_ps(array_sse[2], tmp);
            array_sse[3] = _mm_mul_ps(array_sse[3], tmp);

#elif defined(ARIBEIRO_NEON)

            float32x4_t tmp = vset1(v);

            array_neon[0] = vmulq_f32(array_neon[0], tmp);
            array_neon[1] = vmulq_f32(array_neon[1], tmp);
            array_neon[2] = vmulq_f32(array_neon[2], tmp);
            array_neon[3] = vmulq_f32(array_neon[3], tmp);

#else
            _11 *= v; _12 *= v; _13 *= v; _14 *= v;
            _21 *= v; _22 *= v; _23 *= v; _24 *= v;
            _31 *= v; _32 *= v; _33 *= v; _34 *= v;
            _41 *= v; _42 *= v; _43 *= v; _44 *= v;
#endif
            return *this;
        }

        /// \brief Divide matrix elements with a scalar
        ///
        /// Example:
        ///
        /// \code
        /// #include <aribeiro/aribeiro.h>
        /// using namespace aRibeiro;
        ///
        /// mat4 matrix;
        ///
        /// matrix /= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the division of the elements 
        ///
        ARIBEIRO_INLINE mat4& operator/=(const float &v) {
#if defined(ARIBEIRO_SSE2)
            __m128 tmp = _mm_set1_ps(v);

            array_sse[0] = _mm_div_ps(array_sse[0], tmp);
            array_sse[1] = _mm_div_ps(array_sse[1], tmp);
            array_sse[2] = _mm_div_ps(array_sse[2], tmp);
            array_sse[3] = _mm_div_ps(array_sse[3], tmp);

#elif defined(ARIBEIRO_NEON)

            float32x4_t tmp = vset1(1.0f / v);

            array_neon[0] = vmulq_f32(array_neon[0], tmp);
            array_neon[1] = vmulq_f32(array_neon[1], tmp);
            array_neon[2] = vmulq_f32(array_neon[2], tmp);
            array_neon[3] = vmulq_f32(array_neon[3], tmp);

#else
            _11 /= v; _12 /= v; _13 /= v; _14 /= v;
            _21 /= v; _22 /= v; _23 /= v; _24 /= v;
            _31 /= v; _32 /= v; _33 /= v; _34 /= v;
            _41 /= v; _42 /= v; _43 /= v; _44 /= v;
#endif
            return *this;
        }

        SSE2_CLASS_NEW_OPERATOR

    };

    INLINE_OPERATION_IMPLEMENTATION(mat4)

    const mat4 mat4_IdentityMatrix (
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );

#if defined(ARIBEIRO_SSE2)
#pragma pack(pop)
#endif

}


#endif
