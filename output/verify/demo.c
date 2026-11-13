/*
 * Small crafted demo for the validation layer.
 * Shows: degenerate inputs return explicit error codes instead of NaN,
 * while normal inputs are accepted and produce the usual result.
 */
#include <stdio.h>
#include <cglm/cglm.h>
#include <cglm/validate.h>

static const char *status_name(glm_validate_status_t s) {
  switch (s) {
    case GLM_VALIDATE_OK:              return "OK";
    case GLM_VALIDATE_ZERO_VECTOR:     return "ZERO_VECTOR";
    case GLM_VALIDATE_SINGULAR_MATRIX: return "SINGULAR_MATRIX";
    default:                           return "UNKNOWN";
  }
}

int main(void) {
  /* 1. zero vector normalization: error code instead of NaN */
  vec3 zero = {0.0f, 0.0f, 0.0f};
  vec3 dir  = {3.0f, 0.0f, 4.0f};
  vec3 out  = {-1.0f, -1.0f, -1.0f};

  printf("[normalize] zero vector  -> %s\n",
         status_name(glm_validate_vec3_normalize_to(zero, out)));
  printf("[normalize] dest untouched on failure: %.1f %.1f %.1f\n",
         out[0], out[1], out[2]);
  printf("[normalize] valid vector -> %s\n",
         status_name(glm_validate_vec3_normalize_to(dir, out)));
  printf("[normalize] result = %.4f %.4f %.4f (length %.6f)\n",
         out[0], out[1], out[2], glm_vec3_norm(out));

  /* 2. degenerate matrix inversion: error code instead of Inf */
  mat4 singular = GLM_MAT4_ZERO_INIT;
  mat4 view = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 1.0f, 0.0f},
    {1.0f, 2.0f, 3.0f, 1.0f}
  };
  mat4 inv;

  printf("[inverse]   zero matrix  -> %s\n",
         status_name(glm_validate_mat4_inv(singular, inv)));
  printf("[inverse]   valid matrix -> %s (det = %.4f)\n",
         status_name(glm_validate_mat4_inv(view, inv)),
         glm_mat4_det(view));

  /* 3. render-pipeline guard: skip a poisoned transform, keep frame clean */
  vec4 vertex = {1.0f, 1.0f, 1.0f, 1.0f};
  vec4 clip;
  glm_validate_status_t st = glm_validate_mat4_inv(view, inv);
  if (st == GLM_VALIDATE_OK) {
    glm_mat4_mulv(inv, vertex, clip);
    printf("[render]    guarded transform emitted, clip w = %.4f\n", clip[3]);
  } else {
    printf("[render]    transform rejected with %s, frame not polluted\n",
           status_name(st));
  }
  return 0;
}
