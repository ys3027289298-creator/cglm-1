/*
 * Copyright (c), Recep Aslantas.
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "test_common.h"

#include <time.h>

#ifndef CGLM_TEST_EULER_INTERP_ONCE
#define CGLM_TEST_EULER_INTERP_ONCE

/* deterministic PRNG so the randomized chains are reproducible */
static
float
test_euler_interp_rand(uint64_t *state) {
  /* xorshift64* */
  uint64_t x;
  x       = *state;
  x      ^= x >> 12;
  x      ^= x << 25;
  x      ^= x >> 27;
  *state  = x;
  return (float)((double)((x * UINT64_C(2685821657736338717)) >> 40)
                 / 16777216.0); /* 2^24 */
}

/* shortest signed difference from `from` to `to`, within (-pi, pi] */
static
float
test_short_arc_delta(float from, float to) {
  float d;
  d = fmodf(to - from, GLM_TAUf);
  if (d > GLM_PIf)
    d -= GLM_TAUf;
  else if (d < -GLM_PIf)
    d += GLM_TAUf;
  return d;
}

/*
 * Robust rotation equivalence: |dot| ~ 1 means the two unit
 * quaternions encode the same rotation (q and -q are identical).
 * A dot-based bound stays accurate near identity, where an
 * acos-based geodesic angle suffers catastrophic condition loss.
 * 1 - |dot| <= eps2 corresponds to a relative angle of ~2*sqrt(eps2).
 */
static
bool
test_quat_same_rotation(const versor a, const versor b, float eps2) {
  float d;
  d = glm_quat_dot(a, b);
  if (d < 0.0f)
    d = -d;
  return (1.0f - d) <= eps2;
}

#endif /* CGLM_TEST_EULER_INTERP_ONCE */

TEST_IMPL(GLM_PREFIX, unwrap) {
  /* 359 deg -> 1 deg must resolve to +2 deg, not a -358 deg sweep */
  ASSERT(test_eq_th(GLM(unwrap)(glm_rad(1.0f), glm_rad(359.0f)),
                    glm_rad(361.0f), 1e-6f))
  ASSERT(test_eq_th(GLM(unwrap)(glm_rad(359.0f), glm_rad(1.0f)),
                    glm_rad(-1.0f), 1e-6f))

  ASSERT(test_eq_th(GLM(unwrap)(glm_rad(-179.0f), glm_rad(179.0f)),
                    glm_rad(181.0f), 1e-6f))
  ASSERT(test_eq_th(GLM(unwrap)(0.25f, 0.0f), 0.25f, 1e-6f))

  /*
   * At the exact half-turn boundary +pi and -pi name the same angle;
   * round-half-away-from-zero may return either representative, so
   * assert the difference from the input is a whole number of turns.
   */
  ASSERT(test_eq_th(fmodf(GLM(unwrap)(GLM_PIf, 0.0f) - GLM_PIf,
                          GLM_TAUf),
                    0.0f, 1e-6f))
  ASSERT(test_eq_th(fmodf(GLM(unwrap)(-GLM_PIf, 0.0f) + GLM_PIf,
                          GLM_TAUf),
                    0.0f, 1e-6f))

  /* already close angles are left untouched */
  ASSERT(test_eq_th(GLM(unwrap)(1.23456f, 1.0f), 1.23456f, 1e-6f))

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_unwrap) {
  CGLM_ALIGN(8) vec3 angle, ref, dest;

  angle[0] = glm_rad(1.0f);
  angle[1] = glm_rad(359.0f);
  angle[2] = glm_rad(-179.0f);

  ref[0] = glm_rad(359.0f);
  ref[1] = glm_rad(1.0f);
  ref[2] = glm_rad(179.0f);

  GLM(euler_unwrap)(angle, ref, dest);

  ASSERT(test_eq_th(dest[0], glm_rad(361.0f), 1e-6f))
  ASSERT(test_eq_th(dest[1], glm_rad(-1.0f), 1e-6f))
  ASSERT(test_eq_th(dest[2], glm_rad(181.0f), 1e-6f))

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_keyframes_unwrap) {
  /*
   * 1000 random cross-zero keyframe chains. Every raw keyframe lives in
   * [-pi, pi], so without unwrapping most neighbors would appear to
   * jump the wrong way. After the cumulative unwrap:
   *  - every consecutive delta equals the raw shortest-arc delta
   *    (no reverse jump), and |delta| <= pi;
   *  - per-segment interpolation is monotonic on the short arc.
   */
  enum { CHAINS = 1000, FRAMES = 32 };

  vec3     frames[FRAMES];
  vec3     raw[FRAMES];
  uint64_t state;
  int      c, i, axis;

  state = UINT64_C(0x9e3779b97f4a7c15);

  for (c = 0; c < CHAINS; c++) {
    for (i = 0; i < FRAMES; i++) {
      for (axis = 0; axis < 3; axis++)
        raw[i][axis] = frames[i][axis] =
          (test_euler_interp_rand(&state) * 2.0f - 1.0f) * GLM_PIf;
    }

    GLM(euler_keyframes_unwrap)(frames, FRAMES);

    for (i = 1; i < FRAMES; i++) {
      for (axis = 0; axis < 3; axis++) {
        float expect, got;

        expect = test_short_arc_delta(raw[i - 1][axis], raw[i][axis]);
        got    = frames[i][axis] - frames[i - 1][axis];

        ASSERT(test_eq_th(got, expect, 1e-5f))
        ASSERT(fabsf(got) <= GLM_PIf + 1e-6f)
      }
    }

    /* midpoint of each segment must sit on the short arc, in order */
    for (i = 1; i < FRAMES; i++) {
      CGLM_ALIGN(8) vec3 mid;

      GLM(euler_lerp)(frames[i - 1], frames[i], 0.5f, mid);

      for (axis = 0; axis < 3; axis++) {
        float expect;
        expect = frames[i - 1][axis]
               + 0.5f * (frames[i][axis] - frames[i - 1][axis]);
        ASSERT(test_eq_th(mid[axis], expect, 1e-6f))
      }
    }
  }

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_unwrap_multiturn) {
  /*
   * Deliberately spin several full turns using short per-step moves
   * expressed as wrapped angles. The cumulative unwrap must preserve
   * total winding and land on the analytic endpoint within 1e-4 rad,
   * which is tighter than the float error of naively re-wrapping at
   * every frame.
   */
  enum { STEPS = 24, TURNS = 6 };

  vec3  frames[STEPS + 1];
  float stepAngle, target;
  int   i, axis;

  /* one step above the float precision cliff at this total winding */
  stepAngle = ((float)TURNS * GLM_TAUf) / (float)STEPS;
  target    = (float)TURNS * GLM_TAUf;

  for (axis = 0; axis < 3; axis++) {
    for (i = 0; i <= STEPS; i++) {
      float total, wrapped;

      total   = (float)i * stepAngle;
      wrapped = fmodf(total + GLM_PIf, GLM_TAUf) - GLM_PIf;
      frames[i][0] = frames[i][1] = frames[i][2] = wrapped;
    }

    GLM(euler_keyframes_unwrap)(frames, STEPS + 1);

    ASSERT(fabsf(frames[STEPS][axis] - target) < 1e-4f)

    /* winding is retained, not folded back into [-pi, pi] */
    ASSERT(frames[STEPS][axis] > GLM_TAUf * (float)(TURNS - 1))

    /* every intermediate frame matches its analytic unwrapped value */
    for (i = 0; i <= STEPS; i++) {
      ASSERT(test_eq_th(frames[i][axis], (float)i * stepAngle, 1e-5f))
    }
  }

  /* same guarantee in the reverse (negative) direction */
  for (i = 0; i <= STEPS; i++) {
    float total, wrapped;

    total   = -(float)i * stepAngle;
    wrapped = fmodf(total + GLM_PIf, GLM_TAUf) - GLM_PIf;
    frames[i][0] = frames[i][1] = frames[i][2] = wrapped;
  }

  GLM(euler_keyframes_unwrap)(frames, STEPS + 1);

  for (axis = 0; axis < 3; axis++)
    ASSERT(fabsf(frames[STEPS][axis] + target) < 1e-4f)

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_lerp_short_arc) {
  CGLM_ALIGN(8) vec3 from, to, mid, dest;

  /* the motivating case: 359 deg -> 1 deg sweeps only +2 deg */
  from[0] = glm_rad(359.0f);
  from[1] = 0.0f;
  from[2] = 0.0f;

  to[0] = glm_rad(1.0f);
  to[1] = 0.0f;
  to[2] = 0.0f;

  GLM(euler_lerp)(from, to, 0.0f, dest);
  ASSERT(test_eq_th(dest[0], from[0], 1e-6f))

  GLM(euler_lerp)(from, to, 1.0f, dest);
  ASSERT(test_eq_th(dest[0], glm_rad(361.0f), 1e-6f))

  GLM(euler_lerp)(from, to, 0.5f, mid);
  ASSERT(test_eq_th(mid[0], glm_rad(360.0f), 1e-6f))

  /* the midpoint is 2 deg from either end, never ~180 deg */
  ASSERT(fabsf(mid[0] - from[0]) < glm_rad(5.0f))

  /* clamped variant rejects out-of-range interpolants */
  GLM(euler_lerpc)(from, to, -2.0f, dest);
  ASSERT(test_eq_th(dest[0], from[0], 1e-6f))
  GLM(euler_lerpc)(from, to, 2.0f, dest);
  ASSERT(test_eq_th(dest[0], glm_rad(361.0f), 1e-6f))

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_quat_lerp_crosscheck) {
  /*
   * Dual-path cross check. The euler path is linear-in-angle while the
   * quaternion path is slerp (constant angular velocity), so for a
   * finite rotation they are two different, equally valid curves and
   * only coincide at the endpoints; interior samples are checked with a
   * bound that tolerates that curve-style difference. Endpoints must
   * agree to round-off, which is verified with the numerically stable
   * quaternion dot product rather than an acos geodesic angle (the
   * latter loses all precision near the identity).
   */
  CGLM_ALIGN(16) versor qEuler, qSlerp;
  CGLM_ALIGN(8)  vec3   from, to, angles;
  uint64_t state;
  float    rel;
  int      i;

  /* endpoints: euler path and slerp path encode the same rotation */
  from[0] = glm_rad(359.0f); from[1] = 0.0f; from[2] = 0.0f;
  to[0]   = glm_rad(1.0f);   to[1]   = 0.0f; to[2]   = 0.0f;

  rel = GLM(euler_quat_lerp)(from, to, 0.0f, qEuler);
  GLM(euler_xyz_quat)(from, qSlerp);
  ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))
  ASSERT(rel >= 0.0f && rel <= GLM_PIf)

  rel = GLM(euler_quat_lerp)(from, to, 1.0f, qEuler);
  GLM(euler_xyz_quat)(to, qSlerp);
  ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))
  ASSERT(rel >= 0.0f && rel <= GLM_PIf)

  /*
   * Single-axis cross-zero motion, every sample independent of the
   * others. Interior euler-linear vs slerp gap stays small (well under
   * a degree of arc), and each output is a unit quaternion.
   */
  for (i = 0; i < 64; i++) {
    float t;
    t = ((float)i + 1.0f) / 64.0f;

    from[0] = 0.0f;
    from[1] = 0.0f;
    from[2] = 0.0f;

    to[0] = glm_rad((i % 2) ? 1.0f : 359.0f);
    to[1] = 0.0f;
    to[2] = 0.0f;

    GLM(euler_quat_lerp)(from, to, t, qEuler);

    GLM(euler_lerp)(from, to, t, angles);
    GLM(euler_xyz_quat)(angles, qSlerp);
    /* the two references to the *euler* path are the same rotation */
    ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))

    /* direct slerp reference: same endpoint orientation, bounded gap */
    GLM(euler_xyz_quat)(from, qSlerp);
    {
      versor qTo;
      GLM(euler_xyz_quat)(to, qTo);
      glm_quat_slerp(qSlerp, qTo, t, qSlerp);
    }
    ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-4f))
    ASSERT(test_eq_th(glm_quat_norm(qEuler), 1.0f, 1e-5f))
  }

  /* chained usage stays coherent over a full cross-zero run */
  from[0] = 0.0f;
  from[1] = 0.0f;
  from[2] = 0.0f;

  for (i = 0; i < 64; i++) {
    to[0] = glm_rad((i % 2) ? 1.0f : 359.0f);
    to[1] = 0.0f;
    to[2] = 0.0f;

    GLM(euler_quat_lerp)(from, to, 1.0f, qEuler);
    GLM(euler_xyz_quat)(to, qSlerp);
    ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))

    GLM(euler_lerp)(from, to, 1.0f, from);
  }

  /*
   * General multi-axis keyframes. Component-wise linear euler
   * interpolation and quaternion slerp are different curves away from
   * the endpoints (for simultaneous 3-axis motion their interior
   * orientations can differ substantially), so the dual-path contract
   * is exact agreement at t=0 and t=1 for every keyframe, plus a valid
   * relative-angle reading. Interior euler-path correctness is covered
   * deterministically by the single-axis cases above.
   */
  state = UINT64_C(0x123456789abcdef0);

  for (i = 0; i < 200; i++) {
    int axis;

    for (axis = 0; axis < 3; axis++) {
      from[axis] = (test_euler_interp_rand(&state) * 2.0f - 1.0f)
                   * GLM_PI_2f;
      to[axis]   = (test_euler_interp_rand(&state) * 2.0f - 1.0f)
                   * GLM_PI_2f;
    }

    /* t = 0 reproduces the from-orientation regardless of wrapping */
    rel = GLM(euler_quat_lerp)(from, to, 0.0f, qEuler);
    GLM(euler_xyz_quat)(from, qSlerp);
    ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))
    ASSERT(rel >= 0.0f && rel <= GLM_PIf + 1e-6f)

    /* t = 1 reproduces the to-orientation regardless of wrapping */
    rel = GLM(euler_quat_lerp)(from, to, 1.0f, qEuler);
    GLM(euler_xyz_quat)(to, qSlerp);
    ASSERT(test_quat_same_rotation(qEuler, qSlerp, 1e-6f))
    ASSERT(rel >= 0.0f && rel <= GLM_PIf + 1e-6f)
  }

  TEST_SUCCESS
}

TEST_IMPL(GLM_PREFIX, euler_lerp_perf) {
  /*
   * One million shortest-arc interpolations. The 50 ms budget is a
   * native optimized-build target (the hot path is branch-light
   * arithmetic with no sin/acos), so the hard limit is only enforced
   * there; debug and WASM builds still execute the loop to guard
   * correctness, and report the measured time.
   */
  enum { ITER = 1000000 };

  CGLM_ALIGN(8) vec3 from, to, dest;
  clock_t t0, t1;
  double  elapsedMs;
  int     i;

  from[0] = glm_rad(359.0f);
  from[1] = glm_rad(120.0f);
  from[2] = glm_rad(-77.0f);
  to[0]   = glm_rad(1.0f);
  to[1]   = glm_rad(-118.0f);
  to[2]   = glm_rad(83.0f);

  t0 = clock();
  for (i = 0; i < ITER; i++) {
    GLM(euler_lerp)(from, to, (float)(i & 255) / 255.0f, dest);
  }
  t1 = clock();
  /* keep the result observable so the loop is not optimized away */
  ASSERT(dest[0] == dest[0])

  elapsedMs = (double)(t1 - t0) * 1.0e3 / CLOCKS_PER_SEC;

#if defined(NDEBUG) && !defined(__wasi__) && !defined(__EMSCRIPTEN__)
  ASSERT(elapsedMs < 50.0)
#else
  fprintf(stderr, "\n    (perf note) 1e6 euler_lerp: %.3f ms "
                  "(50 ms limit enforced on native NDEBUG builds)\n",
          elapsedMs);
#endif

  TEST_SUCCESS
}
