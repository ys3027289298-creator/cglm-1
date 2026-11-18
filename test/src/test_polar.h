/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "test_common.h"

#ifndef _USE_MATH_DEFINES
#  define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <time.h>

/* max abs element-wise difference of two column-major double 3x3 matrices */
static double
GLM(polar3_maxdiff_)(CGLM_CONST double a[3][3], CGLM_CONST double b[3][3]) {
  double d, m;
  int i, j;

  m = 0.0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      d = fabs(a[i][j] - b[i][j]);
      if (d > m)
        m = d;
    }
  }
  return m;
}

TEST_IMPL(GLM_PREFIX, polar_pure_rotation) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  double expect_rot[3][3], expect_stretch[3][3];
  double c = cos(0.6), s = sin(0.6);
  glm_polar_status status;
  int i, j;

  /* rotation of 0.6 rad about z (column-major): polar factors are
     rot = m itself and stretch = I */
  m[0][0] = c;   m[0][1] = s;   m[0][2] = 0.0;
  m[1][0] = -s;  m[1][1] = c;   m[1][2] = 0.0;
  m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      expect_rot[i][j]     = m[i][j];
      expect_stretch[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  ASSERT(status == GLM_POLAR_OK);
  ASSERT(GLM(polar3_maxdiff_)(rot, expect_rot) < 1e-10);
  ASSERT(GLM(polar3_maxdiff_)(stretch, expect_stretch) < 1e-10);
  ASSERT(fabs(sv[0] - 1.0) < 1e-10);
  ASSERT(fabs(sv[1] - 1.0) < 1e-10);
  ASSERT(fabs(sv[2] - 1.0) < 1e-10);
  ASSERT(fabs(cond - 1.0) < 1e-10);

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_nonuniform_scale) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  double expect_rot[3][3], expect_stretch[3][3];
  glm_polar_status status;
  int i, j;

  /* diag(2, 0.5, 4): rot = I, stretch = m */
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      m[i][j]              = 0.0;
      expect_rot[i][j]     = (i == j) ? 1.0 : 0.0;
      expect_stretch[i][j] = 0.0;
    }
  }
  m[0][0] = 2.0;
  m[1][1] = 0.5;
  m[2][2] = 4.0;
  expect_stretch[0][0] = 2.0;
  expect_stretch[1][1] = 0.5;
  expect_stretch[2][2] = 4.0;

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  ASSERT(status == GLM_POLAR_OK);
  ASSERT(GLM(polar3_maxdiff_)(rot, expect_rot) < 1e-10);
  ASSERT(GLM(polar3_maxdiff_)(stretch, expect_stretch) < 1e-10);
  ASSERT(fabs(sv[0] - 4.0) < 1e-10);
  ASSERT(fabs(sv[1] - 2.0) < 1e-10);
  ASSERT(fabs(sv[2] - 0.5) < 1e-10);
  ASSERT(fabs(cond - 8.0) < 1e-10);

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_near_singular) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  double expect_rot[3][3], expect_stretch[3][3];
  double c = cos(0.9), s = sin(0.9);
  glm_polar_status status;
  int i, j;

  /* m = R_z(0.9) * diag(1, 1.5, 1e-7), cond(m) = 1.5e7:
     rot = R_z(0.9), stretch = diag(1, 1.5, 1e-7) */
  m[0][0] = c;        m[0][1] = s;        m[0][2] = 0.0;
  m[1][0] = -1.5 * s; m[1][1] = 1.5 * c;  m[1][2] = 0.0;
  m[2][0] = 0.0;      m[2][1] = 0.0;      m[2][2] = 1e-7;

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      expect_stretch[i][j] = 0.0;
    }
  }
  expect_rot[0][0] = c;   expect_rot[0][1] = s;   expect_rot[0][2] = 0.0;
  expect_rot[1][0] = -s;  expect_rot[1][1] = c;   expect_rot[1][2] = 0.0;
  expect_rot[2][0] = 0.0; expect_rot[2][1] = 0.0; expect_rot[2][2] = 1.0;
  expect_stretch[0][0] = 1.0;
  expect_stretch[1][1] = 1.5;
  expect_stretch[2][2] = 1e-7;

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  ASSERT(status == GLM_POLAR_OK);
  ASSERT(GLM(polar3_maxdiff_)(rot, expect_rot) < 1e-10);
  ASSERT(GLM(polar3_maxdiff_)(stretch, expect_stretch) < 1e-10);
  ASSERT(fabs(sv[0] - 1.5) < 1e-10);
  ASSERT(fabs(sv[1] - 1.0) < 1e-10);
  ASSERT(fabs(sv[2] - 1e-7) < 1e-10);
  ASSERT(fabs(cond - 1.5e7) / 1.5e7 < 1e-6);

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_strong_shear) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  double expect_rot[3][3], expect_stretch[3][3];
  double k, nrm, ct, st, tr, disc, lmax, lmin;
  glm_polar_status status;
  int i, j;

  /* pure shear of strength k in the xy plane:
     analytic polar rotation angle is theta = -atan(k / 2) */
  k   = 100.0;
  nrm = sqrt(4.0 + k * k);
  ct  = 2.0 / nrm;   /* cos(theta) */
  st  = -k / nrm;    /* sin(theta) */

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      m[i][j] = (i == j) ? 1.0 : 0.0;
  m[1][0] = k; /* row 0, col 1 */

  /* expected rot = R_z(theta) */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      expect_rot[i][j] = 0.0;
  expect_rot[0][0] = ct;  expect_rot[0][1] = st;  expect_rot[0][2] = 0.0;
  expect_rot[1][0] = -st; expect_rot[1][1] = ct;  expect_rot[1][2] = 0.0;
  expect_rot[2][0] = 0.0; expect_rot[2][1] = 0.0; expect_rot[2][2] = 1.0;

  /* expected stretch = rot^T * m (closed form, symmetric) */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      expect_stretch[i][j] = 0.0;
  expect_stretch[0][0] = ct;
  expect_stretch[0][1] = -st;
  expect_stretch[1][0] = -st;
  expect_stretch[1][1] = ct - st * k;
  expect_stretch[2][2] = 1.0;

  /* singular values: eigenvalues of the stretch; the xy block has
     determinant 1, so lmax * lmin = 1 and the third one is 1 */
  tr   = expect_stretch[0][0] + expect_stretch[1][1];
  disc = sqrt(tr * tr - 4.0);
  lmax = 0.5 * (tr + disc);
  lmin = 0.5 * (tr - disc);

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  ASSERT(status == GLM_POLAR_OK);
  ASSERT(GLM(polar3_maxdiff_)(rot, expect_rot) < 1e-10);
  ASSERT(GLM(polar3_maxdiff_)(stretch, expect_stretch) < 1e-10);
  ASSERT(fabs(sv[0] - lmax) < 1e-10);
  ASSERT(fabs(sv[1] - 1.0) < 1e-10);
  ASSERT(fabs(sv[2] - lmin) < 1e-10);
  ASSERT(fabs(cond - lmax / lmin) / (lmax / lmin) < 1e-6);

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_ill_conditioned) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  glm_polar_status status;
  int i, j;

  /* cond(m) = 1e14: must report the condition number, not diverge */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      m[i][j] = (i == j) ? 1.0 : 0.0;
  m[2][2] = 1e-14;

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  /* reported as ill-conditioned, with the condition number filled in */
  ASSERT(status == GLM_POLAR_ILL_CONDITIONED);
  ASSERT(cond > 1e12 && cond < 1e16);

  /* nothing diverged: all outputs are finite */
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      ASSERT(isfinite(rot[i][j]));
      ASSERT(isfinite(stretch[i][j]));
    }
    ASSERT(isfinite(sv[i]));
  }

  /* diagonal input keeps the iterates diagonal, so the factors stay
     accurate even here: rot = I, stretch = m */
  ASSERT(fabs(rot[0][0] - 1.0) < 1e-8);
  ASSERT(fabs(rot[1][1] - 1.0) < 1e-8);
  ASSERT(fabs(rot[2][2] - 1.0) < 1e-8);
  ASSERT(fabs(stretch[2][2] - 1e-14) < 1e-16);

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_no_converge) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  glm_polar_status status;
  int i, j;

  /* cond(m) = 1e14 needs far more than 3 Newton steps: the iteration
     cap must be honored and reported */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      m[i][j] = (i == j) ? 1.0 : 0.0;
  m[2][2] = 1e-14;

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 3, &cond);

  ASSERT(status == GLM_POLAR_NO_CONVERGE);
  ASSERT(isfinite(cond));

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_singular) {
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  glm_polar_status status;
  int i, j;

  /* exactly singular: reported with cond = INFINITY, no divergence */
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
      m[i][j] = (i == j) ? 1.0 : 0.0;
  m[2][2] = 0.0;

  status = GLM(polar3)(m, rot, stretch, sv, 1e-13, 100, &cond);

  ASSERT(status == GLM_POLAR_SINGULAR);
  ASSERT(isinf(cond));

  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      ASSERT(isfinite(rot[i][j]));
      ASSERT(isfinite(stretch[i][j]));
    }
    ASSERT(isfinite(sv[i]));
  }

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, polar_perf) {
  enum { N = 100000 };
  double m[3][3], rot[3][3], stretch[3][3], sv[3], cond;
  double angle, c, s, checksum, elapsed;
  glm_polar_status status;
  clock_t start;
  int i;

  checksum = 0.0;
  start    = clock();

  for (i = 0; i < N; i++) {
    /* rotation about z composed with non-uniform scale (cond = 2) */
    angle = 0.001 * (double)i;
    c     = cos(angle);
    s     = sin(angle);

    m[0][0] = c;         m[0][1] = s;         m[0][2] = 0.0;
    m[1][0] = -1.5 * s;  m[1][1] = 1.5 * c;   m[1][2] = 0.0;
    m[2][0] = 0.0;       m[2][1] = 0.0;       m[2][2] = 2.0;

    status = GLM(polar3)(m, rot, stretch, sv, 1e-12, 32, &cond);
    ASSERT(status == GLM_POLAR_OK);

    checksum += sv[0];
  }

  elapsed = (double)(clock() - start) / (double)CLOCKS_PER_SEC;

  fprintf(stderr,
          "  100k polar decompositions: %.2f ms\n",
          1e3 * elapsed);

  ASSERT(isfinite(checksum));

#ifdef NDEBUG
  /* optimized builds must process 100k matrices within 80 ms */
  ASSERT(elapsed < 0.08);
#endif

  TEST_SUCCESS
}
