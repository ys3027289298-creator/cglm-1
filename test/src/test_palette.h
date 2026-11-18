/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "test_common.h"

#include <stdint.h>
#include <time.h>

#ifndef TEST_PALETTE_HELPERS_H
#define TEST_PALETTE_HELPERS_H

#define TEST_PALETTE_BONES 200

/* random gentle rigid transform: rotation + small translation */
static void
test_palette_rand_trs(mat4 m) {
  vec3   axis;
  versor q;

  test_rand_vec3(axis);
  glm_vec3_normalize(axis);
  glm_quatv(q, frand48() * 6.2831853f, axis);
  glm_quat_mat4(q, m);

  m[3][0] = (frand48() - 0.5f) * 0.5f;
  m[3][1] = (frand48() - 0.5f) * 0.5f;
  m[3][2] = (frand48() - 0.5f) * 0.5f;
  m[3][3] = 1.0f;
}

static void
test_palette_rand_hierarchy(int32_t *parents, uint32_t count) {
  uint32_t i;

  parents[0] = -1;
  for (i = 1; i < count; i++)
    parents[i] = (int32_t)(drand48() * (double)i);
}

/*
 * double-precision reference, same two passes as the implementation:
 * first ref[i] = ref[parent] * local (worlds), then ref[i] *= inverseBind
 */
static void
test_palette_ref(uint32_t count,
                 mat4    *locals,
                 int32_t *parents,
                 mat4    *inverseBind,
                 double  *ref) {
  uint32_t i, r, c, k;

  for (i = 0; i < count; i++) {
    int32_t p = parents[i];
    double  l[16], w[16];

    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        l[c * 4 + r] = (double)locals[i][c][r];

    if (p >= 0 && (uint32_t)p < i) {
      const double *pw = ref + 16 * (uint32_t)p;
      for (c = 0; c < 4; c++)
        for (r = 0; r < 4; r++) {
          double s = 0.0;
          for (k = 0; k < 4; k++)
            s += pw[k * 4 + r] * l[c * 4 + k];
          w[c * 4 + r] = s;
        }
    } else {
      memcpy(w, l, sizeof(w));
    }

    memcpy(ref + 16 * i, w, sizeof(w));
  }

  if (inverseBind) {
    for (i = 0; i < count; i++) {
      double ib[16], out[16];

      for (c = 0; c < 4; c++)
        for (r = 0; r < 4; r++)
          ib[c * 4 + r] = (double)inverseBind[i][c][r];

      for (c = 0; c < 4; c++)
        for (r = 0; r < 4; r++) {
          double s = 0.0;
          for (k = 0; k < 4; k++)
            s += ref[16 * i + k * 4 + r] * ib[c * 4 + k];
          out[c * 4 + r] = s;
        }
      memcpy(ref + 16 * i, out, sizeof(out));
    }
  }
}

#endif /* TEST_PALETTE_HELPERS_H */

TEST_IMPL(GLM_PREFIX, palette_worlds) {
  CGLM_ALIGN_MAT mat4 locals[4];
  CGLM_ALIGN_MAT mat4 worlds[4];
  CGLM_ALIGN_MAT mat4 expected;
  int32_t parents[4] = {-1, 0, 1, 7 /* out of range: treated as root */};
  uint32_t i;

  for (i = 0; i < 4; i++)
    test_palette_rand_trs(locals[i]);

  ASSERT(GLM(palette_worlds)(4, locals, parents, worlds) == 4)

  /* root: world == local */
  ASSERTIFY(test_assert_mat4_eq(worlds[0], locals[0]))

  /* chain: world == parent world * local */
  glm_mat4_mul(worlds[0], locals[1], expected);
  ASSERTIFY(test_assert_mat4_eq(worlds[1], expected))

  glm_mat4_mul(worlds[1], locals[2], expected);
  ASSERTIFY(test_assert_mat4_eq(worlds[2], expected))

  /* invalid parent index must not read out of bounds: root fallback */
  ASSERTIFY(test_assert_mat4_eq(worlds[3], locals[3]))

  /* NULL input writes nothing */
  ASSERT(GLM(palette_worlds)(4, NULL, parents, worlds) == 0)
  ASSERT(GLM(palette_worlds)(4, locals, NULL, worlds) == 0)
  ASSERT(GLM(palette_worlds)(4, locals, parents, NULL) == 0)

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, palette_build) {
  CGLM_ALIGN_MAT mat4 locals[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 invbind[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 palette[TEST_PALETTE_BONES];
  int32_t parents[TEST_PALETTE_BONES];
  double  ref[16 * TEST_PALETTE_BONES];
  double  err, maxerr;
  uint32_t i, c, r;

  for (i = 0; i < TEST_PALETTE_BONES; i++) {
    test_palette_rand_trs(locals[i]);
    test_palette_rand_trs(invbind[i]);
  }
  test_palette_rand_hierarchy(parents, TEST_PALETTE_BONES);

  ASSERT(GLM(palette_build)(TEST_PALETTE_BONES, TEST_PALETTE_BONES,
                            locals, parents, invbind, palette)
         == TEST_PALETTE_BONES)

  test_palette_ref(TEST_PALETTE_BONES, locals, parents, invbind, ref);

  maxerr = 0.0;
  for (i = 0; i < TEST_PALETTE_BONES; i++)
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++) {
        err = fabs((double)palette[i][c][r] - ref[16 * i + c * 4 + r]);
        if (err > maxerr)
          maxerr = err;
      }

  /* 200 bones: rebuild must match the reference within 1e-6 */
  ASSERT(maxerr <= 1e-6)

  /* NULL inverseBind: palette == worlds */
  ASSERT(GLM(palette_build)(TEST_PALETTE_BONES, TEST_PALETTE_BONES,
                            locals, parents, NULL, palette)
         == TEST_PALETTE_BONES)

  test_palette_ref(TEST_PALETTE_BONES, locals, parents, NULL, ref);

  maxerr = 0.0;
  for (i = 0; i < TEST_PALETTE_BONES; i++)
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++) {
        err = fabs((double)palette[i][c][r] - ref[16 * i + c * 4 + r]);
        if (err > maxerr)
          maxerr = err;
      }

  ASSERT(maxerr <= 1e-6)

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, palette_build_bounds) {
  CGLM_ALIGN_MAT mat4 locals[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 invbind[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 palette[TEST_PALETTE_BONES];
  int32_t parents[TEST_PALETTE_BONES];
  uint32_t i, c, r, written;

  for (i = 0; i < TEST_PALETTE_BONES; i++) {
    test_palette_rand_trs(locals[i]);
    test_palette_rand_trs(invbind[i]);
  }
  test_palette_rand_hierarchy(parents, TEST_PALETTE_BONES);

  /* sentinel canaries over the whole buffer */
  for (i = 0; i < TEST_PALETTE_BONES; i++)
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        palette[i][c][r] = -999.0f;

  /* count exceeds capacity: must not write past capacity */
  written = GLM(palette_build)(128, TEST_PALETTE_BONES,
                               locals, parents, invbind, palette);
  ASSERT(written == 128)

  for (i = 128; i < TEST_PALETTE_BONES; i++)
    for (c = 0; c < 4; c++)
      for (r = 0; r < 4; r++)
        ASSERT(test_eq(palette[i][c][r], -999.0f))

  /* zero capacity / NULL palette: nothing written */
  ASSERT(GLM(palette_build)(0, TEST_PALETTE_BONES,
                            locals, parents, invbind, palette) == 0)
  ASSERT(GLM(palette_build)(128, TEST_PALETTE_BONES,
                            locals, parents, invbind, NULL) == 0)

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, palette_build_perf) {
  CGLM_ALIGN_MAT mat4 locals[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 invbind[TEST_PALETTE_BONES];
  CGLM_ALIGN_MAT mat4 palette[TEST_PALETTE_BONES];
  int32_t parents[TEST_PALETTE_BONES];
  double  dt, best;
  clock_t t0, t1;
  uint32_t i;
  int      iter;

  for (i = 0; i < TEST_PALETTE_BONES; i++) {
    test_palette_rand_trs(locals[i]);
    test_palette_rand_trs(invbind[i]);
  }
  test_palette_rand_hierarchy(parents, TEST_PALETTE_BONES);

  /* warmup */
  GLM(palette_build)(TEST_PALETTE_BONES, TEST_PALETTE_BONES,
                     locals, parents, invbind, palette);

  best = 1e9;
  for (iter = 0; iter < 100; iter++) {
    t0 = clock();
    GLM(palette_build)(TEST_PALETTE_BONES, TEST_PALETTE_BONES,
                       locals, parents, invbind, palette);
    t1 = clock();
    dt = (double)(t1 - t0) / (double)CLOCKS_PER_SEC;
    if (dt < best)
      best = dt;
  }

  /* 200 bones: full palette rebuild must stay under 0.2 ms */
  ASSERT(best < 0.0002)

  TEST_SUCCESS
}
