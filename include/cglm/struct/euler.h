/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

/*
 NOTE:
  angles must be passed as [X-Angle, Y-Angle, Z-angle] order
  For instance you don't pass angles as [Z-Angle, X-Angle, Y-angle] to
  glm_euler_zxy function, All RELATED functions accept angles same order
  which is [X, Y, Z].
 */

/*
 Types:
   enum glm_euler_seq

 Functions:
   CGLM_INLINE vec3s glms_euler_angles(mat4s m)
   CGLM_INLINE mat4s glms_euler_xyz(vec3s angles)
   CGLM_INLINE mat4s glms_euler_xzy(vec3s angles)
   CGLM_INLINE mat4s glms_euler_yxz(vec3s angles)
   CGLM_INLINE mat4s glms_euler_yzx(vec3s angles)
   CGLM_INLINE mat4s glms_euler_zxy(vec3s angles)
   CGLM_INLINE mat4s glms_euler_zyx(vec3s angles)
   CGLM_INLINE mat4s glms_euler_by_order(vec3s angles, glm_euler_seq ord)
   CGLM_INLINE versors glms_euler_xyz_quat(vec3s angles)
   CGLM_INLINE versors glms_euler_xzy_quat(vec3s angles)
   CGLM_INLINE versors glms_euler_yxz_quat(vec3s angles)
   CGLM_INLINE versors glms_euler_yzx_quat(vec3s angles)
   CGLM_INLINE versors glms_euler_zxy_quat(vec3s angles)
   CGLM_INLINE versors glms_euler_zyx_quat(vec3s angles)
   CGLM_INLINE float   glms_unwrap(float angle, float refAngle)
   CGLM_INLINE vec3s   glms_euler_unwrap(vec3s angle, vec3s refAngle)
   CGLM_INLINE void    glms_euler_keyframes_unwrap(vec3s *keyframes, size_t count)
   CGLM_INLINE vec3s   glms_euler_lerp(vec3s from, vec3s to, float t)
   CGLM_INLINE vec3s   glms_euler_lerpc(vec3s from, vec3s to, float t)
   CGLM_INLINE versors glms_euler_quat_lerp(vec3s from,
                                            vec3s to,
                                            float t,
                                            float *relAngle)
 */

#ifndef cglms_euler_h
#define cglms_euler_h

#include "../common.h"
#include "../types-struct.h"
#include "../euler.h"

/*!
 * @brief extract euler angles (in radians) using xyz order
 *
 * @param[in]  m    affine transform
 * @returns angles vector [x, y, z]
 */
CGLM_INLINE
vec3s
glms_euler_angles(mat4s m) {
  vec3s dest;
  glm_euler_angles(m.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_xyz(vec3s angles) {
  mat4s dest;
  glm_euler_xyz(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_xzy(vec3s angles) {
  mat4s dest;
  glm_euler_xzy(angles.raw, dest.raw);
  return dest;
}


/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_yxz(vec3s angles) {
  mat4s dest;
  glm_euler_yxz(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_yzx(vec3s angles) {
  mat4s dest;
  glm_euler_yzx(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_zxy(vec3s angles) {
  mat4s dest;
  glm_euler_zxy(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_zyx(vec3s angles) {
  mat4s dest;
  glm_euler_zyx(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[in]  ord    euler order
 * @returns rotation matrix
 */
CGLM_INLINE
mat4s
glms_euler_by_order(vec3s angles, glm_euler_seq ord) {
  mat4s dest;
  glm_euler_by_order(angles.raw, ord, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in x y z order (roll pitch yaw)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_xyz_quat(vec3s angles) {
  versors dest;
  glm_euler_xyz_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in x z y order (roll yaw pitch)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_xzy_quat(vec3s angles) {
  versors dest;
  glm_euler_xzy_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in y x z order (pitch roll yaw)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_yxz_quat(vec3s angles) {
  versors dest;
  glm_euler_yxz_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in y z x order (pitch yaw roll)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_yzx_quat(vec3s angles) {
  versors dest;
  glm_euler_yzx_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in z x y order (yaw roll pitch)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_zxy_quat(vec3s angles) {
  versors dest;
  glm_euler_zxy_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in z y x order (yaw pitch roll)
 * 
 * @param[in]   angles angles x y z (radians)
 * @returns quaternion
 */
CGLM_INLINE
versors
glms_euler_zyx_quat(vec3s angles) {
  versors dest;
  glm_euler_zyx_quat(angles.raw, dest.raw);
  return dest;
}

/*!
 * @brief unwrap an angle onto the shortest arc around a reference angle
 *
 * @param[in] angle    angle in radians (any range, may wrap)
 * @param[in] refAngle reference angle in radians
 * @returns unwrapped angle, within pi of refAngle
 */
CGLM_INLINE
float
glms_unwrap(float angle, float refAngle) {
  return glm_unwrap(angle, refAngle);
}

/*!
 * @brief unwrap each euler component onto the shortest arc
 *        around a reference triple
 *
 * @param[in] angle    angles [x, y, z] in radians
 * @param[in] refAngle reference angles [x, y, z] in radians
 * @returns unwrapped angles
 */
CGLM_INLINE
vec3s
glms_euler_unwrap(vec3s angle, vec3s refAngle) {
  vec3s dest;
  glm_euler_unwrap(angle.raw, refAngle.raw, dest.raw);
  return dest;
}

/*!
 * @brief cumulatively unwrap an euler keyframe chain in place
 *
 * @param[in, out] keyframes euler keyframes in radians
 * @param[in]      count     number of keyframes
 */
CGLM_INLINE
void
glms_euler_keyframes_unwrap(vec3s *keyframes, size_t count) {
  size_t i;

  if (count < 2)
    return;

  for (i = 1; i < count; i++) {
    glm_euler_unwrap(keyframes[i].raw,
                     keyframes[i - 1].raw,
                     keyframes[i].raw);
  }
}

/*!
 * @brief shortest-arc linear interpolation between euler angle triples
 *
 * @param[in] from angles [x, y, z] in radians
 * @param[in] to   angles [x, y, z] in radians
 * @param[in] t    interpolant, 0 returns from, 1 returns unwrapped to
 * @returns interpolated angles
 */
CGLM_INLINE
vec3s
glms_euler_lerp(vec3s from, vec3s to, float t) {
  vec3s dest;
  glm_euler_lerp(from.raw, to.raw, t, dest.raw);
  return dest;
}

/*!
 * @brief clamped variant of glms_euler_lerp()
 *
 * @param[in] from angles [x, y, z] in radians
 * @param[in] to   angles [x, y, z] in radians
 * @param[in] t    interpolant clamped between 0 and 1
 * @returns interpolated angles
 */
CGLM_INLINE
vec3s
glms_euler_lerpc(vec3s from, vec3s to, float t) {
  vec3s dest;
  glm_euler_lerpc(from.raw, to.raw, t, dest.raw);
  return dest;
}

/*!
 * @brief dual-path shortest-arc interpolation between euler keyframes
 *
 * @param[in]  from     angles [x, y, z] in radians
 * @param[in]  to       angles [x, y, z] in radians
 * @param[in]  t        interpolant
 * @param[out] relAngle geodesic angle between the euler-path and
 *                      quaternion-path results (may be NULL)
 * @returns interpolated orientation quaternion (euler path)
 */
CGLM_INLINE
versors
glms_euler_quat_lerp(vec3s from, vec3s to, float t, float *relAngle) {
  versors dest;
  float angle;

  angle = glm_euler_quat_lerp(from.raw, to.raw, t, dest.raw);

  if (relAngle)
    *relAngle = angle;

  return dest;
}


#endif /* cglms_euler_h */
