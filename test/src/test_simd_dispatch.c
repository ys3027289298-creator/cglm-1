/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "test_common.h"

/*
 * Runtime SIMD dispatch tests (see include/cglm/simd/dispatch.h).
 *
 * cglm_simd_force_level() is per-translation-unit, which is exactly what
 * these single-TU tests need. On targets without runtime dispatch the
 * force call is a no-op and the tests simply exercise the compile-time
 * implementation against the scalar reference computed inline here.
 */

/* scalar references, independent of the library dispatch */
static void
test__mat4_mul_ref(CGLM_CONST mat4 m1, CGLM_CONST mat4 m2, mat4 dest) {
  int c, r, k;
  for (c = 0; c < 4; c++)
    for (r = 0; r < 4; r++) {
      dest[c][r] = 0.0f;
      for (k = 0; k < 4; k++)
        dest[c][r] += m1[k][r] * m2[c][k];
    }
}

static void
test__fill_rand(mat4 m, vec4 v, versor q) {
  int i;
  for (i = 0; i < 4; i++) {
    v[i] = (float)(drand48() - 0.5) * 8.0f;
    q[i] = (float)(drand48() - 0.5) * 2.0f;
    m[0][i] = (float)(drand48() - 0.5) * 8.0f;
    m[1][i] = (float)(drand48() - 0.5) * 8.0f;
    m[2][i] = (float)(drand48() - 0.5) * 8.0f;
    m[3][i] = (float)(drand48() - 0.5) * 8.0f;
  }
  /* keep the matrix comfortably invertible */
  m[0][0] += 6.0f;
  m[1][1] += 6.0f;
  m[2][2] += 6.0f;
  m[3][3] += 6.0f;
}

TEST_IMPL(simd_dispatch_level) {
  int lvl = cglm_simd_level();

  ASSERT(lvl == CGLM_SIMD_LEVEL_SCALAR
         || lvl == CGLM_SIMD_LEVEL_SSE2
         || lvl == CGLM_SIMD_LEVEL_AVX)

  ASSERT(cglm_simd_level_name() != NULL)

  TEST_SUCCESS
}

TEST_IMPL(simd_dispatch_env_clamp) {
  /* forcing above the hardware level must clamp, never SIGILL */
  cglm_simd_force_level(CGLM_SIMD_LEVEL_AVX);
  ASSERT(cglm_simd_level() <= CGLM_SIMD_LEVEL_AVX)
  cglm_simd_force_level(-1);
  TEST_SUCCESS
}

TEST_IMPL(simd_dispatch_mat4) {
  mat4  m1, m2, got, ref, inv;
  vec4  v, vgot;
  int   i, j, lvl;
  const int levels[3] = {CGLM_SIMD_LEVEL_SCALAR,
                         CGLM_SIMD_LEVEL_SSE2,
                         CGLM_SIMD_LEVEL_AVX};

  srand48(42);
  for (i = 0; i < 64; i++) {
    test__fill_rand(m1, v, vgot /* unused as vec here */);
    test__fill_rand(m2, v, vgot);

    test__mat4_mul_ref(m1, m2, ref);

    for (j = 0; j < 3; j++) {
      lvl = levels[j];
      cglm_simd_force_level(lvl);
      if (cglm_simd_level() != lvl)
        continue; /* level not supported by this hardware/build */

      glm_mat4_mul(m1, m2, got);
      /* dispatched paths must stay within a few ulp of the reference:
         1e-4 absolute for entries of magnitude <= ~64 */
      ASSERT(fabsf(got[0][0] - ref[0][0]) <= 1e-4f)
      ASSERT(fabsf(got[1][2] - ref[1][2]) <= 1e-4f)
      ASSERT(fabsf(got[2][1] - ref[2][1]) <= 1e-4f)
      ASSERT(fabsf(got[3][3] - ref[3][3]) <= 1e-4f)

      glm_mat4_inv(m1, inv);
      glm_mat4_mul(m1, inv, got);
      /* m * inv(m) ~= identity */
      ASSERT(fabsf(got[0][0] - 1.0f) < 1e-3f)
      ASSERT(fabsf(got[1][1] - 1.0f) < 1e-3f)
      ASSERT(fabsf(got[2][2] - 1.0f) < 1e-3f)
      ASSERT(fabsf(got[3][3] - 1.0f) < 1e-3f)
      ASSERT(fabsf(got[0][1]) < 1e-3f)
      ASSERT(fabsf(got[2][3]) < 1e-3f)
    }
  }

  cglm_simd_force_level(-1);
  TEST_SUCCESS
}

TEST_IMPL(simd_dispatch_vec4_quat) {
  vec4   a, b;
  versor p, q, got;
  float  d, n;
  int    i, j, lvl;
  const int levels[3] = {CGLM_SIMD_LEVEL_SCALAR,
                         CGLM_SIMD_LEVEL_SSE2,
                         CGLM_SIMD_LEVEL_AVX};

  srand48(7);
  for (i = 0; i < 64; i++) {
    a[0] = (float)drand48() * 4.0f; a[1] = (float)drand48();
    a[2] = (float)drand48() * 2.0f; a[3] = (float)drand48() * 3.0f;
    b[0] = (float)drand48();        b[1] = (float)drand48() * 5.0f;
    b[2] = (float)drand48();        b[3] = (float)drand48() * 0.5f;
    p[0] = (float)drand48(); p[1] = (float)drand48();
    p[2] = (float)drand48(); p[3] = (float)drand48();
    q[0] = (float)drand48(); q[1] = (float)drand48();
    q[2] = (float)drand48(); q[3] = (float)drand48();

    for (j = 0; j < 3; j++) {
      lvl = levels[j];
      cglm_simd_force_level(lvl);
      if (cglm_simd_level() != lvl)
        continue;

      d = glm_vec4_dot(a, b);
      n = glm_vec4_norm(a);
      ASSERT(fabsf(d - (a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3])) < 1e-4f)
      ASSERT(fabsf(n - sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2] + a[3]*a[3])) < 1e-4f)

      glm_quat_mul(p, q, got);
      ASSERT(fabsf(got[3] - (p[3]*q[3] - p[0]*q[0] - p[1]*q[1] - p[2]*q[2])) < 1e-4f)
      ASSERT(fabsf(got[0] - (p[3]*q[0] + p[0]*q[3] + p[1]*q[2] - p[2]*q[1])) < 1e-4f)
    }
  }

  cglm_simd_force_level(-1);
  TEST_SUCCESS
}
