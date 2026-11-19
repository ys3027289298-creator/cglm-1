/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 * Runtime SIMD dispatch: error and throughput comparison
 * -------------------------------------------------------
 *
 * Measures, for each dispatched hot operation and each SIMD level
 * (scalar / sse2 / avx):
 *
 *   - max abs/rel/ulp error vs the scalar reference implementation
 *     (1e6 deterministic random inputs)
 *   - throughput in Mops/sec (adaptive timing, best of 5 rounds)
 *
 * Build for the x86-64 baseline so that runtime dispatch is active:
 *
 *   gcc -O2 -I include bench/runtime_dispatch.c -o bench_runtime -lm
 *
 * (compiling with -mavx/-march=native selects the compile-time AVX path
 * and all rows collapse to it; CGLM_SIMD=<level> forces a level from the
 * environment, cglm_simd_force_level() is used in-process here)
 */

#include <cglm/cglm.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define BATCH      512
#define ERR_SAMPLES 1000000

static volatile float sink;

static uint32_t rng_s = 0x9e3779b9u;
static uint32_t xrnd(void) {
  uint32_t x = rng_s;
  x ^= x << 13; x ^= x >> 17; x ^= x << 5;
  return rng_s = x;
}
/* uniform in [-4, 4] */
static float frand(void) { return 8.0f * ((float)xrnd() / 4294967296.0f) - 4.0f; }
/* uniform in [-1, 1] */
static float frand1(void) { return 2.0f * ((float)xrnd() / 4294967296.0f) - 1.0f; }

static double now_sec(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec + 1e-9 * (double)ts.tv_nsec;
}

static int64_t ford(float f) {
  int32_t i;
  memcpy(&i, &f, sizeof(i));
  return i < 0 ? (int64_t)INT32_MIN - i : (int64_t)i;
}

typedef struct {
  double   max_abs, max_rel;
  uint64_t max_ulp;
} errstat_t;

static void gen_inputs(void);

static void err_add(errstat_t *e, float got, float ref) {
  double a = fabs((double)got - (double)ref);
  if (a > e->max_abs)
    e->max_abs = a;
  /* relative error is only meaningful away from cancellation; near-zero
     references are covered by the absolute error instead */
  if (fabs((double)ref) >= 1e-3) {
    double r = a / fabs((double)ref);
    if (r > e->max_rel)
      e->max_rel = r;
  }
  /* ulp distance is only meaningful on the same side of zero and away
     from cancellation (same threshold as the relative error) */
  if (((got < 0.0f) == (ref < 0.0f) || got == ref)
      && fabs((double)ref) >= 1e-3) {
    uint64_t ulp = (uint64_t)llabs(ford(got) - ford(ref));
    if (ulp > e->max_ulp)
      e->max_ulp = ulp;
  }
}

/* ---------------------------------------------------------------- inputs */

static mat4   in_a[BATCH], in_b[BATCH], out_m[BATCH];
static vec4   in_v[BATCH], in_w[BATCH];
static versor in_q[BATCH];
static vec4   out_v[BATCH];

static void gen_inputs(void) {
  int i, c, r;
  for (i = 0; i < BATCH; i++) {
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        in_a[i][c][r] = frand1();
    /* strictly diagonally dominant: well-conditioned for inversion */
    in_a[i][0][0] += 4.0f; in_a[i][1][1] += 4.0f;
    in_a[i][2][2] += 4.0f; in_a[i][3][3] += 4.0f;
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        in_b[i][c][r] = frand();
    for (c = 0; c < 4; c++) {
      in_v[i][c] = frand();
      in_w[i][c] = frand();
      in_q[i][c] = frand() * 0.25f;
    }
  }
}

/* ---------------------------------------------------------- error passes */

static void err_mat4_mul(errstat_t *e, int level) {
  mat4 ref, got;
  int  i, c, r;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    if (k == 0 && i > 0)
      gen_inputs(); /* fresh random inputs each pass over the batch */
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    glm_mat4_mul(in_a[k], in_b[k], ref);
    cglm_simd_force_level(level);
    glm_mat4_mul(in_a[k], in_b[k], got);
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        err_add(e, got[c][r], ref[c][r]);
  }
}

static void err_mat4_mulv(errstat_t *e, int level) {
  vec4 ref, got;
  int  i, c;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    if (k == 0 && i > 0)
      gen_inputs();
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    glm_mat4_mulv(in_a[k], in_v[k], ref);
    cglm_simd_force_level(level);
    glm_mat4_mulv(in_a[k], in_v[k], got);
    for (c = 0; c < 4; c++)
      err_add(e, got[c], ref[c]);
  }
}

static void err_mat4_inv(errstat_t *e, int level) {
  mat4 ref, got;
  int  i, c, r;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    if (k == 0 && i > 0)
      gen_inputs();
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    glm_mat4_inv(in_a[k], ref);
    cglm_simd_force_level(level);
    glm_mat4_inv(in_a[k], got);
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        err_add(e, got[c][r], ref[c][r]);
  }
}

static void err_quat_mul(errstat_t *e, int level) {
  versor ref, got;
  int    i, c;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    if (k == 0 && i > 0)
      gen_inputs();
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    glm_quat_mul(in_q[k], in_q[(k + 7) & (BATCH - 1)], ref);
    cglm_simd_force_level(level);
    glm_quat_mul(in_q[k], in_q[(k + 7) & (BATCH - 1)], got);
    for (c = 0; c < 4; c++)
      err_add(e, got[c], ref[c]);
  }
}

static void err_vec4_dot(errstat_t *e, int level) {
  int i;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    float ref, got;
    if (k == 0 && i > 0)
      gen_inputs();
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    ref = glm_vec4_dot(in_v[k], in_w[k]);
    cglm_simd_force_level(level);
    got = glm_vec4_dot(in_v[k], in_w[k]);
    err_add(e, got, ref);
  }
}

static void err_vec4_norm(errstat_t *e, int level) {
  int i;
  for (i = 0; i < ERR_SAMPLES; i++) {
    int k = i & (BATCH - 1);
    float ref, got;
    if (k == 0 && i > 0)
      gen_inputs();
    cglm_simd_force_level(CGLM_SIMD_LEVEL_SCALAR);
    ref = glm_vec4_norm(in_v[k]);
    cglm_simd_force_level(level);
    got = glm_vec4_norm(in_v[k]);
    err_add(e, got, ref);
  }
}

/* ------------------------------------------------------------ perf passes */

typedef void (*perf_fn)(void);

static double perf_measure(perf_fn fn) {
  /* returns Mops/sec, best of 5 adaptive rounds */
  double best = 0.0;
  int    round;
  for (round = 0; round < 5; round++) {
    double t0, el;
    long   iters = BATCH, done = 0;
    /* calibrate to >= ~60 ms per round */
    t0 = now_sec();
    fn(); /* warm */
    for (;;) {
      long j;
      for (j = 0; j < iters; j++)
        fn();
      done += iters;
      el = now_sec() - t0;
      if (el > 0.06)
        break;
      iters *= 2;
    }
    {
      double mops = (double)done / el / 1e6;
      if (mops > best)
        best = mops;
    }
  }
  return best;
}

static int pc;
#define PERF_BODY(OP)                                   \
  pc = (pc + 1) & (BATCH - 1);                          \
  OP

static void perf_mat4_mul(void)  { PERF_BODY(glm_mat4_mul(in_a[pc], in_b[pc], out_m[pc])); }
static void perf_mat4_mulv(void) { PERF_BODY(glm_mat4_mulv(in_a[pc], in_v[pc], out_v[pc])); }
static void perf_mat4_inv(void)  { PERF_BODY(glm_mat4_inv(in_a[pc], out_m[pc])); }
static void perf_quat_mul(void)  { PERF_BODY(glm_quat_mul(in_q[pc], in_q[(pc + 7) & (BATCH - 1)], out_v[pc])); }
static float dacc;
static void perf_vec4_dot(void)  { PERF_BODY(dacc += glm_vec4_dot(in_v[pc], in_w[pc])); }
static void perf_vec4_norm(void) { PERF_BODY(dacc += glm_vec4_norm(in_v[pc])); }

/* ---------------------------------------------------------------- driver */

typedef struct {
  const char *name;
  void      (*err)(errstat_t *, int level);
  perf_fn    perf;
} op_t;

static const op_t ops[] = {
  {"glm_mat4_mul  (mat4 x mat4)",   err_mat4_mul,  perf_mat4_mul},
  {"glm_mat4_mulv (mat4 x vec4)",   err_mat4_mulv, perf_mat4_mulv},
  {"glm_mat4_inv  (inverse)",       err_mat4_inv,  perf_mat4_inv},
  {"glm_quat_mul  (quat x quat)",   err_quat_mul,  perf_quat_mul},
  {"glm_vec4_dot  (dot product)",   err_vec4_dot,  perf_vec4_dot},
  {"glm_vec4_norm (L2 norm)",       err_vec4_norm, perf_vec4_norm},
};

static const int    levels[] = {CGLM_SIMD_LEVEL_SCALAR, CGLM_SIMD_LEVEL_SSE2, CGLM_SIMD_LEVEL_AVX};
static const char  *lnames[] = {"scalar", "sse2", "avx"};

int main(void) {
  size_t o;
  int    li;
#ifndef CGLM_RT_DISPATCH
  printf("note: CGLM_RT_DISPATCH is OFF for this build (compiled with AVX "
         "or CGLM_NO_RUNTIME_DISPATCH?); all rows use the compile-time path\n\n");
#endif
  gen_inputs();

  printf("| operation | impl | Mops/s | speedup | max abs err | max rel err* | max ulp* |\n");
  printf("|-----------|------|-------:|--------:|------------:|-------------:|---------:|\n");

  for (o = 0; o < sizeof(ops) / sizeof(ops[0]); o++) {
    double base = 0.0;
    for (li = 0; li < 3; li++) {
      errstat_t e = {0.0, 0.0, 0};
      double    mops;
      cglm_simd_force_level(levels[li]);
      if (cglm_simd_level() != levels[li])
        continue; /* hardware does not support this level */
      rng_s = 0x9e3779b9u; /* identical input stream for every level */
      gen_inputs();
      ops[o].err(&e, levels[li]);
      cglm_simd_force_level(levels[li]); /* err pass leaves level forced */
      mops = perf_measure(ops[o].perf);
      if (li == 0)
        base = mops;
      printf("| %s | %s | %.1f | %.2fx | %.3e | %.3e | %llu |\n",
             ops[o].name, lnames[li], mops, base > 0 ? mops / base : 1.0,
             e.max_abs, e.max_rel, (unsigned long long)e.max_ulp);
    }
  }

  cglm_simd_force_level(-1);
  sink = dacc;
  printf("\n* relative/ulp error only counted where |reference| >= 1e-3;\n");
  printf("  near-zero results (cancellation) are covered by max abs err.\n");
  printf("(active level after reset: %s; sink=%f)\n",
         cglm_simd_level_name(), (double)sink);
  return 0;
}
