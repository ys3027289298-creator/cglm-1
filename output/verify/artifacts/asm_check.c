#include <cglm/validate.h>

float f_plain(const vec3 v, vec3 dest) {
  glm_vec3_normalize_to(v, dest);
  return dest[0];
}

float f_checked(const vec3 v, vec3 dest) {
  glm_validate_vec3_normalize_to(v, dest);
  return dest[0];
}

void g_plain(CGLM_CONST mat4 m, mat4 dest) {
  glm_mat4_inv(m, dest);
}

void g_checked(CGLM_CONST mat4 m, mat4 dest) {
  glm_validate_mat4_inv(m, dest);
}
