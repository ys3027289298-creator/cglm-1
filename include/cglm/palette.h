/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef cglm_palette_h
#define cglm_palette_h

#include "common.h"
#include "mat4.h"

#include <stdint.h>

/*
 Bone matrix palette (linear blend skinning).

 The palette is rebuilt from scratch every frame from packed per-bone
 data:

   - locals:      local (parent-relative) bone transforms, one mat4 per bone
   - parents:     packed parent indices, one int32 per bone. Bones are
                  topologically ordered, so parents[i] < i; a negative
                  value marks a root. Out-of-range indices are treated
                  as roots, so a corrupt index can never cause an
                  out-of-bounds read.
   - inverseBind: inverse bind-pose matrices, or NULL

 All matrices are 16-byte aligned (plain mat4 arrays / CGLM_ALIGN_MAT).

 Functions:
   CGLM_INLINE uint32_t glm_palette_worlds(uint32_t count,
                                           mat4 *locals,
                                           const int32_t *parents,
                                           mat4 *worlds);
   CGLM_INLINE uint32_t glm_palette_build(uint32_t capacity,
                                          uint32_t count,
                                          mat4 *locals,
                                          const int32_t *parents,
                                          mat4 *inverseBind,
                                          mat4 *palette);
 */

/*!
 * @brief compose world-space bone matrices from packed local transforms
 *
 * Single linear pass over the packed hierarchy:
 *   worlds[i] = worlds[parents[i]] * locals[i]
 *
 * @param[in]  count   number of bones
 * @param[in]  locals  local (parent-relative) transforms, count entries
 * @param[in]  parents packed parent indices, count entries (negative = root)
 * @param[out] worlds  world-space bone matrices, count entries
 *
 * @returns number of matrices written (0 on NULL input)
 */
CGLM_INLINE
uint32_t
glm_palette_worlds(uint32_t       count,
                   mat4            *locals,
                   const int32_t *parents,
                   mat4          *worlds) {
  uint32_t i;
  int32_t  parent;

  if (!locals || !parents || !worlds)
    return 0;

  for (i = 0; i < count; i++) {
    parent = parents[i];
    if (parent >= 0 && (uint32_t)parent < i) {
      glm_mat4_mul(worlds[parent], locals[i], worlds[i]);
    } else {
      /* root or invalid index: never read outside [0, i) */
      glm_mat4_copy(locals[i], worlds[i]);
    }
  }

  return count;
}

/*!
 * @brief rebuild the skinning palette, bounded by the destination capacity
 *
 *   palette[i] = worlds[i] * inverseBind[i]   (worlds[i] if inverseBind is NULL)
 *
 * At most @p capacity entries are written even if @p count is larger, so a
 * mismatched bone count can never write past the end of the palette buffer.
 *
 * @param[in]  capacity    allocated capacity of palette, in matrices
 * @param[in]  count       number of bones
 * @param[in]  locals      local (parent-relative) transforms, count entries
 * @param[in]  parents     packed parent indices, count entries (negative = root)
 * @param[in]  inverseBind inverse bind-pose matrices, count entries, or NULL
 * @param[out] palette     destination palette, capacity entries
 *
 * @returns number of palette entries written: min(count, capacity)
 */
CGLM_INLINE
uint32_t
glm_palette_build(uint32_t       capacity,
                  uint32_t       count,
                  mat4            *locals,
                  const int32_t *parents,
                  mat4            *inverseBind,
                  mat4          *palette) {
  uint32_t i, n;

  if (!palette || capacity == 0)
    return 0;

  n = count < capacity ? count : capacity; /* never write past capacity */

  if (n > 0 && glm_palette_worlds(n, locals, parents, palette) == 0)
    return 0;

  if (inverseBind) {
    for (i = 0; i < n; i++)
      glm_mat4_mul(palette[i], inverseBind[i], palette[i]);
  }

  return n;
}

#endif /* cglm_palette_h */
