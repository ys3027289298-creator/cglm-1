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
   CGLM_INLINE glm_euler_seq glm_euler_order(const int ord[3]);
   CGLM_INLINE void glm_euler_angles(CGLM_CONST mat4 m, vec3 dest);
   CGLM_INLINE void glm_euler(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_xyz(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_zyx(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_zxy(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_xzy(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_yzx(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_yxz(const vec3 angles, mat4 dest);
   CGLM_INLINE void glm_euler_by_order(const vec3    angles,
                                       glm_euler_seq ord,
                                       mat4         dest);
   CGLM_INLINE void glm_euler_xyz_quat(const vec3 angles, versor dest);
   CGLM_INLINE void glm_euler_xzy_quat(const vec3 angles, versor dest);
   CGLM_INLINE void glm_euler_yxz_quat(const vec3 angles, versor dest);
   CGLM_INLINE void glm_euler_yzx_quat(const vec3 angles, versor dest);
   CGLM_INLINE void glm_euler_zxy_quat(const vec3 angles, versor dest);
   CGLM_INLINE void glm_euler_zyx_quat(const vec3 angles, versor dest);
   CGLM_INLINE float glm_unwrap(float angle, float refAngle);
   CGLM_INLINE void glm_euler_unwrap(const vec3 angle,
                                     const vec3 refAngle,
                                     vec3 dest);
   CGLM_INLINE void glm_euler_keyframes_unwrap(vec3 *keyframes, size_t count);
   CGLM_INLINE void glm_euler_lerp(const vec3 from,
                                   const vec3 to,
                                   float t,
                                   vec3 dest);
   CGLM_INLINE void glm_euler_lerpc(const vec3 from,
                                    const vec3 to,
                                    float t,
                                    vec3 dest);
   CGLM_INLINE float glm_quat_rel_angle(const versor from, const versor to);
   CGLM_INLINE float glm_euler_quat_lerp(const vec3 from,
                                         const vec3 to,
                                         float t,
                                         versor dest);
 */

#ifndef cglm_euler_h
#define cglm_euler_h

#include "common.h"
#include "quat.h"
#include "util.h"

#ifdef CGLM_FORCE_LEFT_HANDED
#  include "handed/euler_to_quat_lh.h"
#else
#  include "handed/euler_to_quat_rh.h"
#endif


#ifndef CGLM_CLIPSPACE_INCLUDE_ALL
#  if CGLM_CONFIG_CLIP_CONTROL == CGLM_CLIP_CONTROL_LH_ZO
#    include "clipspace/ortho_lh_zo.h"
#    include "clipspace/persp_lh_zo.h"
#    include "clipspace/view_lh_zo.h"
#  elif CGLM_CONFIG_CLIP_CONTROL == CGLM_CLIP_CONTROL_LH_NO
#    include "clipspace/ortho_lh_no.h"
#    include "clipspace/persp_lh_no.h"
#    include "clipspace/view_lh_no.h"
#  elif CGLM_CONFIG_CLIP_CONTROL == CGLM_CLIP_CONTROL_RH_ZO
#    include "clipspace/ortho_rh_zo.h"
#    include "clipspace/persp_rh_zo.h"
#    include "clipspace/view_rh_zo.h"
#  elif CGLM_CONFIG_CLIP_CONTROL == CGLM_CLIP_CONTROL_RH_NO
#    include "clipspace/ortho_rh_no.h"
#    include "clipspace/persp_rh_no.h"
#    include "clipspace/view_rh_no.h"
#  endif
#else
#  include "clipspace/ortho_lh_zo.h"
#  include "clipspace/persp_lh_zo.h"
#  include "clipspace/ortho_lh_no.h"
#  include "clipspace/persp_lh_no.h"
#  include "clipspace/ortho_rh_zo.h"
#  include "clipspace/persp_rh_zo.h"
#  include "clipspace/ortho_rh_no.h"
#  include "clipspace/persp_rh_no.h"
#  include "clipspace/view_lh_zo.h"
#  include "clipspace/view_lh_no.h"
#  include "clipspace/view_rh_zo.h"
#  include "clipspace/view_rh_no.h"
#endif

CGLM_INLINE
glm_euler_seq
glm_euler_order(const int ord[3]) {
  return (glm_euler_seq)(ord[0] << 0 | ord[1] << 2 | ord[2] << 4);
}

/*!
 * @brief extract euler angles (in radians) using xyz order
 *
 * @param[in]  m    affine transform
 * @param[out] dest angles vector [x, y, z]
 */
CGLM_INLINE
void
glm_euler_angles(CGLM_CONST mat4 m, vec3 dest) {
  float m00, m01, m10, m11, m20, m21, m22;
  float thetaX, thetaY, thetaZ;

  m00 = m[0][0];  m10 = m[1][0];  m20 = m[2][0];
  m01 = m[0][1];  m11 = m[1][1];  m21 = m[2][1];
                                  m22 = m[2][2];

  if (m20 < 1.0f) {
    if (m20 > -1.0f) {
      thetaY = asinf(m20);
      thetaX = atan2f(-m21, m22);
      thetaZ = atan2f(-m10, m00);
    } else { /* m20 == -1 */
      /* Not a unique solution */
      thetaY = -GLM_PI_2f;
      thetaX = -atan2f(m01, m11);
      thetaZ =  0.0f;
    }
  } else { /* m20 == +1 */
    thetaY = GLM_PI_2f;
    thetaX = atan2f(m01, m11);
    thetaZ = 0.0f;
  }

  dest[0] = thetaX;
  dest[1] = thetaY;
  dest[2] = thetaZ;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_xyz(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz, czsx, cxcz, sysz;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  czsx = cz * sx;
  cxcz = cx * cz;
  sysz = sy * sz;

  dest[0][0] =  cy * cz;
  dest[0][1] =  czsx * sy + cx * sz;
  dest[0][2] = -cxcz * sy + sx * sz;
  dest[1][0] = -cy * sz;
  dest[1][1] =  cxcz - sx * sysz;
  dest[1][2] =  czsx + cx * sysz;
  dest[2][0] =  sy;
  dest[2][1] = -cy * sx;
  dest[2][2] =  cx * cy;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler(const vec3 angles, mat4 dest) {
  glm_euler_xyz(angles, dest);
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_xzy(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
  sx, sy, sz, sxsy, cysx, cxsy, cxcy;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  sxsy = sx * sy;
  cysx = cy * sx;
  cxsy = cx * sy;
  cxcy = cx * cy;

  dest[0][0] =  cy * cz;
  dest[0][1] =  sxsy + cxcy * sz;
  dest[0][2] = -cxsy + cysx * sz;
  dest[1][0] = -sz;
  dest[1][1] =  cx * cz;
  dest[1][2] =  cz * sx;
  dest[2][0] =  cz * sy;
  dest[2][1] = -cysx + cxsy * sz;
  dest[2][2] =  cxcy + sxsy * sz;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_yxz(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz, cycz, sysz, czsy, cysz;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  cycz = cy * cz;
  sysz = sy * sz;
  czsy = cz * sy;
  cysz = cy * sz;

  dest[0][0] =  cycz + sx * sysz;
  dest[0][1] =  cx * sz;
  dest[0][2] = -czsy + cysz * sx;
  dest[1][0] = -cysz + czsy * sx;
  dest[1][1] =  cx * cz;
  dest[1][2] =  cycz * sx + sysz;
  dest[2][0] =  cx * sy;
  dest[2][1] = -sx;
  dest[2][2] =  cx * cy;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_yzx(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz, sxsy, cxcy, cysx, cxsy;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  sxsy = sx * sy;
  cxcy = cx * cy;
  cysx = cy * sx;
  cxsy = cx * sy;

  dest[0][0] =  cy * cz;
  dest[0][1] =  sz;
  dest[0][2] = -cz * sy;
  dest[1][0] =  sxsy - cxcy * sz;
  dest[1][1] =  cx * cz;
  dest[1][2] =  cysx + cxsy * sz;
  dest[2][0] =  cxsy + cysx * sz;
  dest[2][1] = -cz * sx;
  dest[2][2] =  cxcy - sxsy * sz;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_zxy(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz, cycz, sxsy, cysz;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  cycz = cy * cz;
  sxsy = sx * sy;
  cysz = cy * sz;

  dest[0][0] =  cycz - sxsy * sz;
  dest[0][1] =  cz * sxsy + cysz;
  dest[0][2] = -cx * sy;
  dest[1][0] = -cx * sz;
  dest[1][1] =  cx * cz;
  dest[1][2] =  sx;
  dest[2][0] =  cz * sy + cysz * sx;
  dest[2][1] = -cycz * sx + sy * sz;
  dest[2][2] =  cx * cy;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_zyx(const vec3 angles, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz, czsx, cxcz, sysz;

  sx   = sinf(angles[0]); cx = cosf(angles[0]);
  sy   = sinf(angles[1]); cy = cosf(angles[1]);
  sz   = sinf(angles[2]); cz = cosf(angles[2]);

  czsx = cz * sx;
  cxcz = cx * cz;
  sysz = sy * sz;

  dest[0][0] =  cy * cz;
  dest[0][1] =  cy * sz;
  dest[0][2] = -sy;
  dest[1][0] =  czsx * sy - cx * sz;
  dest[1][1] =  cxcz + sx * sysz;
  dest[1][2] =  cy * sx;
  dest[2][0] =  cxcz * sy + sx * sz;
  dest[2][1] = -czsx + cx * sysz;
  dest[2][2] =  cx * cy;
  dest[0][3] =  0.0f;
  dest[1][3] =  0.0f;
  dest[2][3] =  0.0f;
  dest[3][0] =  0.0f;
  dest[3][1] =  0.0f;
  dest[3][2] =  0.0f;
  dest[3][3] =  1.0f;
}

/*!
 * @brief build rotation matrix from euler angles
 *
 * @param[in]  angles angles as vector [Xangle, Yangle, Zangle]
 * @param[in]  ord    euler order
 * @param[out] dest   rotation matrix
 */
CGLM_INLINE
void
glm_euler_by_order(const vec3 angles, glm_euler_seq ord, mat4 dest) {
  float cx, cy, cz,
        sx, sy, sz;

  float cycz, cysz, cysx, cxcy,
        czsy, cxcz, czsx, cxsz,
        sysz;

  sx = sinf(angles[0]); cx = cosf(angles[0]);
  sy = sinf(angles[1]); cy = cosf(angles[1]);
  sz = sinf(angles[2]); cz = cosf(angles[2]);

  cycz = cy * cz; cysz = cy * sz;
  cysx = cy * sx; cxcy = cx * cy;
  czsy = cz * sy; cxcz = cx * cz;
  czsx = cz * sx; cxsz = cx * sz;
  sysz = sy * sz;

  switch (ord) {
    case GLM_EULER_XZY:
      dest[0][0] =  cycz;
      dest[0][1] =  sx * sy + cx * cysz;
      dest[0][2] = -cx * sy + cysx * sz;
      dest[1][0] = -sz;
      dest[1][1] =  cxcz;
      dest[1][2] =  czsx;
      dest[2][0] =  czsy;
      dest[2][1] = -cysx + cx * sysz;
      dest[2][2] =  cxcy + sx * sysz;
      break;
    case GLM_EULER_XYZ:
      dest[0][0] =  cycz;
      dest[0][1] =  czsx * sy + cxsz;
      dest[0][2] = -cx * czsy + sx * sz;
      dest[1][0] = -cysz;
      dest[1][1] =  cxcz - sx * sysz;
      dest[1][2] =  czsx + cx * sysz;
      dest[2][0] =  sy;
      dest[2][1] = -cysx;
      dest[2][2] =  cxcy;
      break;
    case GLM_EULER_YXZ:
      dest[0][0] =  cycz + sx * sysz;
      dest[0][1] =  cxsz;
      dest[0][2] = -czsy + cysx * sz;
      dest[1][0] =  czsx * sy - cysz;
      dest[1][1] =  cxcz;
      dest[1][2] =  cycz * sx + sysz;
      dest[2][0] =  cx * sy;
      dest[2][1] = -sx;
      dest[2][2] =  cxcy;
      break;
    case GLM_EULER_YZX:
      dest[0][0] =  cycz;
      dest[0][1] =  sz;
      dest[0][2] = -czsy;
      dest[1][0] =  sx * sy - cx * cysz;
      dest[1][1] =  cxcz;
      dest[1][2] =  cysx + cx * sysz;
      dest[2][0] =  cx * sy + cysx * sz;
      dest[2][1] = -czsx;
      dest[2][2] =  cxcy - sx * sysz;
      break;
    case GLM_EULER_ZXY:
      dest[0][0] =  cycz - sx * sysz;
      dest[0][1] =  czsx * sy + cysz;
      dest[0][2] = -cx * sy;
      dest[1][0] = -cxsz;
      dest[1][1] =  cxcz;
      dest[1][2] =  sx;
      dest[2][0] =  czsy + cysx * sz;
      dest[2][1] = -cycz * sx + sysz;
      dest[2][2] =  cxcy;
      break;
    case GLM_EULER_ZYX:
      dest[0][0] =  cycz;
      dest[0][1] =  cysz;
      dest[0][2] = -sy;
      dest[1][0] =  czsx * sy - cxsz;
      dest[1][1] =  cxcz + sx * sysz;
      dest[1][2] =  cysx;
      dest[2][0] =  cx * czsy + sx * sz;
      dest[2][1] = -czsx + cx * sysz;
      dest[2][2] =  cxcy;
      break;
  }

  dest[0][3] = 0.0f;
  dest[1][3] = 0.0f;
  dest[2][3] = 0.0f;
  dest[3][0] = 0.0f;
  dest[3][1] = 0.0f;
  dest[3][2] = 0.0f;
  dest[3][3] = 1.0f;
}


/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in x y z order (roll pitch yaw)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_xyz_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_xyz_quat_lh(angles, dest);
#else
  glm_euler_xyz_quat_rh(angles, dest);
#endif
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in x z y order (roll yaw pitch)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_xzy_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_xzy_quat_lh(angles, dest);
#else
  glm_euler_xzy_quat_rh(angles, dest);
#endif
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in y x z order (pitch roll yaw)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_yxz_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_yxz_quat_lh(angles, dest);
#else
  glm_euler_yxz_quat_rh(angles, dest);
#endif
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in y z x order (pitch yaw roll)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_yzx_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_yzx_quat_lh(angles, dest);
#else
  glm_euler_yzx_quat_rh(angles, dest);
#endif
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in z x y order (yaw roll pitch)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_zxy_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_zxy_quat_lh(angles, dest);
#else
  glm_euler_zxy_quat_rh(angles, dest);
#endif
}

/*!
 * @brief creates NEW quaternion using rotation angles and does
 *        rotations in z y x order (yaw pitch roll)
 * 
 * @param[in]   angles angles x y z (radians)
 * @param[out]  dest   quaternion
 */
CGLM_INLINE
void
glm_euler_zyx_quat(const vec3 angles, versor dest) {
#ifdef CGLM_FORCE_LEFT_HANDED
  glm_euler_zyx_quat_lh(angles, dest);
#else
  glm_euler_zyx_quat_rh(angles, dest);
#endif
}

/*!
 * @brief unwrap an angle so that it is the closest (shortest arc)
 *        representative of @p angle around the reference angle
 *
 * The result is @p angle shifted by an integer multiple of 2*pi until
 * the distance to @p refAngle is at most pi. This resolves the
 * 359 deg -> 1 deg style discontinuity: instead of interpolating the
 * long way around (-358 deg), the angle becomes 361 deg and the
 * interpolated motion spans only +2 deg.
 *
 * @param[in]  angle    angle in radians (any range, may wrap)
 * @param[in]  refAngle reference angle in radians (any range)
 * @return unwrapped angle, within pi of @p refAngle
 */
CGLM_INLINE
float
glm_unwrap(float angle, float refAngle) {
  return angle - GLM_TAUf * roundf((angle - refAngle) * GLM_1_TAUf);
}

/*!
 * @brief unwrap each component of an euler angle triple onto the
 *        shortest arc around a reference triple
 *
 * @param[in]  angle    angles [x, y, z] in radians (may wrap)
 * @param[in]  refAngle reference angles [x, y, z] in radians
 * @param[out] dest     unwrapped angles, each within pi of refAngle
 */
CGLM_INLINE
void
glm_euler_unwrap(const vec3 angle, const vec3 refAngle, vec3 dest) {
  dest[0] = glm_unwrap(angle[0], refAngle[0]);
  dest[1] = glm_unwrap(angle[1], refAngle[1]);
  dest[2] = glm_unwrap(angle[2], refAngle[2]);
}

/*!
 * @brief cumulatively unwrap an euler keyframe chain in place
 *
 * Each keyframe is shifted per component by whole 2*pi turns so that
 * consecutive keyframes always differ by at most pi, i.e. the chain
 * follows the shortest arc. Multi-turn rotations are preserved through
 * the accumulated unwrap, so a chain spinning several turns keeps the
 * total winding instead of snapping back at every wrap.
 *
 * @param[in, out] keyframes euler keyframes [x, y, z] in radians
 * @param[in]      count     number of keyframes
 */
CGLM_INLINE
void
glm_euler_keyframes_unwrap(vec3 *keyframes, size_t count) {
  size_t i;

  if (count < 2)
    return;

  for (i = 1; i < count; i++) {
    glm_euler_unwrap(keyframes[i], keyframes[i - 1], keyframes[i]);
  }
}

/*!
 * @brief linearly interpolate two euler angle triples along the
 *        shortest arc
 *
 * The @p to angle is unwrapped onto the shortest arc around @p from
 * before the per-component lerp, so an interpolation across the
 * +/-pi boundary sweeps the small angle instead of almost a full
 * revolution. The unwrapped target is also returned implicitly through
 * @p dest at t == 1, making the function safe to call repeatedly while
 * accumulating keyframes.
 *
 * @param[in]  from angles [x, y, z] in radians
 * @param[in]  to   angles [x, y, z] in radians
 * @param[in]  t    interpolant, 0 returns from, 1 returns unwrapped to
 * @param[out] dest interpolated angles
 */
CGLM_INLINE
void
glm_euler_lerp(const vec3 from, const vec3 to, float t, vec3 dest) {
  CGLM_ALIGN(8) vec3 target;

  glm_euler_unwrap(to, from, target);

  dest[0] = glm_lerp(from[0], target[0], t);
  dest[1] = glm_lerp(from[1], target[1], t);
  dest[2] = glm_lerp(from[2], target[2], t);
}

/*!
 * @brief clamped variant of glm_euler_lerp(), @p t is clamped to [0, 1]
 *
 * @param[in]  from angles [x, y, z] in radians
 * @param[in]  to   angles [x, y, z] in radians
 * @param[in]  t    interpolant clamped between 0 and 1
 * @param[out] dest interpolated angles
 */
CGLM_INLINE
void
glm_euler_lerpc(const vec3 from, const vec3 to, float t, vec3 dest) {
  glm_euler_lerp(from, to, glm_clamp_zo(t), dest);
}

/*!
 * @brief shortest rotation angle (geodesic distance) between two
 *        unit quaternions, in radians and within [0, pi]
 *
 * @param[in] from quaternion
 * @param[in] to   quaternion
 * @return angle of the relative rotation in radians
 */
CGLM_INLINE
float
glm_quat_rel_angle(const versor from, const versor to) {
  float d;

  d = glm_quat_dot(from, to);

  if (d < 0.0f)
    d = -d;

  /* clamp guards the acos domain against float round-off */
  if (d > 1.0f)
    d = 1.0f;

  return 2.0f * acosf(d);
}

/*!
 * @brief dual-path shortest-arc interpolation between euler keyframes
 *
 * Euler path: the target angles are unwrapped to the shortest arc and
 * lerped, then converted to a quaternion. Quaternion path: the two
 * keyframes are converted directly and slerped on the short way.
 *
 * The two paths always agree at the endpoints (t = 0 and t = 1). For
 * interior interpolants they are different, equally valid curves:
 * component-wise euler lerp is linear in angle while slerp has constant
 * angular velocity, and for simultaneous multi-axis motion the gap can
 * be large. The returned geodesic angle lets a caller cross-check the
 * paths; prefer the quaternion dot product for a numerically stable
 * equivalence test near the identity.
 *
 * @param[in]  from angles [x, y, z] in radians
 * @param[in]  to   angles [x, y, z] in radians
 * @param[in]  t    interpolant, 0 returns from, 1 returns to
 * @param[out] dest interpolated orientation quaternion (euler path)
 * @return geodesic angle in radians between the euler-path and
 *         quaternion-path results
 */
CGLM_INLINE
float
glm_euler_quat_lerp(const vec3 from, const vec3 to, float t, versor dest) {
  CGLM_ALIGN(16) versor qFrom, qTo, qSlerp;
  CGLM_ALIGN(8)  vec3   angles;

  /* euler path: shortest-arc unwrap, lerp, then convert to quaternion */
  glm_euler_lerp(from, to, t, angles);
  glm_euler_xyz_quat(angles, dest);

  /* quaternion path: slerp always takes the short rotation itself */
  glm_euler_xyz_quat(from, qFrom);
  glm_euler_xyz_quat(to,   qTo);
  glm_quat_slerp(qFrom, qTo, t, qSlerp);

  return glm_quat_rel_angle(dest, qSlerp);
}


#endif /* cglm_euler_h */
