/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"

CGLM_EXPORT
float
glmc_bezier(float s, float p0, float c0, float c1, float p1) {
  return glm_bezier(s, p0, c0, c1, p1);
}

CGLM_EXPORT
float
glmc_hermite(float s, float p0, float t0, float t1, float p1) {
  return glm_hermite(s, p0, t0, t1, p1);
}

CGLM_EXPORT
float
glmc_decasteljau(float prm, float p0, float c0, float c1, float p1) {
  return glm_decasteljau(prm, p0, c0, c1, p1);
}

CGLM_EXPORT
void
glmc_bezier_eval(int degree, CGLM_CONST vec2 *ctrl, float s, vec2 dest) {
  glm_bezier_eval(degree, ctrl, s, dest);
}

CGLM_EXPORT
bool
glmc_bezier_fit_lsq(CGLM_CONST vec2 *pts,
                    size_t      npts,
                    int         degree,
                    vec2       *ctrl) {
  return glm_bezier_fit_lsq(pts, npts, degree, ctrl);
}

CGLM_EXPORT
float
glmc_bezier_fit_error(CGLM_CONST vec2 *pts,
                      size_t      npts,
                      int         degree,
                      CGLM_CONST vec2 *ctrl,
                      float      *mse) {
  return glm_bezier_fit_error(pts, npts, degree, ctrl, mse);
}

CGLM_EXPORT
bool
glmc_bezier_fit(CGLM_CONST vec2 *pts,
                size_t      npts,
                const glm_bezier_fit_opts   *opts,
                glm_bezier_fit_result       *dest) {
  return glm_bezier_fit(pts, npts, opts, dest);
}
