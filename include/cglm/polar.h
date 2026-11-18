/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Macros:
   GLM_POLAR_DEFAULT_TOL
   GLM_POLAR_DEFAULT_MAX_ITER
   GLM_POLAR_COND_THRESHOLD

 Functions:
   CGLM_INLINE glm_polar_status glm_polar3(CGLM_CONST double m[3][3],
                                           double rot[3][3],
                                           double stretch[3][3],
                                           double sv[3],
                                           double tol,
                                           int max_iter,
                                           double *cond);
 */

#ifndef cglm_polar_h
#define cglm_polar_h

#include "common.h"

#define GLM_POLAR_DEFAULT_TOL      1e-12
#define GLM_POLAR_DEFAULT_MAX_ITER 100
/* cond(m) beyond 1/sqrt(DBL_EPSILON) is reported as GLM_POLAR_ILL_CONDITIONED */
#define GLM_POLAR_COND_THRESHOLD   6.7108864e7

/*!
 * @brief polar decomposition of a 3x3 matrix: m = rot * stretch
 *
 * Splits m into the nearest orthogonal matrix rot (pure rotation, or
 * rotation+reflection when det(m) < 0) and a symmetric positive
 * (semi-)definite stretch, such that m = rot * stretch. This removes shear
 * from e.g. skinned matrices: rot is the shear-free rigid part and stretch
 * holds the non-uniform scale / residual stretch.
 *
 * The orthogonal factor is found by Newton's iteration
 *   Q_{k+1} = 0.5 * (Q_k + Q_k^{-T}),  Q_0 = m
 * which converges globally (and quadratically near the limit) for any
 * nonsingular m. The stretch is then S = Q^T m, symmetrized to cancel
 * roundoff, and sv receives the singular values of m (the eigenvalues of
 * the stretch) in descending order: sv[0] >= sv[1] >= sv[2] >= 0.
 *
 * The computation is done in double precision; all matrices are
 * column-major like mat3. Input and outputs must not overlap.
 *
 * The convergence criterion and the iteration cap are part of the
 * interface: the iteration stops as soon as
 *   ||Q_{k+1} - Q_k||_F <= tol * ||Q_{k+1}||_F
 * or when max_iter steps are exhausted. Pass tol <= 0 to use
 * GLM_POLAR_DEFAULT_TOL and max_iter <= 0 to use GLM_POLAR_DEFAULT_MAX_ITER.
 *
 * Ill-conditioned input never diverges: the condition number
 * cond(m) = sv[0] / sv[2] is always reported through cond and the return
 * value tells how much to trust the decomposition:
 *   GLM_POLAR_OK              converged, cond(m) <= GLM_POLAR_COND_THRESHOLD
 *   GLM_POLAR_ILL_CONDITIONED converged, but cond(m) is huge (see cond)
 *   GLM_POLAR_NO_CONVERGE     max_iter exhausted; outputs are best-effort
 *   GLM_POLAR_SINGULAR        (numerically) singular input; cond is
 *                             INFINITY, rot is identity, stretch and sv
 *                             are zero
 *
 * @param[in]  m        matrix to decompose (column-major)
 * @param[out] rot      nearest orthogonal matrix (rotation factor)
 * @param[out] stretch  symmetric positive (semi-)definite stretch factor
 * @param[out] sv       singular values of m, descending
 * @param[in]  tol      convergence criterion (<= 0 selects the default)
 * @param[in]  max_iter iteration upper bound (<= 0 selects the default)
 * @param[out] cond     condition number of m, may be NULL
 *
 * @return glm_polar_status, see above
 */
CGLM_INLINE
glm_polar_status
glm_polar3(CGLM_CONST double m[3][3],
           double rot[3][3],
           double stretch[3][3],
           double sv[3],
           double tol,
           int max_iter,
           double *cond) {
  double q[3][3], inv[3][3], s[3][3];
  double a, b, c, d, e, f, g, h, i;
  double c1, c2, c3, det, idt, tmp, delta, qnrm;
  double p1, p2, p, qbar, r, phi;
  double b00, b01, b02, b11, b12, b22, bdet;
  double e0, e1, e2, t;
  int iter, row, col, k, singular;
  glm_polar_status status;

  if (tol <= 0.0)
    tol = GLM_POLAR_DEFAULT_TOL;
  if (max_iter <= 0)
    max_iter = GLM_POLAR_DEFAULT_MAX_ITER;

  /* Q0 = m */
  for (col = 0; col < 3; col++)
    for (row = 0; row < 3; row++)
      q[col][row] = m[col][row];

  /* Newton iteration: Q <- 0.5 (Q + Q^{-T}), converges to the nearest
     orthogonal matrix, i.e. the orthogonal polar factor of m */
  singular = 0;
  for (iter = 0; iter < max_iter; iter++) {
    a = q[0][0]; b = q[0][1]; c = q[0][2];
    d = q[1][0]; e = q[1][1]; f = q[1][2];
    g = q[2][0]; h = q[2][1]; i = q[2][2];

    c1  = e * i - f * h;
    c2  = d * i - g * f;
    c3  = d * h - g * e;
    det = a * c1 - b * c2 + c * c3;

    /* singular (or overflowed) iterate: report instead of diverging */
    if (!isfinite(det) || det == 0.0) {
      singular = 1;
      break;
    }

    idt = 1.0 / det;

    inv[0][0] =  idt * c1;
    inv[0][1] = -idt * (b * i - h * c);
    inv[0][2] =  idt * (b * f - e * c);
    inv[1][0] = -idt * c2;
    inv[1][1] =  idt * (a * i - g * c);
    inv[1][2] = -idt * (a * f - d * c);
    inv[2][0] =  idt * c3;
    inv[2][1] = -idt * (a * h - g * b);
    inv[2][2] =  idt * (a * e - d * b);

    /* Q <- 0.5 (Q + inv(Q)^T), tracking ||dQ||_F and ||Q||_F */
    delta = 0.0;
    qnrm  = 0.0;
    for (col = 0; col < 3; col++) {
      for (row = 0; row < 3; row++) {
        tmp          = 0.5 * (q[col][row] + inv[row][col]);
        delta       += (tmp - q[col][row]) * (tmp - q[col][row]);
        qnrm        += tmp * tmp;
        q[col][row]  = tmp;
      }
    }

    /* convergence criterion: ||dQ||_F <= tol * ||Q||_F */
    if (delta <= tol * tol * qnrm)
      break;
  }

  if (singular) {
    /* report the singularity through cond; keep outputs deterministic */
    for (col = 0; col < 3; col++) {
      for (row = 0; row < 3; row++) {
        rot[col][row]     = (col == row) ? 1.0 : 0.0;
        stretch[col][row] = 0.0;
      }
      sv[col] = 0.0;
    }
    if (cond != NULL)
      *cond = INFINITY;
    return GLM_POLAR_SINGULAR;
  }

  /* stretch: S = Q^T m, symmetrized to cancel roundoff */
  for (col = 0; col < 3; col++) {
    for (row = 0; row < 3; row++) {
      tmp = 0.0;
      for (k = 0; k < 3; k++)
        tmp += q[row][k] * m[col][k];
      s[col][row] = tmp;
    }
  }
  for (col = 0; col < 3; col++)
    for (row = 0; row < 3; row++)
      stretch[col][row] = 0.5 * (s[col][row] + s[row][col]);

  /* singular values: eigenvalues of the symmetric stretch, in closed form
     (trigonometric method for the characteristic polynomial) */
  a   = stretch[0][0];
  e   = stretch[1][1];
  i   = stretch[2][2];
  b01 = stretch[1][0];
  b02 = stretch[2][0];
  b12 = stretch[2][1];

  p1 = b01 * b01 + b02 * b02 + b12 * b12;
  if (p1 == 0.0) {
    /* diagonal stretch: eigenvalues are the diagonal entries */
    e0 = a;
    e1 = e;
    e2 = i;
  } else {
    qbar = (a + e + i) / 3.0;
    p2   = (a - qbar) * (a - qbar)
         + (e - qbar) * (e - qbar)
         + (i - qbar) * (i - qbar) + 2.0 * p1;
    p    = sqrt(p2 / 6.0);

    /* B = (S - qbar I) / p, r = det(B) / 2 */
    b00  = (a - qbar) / p;
    b11  = (e - qbar) / p;
    b22  = (i - qbar) / p;
    b01 /= p;
    b02 /= p;
    b12 /= p;
    bdet = b00 * (b11 * b22 - b12 * b12)
         - b01 * (b01 * b22 - b12 * b02)
         + b02 * (b01 * b12 - b11 * b02);
    r    = 0.5 * bdet;

    if (r <= -1.0)
      phi = GLM_PI / 3.0;
    else if (r >= 1.0)
      phi = 0.0;
    else
      phi = acos(r) / 3.0;

    e0 = qbar + 2.0 * p * cos(phi);
    e2 = qbar + 2.0 * p * cos(phi + 2.0 * GLM_PI / 3.0);
    e1 = 3.0 * qbar - e0 - e2;
  }

  /* sort descending: e0 >= e1 >= e2 */
  if (e0 < e1) { t = e0; e0 = e1; e1 = t; }
  if (e1 < e2) { t = e1; e1 = e2; e2 = t; }
  if (e0 < e1) { t = e0; e0 = e1; e1 = t; }

  /* the stretch is positive semidefinite; clamp roundoff-level negatives */
  if (e0 < 0.0) e0 = 0.0;
  if (e1 < 0.0) e1 = 0.0;
  if (e2 < 0.0) e2 = 0.0;

  sv[0] = e0;
  sv[1] = e1;
  sv[2] = e2;

  for (col = 0; col < 3; col++)
    for (row = 0; row < 3; row++)
      rot[col][row] = q[col][row];

  if (cond != NULL)
    *cond = (e2 > 0.0) ? e0 / e2 : INFINITY;

  if (iter >= max_iter)
    status = GLM_POLAR_NO_CONVERGE;
  else if (e2 <= 0.0 || e0 / e2 > GLM_POLAR_COND_THRESHOLD)
    status = GLM_POLAR_ILL_CONDITIONED;
  else
    status = GLM_POLAR_OK;

  return status;
}

#endif /* cglm_polar_h */
