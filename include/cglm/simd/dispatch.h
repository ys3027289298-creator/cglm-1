/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*

  Runtime SIMD dispatch
  =====================

  cglm is a header-only library. By default the SIMD implementation of each
  function is selected at COMPILE time from the target flags (e.g. -mavx),
  exactly as before. When the library is compiled for a generic x86-64
  baseline (SSE2, no AVX) with GCC or Clang, cglm additionally enables
  RUNTIME dispatch for hot operations: the first call in each translation
  unit probes the CPU (via __builtin_cpu_supports) and caches the best
  supported level; subsequent calls branch on the cached level.

  Levels (x86):
    CGLM_SIMD_LEVEL_SCALAR  0   portable scalar C (reference implementation)
    CGLM_SIMD_LEVEL_SSE2    1   SSE2 kernels (x86-64 baseline)
    CGLM_SIMD_LEVEL_AVX     2   AVX kernels (256-bit for mat4 mul, VEX
                                encoded 128-bit for mat4 inv / quat mul),
                                compiled with
                                __attribute__((target("avx"))) so they can
                                live in the same translation unit as the
                                baseline code. For mat4 mulv and the vec4
                                ops the AVX level uses the inline SSE2
                                kernel: a non-inlined VEX-128 call costs
                                more than the re-encoding saves (measured).

  The level can be forced with the environment variable:

    CGLM_SIMD=auto     (default) use the best level the CPU supports
    CGLM_SIMD=scalar   force the scalar reference implementation
    CGLM_SIMD=sse2     force the SSE2 implementation
    CGLM_SIMD=avx      force the AVX implementation

  A forced level above the hardware capability is clamped down to the
  hardware level, so a forced value can never execute instructions the CPU
  lacks. The variable is read once per translation unit on first use.

  cglm_simd_force_level() does the same programmatically for the CURRENT
  translation unit only (the cache is a per-TU static, which is what keeps
  the library header-only); it is intended for tests and benchmarks. The
  environment variable is the supported process-wide mechanism.

  Runtime dispatch is enabled only when ALL of the following hold:
    - x86 target, GCC >= 5 or Clang (function target attributes required)
    - compiled WITHOUT -mavx/-march=avx... (with __AVX__ defined the
      compile-time AVX path is already optimal and is kept as-is)
    - SSE2 baseline (always true on x86-64)
    - CGLM_NO_RUNTIME_DISPATCH is not defined

  Define CGLM_NO_RUNTIME_DISPATCH to restore pure compile-time selection.

  Accuracy (error bounds vs the scalar reference, no -ffast-math):
  All levels compute the same mathematical expression; results differ
  only through floating-point reassociation (summation order). No level
  uses FMA (AVX does not imply FMA) or rcp/rsqrt approximations; all
  divides and square roots are IEEE-exact.
    - glm_mat4_mul:  SSE2 bitwise identical (same order); AVX sums each
                     element in order k=0,2,1,3 -> a few ulp.
    - glm_mat4_mulv: SSE2/AVX sum columns in order k=3,2,1,0 -> a few ulp.
    - glm_mat4_inv:  SSE2/AVX reorder the cofactor arithmetic -> a few
                     ulp, amplified by cond(A) as for any algorithm.
    - glm_quat_mul:  sign flips via XOR (exact), same summation order
                     -> bitwise identical at all levels.
    - glm_vec4_dot:  SIMD reduces (a0b0+a1b1)+(a2b2+a3b3) -> a few ulp;
                     glm_vec4_norm adds only an exact sqrtss.
  Backward-error bound for the reordered 4-term sums:
  |err| <= 3 * eps * sum_k|term_k|  (eps = 1.19e-7), i.e. dispatched
  results are as accurate as the scalar reference in the backward-error
  sense. Measured maxima over 1e6 random inputs (full table and
  methodology in docs/source/runtime-dispatch.rst, reproducible with
  bench/runtime_dispatch.c): abs err <= 3.9e-6 for mat4_mul/mulv and
  vec4_dot (inputs ~[-4,4]), <= 1.5e-7 for mat4_inv (diagonally
  dominant), rel err <= 1.5e-7 for vec4_norm; quat_mul is exact.
*/

#ifndef cglm_simd_dispatch_h
#define cglm_simd_dispatch_h

#include <string.h>

#define CGLM_SIMD_LEVEL_SCALAR 0
#define CGLM_SIMD_LEVEL_SSE2   1
#define CGLM_SIMD_LEVEL_AVX    2

#if !defined(CGLM_NO_RUNTIME_DISPATCH)                                     \
    && !defined(__AVX__)                                                   \
    && defined(__SSE2__)                                                   \
    && (defined(__x86_64__) || defined(__i386__))                          \
    && !defined(_MSC_VER)                                                  \
    && (defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 5))
#  define CGLM_RT_DISPATCH 1
#endif

#ifdef CGLM_RT_DISPATCH

/* AVX kernels for runtime dispatch: compiled with the avx target attribute
   into an otherwise baseline translation unit. They must NOT be
   always_inline: a caller compiled for the baseline target cannot inline
   them, the call is the dispatch boundary. "unused" silences warnings in
   translation units that include the header but dispatch elsewhere. */
#define CGLM_RT_AVX_FN static __attribute__((target("avx"), unused))

/* per-translation-unit cache; -1 = not resolved yet. A data race on this
   is benign: every racer computes and stores the same value. */
static int cglm__simd_level_cache = -1;

CGLM_INLINE
int
cglm__simd_hw_level(void) {
  /* __builtin_cpu_supports also checks OSXSAVE/XGETBV for avx */
  if (__builtin_cpu_supports("avx"))
    return CGLM_SIMD_LEVEL_AVX;
#if defined(__x86_64__)
  return CGLM_SIMD_LEVEL_SSE2; /* SSE2 is part of the x86-64 baseline */
#else
  if (__builtin_cpu_supports("sse2"))
    return CGLM_SIMD_LEVEL_SSE2;
  return CGLM_SIMD_LEVEL_SCALAR;
#endif
}

/* CGLM_SIMD env var: auto|scalar|sse2|avx; returns -1 for "auto" */
CGLM_INLINE
int
cglm__simd_env_level(void) {
  const char *env = getenv("CGLM_SIMD");
  if (!env || !*env || strcmp(env, "auto") == 0)
    return -1;
  if (strcmp(env, "scalar") == 0) return CGLM_SIMD_LEVEL_SCALAR;
  if (strcmp(env, "sse2")   == 0) return CGLM_SIMD_LEVEL_SSE2;
  if (strcmp(env, "avx")    == 0) return CGLM_SIMD_LEVEL_AVX;
  return -1; /* unknown value: fall back to auto */
}

CGLM_INLINE
int
cglm__simd_resolve(void) {
  int hw  = cglm__simd_hw_level();
  int env = cglm__simd_env_level();
  if (env < 0)
    return hw;
  return env < hw ? env : hw; /* clamp forced level to hardware */
}

/*!
 * @brief active SIMD level for this translation unit:
 *        CGLM_SIMD_LEVEL_SCALAR, CGLM_SIMD_LEVEL_SSE2 or CGLM_SIMD_LEVEL_AVX
 */
CGLM_INLINE
int
cglm_simd_level(void) {
  int lvl = cglm__simd_level_cache;
  if (CGLM_UNLIKELY(lvl < 0)) {
    lvl = cglm__simd_resolve();
    cglm__simd_level_cache = lvl;
  }
  return lvl;
}

/*!
 * @brief force the SIMD level for the CURRENT translation unit
 *        (test/benchmark hook; the process-wide mechanism is the
 *        CGLM_SIMD environment variable)
 *
 * @param[in] level CGLM_SIMD_LEVEL_* value, clamped to hardware
 *                  capability, or -1 to restore automatic selection
 */
CGLM_INLINE
void
cglm_simd_force_level(int level) {
  if (level < 0) {
    cglm__simd_level_cache = -1; /* re-resolve on next use */
    return;
  }
  if (level > CGLM_SIMD_LEVEL_AVX)
    level = CGLM_SIMD_LEVEL_AVX;
  {
    int hw = cglm__simd_hw_level();
    if (level > hw)
      level = hw;
  }
  cglm__simd_level_cache = level;
}

#else /* !CGLM_RT_DISPATCH */

/* portable stubs: report the compile-time selection so user code and
   benchmarks can query the level on any platform */
CGLM_INLINE
int
cglm_simd_level(void) {
#if defined(CGLM_AVX_FP)
  return CGLM_SIMD_LEVEL_AVX;
#elif defined(CGLM_SSE_FP) || defined(CGLM_NEON_FP) || defined(CGLM_SIMD_WASM)
  return CGLM_SIMD_LEVEL_SSE2;
#else
  return CGLM_SIMD_LEVEL_SCALAR;
#endif
}

CGLM_INLINE
void
cglm_simd_force_level(int level) {
  (void)level; /* no runtime dispatch on this target */
}

#endif /* CGLM_RT_DISPATCH */

/*!
 * @brief human readable name of the active SIMD level ("scalar", "sse2",
 *        "avx"; "neon"/"wasm" on those compile-time targets)
 */
CGLM_INLINE
const char *
cglm_simd_level_name(void) {
#ifdef CGLM_RT_DISPATCH
  switch (cglm_simd_level()) {
    case CGLM_SIMD_LEVEL_AVX:  return "avx";
    case CGLM_SIMD_LEVEL_SSE2: return "sse2";
    default:                   return "scalar";
  }
#elif defined(CGLM_AVX_FP)
  return "avx";
#elif defined(CGLM_SSE_FP)
  return "sse2";
#elif defined(CGLM_NEON_FP)
  return "neon";
#elif defined(CGLM_SIMD_WASM)
  return "wasm";
#else
  return "scalar";
#endif
}

#endif /* cglm_simd_dispatch_h */
