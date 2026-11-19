/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/* AVX kernel for RUNTIME dispatch (see simd/dispatch.h and
   simd/x86rt/mat4.h for the rationale). */

#ifndef cglm_quat_simd_x86rt_h
#define cglm_quat_simd_x86rt_h
#ifdef CGLM_RT_DISPATCH

#include "../../common.h"
#include "../intrin.h"

CGLM_RT_AVX_FN
void
glm_quat_mul_avx_rt(const versor p, const versor q, versor dest) {
  /* SSE2 kernel inlined and re-encoded as VEX under the avx target;
     bitwise identical to glm_quat_mul_sse2 */
  glm_quat_mul_sse2(p, q, dest);
}

#endif /* CGLM_RT_DISPATCH */
#endif /* cglm_quat_simd_x86rt_h */
