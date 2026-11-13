/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 Functions:
   glm_validate_vec2_normalize
   glm_validate_vec2_normalize_to
   glm_validate_vec3_normalize
   glm_validate_vec3_normalize_to
   glm_validate_vec4_normalize
   glm_validate_vec4_normalize_to
   glm_validate_quat_normalize
   glm_validate_quat_normalize_to
   glm_validate_mat2_inv
   glm_validate_mat3_inv
   glm_validate_mat4_inv
   glm_validate_mat4_inv_fast

 Brief:
   Optional, compile-time-switchable input validation for the operations that
   can otherwise produce NaN / Inf and silently poison downstream results:
     - normalization of a zero (or sub-epsilon) vector / quaternion
     - inversion of a singular / degenerate matrix

   Each checked entry returns an explicit status code instead of relying on
   the caller to discover a NaN later. On success the mathematical result is
   bit-for-bit identical to the underlying unchecked function.

   Define CGLM_VALIDATE_OFF before including this header (or pass
   -DCGLM_VALIDATE_OFF on the compiler command line) to remove every check.
   In that mode each entry is a thin wrapper over the original function and
   compiles to the same machine code, so there is no runtime cost.
 */

#ifndef cglm_validate_h
#define cglm_validate_h

#include "common.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat2.h"
#include "mat3.h"
#include "mat4.h"
#include "quat.h"

/*!
 * @brief status codes returned by the glm_validate_* entry points.
 *
 * GLM_VALIDATE_OK is always zero; every failure code is negative so callers
 * can test `if (status < 0)` without caring about the exact reason.
 */
typedef enum glm_validate_status_t {
  /*! success; the destination was written exactly as the unchecked API does */
  GLM_VALIDATE_OK             = 0,
  /*! input vector / quaternion has a norm below the epsilon threshold */
  GLM_VALIDATE_ZERO_VECTOR    = -1,
  /*! input matrix is singular: determinant is zero within tolerance */
  GLM_VALIDATE_SINGULAR_MATRIX = -2
} glm_validate_status_t;

/*!
 * @brief threshold below which a determinant is considered zero.
 *
 * Uses the same default epsilon the rest of cglm relies on
 * (GLM_FLT_EPSILON, 1e-5f unless the project default is overridden).
 */
#ifndef GLM_VALIDATE_DET_EPSILON
#  define GLM_VALIDATE_DET_EPSILON GLM_FLT_EPSILON
#endif

#ifdef CGLM_VALIDATE_OFF

/*
 * Validation disabled at compile time: each entry forwards directly to the
 * original function. Because these are CGLM_INLINE (always_inline) one-line
 * wrappers, the generated code is identical to calling the unchecked API.
 */

CGLM_INLINE
glm_validate_status_t
glm_validate_vec2_normalize(vec2 v) {
  glm_vec2_normalize(v);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_vec2_normalize_to(const vec2 v, vec2 dest) {
  glm_vec2_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_vec3_normalize(vec3 v) {
  glm_vec3_normalize(v);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_vec3_normalize_to(const vec3 v, vec3 dest) {
  glm_vec3_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_vec4_normalize(vec4 v) {
  glm_vec4_normalize(v);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_vec4_normalize_to(const vec4 v, vec4 dest) {
  glm_vec4_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_quat_normalize(versor q) {
  glm_quat_normalize(q);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_quat_normalize_to(const versor q, versor dest) {
  glm_quat_normalize_to(q, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_mat2_inv(CGLM_CONST mat2 mat, mat2 dest) {
  glm_mat2_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_mat3_inv(CGLM_CONST mat3 mat, mat3 dest) {
  glm_mat3_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_mat4_inv(CGLM_CONST mat4 mat, mat4 dest) {
  glm_mat4_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

CGLM_INLINE
glm_validate_status_t
glm_validate_mat4_inv_fast(CGLM_CONST mat4 mat, mat4 dest) {
  glm_mat4_inv_fast(mat, dest);
  return GLM_VALIDATE_OK;
}

#else /* CGLM_VALIDATE_OFF */

/*
 * Validation enabled: reject degenerate inputs up front and leave the
 * destination untouched on failure so NaN can never propagate through it.
 */

/*!
 * @brief checked vec2 normalize (in place)
 *
 * @param[in, out] v vector; unchanged on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec2_normalize(vec2 v) {
  if (CGLM_UNLIKELY(glm_vec2_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec2_normalize(v);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked vec2 normalize to dest
 *
 * @param[in]  v    source
 * @param[out] dest destination; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec2_normalize_to(const vec2 v, vec2 dest) {
  if (CGLM_UNLIKELY(glm_vec2_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec2_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked vec3 normalize (in place)
 *
 * @param[in, out] v vector; unchanged on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec3_normalize(vec3 v) {
  if (CGLM_UNLIKELY(glm_vec3_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec3_normalize(v);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked vec3 normalize to dest
 *
 * @param[in]  v    source
 * @param[out] dest destination; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec3_normalize_to(const vec3 v, vec3 dest) {
  if (CGLM_UNLIKELY(glm_vec3_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec3_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked vec4 normalize (in place)
 *
 * @param[in, out] v vector; unchanged on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec4_normalize(vec4 v) {
  if (CGLM_UNLIKELY(glm_vec4_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec4_normalize(v);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked vec4 normalize to dest
 *
 * @param[in]  v    source
 * @param[out] dest destination; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_vec4_normalize_to(const vec4 v, vec4 dest) {
  if (CGLM_UNLIKELY(glm_vec4_norm(v) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_vec4_normalize_to(v, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked quaternion normalize (in place)
 *
 * @param[in, out] q quaternion; unchanged on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_quat_normalize(versor q) {
  if (CGLM_UNLIKELY(glm_quat_norm(q) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_quat_normalize(q);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked quaternion normalize to dest
 *
 * @param[in]  q    source quaternion
 * @param[out] dest destination; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_ZERO_VECTOR
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_quat_normalize_to(const versor q, versor dest) {
  if (CGLM_UNLIKELY(glm_quat_norm(q) < FLT_EPSILON))
    return GLM_VALIDATE_ZERO_VECTOR;

  glm_quat_normalize_to(q, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked mat2 inverse
 *
 * @param[in]  mat  matrix
 * @param[out] dest inverse; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_SINGULAR_MATRIX
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_mat2_inv(CGLM_CONST mat2 mat, mat2 dest) {
  if (CGLM_UNLIKELY(fabsf(glm_mat2_det(mat)) < GLM_VALIDATE_DET_EPSILON))
    return GLM_VALIDATE_SINGULAR_MATRIX;

  glm_mat2_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked mat3 inverse
 *
 * @param[in]  mat  matrix
 * @param[out] dest inverse; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_SINGULAR_MATRIX
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_mat3_inv(CGLM_CONST mat3 mat, mat3 dest) {
  if (CGLM_UNLIKELY(fabsf(glm_mat3_det(mat)) < GLM_VALIDATE_DET_EPSILON))
    return GLM_VALIDATE_SINGULAR_MATRIX;

  glm_mat3_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked mat4 inverse
 *
 * @param[in]  mat  matrix
 * @param[out] dest inverse; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_SINGULAR_MATRIX
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_mat4_inv(CGLM_CONST mat4 mat, mat4 dest) {
  if (CGLM_UNLIKELY(fabsf(glm_mat4_det(mat)) < GLM_VALIDATE_DET_EPSILON))
    return GLM_VALIDATE_SINGULAR_MATRIX;

  glm_mat4_inv(mat, dest);
  return GLM_VALIDATE_OK;
}

/*!
 * @brief checked fast mat4 inverse
 *
 * @param[in]  mat  matrix
 * @param[out] dest inverse; untouched on failure
 * @return GLM_VALIDATE_OK or GLM_VALIDATE_SINGULAR_MATRIX
 */
CGLM_INLINE
glm_validate_status_t
glm_validate_mat4_inv_fast(CGLM_CONST mat4 mat, mat4 dest) {
  if (CGLM_UNLIKELY(fabsf(glm_mat4_det(mat)) < GLM_VALIDATE_DET_EPSILON))
    return GLM_VALIDATE_SINGULAR_MATRIX;

  glm_mat4_inv_fast(mat, dest);
  return GLM_VALIDATE_OK;
}

#endif /* CGLM_VALIDATE_OFF */

#endif /* cglm_validate_h */
