#ifndef __VEC3_H__
#define __VEC3_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

typedef double Vec3d[3];
typedef float  Vec3f[3];
typedef double *Vec3d_t;
typedef float  *Vec3f_t;
typedef const double *const_Vec3d_t;
typedef const float  *const_Vec3f_t;

EXTERND const Vec3d VEC3D_NULL;
EXTERND const Vec3f VEC3F_NULL;

/*
 * Function name     Hint
 * cmp               comparison returns an integer less than, equal to, or
 *                   greater than zero if a is found, respectively, to be
 *                   less than, to match, or be greater than b, in the
 *                   lexicographical sense.
 * lerp              linear interpolation between a and b, t in the range 0..1
 *                   v = (1-t)*a + t*b
 * slerp             spherical linear interpolation between a and b,
 *                   t in the range 0..1
 *                   v = [1/sin(alpha)]*[sin((1-t)*alpha)*a + sin(t*alpha)*b]
 *                   a and b are unit vectors and the interpolant v is also
 *                   a unit vector (see Watt, 1992)
 */

/*
 * Vec3d declarations
 */
INLINED Vec3d_t vec3d_set      (Vec3d v, double x, double y, double z);
INLINED Vec3d_t vec3d_eq       (Vec3d v, const Vec3d a);
INLINED Vec3d_t vec3d_addeq    (Vec3d v, const Vec3d a);
INLINED Vec3d_t vec3d_subeq    (Vec3d v, const Vec3d a);
INLINED Vec3d_t vec3d_multeq   (Vec3d v, double s);
INLINED Vec3d_t vec3d_diveq    (Vec3d v, double s);
INLINED Vec3d_t vec3d_add      (Vec3d v, const Vec3d a, const Vec3d b);
INLINED Vec3d_t vec3d_sub      (Vec3d v, const Vec3d a, const Vec3d b);
INLINED Vec3d_t vec3d_mult     (Vec3d v, const Vec3d a, double s);
INLINED Vec3d_t vec3d_div      (Vec3d v, const Vec3d a, double s);
INLINED Vec3d_t vec3d_neg      (Vec3d v);
INLINED int     vec3d_print    (const Vec3d v, FILE *stream);
INLINED int     vec3d_scan     (Vec3d v, FILE *stream);
INLINED int     vec3d_cmp      (const Vec3d a, const Vec3d b);
INLINED double  vec3d_sqnorm   (const Vec3d a);
INLINED double  vec3d_norm     (const Vec3d a);
INLINED Vec3d_t vec3d_normalize(Vec3d v);
INLINED double  vec3d_dot      (const Vec3d a, const Vec3d b);
INLINED Vec3d_t vec3d_cross    (Vec3d v, const Vec3d a, const Vec3d b);
INLINED double  vec3d_sqdist   (const Vec3d a, const Vec3d b);
INLINED double  vec3d_dist     (const Vec3d a, const Vec3d b);
INLINED double  vec3d_cosang   (const Vec3d a, const Vec3d b);
INLINED Vec3d_t vec3d_lerp     (Vec3d v, const Vec3d a, const Vec3d b,
                                double t);
INLINED Vec3d_t vec3d_slerp    (Vec3d v, const Vec3d a, const Vec3d b,
                                double t);

/*
 * Vec3f declarations
 */
INLINED Vec3f_t vec3f_set      (Vec3f v, float x, float y, float z);
INLINED Vec3f_t vec3f_eq       (Vec3f v, const Vec3f a);
INLINED Vec3f_t vec3f_addeq    (Vec3f v, const Vec3f a);
INLINED Vec3f_t vec3f_subeq    (Vec3f v, const Vec3f a);
INLINED Vec3f_t vec3f_multeq   (Vec3f v, float s);
INLINED Vec3f_t vec3f_diveq    (Vec3f v, float s);
INLINED Vec3f_t vec3f_add      (Vec3f v, const Vec3f a, const Vec3f b);
INLINED Vec3f_t vec3f_sub      (Vec3f v, const Vec3f a, const Vec3f b);
INLINED Vec3f_t vec3f_mult     (Vec3f v, const Vec3f a, float s);
INLINED Vec3f_t vec3f_div      (Vec3f v, const Vec3f a, float s);
INLINED Vec3f_t vec3f_neg      (Vec3f v);
INLINED int     vec3f_print    (const Vec3f v, FILE *stream);
INLINED int     vec3f_scan     (Vec3f v, FILE *stream);
INLINED int     vec3f_cmp      (const Vec3f a, const Vec3f b);
INLINED float   vec3f_sqnorm   (const Vec3f a);
INLINED float   vec3f_norm     (const Vec3f a);
INLINED Vec3f_t vec3f_normalize(Vec3f v);
INLINED float   vec3f_dot      (const Vec3f a, const Vec3f b);
INLINED Vec3f_t vec3f_cross    (Vec3f v, const Vec3f a, const Vec3f b);
INLINED float   vec3f_sqdist   (const Vec3f a, const Vec3f b);
INLINED float   vec3f_dist     (const Vec3f a, const Vec3f b);
INLINED float   vec3f_cosang   (const Vec3f a, const Vec3f b);
INLINED Vec3f_t vec3f_lerp     (Vec3f v, const Vec3f a, const Vec3f b,
                                float t);
INLINED Vec3f_t vec3f_slerp    (Vec3f v, const Vec3f a, const Vec3f b,
                                double t);

/*
 * Vec3d definitions
 */
INLINED Vec3d_t
vec3d_set(Vec3d v, double x, double y, double z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

INLINED Vec3d_t
vec3d_eq(Vec3d v, const Vec3d a) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  return v;
}

INLINED Vec3d_t
vec3d_addeq(Vec3d v, const Vec3d a) {
  v[0] += a[0];
  v[1] += a[1];
  v[2] += a[2];
  return v;
}

INLINED Vec3d_t
vec3d_subeq(Vec3d v, const Vec3d a) {
  v[0] -= a[0];
  v[1] -= a[1];
  v[2] -= a[2];
  return v;
}

INLINED Vec3d_t
vec3d_multeq(Vec3d v, double s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  return v;
}

INLINED Vec3d_t
vec3d_diveq(Vec3d v, double s) {
  v[0] /= s;
  v[1] /= s;
  v[2] /= s;
  return v;
}

INLINED Vec3d_t
vec3d_add(Vec3d v, const Vec3d a, const Vec3d b) {
  v[0] = a[0] + b[0];
  v[1] = a[1] + b[1];
  v[2] = a[2] + b[2];
  return v;
}

INLINED Vec3d_t
vec3d_sub(Vec3d v, const Vec3d a, const Vec3d b) {
  v[0] = a[0] - b[0];
  v[1] = a[1] - b[1];
  v[2] = a[2] - b[2];
  return v;
}

INLINED Vec3d_t
vec3d_mult(Vec3d v, const Vec3d a, double s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  v[2] = a[2] * s;
  return v;
}

INLINED Vec3d_t
vec3d_div(Vec3d v, const Vec3d a, double s) {
  v[0] = a[0] / s;
  v[1] = a[1] / s;
  v[2] = a[2] / s;
  return v;
}

INLINED Vec3d_t
vec3d_neg(Vec3d v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
  return v;
}

INLINED int
vec3d_print(const Vec3d v, FILE *stream) {
  return (fprintf(stream, "%f %f %f", v[0], v[1], v[2]));
}

INLINED int
vec3d_scan(Vec3d v, FILE *stream) {
  return (fscanf(stream, "%lf %lf %lf", &(v[0]), &(v[1]), &(v[2])));
}

INLINED int
vec3d_cmp(const Vec3d a, const Vec3d b) {
  return (memcmp(a, b, sizeof(Vec3d)));
}

INLINED double
vec3d_sqnorm(const Vec3d a) {
  return ((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
}

INLINED double
vec3d_norm(const Vec3d a) {
  return (sqrt(vec3d_sqnorm(a)));
}

INLINED Vec3d_t
vec3d_normalize(Vec3d v) {
  double nrm_inv = 1.0 / vec3d_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  v[2] *= nrm_inv;
  return v;
}

INLINED double
vec3d_dot(const Vec3d a, const Vec3d b) {
  return ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]));
}

INLINED Vec3d_t
vec3d_cross(Vec3d v, const Vec3d a, const Vec3d b) {
  v[0] = (a[1] * b[2]) - (a[2] * b[1]);
  v[1] = (a[2] * b[0]) - (a[0] * b[2]);
  v[2] = (a[0] * b[1]) - (a[1] * b[0]);
  return v;
}

INLINED double
vec3d_sqdist(const Vec3d a, const Vec3d b) {
  Vec3d d;
  vec3d_sub(d, a, b);
  return (vec3d_sqnorm(d));
}

INLINED double
vec3d_dist(const Vec3d a, const Vec3d b) {
  return (sqrt(vec3d_sqdist(a, b)));
}

INLINED double
vec3d_cosang(const Vec3d a, const Vec3d b) {
  return (vec3d_dot(a, b) / sqrt(vec3d_sqnorm(a) * vec3d_sqnorm(b)));
}

INLINED Vec3d_t
vec3d_lerp(Vec3d v, const Vec3d a, const Vec3d b, double t) {
  v[0] = (b[0] - a[0]) * t + a[0];
  v[1] = (b[1] - a[1]) * t + a[1];
  v[2] = (b[2] - a[2]) * t + a[2];
  return v;
}

INLINED Vec3d_t
vec3d_slerp(Vec3d v, const Vec3d a, const Vec3d b, double t) {
  double alpha = acos(vec3d_dot(a, b)); /* unit vectors */
  double sin_alpha_inv = 1.0 / sin(alpha);
  double weight_a = sin((1.0 - t) * alpha) * sin_alpha_inv;
  double weight_b = sin(t * alpha) * sin_alpha_inv;
  assert(errno != EDOM); /* acos */
  v[0] = weight_a * a[0] + weight_b * b[0];
  v[1] = weight_a * a[1] + weight_b * b[1];
  v[2] = weight_a * a[2] + weight_b * b[2];
  return v;
}

/*
 * Vec3f definitions
 */
INLINED Vec3f_t
vec3f_set(Vec3f v, float x, float y, float z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

INLINED Vec3f_t
vec3f_eq(Vec3f v, const Vec3f a) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  return v;
}

INLINED Vec3f_t
vec3f_addeq(Vec3f v, const Vec3f a) {
  v[0] += a[0];
  v[1] += a[1];
  v[2] += a[2];
  return v;
}

INLINED Vec3f_t
vec3f_subeq(Vec3f v, const Vec3f a) {
  v[0] -= a[0];
  v[1] -= a[1];
  v[2] -= a[2];
  return v;
}

INLINED Vec3f_t
vec3f_multeq(Vec3f v, float s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  return v;
}

INLINED Vec3f_t
vec3f_diveq(Vec3f v, float s) {
  v[0] /= s;
  v[1] /= s;
  v[2] /= s;
  return v;
}

INLINED Vec3f_t
vec3f_add(Vec3f v, const Vec3f a, const Vec3f b) {
  v[0] = a[0] + b[0];
  v[1] = a[1] + b[1];
  v[2] = a[2] + b[2];
  return v;
}

INLINED Vec3f_t
vec3f_sub(Vec3f v, const Vec3f a, const Vec3f b) {
  v[0] = a[0] - b[0];
  v[1] = a[1] - b[1];
  v[2] = a[2] - b[2];
  return v;
}

INLINED Vec3f_t
vec3f_mult(Vec3f v, const Vec3f a, float s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  v[2] = a[2] * s;
  return v;
}

INLINED Vec3f_t
vec3f_div(Vec3f v, const Vec3f a, float s) {
  v[0] = a[0] / s;
  v[1] = a[1] / s;
  v[2] = a[2] / s;
  return v;
}

INLINED Vec3f_t
vec3f_neg(Vec3f v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
  return v;
}

INLINED int
vec3f_print(const Vec3f v, FILE *stream) {
  return (fprintf(stream, "%f %f %f", v[0], v[1], v[2]));
}

INLINED int
vec3f_scan(Vec3f v, FILE *stream) {
  return (fscanf(stream, "%f %f %f", &(v[0]), &(v[1]), &(v[2])));
}

INLINED int
vec3f_cmp(const Vec3f a, const Vec3f b) {
  return (memcmp(a, b, sizeof(Vec3f)));
}

INLINED float
vec3f_sqnorm(const Vec3f a) {
  return ((a[0] * a[0]) + (a[1] * a[1]) + (a[2] * a[2]));
}

INLINED float
vec3f_norm(const Vec3f a) {
  return (sqrtf(vec3f_sqnorm(a)));
}

INLINED Vec3f_t
vec3f_normalize(Vec3f v) {
  float nrm_inv = 1.0f / vec3f_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  v[2] *= nrm_inv;
  return v;
}

INLINED float
vec3f_dot(const Vec3f a, const Vec3f b) {
  return ((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]));
}

INLINED Vec3f_t
vec3f_cross(Vec3f v, const Vec3f a, const Vec3f b) {
  v[0] = (a[1] * b[2]) - (a[2] * b[1]);
  v[1] = (a[2] * b[0]) - (a[0] * b[2]);
  v[2] = (a[0] * b[1]) - (a[1] * b[0]);
  return v;
}

INLINED float
vec3f_sqdist(const Vec3f a, const Vec3f b) {
  Vec3f d;
  vec3f_sub(d, a, b);
  return (vec3f_sqnorm(d));
}

INLINED float
vec3f_dist(const Vec3f a, const Vec3f b) {
  return (sqrtf(vec3f_sqdist(a, b)));
}

INLINED float
vec3f_cosang(const Vec3f a, const Vec3f b) {
  return (vec3f_dot(a, b) / sqrtf(vec3f_sqnorm(a) * vec3f_sqnorm(b)));
}

INLINED Vec3f_t
vec3f_lerp(Vec3f v, const Vec3f a, const Vec3f b, float t) {
  v[0] = (b[0] - a[0]) * t + a[0];
  v[1] = (b[1] - a[1]) * t + a[1];
  v[2] = (b[2] - a[2]) * t + a[2];
  return v;
}

INLINED Vec3f_t
vec3f_slerp(Vec3f v, const Vec3f a, const Vec3f b, double t) {
  double alpha = acos(vec3f_dot(a, b)); /* unit vectors */
  double sin_alpha_inv = 1.0 / sin(alpha);
  double weight_a = sin((1.0 - t) * alpha) * sin_alpha_inv;
  double weight_b = sin(t * alpha) * sin_alpha_inv;
  assert(errno != EDOM); /* acos */
  v[0] = (float) (weight_a * a[0] + weight_b * b[0]);
  v[1] = (float) (weight_a * a[1] + weight_b * b[1]);
  v[2] = (float) (weight_a * a[2] + weight_b * b[2]);
  return v;
}

#ifdef __cplusplus
}
#endif

#endif /* !__VEC3_H__ */
