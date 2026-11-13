/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef test_validate_h
#define test_validate_h

#include <cglm/validate.h>

/*
 * Degenerate inputs must be reported with an explicit status code instead of
 * silently producing NaN/Inf. Covers every checked normalize entry point.
 */
TEST_IMPL(validate_zero_vector_detected) {
  vec2 z2 = {0.0f, 0.0f};
  vec3 z3 = {0.0f, 0.0f, 0.0f};
  vec4 z4 = {0.0f, 0.0f, 0.0f, 0.0f};
  vec2 d2;
  vec3 d3;
  vec4 d4;

  ASSERT(glm_validate_vec2_normalize(z2) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_vec2_normalize_to(z2, d2) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_vec3_normalize(z3) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_vec3_normalize_to(z3, d3) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_vec4_normalize(z4) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_vec4_normalize_to(z4, d4) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_quat_normalize(z4) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(glm_validate_quat_normalize_to(z4, d4) == GLM_VALIDATE_ZERO_VECTOR)

  /* near-zero vector whose norm is still below the epsilon threshold */
  vec3 tiny = {1e-20f, -1e-20f, 0.0f};
  ASSERT(glm_validate_vec3_normalize_to(tiny, d3) == GLM_VALIDATE_ZERO_VECTOR)

  TEST_SUCCESS
}

/*
 * Singular / degenerate matrices must be rejected before the division by a
 * zero determinant can inject Inf into the result.
 */
TEST_IMPL(validate_singular_matrix_detected) {
  mat2 s2 = GLM_MAT2_ZERO_INIT;
  mat3 s3 = GLM_MAT3_ZERO_INIT;
  mat4 s4 = GLM_MAT4_ZERO_INIT;
  mat2 d2;
  mat3 d3;
  mat4 d4;

  ASSERT(glm_validate_mat2_inv(s2, d2) == GLM_VALIDATE_SINGULAR_MATRIX)
  ASSERT(glm_validate_mat3_inv(s3, d3) == GLM_VALIDATE_SINGULAR_MATRIX)
  ASSERT(glm_validate_mat4_inv(s4, d4) == GLM_VALIDATE_SINGULAR_MATRIX)
  ASSERT(glm_validate_mat4_inv_fast(s4, d4) == GLM_VALIDATE_SINGULAR_MATRIX)

  /* rank-deficient: second row is a multiple of the first, det == 0 */
  mat4 rank_def = {
    {1.0f, 2.0f, 3.0f, 4.0f},
    {2.0f, 4.0f, 6.0f, 8.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f}
  };
  ASSERT(glm_validate_mat4_inv(rank_def, d4) == GLM_VALIDATE_SINGULAR_MATRIX)

  TEST_SUCCESS
}

/*
 * On failure the destination buffer must be left untouched (filled with a
 * sentinel here) so NaN can never travel through an "output" of a failed op.
 */
TEST_IMPL(validate_dest_untouched_on_failure) {
  vec3 zero = {0.0f, 0.0f, 0.0f};
  vec3 vdest = {7.0f, 7.0f, 7.0f};
  mat4 singular = GLM_MAT4_ZERO_INIT;
  mat4 mdest;
  int  i;

  for (i = 0; i < 16; i++)
    ((float *)mdest)[i] = 9.0f;

  ASSERT(glm_validate_vec3_normalize_to(zero, vdest) == GLM_VALIDATE_ZERO_VECTOR)
  ASSERT(test_eq(vdest[0], 7.0f))
  ASSERT(test_eq(vdest[1], 7.0f))
  ASSERT(test_eq(vdest[2], 7.0f))

  ASSERT(glm_validate_mat4_inv(singular, mdest) == GLM_VALIDATE_SINGULAR_MATRIX)
  for (i = 0; i < 16; i++)
    ASSERT(test_eq(((float *)mdest)[i], 9.0f))

  TEST_SUCCESS
}

/*
 * For valid inputs the checked entry must return GLM_VALIDATE_OK and produce
 * a result that is bit-for-bit identical to the unchecked cglm function.
 */
TEST_IMPL(validate_normal_input_bitexact) {
  vec3 v3a = {2.0f, -3.0f, 4.0f};
  vec3 v3b = {2.0f, -3.0f, 4.0f};
  vec3 r3_checked, r3_plain;
  vec4 v4a = {1.0f, -2.0f, 3.0f, -4.0f};
  vec4 v4b = {1.0f, -2.0f, 3.0f, -4.0f};
  vec4 r4_checked, r4_plain;
  mat4 m4 = {
    {1.0f,  0.0f, 0.0f, 0.0f},
    {0.0f,  2.0f, 0.0f, 1.0f},
    {0.0f,  0.0f, 3.0f, 2.0f},
    {1.0f, -1.0f, 0.0f, 1.0f}
  };
  mat4 m4_copy, r4m_checked, r4m_plain;
  int  i;

  ASSERT(glm_validate_vec3_normalize_to(v3a, r3_checked) == GLM_VALIDATE_OK)
  glm_vec3_normalize_to(v3b, r3_plain);
  ASSERT(memcmp(r3_checked, r3_plain, sizeof(r3_checked)) == 0)

  ASSERT(glm_validate_vec4_normalize_to(v4a, r4_checked) == GLM_VALIDATE_OK)
  glm_vec4_normalize_to(v4b, r4_plain);
  ASSERT(memcmp(r4_checked, r4_plain, sizeof(r4_checked)) == 0)

  glm_mat4_copy(m4, m4_copy);
  ASSERT(glm_validate_mat4_inv(m4, r4m_checked) == GLM_VALIDATE_OK)
  glm_mat4_inv(m4_copy, r4m_plain);
  ASSERT(memcmp(r4m_checked, r4m_plain, sizeof(r4m_checked)) == 0)

  /* in-place variant must also match the unchecked API bit-for-bit */
  {
    mat4 in_checked, in_plain;

    glm_mat4_copy(m4, in_checked);
    glm_mat4_copy(m4, in_plain);
    ASSERT(glm_validate_mat4_inv(in_checked, in_checked) == GLM_VALIDATE_OK)
    glm_mat4_inv(in_plain, in_plain);
    ASSERT(memcmp(in_checked, in_plain, sizeof(in_checked)) == 0)
  }

  /* success path leaves no NaN/Inf behind */
  for (i = 0; i < 4; i++) {
    ASSERT(!glm_vec3_isnan(r3_checked))
    ASSERT(!glm_vec3_isinf(r3_checked))
  }

  TEST_SUCCESS
}

#endif /* test_validate_h */
