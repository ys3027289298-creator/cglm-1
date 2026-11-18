/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef cglm_bezier_h
#define cglm_bezier_h

#include "common.h"
#include "util.h"
#include <limits.h>

#define GLM_BEZIER_MAT_INIT  {{-1.0f,  3.0f, -3.0f,  1.0f},                   \
                              { 3.0f, -6.0f,  3.0f,  0.0f},                   \
                              {-3.0f,  3.0f,  0.0f,  0.0f},                   \
                              { 1.0f,  0.0f,  0.0f,  0.0f}}
#define GLM_HERMITE_MAT_INIT {{ 2.0f, -3.0f,  0.0f,  1.0f},                   \
                              {-2.0f,  3.0f,  0.0f,  0.0f},                   \
                              { 1.0f, -2.0f,  1.0f,  0.0f},                   \
                              { 1.0f, -1.0f,  0.0f,  0.0f}}
/* for C only */
#define GLM_BEZIER_MAT  ((mat4)GLM_BEZIER_MAT_INIT)
#define GLM_HERMITE_MAT ((mat4)GLM_HERMITE_MAT_INIT)

#define CGLM_DECASTEL_EPS   1e-9f
#define CGLM_DECASTEL_MAX   1000
#define CGLM_DECASTEL_SMALL 1e-20f

/* maximum degree supported by the least squares fitting helpers */
#ifndef CGLM_BEZIER_FIT_MAX_DEGREE
#  define CGLM_BEZIER_FIT_MAX_DEGREE 16
#endif

/* maximum subdivision depth used while measuring fitting error */
#ifndef CGLM_BEZIER_FIT_ERR_DEPTH
#  define CGLM_BEZIER_FIT_ERR_DEPTH 16
#endif

/* max frames alive at once: one pending sibling per subdivision level */
#define CGLM_BEZIER_FIT_STACK_MAX (CGLM_BEZIER_FIT_ERR_DEPTH + 2)

/* maximum number of points in the adaptive polyline approximation */
#ifndef CGLM_BEZIER_FIT_POLYLINE_MAX
#  define CGLM_BEZIER_FIT_POLYLINE_MAX 512
#endif

/* flatness tolerance of the adaptive error polyline, relative to curve size */
#ifndef CGLM_BEZIER_FIT_FLATNESS
#  define CGLM_BEZIER_FIT_FLATNESS 1e-4f
#endif

/*!
 * @brief cubic bezier interpolation
 *
 * Formula:
 *  B(s) = P0*(1-s)^3 + 3*C0*s*(1-s)^2 + 3*C1*s^2*(1-s) + P1*s^3
 *
 * similar result using matrix:
 *  B(s) = glm_smc(t, GLM_BEZIER_MAT, (vec4){p0, c0, c1, p1})
 *
 * glm_eq(glm_smc(...), glm_bezier(...)) should return TRUE
 *
 * @param[in]  s    parameter between 0 and 1
 * @param[in]  p0   begin point
 * @param[in]  c0   control point 1
 * @param[in]  c1   control point 2
 * @param[in]  p1   end point
 *
 * @return B(s)
 */
CGLM_INLINE
float
glm_bezier(float s, float p0, float c0, float c1, float p1) {
  float x, xx, ss, xs3, a;

  x   = 1.0f - s;
  xx  = x * x;
  ss  = s * s;
  xs3 = (s - ss) * 3.0f;
  a   = p0 * xx + c0 * xs3;

  return a + s * (c1 * xs3 + p1 * ss - a);
}

/*!
 * @brief cubic hermite interpolation
 *
 * Formula:
 *  H(s) = P0*(2*s^3 - 3*s^2 + 1) + T0*(s^3 - 2*s^2 + s)
 *            + P1*(-2*s^3 + 3*s^2) + T1*(s^3 - s^2)
 *
 * similar result using matrix:
 *  H(s) = glm_smc(t, GLM_HERMITE_MAT, (vec4){p0, p1, c0, c1})
 *
 * glm_eq(glm_smc(...), glm_hermite(...)) should return TRUE
 *
 * @param[in]  s    parameter between 0 and 1
 * @param[in]  p0   begin point
 * @param[in]  t0   tangent 1
 * @param[in]  t1   tangent 2
 * @param[in]  p1   end point
 *
 * @return H(s)
 */
CGLM_INLINE
float
glm_hermite(float s, float p0, float t0, float t1, float p1) {
  float ss, d, a, b, c, e, f;

  ss = s  * s;
  a  = ss + ss;
  c  = a  + ss;
  b  = a  * s;
  d  = s  * ss;
  f  = d  - ss;
  e  = b  - c;

  return p0 * (e + 1.0f) + t0 * (f - ss + s) + t1 * f - p1 * e;
}

/*!
 * @brief iterative way to solve cubic equation
 *
 * @param[in]  prm  parameter between 0 and 1
 * @param[in]  p0   begin point
 * @param[in]  c0   control point 1
 * @param[in]  c1   control point 2
 * @param[in]  p1   end point
 *
 * @return parameter to use in cubic equation
 */
CGLM_INLINE
float
glm_decasteljau(float prm, float p0, float c0, float c1, float p1) {
  float u, v, a, b, c, d, e, f;
  int   i;

  if (prm - p0 < CGLM_DECASTEL_SMALL)
    return 0.0f;

  if (p1 - prm < CGLM_DECASTEL_SMALL)
    return 1.0f;

  u  = 0.0f;
  v  = 1.0f;

  for (i = 0; i < CGLM_DECASTEL_MAX; i++) {
    /* de Casteljau Subdivision */
    a  = (p0 + c0) * 0.5f;
    b  = (c0 + c1) * 0.5f;
    c  = (c1 + p1) * 0.5f;
    d  = (a  + b)  * 0.5f;
    e  = (b  + c)  * 0.5f;
    f  = (d  + e)  * 0.5f; /* this one is on the curve! */

    /* The curve point is close enough to our wanted t */
    if (fabsf(f - prm) < CGLM_DECASTEL_EPS)
      return glm_clamp_zo((u  + v) * 0.5f);

    /* dichotomy */
    if (f < prm) {
      p0 = f;
      c0 = e;
      c1 = c;
      u  = (u  + v) * 0.5f;
    } else {
      c0 = a;
      c1 = d;
      p1 = f;
      v  = (u  + v) * 0.5f;
    }
  }

  return glm_clamp_zo((u  + v) * 0.5f);
}

/*!
 * @brief options for least squares bezier fitting
 */
typedef struct glm_bezier_fit_opts {
  int   degree;    /* requested degree of the fitted curve               */
  int   max_degree;/* max degree allowed for automatic degree raising      */
  float tolerance; /* max allowed deviation; <= 0 disables degree raising */
} glm_bezier_fit_opts;

/*!
 * @brief result of a least squares bezier fitting
 *
 * control[0 .. degree] holds the fitted control points; the first and the
 * last control points are always the first and the last input points.
 */
typedef struct glm_bezier_fit_result {
  vec2  control[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  int   degree;
  int   raised;     /* whether the degree was raised automatically */
  float max_error;  /* maximum deviation between points and the curve */
  float mse;        /* mean squared deviation */
} glm_bezier_fit_result;

#define GLM_BEZIER_FIT_OPTS_INIT {3, 3, 0.0f}

/* internal: binomial coefficients C(degree, k) by Pascal's triangle */
CGLM_INLINE
void
glm__bezier_fit_binom(int degree, double *coeff) {
  int i, k;

  for (i = 0; i <= degree; i++)
    coeff[i] = 0.0;

  coeff[0] = 1.0;

  for (k = 1; k <= degree; k++) {
    for (i = k; i >= 1; i--)
      coeff[i] += coeff[i - 1];
  }
}

/* internal: Bernstein basis of degree at u, b[0 .. degree] */
CGLM_INLINE
void
glm__bezier_fit_basis(int    degree,
                      CGLM_CONST double *binom,
                      double u,
                      double *b) {
  double up[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  double vp[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  double v;
  int    k;

  if (u <= 0.0) {
    b[0] = 1.0;
    for (k = 1; k <= degree; k++)
      b[k] = 0.0;
    return;
  }

  if (u >= 1.0) {
    for (k = 0; k < degree; k++)
      b[k] = 0.0;
    b[degree] = 1.0;
    return;
  }

  v     = 1.0 - u;
  up[0] = 1.0;
  vp[0] = 1.0;

  for (k = 1; k <= degree; k++) {
    up[k] = up[k - 1] * u;
    vp[k] = vp[k - 1] * v;
  }

  for (k = 0; k <= degree; k++)
    b[k] = binom[k] * up[k] * vp[degree - k];
}

/* internal: split a bezier at u = 0.5, left/right must hold degree+1 points */
CGLM_INLINE
void
glm__bezier_fit_split(int degree, CGLM_CONST vec2 *c, vec2 *left, vec2 *right) {
  vec2 tmp[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  int  i, r;

  for (i = 0; i <= degree; i++) {
    tmp[i][0] = c[i][0];
    tmp[i][1] = c[i][1];
  }

  left[0][0]       = tmp[0][0];
  left[0][1]       = tmp[0][1];
  right[degree][0] = tmp[degree][0];
  right[degree][1] = tmp[degree][1];

  for (r = 1; r <= degree; r++) {
    for (i = 0; i <= degree - r; i++) {
      tmp[i][0] = (tmp[i][0] + tmp[i + 1][0]) * 0.5f;
      tmp[i][1] = (tmp[i][1] + tmp[i + 1][1]) * 0.5f;
    }

    left[r][0]            = tmp[0][0];
    left[r][1]            = tmp[0][1];
    right[degree - r][0]  = tmp[degree - r][0];
    right[degree - r][1]  = tmp[degree - r][1];
  }
}

/* internal: squared distance between point p and segment [a, b] */
CGLM_INLINE
float
glm__bezier_fit_segdist2(const vec2 p, const vec2 a, const vec2 b) {
  float abx, aby, apx, apy, denom, t, dx, dy;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  apx = p[0] - a[0];
  apy = p[1] - a[1];

  denom = abx * abx + aby * aby;

  if (denom <= FLT_MIN)
    return apx * apx + apy * apy;

  t = (apx * abx + apy * aby) / denom;

  if (t < 0.0f)
    t = 0.0f;
  else if (t > 1.0f)
    t = 1.0f;

  dx = apx - abx * t;
  dy = apy - aby * t;

  return dx * dx + dy * dy;
}

/* internal: max squared distance of control points to the chord */
CGLM_INLINE
float
glm__bezier_fit_flat2(int degree, CGLM_CONST vec2 *c) {
  float dx, dy, len2, cross, d2, maxd2;
  int   i;

  dx   = c[degree][0] - c[0][0];
  dy   = c[degree][1] - c[0][1];
  len2 = dx * dx + dy * dy;
  maxd2 = 0.0f;

  if (len2 <= FLT_MIN) {
    for (i = 1; i < degree; i++) {
      d2 = (c[i][0] - c[0][0]) * (c[i][0] - c[0][0])
         + (c[i][1] - c[0][1]) * (c[i][1] - c[0][1]);
      if (d2 > maxd2)
        maxd2 = d2;
    }
    return maxd2;
  }

  for (i = 1; i < degree; i++) {
    cross = dx * (c[i][1] - c[0][1]) - dy * (c[i][0] - c[0][0]);
    d2    = cross * cross / len2;
    if (d2 > maxd2)
      maxd2 = d2;
  }

  return maxd2;
}

/* internal: a bezier control set waiting to be subdivided */
typedef struct glm__bezier_fit_frame {
  vec2 c[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  int  depth;
} glm__bezier_fit_frame;

/*
 * internal: approximate a bezier with an adaptive polyline.
 *
 * The curve is subdivided with de Casteljau until every piece is flat
 * enough. The resulting vertices are ordered along the curve and can be
 * used to measure the distance of sample points to the curve.
 */
CGLM_INLINE
int
glm__bezier_fit_polyline(int         degree,
                         CGLM_CONST vec2 *c,
                         float       tol,
                         vec2       *verts,
                         int         maxverts) {
  glm__bezier_fit_frame stack[CGLM_BEZIER_FIT_STACK_MAX];
  glm__bezier_fit_frame *fr;
  vec2  left[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  vec2  right[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  float tol2;
  int   count, sp, i;

  if (maxverts < 2)
    return 0;

  tol2 = tol * tol;

  verts[0][0] = c[0][0];
  verts[0][1] = c[0][1];
  count       = 1;

  for (i = 0; i <= degree; i++) {
    stack[0].c[i][0] = c[i][0];
    stack[0].c[i][1] = c[i][1];
  }
  stack[0].depth = 0;
  sp             = 1;

  while (sp > 0) {
    fr = &stack[--sp];

    if (fr->depth >= CGLM_BEZIER_FIT_ERR_DEPTH
        || count + 1 >= maxverts
        || sp + 2 > CGLM_BEZIER_FIT_STACK_MAX
        || glm__bezier_fit_flat2(degree, fr->c) <= tol2) {
      if (count < maxverts) {
        verts[count][0] = fr->c[degree][0];
        verts[count][1] = fr->c[degree][1];
        count++;
      }
      continue;
    }

    glm__bezier_fit_split(degree, fr->c, left, right);

    /* push right first so that left is processed first (curve order) */
    for (i = 0; i <= degree; i++) {
      stack[sp].c[i][0] = right[i][0];
      stack[sp].c[i][1] = right[i][1];
    }
    stack[sp].depth = fr->depth + 1;
    sp++;

    for (i = 0; i <= degree; i++) {
      stack[sp].c[i][0] = left[i][0];
      stack[sp].c[i][1] = left[i][1];
    }
    stack[sp].depth = fr->depth + 1;
    sp++;
  }

  return count;
}

/* internal: solve m x m system A * x = bx, A * y = by, in place */
CGLM_INLINE
bool
glm__bezier_fit_solve(double *a, double *bx, double *by, int m) {
  double maxdiag, pivmax, f, sx, sy, tmp;
  int    i, j, k, pivot;

  if (m <= 0)
    return true;

  maxdiag = 0.0;
  for (i = 0; i < m; i++) {
    if (fabs(a[i * m + i]) > maxdiag)
      maxdiag = fabs(a[i * m + i]);
  }

  if (maxdiag <= 0.0)
    return false;

  for (k = 0; k < m; k++) {
    pivot  = k;
    pivmax = fabs(a[k * m + k]);

    for (i = k + 1; i < m; i++) {
      if (fabs(a[i * m + k]) > pivmax) {
        pivmax = fabs(a[i * m + k]);
        pivot  = i;
      }
    }

    if (pivmax <= maxdiag * 1e-12)
      return false;

    if (pivot != k) {
      for (j = 0; j < m; j++) {
        tmp                = a[k * m + j];
        a[k * m + j]       = a[pivot * m + j];
        a[pivot * m + j]   = tmp;
      }

      tmp   = bx[k];
      bx[k] = bx[pivot];
      bx[pivot] = tmp;

      tmp   = by[k];
      by[k] = by[pivot];
      by[pivot] = tmp;
    }

    for (i = k + 1; i < m; i++) {
      f = a[i * m + k] / a[k * m + k];

      if (f == 0.0)
        continue;

      for (j = k; j < m; j++)
        a[i * m + j] -= f * a[k * m + j];

      bx[i] -= f * bx[k];
      by[i] -= f * by[k];
    }
  }

  for (k = m - 1; k >= 0; k--) {
    sx = bx[k];
    sy = by[k];

    for (j = k + 1; j < m; j++) {
      sx -= a[k * m + j] * bx[j];
      sy -= a[k * m + j] * by[j];
    }

    bx[k] = sx / a[k * m + k];
    by[k] = sy / a[k * m + k];
  }

  return true;
}

/*!
 * @brief evaluate a bezier curve of arbitrary degree at s
 *
 * Uses de Casteljau's algorithm.
 *
 * @param[in]  degree degree of the curve
 * @param[in]  ctrl   degree+1 control points
 * @param[in]  s      parameter between 0 and 1
 * @param[out] dest   point on the curve
 */
CGLM_INLINE
void
glm_bezier_eval(int degree, CGLM_CONST vec2 *ctrl, float s, vec2 dest) {
  vec2 tmp[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  int  i, r;

  if (degree < 0) {
    dest[0] = dest[1] = 0.0f;
    return;
  }

  if (degree > CGLM_BEZIER_FIT_MAX_DEGREE)
    degree = CGLM_BEZIER_FIT_MAX_DEGREE;

  for (i = 0; i <= degree; i++) {
    tmp[i][0] = ctrl[i][0];
    tmp[i][1] = ctrl[i][1];
  }

  for (r = 1; r <= degree; r++) {
    for (i = 0; i <= degree - r; i++) {
      tmp[i][0] += (tmp[i + 1][0] - tmp[i][0]) * s;
      tmp[i][1] += (tmp[i + 1][1] - tmp[i][1]) * s;
    }
  }

  dest[0] = tmp[0][0];
  dest[1] = tmp[0][1];
}

/*!
 * @brief least squares bezier fit with fixed endpoints
 *
 * Fits a bezier curve of the given degree to the points with chord-length
 * parameterization. The first and last control points are fixed to the
 * first and last input points; the remaining control points are obtained by
 * solving the normal equations of the least squares problem.
 *
 * @param[in]  pts    input points
 * @param[in]  npts   number of input points, must be >= 2
 * @param[in]  degree requested degree, clamped to [1, npts-1]
 * @param[out] ctrl   degree+1 control points
 *
 * @return false if the fit failed
 */
CGLM_INLINE
bool
glm_bezier_fit_lsq(CGLM_CONST vec2 *pts,
                   size_t      npts,
                   int         degree,
                   vec2       *ctrl) {
  double a[CGLM_BEZIER_FIT_MAX_DEGREE * CGLM_BEZIER_FIT_MAX_DEGREE];
  double aorig[CGLM_BEZIER_FIT_MAX_DEGREE * CGLM_BEZIER_FIT_MAX_DEGREE];
  double rhs[2][CGLM_BEZIER_FIT_MAX_DEGREE];
  double b0x[CGLM_BEZIER_FIT_MAX_DEGREE];
  double b0y[CGLM_BEZIER_FIT_MAX_DEGREE];
  double binom[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  double basis[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  double total, cum, u, rx, ry, dx, dy;
  size_t i;
  int    j, k, m, p, n;

  if (pts == NULL || ctrl == NULL || npts < 2)
    return false;

  if (npts > (size_t)INT_MAX)
    return false;

  n = (int)npts;

  p = degree;
  if (p < 1)
    p = 1;
  if (p > CGLM_BEZIER_FIT_MAX_DEGREE)
    p = CGLM_BEZIER_FIT_MAX_DEGREE;
  if (p > n - 1)
    p = n - 1;

  ctrl[0][0] = pts[0][0];
  ctrl[0][1] = pts[0][1];
  ctrl[p][0] = pts[npts - 1][0];
  ctrl[p][1] = pts[npts - 1][1];

  if (p == 1)
    return true;

  total = 0.0;
  for (i = 1; i < npts; i++) {
    dx     = (double)pts[i][0]     - (double)pts[i - 1][0];
    dy     = (double)pts[i][1]     - (double)pts[i - 1][1];
    total += sqrt(dx * dx + dy * dy);
  }

  if (total <= 0.0) {
    /* all points coincide: the curve degenerates to that single point */
    for (j = 1; j < p; j++) {
      ctrl[j][0] = pts[0][0];
      ctrl[j][1] = pts[0][1];
    }

    return true;
  }

  glm__bezier_fit_binom(p, binom);

  for (j = 0; j < p - 1; j++) {
    rhs[0][j] = 0.0;
    rhs[1][j] = 0.0;
    for (k = 0; k < p - 1; k++)
      a[j * (p - 1) + k] = 0.0;
  }

  cum = 0.0;

  for (i = 0; i < npts; i++) {
    if (i > 0) {
      dx   = (double)pts[i][0] - (double)pts[i - 1][0];
      dy   = (double)pts[i][1] - (double)pts[i - 1][1];
      cum += sqrt(dx * dx + dy * dy);
    }

    u = cum / total;
    if (u < 0.0)
      u = 0.0;
    else if (u > 1.0)
      u = 1.0;

    glm__bezier_fit_basis(p, binom, u, basis);

    rx = (double)pts[i][0]
       - basis[0] * (double)pts[0][0]
       - basis[p] * (double)pts[npts - 1][0];
    ry = (double)pts[i][1]
       - basis[0] * (double)pts[0][1]
       - basis[p] * (double)pts[npts - 1][1];

    for (j = 1; j < p; j++) {
      rhs[0][j - 1] += basis[j] * rx;
      rhs[1][j - 1] += basis[j] * ry;

      for (k = 1; k < p; k++)
        a[(j - 1) * (p - 1) + (k - 1)] += basis[j] * basis[k];
    }
  }

  m = p - 1;

  /*
   * keep the untouched normal equations: a failed solve modifies them in
   * place, so the regularized retry needs the original system.
   */
  for (j = 0; j < m * m; j++)
    aorig[j] = a[j];

  for (j = 0; j < m; j++) {
    b0x[j] = rhs[0][j];
    b0y[j] = rhs[1][j];
  }

  if (!glm__bezier_fit_solve(a, rhs[0], rhs[1], m)) {
    /*
     * the point set does not constrain every control point (e.g. collinear
     * or repeated points). Solve a lightly regularized system instead; the
     * endpoints are already fixed so this stays stable and still returns a
     * usable curve.
     */
    double ridge = 0.0;

    for (j = 0; j < m; j++) {
      if (aorig[j * m + j] > ridge)
        ridge = aorig[j * m + j];
    }

    ridge *= 1e-9;
    if (ridge <= 0.0)
      return false;

    for (j = 0; j < m * m; j++)
      a[j] = aorig[j];

    for (j = 0; j < m; j++) {
      a[j * m + j] += ridge;
      rhs[0][j]     = b0x[j];
      rhs[1][j]     = b0y[j];
    }

    if (!glm__bezier_fit_solve(a, rhs[0], rhs[1], m))
      return false;
  }

  for (j = 1; j < p; j++) {
    ctrl[j][0] = (float)rhs[0][j - 1];
    ctrl[j][1] = (float)rhs[1][j - 1];
  }

  return true;
}

/*!
 * @brief measure how well a bezier fits a point set
 *
 * The curve is approximated by an adaptive polyline (de Casteljau
 * subdivision) and the distance from every input point to the nearest
 * polyline segment is measured.
 *
 * @param[in]  pts    input points
 * @param[in]  npts   number of input points
 * @param[in]  degree degree of the curve
 * @param[in]  ctrl   degree+1 control points
 * @param[out] mse    mean squared deviation, may be NULL
 *
 * @return maximum deviation, FLT_MAX on invalid input
 */
CGLM_INLINE
float
glm_bezier_fit_error(CGLM_CONST vec2 *pts,
                     size_t      npts,
                     int         degree,
                     CGLM_CONST vec2 *ctrl,
                     float      *mse) {
  vec2  verts[CGLM_BEZIER_FIT_POLYLINE_MAX];
  float scale, tol, d2, maxd2, sum;
  int   count, i, s;
  size_t n;

  if (mse != NULL)
    *mse = 0.0f;

  if (pts == NULL || ctrl == NULL || npts < 1)
    return FLT_MAX;

  if (degree < 0 || degree > CGLM_BEZIER_FIT_MAX_DEGREE)
    return FLT_MAX;

  scale = 0.0f;
  for (i = 0; i < degree; i++) {
    for (s = i + 1; s <= degree; s++) {
      d2 = fabsf(ctrl[s][0] - ctrl[i][0]);
      if (d2 > scale)
        scale = d2;
      d2 = fabsf(ctrl[s][1] - ctrl[i][1]);
      if (d2 > scale)
        scale = d2;
    }
  }

  if (scale <= FLT_MIN) {
    /* the curve is a single point */
    maxd2 = 0.0f;
    sum   = 0.0f;

    for (n = 0; n < npts; n++) {
      d2 = (pts[n][0] - ctrl[0][0]) * (pts[n][0] - ctrl[0][0])
         + (pts[n][1] - ctrl[0][1]) * (pts[n][1] - ctrl[0][1]);
      if (d2 > maxd2)
        maxd2 = d2;
      sum += d2;
    }

    if (mse != NULL)
      *mse = (float)((double)sum / (double)npts);

    return sqrtf(maxd2);
  }

  tol   = scale * CGLM_BEZIER_FIT_FLATNESS;
  count = glm__bezier_fit_polyline(degree, ctrl, tol,
                                   verts, CGLM_BEZIER_FIT_POLYLINE_MAX);

  if (count <= 0)
    return FLT_MAX;

  maxd2 = 0.0f;
  sum   = 0.0f;

  for (n = 0; n < npts; n++) {
    d2 = (pts[n][0] - verts[0][0]) * (pts[n][0] - verts[0][0])
       + (pts[n][1] - verts[0][1]) * (pts[n][1] - verts[0][1]);

    for (i = 1; i < count; i++) {
      float sd2 = glm__bezier_fit_segdist2(pts[n], verts[i - 1], verts[i]);
      if (sd2 < d2)
        d2 = sd2;
    }

    if (d2 > maxd2)
      maxd2 = d2;

    sum += d2;
  }

  if (mse != NULL)
    *mse = (float)((double)sum / (double)npts);

  return sqrtf(maxd2);
}

/*!
 * @brief least squares bezier fit with optional automatic degree raising
 *
 * Fits a bezier curve with fixed endpoints. If the maximum deviation is
 * greater than opts->tolerance and the degree can be raised, the points are
 * re-fitted once with degree + 1. The initial degree and the threshold are
 * configurable; chords are used to parameterize the points.
 *
 * Example:
 * @code
 *   glm_bezier_fit_opts   opts;
 *   glm_bezier_fit_result res;
 *
 *   opts.degree     = 3;
 *   opts.max_degree = 5;
 *   opts.tolerance  = 0.01f;
 *
 *   if (glm_bezier_fit(points, count, &opts, &res)) { ... }
 * @endcode
 *
 * @param[in]  pts  input points
 * @param[in]  npts number of input points, must be >= 2
 * @param[in]  opts fit options, NULL for defaults (degree 3, no raising)
 * @param[out] dest fitted control points and error metrics
 *
 * @return false if the fit failed
 */
CGLM_INLINE
bool
glm_bezier_fit(CGLM_CONST vec2 *pts,
               size_t      npts,
               const glm_bezier_fit_opts   *opts,
               glm_bezier_fit_result       *dest) {
  glm_bezier_fit_opts   def;
  vec2                  raised_ctrl[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
  float                 maxerr, mse, raised_maxerr, raised_mse;
  int                   degree, maxdeg, i;

  if (dest == NULL)
    return false;

  for (i = 0; i <= CGLM_BEZIER_FIT_MAX_DEGREE; i++) {
    dest->control[i][0] = 0.0f;
    dest->control[i][1] = 0.0f;
  }
  dest->degree    = 0;
  dest->raised    = 0;
  dest->max_error = 0.0f;
  dest->mse       = 0.0f;

  if (pts == NULL || npts < 2 || npts > (size_t)INT_MAX)
    return false;

  if (opts == NULL) {
    def.degree     = 3;
    def.max_degree = 3;
    def.tolerance  = 0.0f;
    opts           = &def;
  }

  degree = opts->degree;
  if (degree < 1)
    degree = 1;
  if (degree > CGLM_BEZIER_FIT_MAX_DEGREE)
    degree = CGLM_BEZIER_FIT_MAX_DEGREE;
  if (degree > (int)npts - 1)
    degree = (int)npts - 1;

  maxdeg = opts->max_degree;
  if (maxdeg < degree)
    maxdeg = degree;
  if (maxdeg > CGLM_BEZIER_FIT_MAX_DEGREE)
    maxdeg = CGLM_BEZIER_FIT_MAX_DEGREE;
  if (maxdeg > (int)npts - 1)
    maxdeg = (int)npts - 1;

  if (!glm_bezier_fit_lsq(pts, npts, degree, dest->control))
    return false;

  maxerr = glm_bezier_fit_error(pts, npts, degree, dest->control, &mse);

  if (opts->tolerance > 0.0f
      && maxerr > opts->tolerance
      && degree < maxdeg) {
    if (glm_bezier_fit_lsq(pts, npts, degree + 1, raised_ctrl)) {
      raised_maxerr = glm_bezier_fit_error(pts, npts, degree + 1,
                                           raised_ctrl, &raised_mse);

      if (raised_maxerr < FLT_MAX) {
        for (i = 0; i <= degree + 1; i++) {
          dest->control[i][0] = raised_ctrl[i][0];
          dest->control[i][1] = raised_ctrl[i][1];
        }

        degree    = degree + 1;
        maxerr    = raised_maxerr;
        mse       = raised_mse;
        dest->raised = 1;
      }
    }
  }

  dest->degree    = degree;
  dest->max_error = maxerr;
  dest->mse       = mse;

  return maxerr < FLT_MAX;
}

#endif /* cglm_bezier_h */
