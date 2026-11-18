/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "test_common.h"

CGLM_INLINE
float
test_bezier_plain(float s, float p0, float c0, float c1, float p1) {
  float x, xx, xxx, ss, sss;

  x   = 1.0f - s;
  xx  = x * x;
  xxx = xx * x;
  ss  = s * s;
  sss = ss * s;

  return p0 * xxx + 3.0f * (c0 * s * xx + c1 * ss * x) + p1 * sss;
}

CGLM_INLINE
float
test_hermite_plain(float s, float p0, float t0, float t1, float p1) {
  float ss, sss;

  ss  = s  * s;
  sss = ss * s;

  return p0 * (2.0f * sss - 3.0f * ss + 1.0f)
       + t0 * (sss - 2.0f * ss + s)
       + p1 * (-2.0f * sss + 3.0f * ss)
       + t1 * (sss - ss);
}

TEST_IMPL(bezier) {
  float s, p0, p1, c0, c1, smc, Bs, Bs_plain;

  s        = test_rand();
  p0       = test_rand();
  p1       = test_rand();
  c0       = test_rand();
  c1       = test_rand();

  /* test cubic bezier */
  smc      = glm_smc(s, GLM_BEZIER_MAT, (vec4){p0, c0, c1, p1});
  Bs       = glm_bezier(s, p0, c0, c1, p1);
  Bs_plain = test_bezier_plain(s, p0, c0, c1, p1);

  ASSERT(test_eq(Bs,  Bs_plain));
  ASSERTIFY(test_assert_eqf(smc, Bs_plain))
  ASSERTIFY(test_assert_eqf(Bs,  smc))

  /* test cubic hermite */
  smc      = glm_smc(s, GLM_HERMITE_MAT, (vec4){p0, p1, c0, c1});
  Bs       = glm_hermite(s, p0, c0, c1, p1);
  Bs_plain = test_hermite_plain(s, p0, c0, c1, p1);

  ASSERT(test_eq(Bs,  Bs_plain));
  ASSERT(test_eq(smc, Bs_plain));
  ASSERT(test_eq(Bs,  smc));

  TEST_SUCCESS
}

#define TEST_BEZIER_FIT_N 128

static void
test_bezier_line(vec2 *pts) {
  int i;

  for (i = 0; i < TEST_BEZIER_FIT_N; i++) {
    float t = (float)i / (float)(TEST_BEZIER_FIT_N - 1);

    pts[i][0] = 10.0f + 500.0f * t;
    pts[i][1] = 20.0f + 300.0f * t;
  }
}

static void
test_bezier_arc(vec2 *pts) {
  int i;

  for (i = 0; i < TEST_BEZIER_FIT_N; i++) {
    float a = (float)i / (float)(TEST_BEZIER_FIT_N - 1) * GLM_PI_2f;

    pts[i][0] = 200.0f * cosf(a);
    pts[i][1] = 200.0f * sinf(a);
  }

  /* avoid the tiny float error of cosf(pi/2) on the fixed endpoint */
  pts[TEST_BEZIER_FIT_N - 1][0] = 0.0f;
  pts[TEST_BEZIER_FIT_N - 1][1] = 200.0f;
}

static void
test_bezier_s(vec2 *pts) {
  int i;

  for (i = 0; i < TEST_BEZIER_FIT_N; i++) {
    float t = (float)i / (float)(TEST_BEZIER_FIT_N - 1);

    pts[i][0] = 600.0f * t;
    pts[i][1] = 300.0f * sinf(t * 2.0f * GLM_PIf);
  }
}

static void
test_bezier_noisy(vec2 *pts) {
  int i;

  test_bezier_arc(pts);

  for (i = 0; i < TEST_BEZIER_FIT_N; i++) {
    pts[i][0] += (test_rand() - 0.5f) * 8.0f;
    pts[i][1] += (test_rand() - 0.5f) * 8.0f;
  }
}

TEST_IMPL(bezier_fit) {
  vec2                  pts[TEST_BEZIER_FIT_N];
  glm_bezier_fit_opts   opts;
  glm_bezier_fit_result res;
  vec2                  eval0, eval1;
  float                 mse, e1, e2;
  int                   i;

  /* --- straight line: a cubic with collinear points must be exact --- */
  test_bezier_line(pts);
  opts.degree     = 3;
  opts.max_degree = 3;
  opts.tolerance  = 0.0f;

  ASSERT(glm_bezier_fit(pts, TEST_BEZIER_FIT_N, &opts, &res));
  ASSERT(res.degree == 3);
  ASSERT(res.raised == 0);

  /* endpoints are fixed */
  ASSERTIFY(test_assert_vec2_eq(res.control[0], pts[0]))
  ASSERTIFY(test_assert_vec2_eq(res.control[3], pts[TEST_BEZIER_FIT_N - 1]))

  glm_bezier_eval(3, res.control, 0.0f, eval0);
  glm_bezier_eval(3, res.control, 1.0f, eval1);
  ASSERTIFY(test_assert_vec2_eq(eval0, pts[0]))
  ASSERTIFY(test_assert_vec2_eq(eval1, pts[TEST_BEZIER_FIT_N - 1]))

  ASSERT(res.max_error < 0.01f);
  ASSERT(res.mse < 0.0001f);

  /* --- degree is clamped when there are fewer points than needed --- */
  {
    vec2 two[2] = {{0.0f, 0.0f}, {10.0f, 0.0f}};

    ASSERT(glm_bezier_fit(two, 2, NULL, &res));
    ASSERT(res.degree == 1);
    ASSERT(res.max_error < 0.0001f);
  }

  /* --- arc: cubic approximation of a quarter circle is accurate --- */
  test_bezier_arc(pts);
  opts.degree     = 3;
  opts.max_degree = 3;
  opts.tolerance  = 0.0f;

  ASSERT(glm_bezier_fit(pts, TEST_BEZIER_FIT_N, &opts, &res));
  ASSERT(res.degree == 3);
  ASSERTIFY(test_assert_vec2_eq(res.control[0], pts[0]))
  ASSERTIFY(test_assert_vec2_eq(res.control[3], pts[TEST_BEZIER_FIT_N - 1]))
  ASSERT(res.max_error < 0.6f);

  mse = glm_bezier_fit_error(pts, TEST_BEZIER_FIT_N, 3, res.control, NULL);
  ASSERTIFY(test_assert_eqf(mse, res.max_error));

  /* --- automatic degree raising on a hard S shaped set --- */
  test_bezier_s(pts);
  opts.degree     = 3;
  opts.max_degree = 7;
  opts.tolerance  = 5.0f;

  ASSERT(glm_bezier_fit(pts, TEST_BEZIER_FIT_N, &opts, &res));
  ASSERT(res.degree == 4);
  ASSERT(res.raised == 1);

  /* raising again must be capped by max_degree */
  opts.degree     = 3;
  opts.max_degree = 3;
  opts.tolerance  = 0.0001f;

  ASSERT(glm_bezier_fit(pts, TEST_BEZIER_FIT_N, &opts, &res));
  ASSERT(res.degree == 3);
  ASSERT(res.raised == 0);

  /* --- noisy set: fit succeeds, error is reported and finite --- */
  test_bezier_noisy(pts);
  opts.degree     = 3;
  opts.max_degree = 6;
  opts.tolerance  = 1.0f;

  ASSERT(glm_bezier_fit(pts, TEST_BEZIER_FIT_N, &opts, &res));
  ASSERT(res.raised == 1);
  ASSERT(res.degree == 4);
  ASSERT(res.max_error > 0.0f && res.max_error < 50.0f);
  ASSERT(res.mse > 0.0f);

  /* raise only once, even if still above tolerance */
  ASSERT(res.degree <= opts.max_degree);

  /* --- explicit lsq and error helpers share the same semantics --- */
  test_bezier_arc(pts);

  {
    vec2 c3[4];

    ASSERT(glm_bezier_fit_lsq(pts, TEST_BEZIER_FIT_N, 3, c3));
    ASSERTIFY(test_assert_vec2_eq(c3[0], pts[0]))
    ASSERTIFY(test_assert_vec2_eq(c3[3], pts[TEST_BEZIER_FIT_N - 1]))

    /* the polyline error estimate must match the true deviation closely */
    e1 = glm_bezier_fit_error(pts, TEST_BEZIER_FIT_N, 3, c3, &mse);
    ASSERT(e1 > 0.48f && e1 < 0.49f);
    ASSERT(mse > 0.0f);

    /* error helper without mse must agree with the one that reports it */
    e2 = glm_bezier_fit_error(pts, TEST_BEZIER_FIT_N, 3, c3, NULL);
    ASSERTIFY(test_assert_eqf(e1, e2))

    /* glmc wrapper must agree */
    {
      vec2 c3c[4];

      ASSERT(glmc_bezier_fit_lsq(pts, TEST_BEZIER_FIT_N, 3, c3c));
      for (i = 0; i < 4; i++)
        ASSERTIFY(test_assert_vec2_eq(c3[i], c3c[i]))
    }
  }

  /* --- degenerate point sets must not fail or produce garbage --- */
  {
    vec2 same[4] = {{1.0f, 2.0f}, {1.0f, 2.0f}, {1.0f, 2.0f}, {1.0f, 2.0f}};
    vec2 c4[5];

    ASSERT(glm_bezier_fit_lsq(same, 4, 3, c4));
    for (i = 0; i <= 3; i++)
      ASSERTIFY(test_assert_vec2_eq(c4[i], (vec2){1.0f, 2.0f}))

    ASSERT(glm_bezier_fit(same, 4, NULL, &res));
    ASSERT(res.max_error < 0.0001f);
    ASSERT(res.mse < 0.0001f);
  }

  /* collinear points with a higher requested degree stay on the line */
  {
    vec2 line[3] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {2.0f, 0.0f}};
    vec2 c5[3];

    ASSERT(glm_bezier_fit_lsq(line, 3, 5, c5));
    ASSERT(glm_bezier_fit_error(line, 3, 2, c5, NULL) < 0.0001f);
  }

  /* --- invalid inputs are rejected, not crashing --- */
  ASSERT(!glm_bezier_fit(pts, 1, NULL, &res));
  ASSERT(!glm_bezier_fit_lsq(pts, 1, 3, res.control));
  ASSERT(glm_bezier_fit_error(pts, 0, 3, res.control, NULL) == FLT_MAX);

  TEST_SUCCESS
}

