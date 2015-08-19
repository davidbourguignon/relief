#ifndef __QUAT_H__
#define __QUAT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <vec3.h>

/*
 * q = w + xi + yj + zk
 *   = cos(alpha/2) + a*sin(alpha/2)i + b*sin(alpha/2)j + c*sin(alpha/2)k
 *
 * where (a, b, c) is the quaternion axis with unit norm
 *       alpha     is the quaternion angle (in radians)
 *
 * and   w^2 + x^2 + y^2 + z^2 = 1.0
 *
 * See the following for more information on quaternions:
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No. 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */

typedef double Quatd[4];
typedef double *Quatd_t;

EXTERND const Quatd QUATD_ID; /* Rotation with null angle */

/*
 * Quatd declarations
 *
 * Function name            Hint
 * quatd_set_axis_angle     axis parameter will be normalized
 * quatd_multeq             quaternions are left-multiplied
 * quatd_normalize          use only normalized quaternions
 */
INLINED Quatd_t  quatd_set_axis_angle(Quatd q, Vec3d axis, const double angle);
INLINED Quatd_t  quatd_eq            (Quatd q, const Quatd a);
INLINED Quatd_t  quatd_multeq        (Quatd q, const Quatd a);
INLINED Quatd_t  quatd_mult          (Quatd q, const Quatd a, const Quatd b);
INLINED Quatd_t  quatd_normalize     (Quatd q);

/*
 * Quatd definitions
 */
INLINED Quatd_t
quatd_set_axis_angle(Quatd q, Vec3d axis, const double angle) {
  const double half_angle = angle / 2.0;
  const double sin_half_angle = sin(half_angle);
  vec3d_normalize(axis);
  q[0] = cos(half_angle);
  q[1] = axis[0] * sin_half_angle;
  q[2] = axis[1] * sin_half_angle;
  q[3] = axis[2] * sin_half_angle;
  return q;
}

INLINED Quatd_t
quatd_eq(Quatd q, const Quatd a) {
  q[0] = a[0];
  q[1] = a[1];
  q[2] = a[2];
  q[3] = a[3];
  return q;
}

INLINED Quatd_t
quatd_multeq(Quatd q, const Quatd a) {
  const double w = q[0]*a[0] - q[1]*a[1] - q[2]*a[2] - q[3]*a[3];
  const double x = q[0]*a[1] + q[1]*a[0] + q[2]*a[3] - q[3]*a[2];
  const double y = q[0]*a[2] - q[1]*a[3] + q[2]*a[0] + q[3]*a[1];
  const double z = q[0]*a[3] + q[1]*a[2] - q[2]*a[1] + q[3]*a[0];
  q[0] = w;
  q[1] = x;
  q[2] = y;
  q[3] = z;
  return q;
}

INLINED Quatd_t
quatd_mult(Quatd q, const Quatd a, const Quatd b) {
  q[0] = b[0]*a[0] - b[1]*a[1] - b[2]*a[2] - b[3]*a[3];
  q[1] = b[0]*a[1] + b[1]*a[0] + b[2]*a[3] - b[3]*a[2];
  q[2] = b[0]*a[2] - b[1]*a[3] + b[2]*a[0] + b[3]*a[1];
  q[3] = b[0]*a[3] + b[1]*a[2] - b[2]*a[1] + b[3]*a[0];
  return q;
}

INLINED Quatd_t
quatd_normalize(Quatd q) {
  const double nrm_inv
    = 1.0 / sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
  q[0] *= nrm_inv;
  q[1] *= nrm_inv;
  q[2] *= nrm_inv;
  q[3] *= nrm_inv;
  return q;
}

#ifdef __cplusplus
}
#endif

#endif /* !__QUAT_H__ */
