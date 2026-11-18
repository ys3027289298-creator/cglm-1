/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef cglmc_palette_h
#define cglmc_palette_h
#ifdef __cplusplus
extern "C" {
#endif

#include "../common.h"

#include <stdint.h>

CGLM_EXPORT
uint32_t
glmc_palette_worlds(uint32_t       count,
                    mat4            *locals,
                    const int32_t *parents,
                    mat4          *worlds);

CGLM_EXPORT
uint32_t
glmc_palette_build(uint32_t       capacity,
                   uint32_t       count,
                   mat4            *locals,
                   const int32_t *parents,
                   mat4            *inverseBind,
                   mat4          *palette);

#ifdef __cplusplus
}
#endif
#endif /* cglmc_palette_h */
