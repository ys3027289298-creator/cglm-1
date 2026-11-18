/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "../include/cglm/cglm.h"
#include "../include/cglm/call.h"

CGLM_EXPORT
void
glmc_euler_angles(CGLM_CONST mat4 m, vec3 dest) {
  glm_euler_angles(m, dest);
}

CGLM_EXPORT
void
glmc_euler(const vec3 angles, mat4 dest) {
  glm_euler(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_xyz(const vec3 angles,  mat4 dest) {
  glm_euler_xyz(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_zyx(const vec3 angles,  mat4 dest) {
  glm_euler_zyx(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_zxy(const vec3 angles, mat4 dest) {
  glm_euler_zxy(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_xzy(const vec3 angles, mat4 dest) {
  glm_euler_xzy(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_yzx(const vec3 angles, mat4 dest) {
  glm_euler_yzx(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_yxz(const vec3 angles, mat4 dest) {
  glm_euler_yxz(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_by_order(const vec3 angles, glm_euler_seq axis, mat4 dest) {
  glm_euler_by_order(angles, axis, dest);
}

CGLM_EXPORT
void
glmc_euler_xyz_quat(const vec3 angles, versor dest) {
  glm_euler_xyz_quat(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_xzy_quat(const vec3 angles, versor dest) {
  glm_euler_xzy_quat(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_yxz_quat(const vec3 angles, versor dest) {
  glm_euler_yxz_quat(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_yzx_quat(const vec3 angles, versor dest) {
  glm_euler_yzx_quat(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_zxy_quat(const vec3 angles, versor dest) {
  glm_euler_zxy_quat(angles, dest);
}

CGLM_EXPORT
void
glmc_euler_zyx_quat(const vec3 angles, versor dest) {
  glm_euler_zyx_quat(angles, dest);
}

CGLM_EXPORT
float
glmc_unwrap(float angle, float refAngle) {
  return glm_unwrap(angle, refAngle);
}

CGLM_EXPORT
void
glmc_euler_unwrap(const vec3 angle, const vec3 refAngle, vec3 dest) {
  glm_euler_unwrap(angle, refAngle, dest);
}

CGLM_EXPORT
void
glmc_euler_keyframes_unwrap(vec3 *keyframes, size_t count) {
  glm_euler_keyframes_unwrap(keyframes, count);
}

CGLM_EXPORT
void
glmc_euler_lerp(const vec3 from, const vec3 to, float t, vec3 dest) {
  glm_euler_lerp(from, to, t, dest);
}

CGLM_EXPORT
void
glmc_euler_lerpc(const vec3 from, const vec3 to, float t, vec3 dest) {
  glm_euler_lerpc(from, to, t, dest);
}

CGLM_EXPORT
float
glmc_quat_rel_angle(const versor from, const versor to) {
  return glm_quat_rel_angle(from, to);
}

CGLM_EXPORT
float
glmc_euler_quat_lerp(const vec3 from,
                     const vec3 to,
                     float t,
                     versor dest) {
  return glm_euler_quat_lerp(from, to, t, dest);
}
