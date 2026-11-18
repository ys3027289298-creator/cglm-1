/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef cglmc_polar_h
#define cglmc_polar_h
#ifdef __cplusplus
extern "C" {
#endif

#include "../common.h"

CGLM_EXPORT
glm_polar_status
glmc_polar3(CGLM_CONST double m[3][3],
            double rot[3][3],
            double stretch[3][3],
            double sv[3],
            double tol,
            int max_iter,
            double *cond);

#ifdef __cplusplus
}
#endif
#endif /* cglmc_polar_h */
