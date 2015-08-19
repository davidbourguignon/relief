#ifndef __COMPLX_H__
#define __COMPLX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <vec2.h>

/*
 * z = x + yi
 *   = r*(cos(theta) + sin(theta)i) = r*exp((theta)i)
 *
 * where r     is the complex number norm
 *       theta is the complex number angle (in radians)
 *
 * and x^2 + y^2 = 1.0 or r = 1.0 (unit norm)
 */

typedef double Complxd[2];
typedef double *Complxd_t;

EXTERND const Complxd COMPLXD_ID; /* Rotation with null angle */

/*
 * Complxd declarations
 *
 * Function name            Hint
 * complxd_set              not normalized
 * complxd_set_axis_angle   unit norm only
 * complxd_normalize        use only normalized complexes for rotations
 */
INLINED Complxd_t complxd_set       (Complxd c,
                                     const double x, const double y);
INLINED Complxd_t complxd_set_angle (Complxd c, const double angle);
INLINED Complxd_t complxd_eq        (Complxd c, const Complxd a);
INLINED Complxd_t complxd_multeq    (Complxd c, const Complxd a);
INLINED Complxd_t complxd_diveq     (Complxd c, const Complxd a);
INLINED Complxd_t complxd_mult      (Complxd c,
                                     const Complxd a, const Complxd b);
INLINED Complxd_t complxd_div       (Complxd c,
                                     const Complxd a, const Complxd b);
INLINED Complxd_t complxd_normalize (Complxd c);

/*
 * Complxd definitions
 */
INLINED Complxd_t
complxd_set(Complxd c, const double x, const double y) {
  c[0] = x;
  c[1] = y;
  return c;
}

INLINED Complxd_t
complxd_set_angle(Complxd c, const double angle) {
  c[0] = cos(angle);
  c[1] = sin(angle);
  return c;
}

INLINED Complxd_t
complxd_eq(Complxd c, const Complxd a) {
  c[0] = a[0];
  c[1] = a[1];
  return c;
}

INLINED Complxd_t
complxd_multeq(Complxd c, const Complxd a) {
  const double x = c[0]*a[0] - c[1]*a[1];
  const double y = c[0]*a[1] + c[1]*a[0];
  c[0] = x;
  c[1] = y;
  return c;
}

INLINED Complxd_t
complxd_diveq(Complxd c, const Complxd a) {
  const double x = c[0]*a[0] + c[1]*a[1];
  const double y = c[1]*a[0] - c[0]*a[1];
  c[0] = x;
  c[1] = y;
  return c;
}

INLINED Complxd_t
complxd_mult(Complxd c, const Complxd a, const Complxd b) {
  c[0] = a[0]*b[0] - a[1]*b[1];
  c[1] = a[0]*b[1] + a[1]*b[0];
  return c;
}

INLINED Complxd_t
complxd_div(Complxd c, const Complxd a, const Complxd b) {
  c[0] = a[0]*b[0] + a[1]*b[1];
  c[1] = a[1]*b[0] - a[0]*b[1];
  return c;
}

INLINED Complxd_t
complxd_normalize(Complxd c) {
  const double nrm_inv = 1.0 / sqrt(c[0]*c[0] + c[1]*c[1]);
  c[0] *= nrm_inv;
  c[1] *= nrm_inv;
  return c;
}

#ifdef __cplusplus
}
#endif

#endif /* !__COMPLX_H__ */
