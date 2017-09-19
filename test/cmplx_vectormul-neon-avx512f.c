#if defined (__i386) || defined (__x86_64)
# include <immintrin.h>
#elif defined(__arm__) || defined (__aarch64__)
# include <arm_neon.h>
#endif

#if defined(__ARM_NEON)
typedef float32x4_t VECTORTYPE; // 4 pcs parallel
//typedef float16x8_t VECTORTYPE; // 8 pcs parallel float16 ieee754:2008
#elif defined(__AVX512F__)
typedef __v16sf VECTORTYPE; // 16 pcs parallel
#elif defined(__AVX__)
typedef __v8sf VECTORTYPE; // 8 pcs parallel
#elif defined(__SSE__)
typedef __m128 VECTORTYPE; // 4 pcs parallel
#else
typedef float VECTORTYPE; // single
#endif


// common code
struct _cmplx {
    VECTORTYPE re;
    VECTORTYPE im;
};

struct _cmplx cmplx_vectormul(struct _cmplx a, struct _cmplx b) {
    struct _cmplx res;
    res.re = a.re * b.re - a.im * b.im;
    res.im = a.re * b.im + a.im * b.re;
    return res;
}

/* gcc -O2 -march=native

---> AArch64:
cmplx_vectormul:
	fmul	v4.4s, v0.4s, v2.4s
	sub	sp, sp, #128
	fmul	v2.4s, v1.4s, v2.4s
	add	sp, sp, 128
	fmls	v4.4s, v1.4s, v3.4s
	mov	v1.16b, v2.16b
	fmla	v1.4s, v0.4s, v3.4s
	mov	v0.16b, v4.16b
	ret

---> AVX512F (newest Intel XEON and Coffee lake or -mavx512f)
cmplx_vectormul:
.LFB4752:
        .cfi_startproc
        vmovaps 72(%rsp), %zmm1
        movq    %rdi, %rax
        vmovaps 136(%rsp), %zmm4
        vmovaps 200(%rsp), %zmm3
        vmovaps 8(%rsp), %zmm0
        vmulps  %zmm3, %zmm1, %zmm2
        vmulps  %zmm4, %zmm1, %zmm1
        vfmsub231ps     %zmm4, %zmm0, %zmm2
        vmovaps %zmm2, (%rdi)
        vfmadd132ps     %zmm3, %zmm1, %zmm0
        vmovaps %zmm0, 64(%rdi)
        ret

---> AVX (Intel i3-3xxx+ or -mavx)
cmplx_vectormul:
.LFB4752:
        .cfi_startproc
        vmovaps 8(%rsp), %ymm0
        movq    %rdi, %rax
        vmovaps 40(%rsp), %ymm1
        vmovaps 72(%rsp), %ymm3
        vmovaps 104(%rsp), %ymm4
        vmulps  %ymm3, %ymm0, %ymm2
        vmulps  %ymm4, %ymm1, %ymm5
        vmulps  %ymm4, %ymm0, %ymm0
        vsubps  %ymm5, %ymm2, %ymm2
        vmulps  %ymm3, %ymm1, %ymm1
        vaddps  %ymm1, %ymm0, %ymm0
        vmovaps %ymm2, (%rdi)
        vmovaps %ymm0, 32(%rdi)
        vzeroupper
        ret

---> SSE: Pentium4+ or -msse:
cmplx_vectormul:
.LFB4752:
        .cfi_startproc
        movaps  8(%rsp), %xmm0
        movq    %rdi, %rax
        movaps  24(%rsp), %xmm1
        movaps  %xmm0, %xmm2
        movaps  40(%rsp), %xmm3
        movaps  %xmm1, %xmm5
        movaps  56(%rsp), %xmm4
        mulps   %xmm3, %xmm2
        mulps   %xmm4, %xmm5
        mulps   %xmm4, %xmm0
        mulps   %xmm3, %xmm1
        subps   %xmm5, %xmm2
        addps   %xmm1, %xmm0
        movaps  %xmm2, (%rdi)
        movaps  %xmm0, 16(%rdi)
        ret

---> i386 .. Pentium  or -mno-sse on Intel 32 bit:
mplx_vectormul:
.LFB0: 
        .cfi_startproc
        flds    8(%esp)
        flds    12(%esp)
        flds    16(%esp)
        flds    20(%esp)
        fld     %st(1)
        fmul    %st(4), %st
        movl    4(%esp), %eax
        fld     %st(1)
        fmul    %st(4), %st
        fsubrp  %st, %st(1)
        fstps   (%eax)
        fmulp   %st, %st(3)
        fmulp   %st, %st(1)
        faddp   %st, %st(1)
        fstps   4(%eax)
        ret     $4
*/
