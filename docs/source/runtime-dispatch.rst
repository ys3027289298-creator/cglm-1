Runtime SIMD dispatch
=====================

cglm is header-only and selects SIMD implementations at **compile time**
from the target flags (``-msse2``, ``-mavx``, ...). When the library is
compiled for a generic x86-64 baseline (SSE2, no AVX) with GCC >= 5 or
Clang, cglm additionally enables **runtime dispatch** for hot operations:
the CPU is probed once per translation unit and each call branches to the
best supported implementation. The public interface and the header-only
inline model are unchanged.

Runtime dispatch is active only when **all** of the following hold:

* x86 target, GCC >= 5 or Clang (function ``target`` attributes required)
* compiled **without** ``-mavx`` / ``-march=...avx`` (with ``__AVX__``
  defined, the compile-time AVX path is already optimal and is kept)
* SSE2 baseline (always true on x86-64)
* ``CGLM_NO_RUNTIME_DISPATCH`` is not defined

Define ``CGLM_NO_RUNTIME_DISPATCH`` to restore pure compile-time
selection. MSVC, ARM and wasm targets keep compile-time selection
(aarch64 always has NEON; 32-bit ARM NEON is a compile-time choice).

Because the precompiled ``glmc_*`` library (``src/``) is built from the
same inline headers, a binary built for the x86-64 baseline also adapts
to the runtime CPU for these operations.

Dispatch levels
---------------

============  =========================================================
level         implementation
============  =========================================================
``scalar``    portable scalar C — the reference implementation
``sse2``      SSE2 kernels (x86-64 baseline), fully inlined as before
``avx``       AVX kernels compiled with
              ``__attribute__((target("avx")))``: true 256-bit for
              ``glm_mat4_mul``, VEX re-encoding of the SSE2 sources
              for ``glm_mat4_inv`` / ``glm_quat_mul``
============  =========================================================

The probed level is cached in a per-translation-unit static (this is what
keeps the library header-only); a data race on the cache is benign because
every racer stores the same value.

Forcing the level
-----------------

Environment variable (process-wide, read once per translation unit)::

  CGLM_SIMD=auto     # default: best level the CPU supports
  CGLM_SIMD=scalar   # force the scalar reference implementation
  CGLM_SIMD=sse2     # force the SSE2 implementation
  CGLM_SIMD=avx      # force the AVX implementation

A forced level above the hardware capability is **clamped** to the
hardware level, so a forced value can never execute instructions the CPU
lacks. Unknown values fall back to ``auto``.

For tests and benchmarks the level can also be forced programmatically:

.. code-block:: c

  int         cglm_simd_level(void);        /* active level */
  const char *cglm_simd_level_name(void);   /* "scalar"/"sse2"/"avx" */
  void        cglm_simd_force_level(int);   /* per-TU override, -1 = auto */

``cglm_simd_force_level()`` affects only the **current translation unit**
(the cache is per-TU); the environment variable is the supported
process-wide mechanism. Both functions exist on all targets — on targets
without runtime dispatch they report the compile-time selection and
forcing is a no-op.

Dispatched operations and implementation notes
----------------------------------------------

===========================  ==========  ==============================
operation                    AVX kernel  note
===========================  ==========  ==============================
``glm_mat4_mul``             256-bit     two columns per ymm register
``glm_mat4_mulv``            —           AVX level uses the inline SSE2
                                         kernel: a non-inlined VEX-128
                                         call costs more than it saves
                                         (measured)
``glm_mat4_inv``             VEX-128     SSE2 source re-encoded
                                         (3-operand)
``glm_quat_mul``             VEX-128     ditto
``glm_vec4_dot`` /           —           AVX level uses the inline SSE2
``glm_vec4_norm``                        kernel (same reason as mulv)
===========================  ==========  ==============================

Error bounds vs the scalar reference
------------------------------------

All implementations compute exactly the same mathematical expression;
differences come **only** from floating-point reassociation (summation
order). No implementation uses FMA (AVX does not imply FMA), ``rcp`` /
``rsqrt`` approximations, or flush-to-zero — every divide and square root
is IEEE-exact. Without ``-ffast-math``:

* ``glm_mat4_mul``: SSE2 sums products in the same order as the scalar
  code → **bitwise identical**. AVX sums each element in order
  k=0,2,1,3 → differs by a few ulp.
* ``glm_mat4_mulv``: SSE2/AVX sum columns in order k=3,2,1,0 vs the
  scalar 0,1,2,3 → a few ulp.
* ``glm_mat4_inv``: the SSE2/AVX kernels reorder the cofactor arithmetic
  → a few ulp, amplified by the matrix condition number as usual.
* ``glm_quat_mul``: sign flips are done with XOR (exact) and the
  summation order matches the scalar code → **bitwise identical**.
* ``glm_vec4_dot``: SIMD reduces (a0b0+a1b1)+(a2b2+a3b3) vs the scalar
  left-to-right order → a few ulp. ``glm_vec4_norm`` uses the same
  reduction plus an exact ``sqrtss``.

A general backward-error bound for the reordered sums is
``|err| <= 3 * eps * sum_k|term_k|`` (4-term dot products,
``eps = 1.19e-7``), i.e. the dispatched results are as accurate as the
scalar reference in the backward-error sense.

Measured (``bench/runtime_dispatch.c``, 1e6 random inputs per cell,
AMD Ryzen 7 9700X, gcc 13, ``-O2``, x86-64 baseline; throughput varies
±10 % run to run):

======================  ======  =======  ===========  ============  ==========
operation               impl    Mops/s   max abs err  max rel err*  max ulp*
======================  ======  =======  ===========  ============  ==========
``glm_mat4_mul``        scalar  278.2    0            0             0
``glm_mat4_mul``        sse2    312.3    0            0             0
``glm_mat4_mul``        avx     379.7    3.8e-06      3.1e-04       4096
``glm_mat4_mulv``       scalar  539.6    0            0             0
``glm_mat4_mulv``       sse2    529.3    3.8e-06      2.6e-04       3840
``glm_mat4_mulv``       avx     531.3    3.8e-06      2.6e-04       3840
``glm_mat4_inv``        scalar  77.3     0            0             0
``glm_mat4_inv``        sse2    148.2    1.5e-07      6.0e-06       57
``glm_mat4_inv``        avx     152.3    1.5e-07      6.0e-06       57
``glm_quat_mul``        scalar  432.9    0            0             0
``glm_quat_mul``        sse2    696.2    0            0             0
``glm_quat_mul``        avx     576.0    0            0             0
``glm_vec4_dot``        scalar  408.9    0            0             0
``glm_vec4_dot``        sse2    417.7    3.8e-06      4.3e-04       4096
``glm_vec4_dot``        avx     412.6    3.8e-06      4.3e-04       4096
``glm_vec4_norm``       scalar  404.3    0            0             0
``glm_vec4_norm``       sse2    417.4    4.8e-07      1.4e-07       2
``glm_vec4_norm``       avx     423.1    4.8e-07      1.4e-07       2
======================  ======  =======  ===========  ===========  ===========

\* relative/ulp error only counted where ``|reference| >= 1e-3``;
near-zero results (cancellation) are covered by the absolute error
column. The larger ulp figures for ``mat4_mul``/``mat4_mulv``/``vec4_dot``
occur only for results close to zero, where cancellation makes the
relative error of *any* summation order large; the absolute error stays
below 4e-6 for inputs of magnitude ~4.

Reading the throughput numbers:

* ``glm_mat4_mul`` benefits most from AVX (256-bit, ~1.4x vs scalar).
* ``glm_mat4_inv`` is ~1.9x with SIMD (either set); AVX's 3-operand
  encoding gives a small further gain.
* ``glm_quat_mul`` is ~1.3-1.6x with SIMD.
* Single ``vec4`` operations are latency-bound; SIMD neither helps nor
  hurts measurably at this granularity, which is why the dispatcher
  keeps the inline SSE2 kernel for the AVX level there.

Reproduce with::

  gcc -O2 -I include bench/runtime_dispatch.c -o bench_runtime -lm
  ./bench_runtime            # or: CGLM_SIMD=sse2 ./bench_runtime
