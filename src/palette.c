/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"

CGLM_EXPORT
uint32_t
glmc_palette_worlds(uint32_t       count,
                    mat4            *locals,
                    const int32_t *parents,
                    mat4          *worlds) {
  return glm_palette_worlds(count, locals, parents, worlds);
}

CGLM_EXPORT
uint32_t
glmc_palette_build(uint32_t       capacity,
                   uint32_t       count,
                   mat4            *locals,
                   const int32_t *parents,
                   mat4            *inverseBind,
                   mat4          *palette) {
  return glm_palette_build(capacity, count, locals, parents, inverseBind,
                           palette);
}
