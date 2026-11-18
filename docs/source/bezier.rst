.. default-domain:: C

Bezier
================================================================================

Header: cglm/bezier.h

Common helpers for cubic bezier and similar curves.

Table of contents (click to go):
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Functions:

1. :c:func:`glm_bezier`
2. :c:func:`glm_hermite`
3. :c:func:`glm_decasteljau`

Functions documentation
~~~~~~~~~~~~~~~~~~~~~~~

.. c:function:: float glm_bezier(float s, float p0, float c0, float c1, float p1)

    | cubic bezier interpolation
    | formula:

    .. code-block:: text

      B(s) = P0*(1-s)^3 + 3*C0*s*(1-s)^2 + 3*C1*s^2*(1-s) + P1*s^3

    | similar result using matrix:

    .. code-block:: text

      B(s) = glm_smc(t, GLM_BEZIER_MAT, (vec4){p0, c0, c1, p1})

    | glm_eq(glm_smc(...), glm_bezier(...)) should return TRUE

    Parameters:
      | *[in]*  **s**   parameter between 0 and 1
      | *[in]*  **p0**  begin point
      | *[in]*  **c0**  control point 1
      | *[in]*  **c1**  control point 2
      | *[in]*  **p1**  end point

    Returns:
        B(s)

.. c:function:: float glm_hermite(float s, float p0, float t0, float t1, float p1)

    | cubic hermite interpolation
    | formula:

    .. code-block:: text

      H(s) = P0*(2*s^3 - 3*s^2 + 1) + T0*(s^3 - 2*s^2 + s) + P1*(-2*s^3 + 3*s^2) + T1*(s^3 - s^2)

    | similar result using matrix:

    .. code-block:: text

      H(s) = glm_smc(t, GLM_HERMITE_MAT, (vec4){p0, p1, c0, c1})

    | glm_eq(glm_smc(...), glm_hermite(...)) should return TRUE


    Parameters:
      | *[in]*  **s**   parameter between 0 and 1
      | *[in]*  **p0**  begin point
      | *[in]*  **t0**  tangent 1
      | *[in]*  **t1**  tangent 2
      | *[in]*  **p1**  end point

    Returns:
        B(s)

.. c:function:: float glm_decasteljau(float prm, float p0, float c0, float c1, float p1)

    | iterative way to solve cubic equation

    Parameters:
      | *[in]*  **prm** parameter between 0 and 1
      | *[in]*  **p0**  begin point
      | *[in]*  **c0**  control point 1
      | *[in]*  **c1**  control point 2
      | *[in]*  **p1**  end point

    Returns:
        parameter to use in cubic equation

Least squares fitting
~~~~~~~~~~~~~~~~~~~~~

The fitting helpers fit a bezier curve to an arbitrary point set:

1. :c:func:`glm_bezier_fit`
2. :c:func:`glm_bezier_fit_lsq`
3. :c:func:`glm_bezier_fit_error`
4. :c:func:`glm_bezier_eval`

.. c:type:: glm_bezier_fit_opts

    Options for :c:func:`glm_bezier_fit`.

    .. code-block:: c

      typedef struct glm_bezier_fit_opts {
        int   degree;     /* requested degree of the fitted curve          */
        int   max_degree; /* max degree allowed for automatic degree raising */
        float tolerance;  /* max allowed deviation, <= 0 disables raising  */
      } glm_bezier_fit_opts;

.. c:type:: glm_bezier_fit_result

    Result of a fitting.

    .. code-block:: c

      typedef struct glm_bezier_fit_result {
        vec2  control[CGLM_BEZIER_FIT_MAX_DEGREE + 1];
        int   degree;
        int   raised;     /* whether the degree was raised automatically */
        float max_error;  /* maximum deviation */
        float mse;        /* mean squared deviation */
      } glm_bezier_fit_result;

.. c:function:: bool glm_bezier_fit(const vec2 *pts, size_t npts, const glm_bezier_fit_opts *opts, glm_bezier_fit_result *dest)

    | Least squares fit of a bezier curve with fixed endpoints.
    |
    | Points are parameterized by chord length and the normal equations are
    | solved for the inner control points. The first and the last control
    | points are always the first and the last input points.
    |
    | If the maximum deviation is greater than ``opts->tolerance`` and the
    | degree can be raised, the points are re-fitted once with degree + 1,
    | up to ``opts->max_degree``.

    Parameters:
      | *[in]*  **pts**  input points
      | *[in]*  **npts** number of input points, must be >= 2
      | *[in]*  **opts** fit options, NULL for defaults (degree 3, no raising)
      | *[out]* **dest** fitted control points and error metrics

    Returns:
        false if the fit failed

.. c:function:: bool glm_bezier_fit_lsq(const vec2 *pts, size_t npts, int degree, vec2 *ctrl)

    | Least squares fit with fixed endpoints and the requested degree.
    | The degree is clamped to ``[1, min(npts - 1, CGLM_BEZIER_FIT_MAX_DEGREE)]``.

    Parameters:
      | *[in]*  **pts**   input points
      | *[in]*  **npts**  number of input points, must be >= 2
      | *[in]*  **degree** requested degree
      | *[out]* **ctrl**  degree + 1 control points

    Returns:
        false if the fit failed

.. c:function:: float glm_bezier_fit_error(const vec2 *pts, size_t npts, int degree, const vec2 *ctrl, float *mse)

    | Measures how well a bezier fits a point set.
    |
    | The curve is approximated by an adaptive polyline (recursive de
    | Casteljau subdivision) and the distance from every input point to the
    | nearest polyline segment is measured.

    Parameters:
      | *[in]*  **pts**   input points
      | *[in]*  **npts**  number of input points
      | *[in]*  **degree** degree of the curve
      | *[in]*  **ctrl**   degree + 1 control points
      | *[out]* **mse**    mean squared deviation, may be NULL

    Returns:
        maximum deviation, FLT_MAX on invalid input

.. c:function:: void glm_bezier_eval(int degree, const vec2 *ctrl, float s, vec2 dest)

    | Evaluates a bezier curve of arbitrary degree at ``s`` using
    | de Casteljau's algorithm.

    Parameters:
      | *[in]*  **degree** degree of the curve
      | *[in]*  **ctrl**   degree + 1 control points
      | *[in]*  **s**      parameter between 0 and 1
      | *[out]* **dest**   point on the curve
