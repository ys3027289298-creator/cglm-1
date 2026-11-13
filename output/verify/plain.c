#include <cglm/cglm.h>
void h_vec(const vec3 v, vec3 dest){ glm_vec3_normalize_to(v, dest); }
void h_mat(CGLM_CONST mat4 m, mat4 dest){ glm_mat4_inv(m, dest); }
