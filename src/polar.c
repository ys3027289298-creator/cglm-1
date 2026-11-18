/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"

CGLM_EXPORT
glm_polar_status
glmc_polar3(CGLM_CONST double m[3][3],
            double rot[3][3],
            double stretch[3][3],
            double sv[3],
            double tol,
            int max_iter,
            double *cond) {
  return glm_polar3(m, rot, stretch, sv, tol, max_iter, cond);
}
