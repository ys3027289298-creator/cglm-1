/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
  AVX kernels for RUNTIME dispatch (see simd/dispatch.h).

  These are compiled with __attribute__((target("avx"))) into translation
  units whose baseline target has no AVX, and are only called after the
  runtime probe confirmed AVX support.

  Notes:
  - In runtime-dispatch builds mat4 is only 16-byte aligned
    (CGLM_ALIGN_MAT), so all 256-bit accesses MUST be loadu/storeu.
  - AVX does not imply FMA, so products and sums are emitted separately;
    results are therefore bit-comparable with the SSE2/scalar paths up to
    summation order (documented in simd/dispatch.h).
  - The 128-bit kernels reuse the SSE2 sources; under the avx target the
    compiler simply re-encodes them as VEX (3-operand, no SSE/AVX
    transition penalties when mixed with 256-bit code).
*/

#ifndef cglm_mat_simd_x86rt_h
#define cglm_mat_simd_x86rt_h
#ifdef CGLM_RT_DISPATCH

#include "../../common.h"
#include "../intrin.h"

/* GCC/Clang define all intrinsics unconditionally in immintrin.h; the
   target("avx") attribute on each kernel enables codegen for them */
#include <immintrin.h>

CGLM_RT_AVX_FN
void
glm_mat4_mul_avx_rt(CGLM_CONST mat4 m1, CGLM_CONST mat4 m2, mat4 dest) {
  /* D = R * L (Column-Major); same math as glm_mat4_mul_avx but with
     unaligned 256-bit accesses and explicit mul+add (no FMA assumed).
     Each result element sums products in order k=0,2,1,3. */

  __m256  y0, y1, y2, y3, y4, y5, y6, y7, y8, y9, y10, y11, y12, y13;
  __m256i yi0, yi1, yi2, yi3;

  y0 = _mm256_loadu_ps(m2[0]); /* h g f e d c b a */
  y1 = _mm256_loadu_ps(m2[2]); /* p o n m l k j i */

  y2 = _mm256_loadu_ps(m1[0]); /* h g f e d c b a */
  y3 = _mm256_loadu_ps(m1[2]); /* p o n m l k j i */

  /* 0x03: 0b00000011 */
  y4 = _mm256_permute2f128_ps(y2, y2, 0x03); /* d c b a h g f e */
  y5 = _mm256_permute2f128_ps(y3, y3, 0x03); /* l k j i p o n m */

  yi0 = _mm256_set_epi32(1, 1, 1, 1, 0, 0, 0, 0);
  yi1 = _mm256_set_epi32(3, 3, 3, 3, 2, 2, 2, 2);
  yi2 = _mm256_set_epi32(0, 0, 0, 0, 1, 1, 1, 1);
  yi3 = _mm256_set_epi32(2, 2, 2, 2, 3, 3, 3, 3);

  /* f f f f a a a a */
  /* h h h h c c c c */
  /* e e e e b b b b */
  /* g g g g d d d d */
  y6 = _mm256_permutevar_ps(y0, yi0);
  y7 = _mm256_permutevar_ps(y0, yi1);
  y8 = _mm256_permutevar_ps(y0, yi2);
  y9 = _mm256_permutevar_ps(y0, yi3);

  /* n n n n i i i i */
  /* p p p p k k k k */
  /* m m m m j j j j */
  /* o o o o l l l l */
  y10 = _mm256_permutevar_ps(y1, yi0);
  y11 = _mm256_permutevar_ps(y1, yi1);
  y12 = _mm256_permutevar_ps(y1, yi2);
  y13 = _mm256_permutevar_ps(y1, yi3);

  y0 = _mm256_mul_ps(y2, y6);
  y1 = _mm256_mul_ps(y2, y10);

  y0 = _mm256_add_ps(y0, _mm256_mul_ps(y3, y7));
  y1 = _mm256_add_ps(y1, _mm256_mul_ps(y3, y11));

  y0 = _mm256_add_ps(y0, _mm256_mul_ps(y4, y8));
  y1 = _mm256_add_ps(y1, _mm256_mul_ps(y4, y12));

  y0 = _mm256_add_ps(y0, _mm256_mul_ps(y5, y9));
  y1 = _mm256_add_ps(y1, _mm256_mul_ps(y5, y13));

  _mm256_storeu_ps(dest[0], y0);
  _mm256_storeu_ps(dest[2], y1);
}

CGLM_RT_AVX_FN
void
glm_mat4_inv_avx_rt(CGLM_CONST mat4 mat, mat4 dest) {
  /* SSE2 kernel inlined and re-encoded as VEX under the avx target */
  glm_mat4_inv_sse2(mat, dest);
}

#endif /* CGLM_RT_DISPATCH */
#endif /* cglm_mat_simd_x86rt_h */
