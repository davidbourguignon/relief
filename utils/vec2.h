#ifndef __VEC2_H__
#define __VEC2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <scalar.h>

typedef double Vec2d[2];
typedef float  Vec2f[2];
typedef int    Vec2i[2];
typedef double *Vec2d_t;
typedef float  *Vec2f_t;
typedef int    *Vec2i_t;
typedef const double *const_Vec2d_t;
typedef const float  *const_Vec2f_t;
typedef const int    *const_Vec2i_t;

EXTERND const Vec2d VEC2D_NULL;
EXTERND const Vec2f VEC2F_NULL;
EXTERND const Vec2i VEC2I_NULL;

/*
 * Function name     Hint
 * cmp               comparison returns an integer less than, equal to, or
 *                   greater than zero if a is found, respectively, to be
 *                   less than, to match, or be greater than b, in the
 *                   lexicographical sense
 * cross             the "cross product" of 2D vectors has many geometrical
 *                   uses: compute the third coordinate of a normal to a
 *                   plane (0, 0, z), a parallelogram area, a sine value, etc.
 * lerp              linear interpolation between a and b, t in the range 0..1
 *                   v = (1-t)*a + t*b
 * slerp             spherical linear interpolation between a and b,
 *                   t in the range 0..1
 *                   v = [1/sin(alpha)]*[sin((1-t)*alpha)*a + sin(t*alpha)*b]
 *                   a and b are unit vectors and the interpolant v is also
 *                   a unit vector (see Watt, 1992)
 */

/*
 * Vec2d declarations
 */
INLINED Vec2d_t vec2d_set      (Vec2d v, double x, double y);
INLINED Vec2d_t vec2d_eq       (Vec2d v, const Vec2d a);
INLINED Vec2d_t vec2d_addeq    (Vec2d v, const Vec2d a);
INLINED Vec2d_t vec2d_subeq    (Vec2d v, const Vec2d a);
INLINED Vec2d_t vec2d_multeq   (Vec2d v, double s);
INLINED Vec2d_t vec2d_diveq    (Vec2d v, double s);
INLINED Vec2d_t vec2d_add      (Vec2d v, const Vec2d a, const Vec2d b);
INLINED Vec2d_t vec2d_sub      (Vec2d v, const Vec2d a, const Vec2d b);
INLINED Vec2d_t vec2d_mult     (Vec2d v, const Vec2d a, double s);
INLINED Vec2d_t vec2d_div      (Vec2d v, const Vec2d a, double s);
INLINED Vec2d_t vec2d_neg      (Vec2d v);
INLINED Vec2d_t vec2d_absub    (Vec2d v, const Vec2d a, const Vec2d b);
INLINED int     vec2d_print    (const Vec2d v, FILE *stream);
INLINED int     vec2d_scan     (Vec2d v, FILE *stream);
INLINED int     vec2d_cmp      (const Vec2d a, const Vec2d b);
INLINED Vec2d_t vec2d_min      (Vec2d v, const Vec2d a, const Vec2d b);
INLINED Vec2d_t vec2d_max      (Vec2d v, const Vec2d a, const Vec2d b);
INLINED double  vec2d_sqnorm   (const Vec2d a);
INLINED double  vec2d_norm     (const Vec2d a);
INLINED Vec2d_t vec2d_normalize(Vec2d v);
INLINED double  vec2d_dot      (const Vec2d a, const Vec2d b);
INLINED double  vec2d_cross    (const Vec2d a, const Vec2d b);
INLINED double  vec2d_sqdist   (const Vec2d a, const Vec2d b);
INLINED double  vec2d_dist     (const Vec2d a, const Vec2d b);
INLINED double  vec2d_cosang   (const Vec2d a, const Vec2d b);
INLINED Vec2d_t vec2d_perp     (Vec2d v, const Vec2d a);
INLINED Vec2d_t vec2d_lerp     (Vec2d v, const Vec2d a, const Vec2d b,
                                double t);
INLINED Vec2d_t vec2d_slerp    (Vec2d v, const Vec2d a, const Vec2d b,
                                double t);

/*
 * Vec2f declarations
 */
INLINED Vec2f_t vec2f_set      (Vec2f v, float x, float y);
INLINED Vec2f_t vec2f_eq       (Vec2f v, const Vec2f a);
INLINED Vec2f_t vec2f_addeq    (Vec2f v, const Vec2f a);
INLINED Vec2f_t vec2f_subeq    (Vec2f v, const Vec2f a);
INLINED Vec2f_t vec2f_multeq   (Vec2f v, float s);
INLINED Vec2f_t vec2f_diveq    (Vec2f v, float s);
INLINED Vec2f_t vec2f_add      (Vec2f v, const Vec2f a, const Vec2f b);
INLINED Vec2f_t vec2f_sub      (Vec2f v, const Vec2f a, const Vec2f b);
INLINED Vec2f_t vec2f_mult     (Vec2f v, const Vec2f a, float s);
INLINED Vec2f_t vec2f_div      (Vec2f v, const Vec2f a, float s);
INLINED Vec2f_t vec2f_neg      (Vec2f v);
INLINED Vec2f_t vec2f_absub    (Vec2f v, const Vec2f a, const Vec2f b);
INLINED int     vec2f_print    (const Vec2f v, FILE *stream);
INLINED int     vec2f_scan     (Vec2f v, FILE *stream);
INLINED int     vec2f_cmp      (const Vec2f a, const Vec2f b);
INLINED Vec2f_t vec2f_min      (Vec2f v, const Vec2f a, const Vec2f b);
INLINED Vec2f_t vec2f_max      (Vec2f v, const Vec2f a, const Vec2f b);
INLINED float   vec2f_sqnorm   (const Vec2f a);
INLINED float   vec2f_norm     (const Vec2f a);
INLINED Vec2f_t vec2f_normalize(Vec2f v);
INLINED float   vec2f_dot      (const Vec2f a, const Vec2f b);
INLINED float   vec2f_cross    (const Vec2f a, const Vec2f b);
INLINED float   vec2f_sqdist   (const Vec2f a, const Vec2f b);
INLINED float   vec2f_dist     (const Vec2f a, const Vec2f b);
INLINED float   vec2f_cosang   (const Vec2f a, const Vec2f b);
INLINED Vec2f_t vec2f_perp     (Vec2f v, const Vec2f a);
INLINED Vec2f_t vec2f_lerp     (Vec2f v, const Vec2f a, const Vec2f b,
                                float t);
INLINED Vec2f_t vec2f_slerp    (Vec2f v, const Vec2f a, const Vec2f b,
                                double t);

/*
 * Vec2i declarations
 */
INLINED Vec2i_t vec2i_set      (Vec2i v, int x, int y);
INLINED Vec2i_t vec2i_eq       (Vec2i v, const Vec2i a);
INLINED int     vec2i_print    (const Vec2i v, FILE *stream);

/*
 * Vec2d definitions
 */
INLINED Vec2d_t
vec2d_set(Vec2d v, double x, double y) {
  v[0] = x;
  v[1] = y;
  return v;
}

INLINED Vec2d_t
vec2d_eq(Vec2d v, const Vec2d a) {
  v[0] = a[0];
  v[1] = a[1];
  return v;
}

INLINED Vec2d_t
vec2d_addeq(Vec2d v, const Vec2d a) {
  v[0] += a[0];
  v[1] += a[1];
  return v;
}

INLINED Vec2d_t
vec2d_subeq(Vec2d v, const Vec2d a) {
  v[0] -= a[0];
  v[1] -= a[1];
  return v;
}

INLINED Vec2d_t
vec2d_multeq(Vec2d v, double s) {
  v[0] *= s;
  v[1] *= s;
  return v;
}

INLINED Vec2d_t
vec2d_diveq(Vec2d v, double s) {
  v[0] /= s;
  v[1] /= s;
  return v;
}

INLINED Vec2d_t
vec2d_add(Vec2d v, const Vec2d a, const Vec2d b) {
  v[0] = a[0] + b[0];
  v[1] = a[1] + b[1];
  return v;
}

INLINED Vec2d_t
vec2d_sub(Vec2d v, const Vec2d a, const Vec2d b) {
  v[0] = a[0] - b[0];
  v[1] = a[1] - b[1];
  return v;
}

INLINED Vec2d_t
vec2d_mult(Vec2d v, const Vec2d a, double s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  return v;
}

INLINED Vec2d_t
vec2d_div(Vec2d v, const Vec2d a, double s) {
  v[0] = a[0] / s;
  v[1] = a[1] / s;
  return v;
}

INLINED Vec2d_t
vec2d_neg(Vec2d v) {
  v[0] = -v[0];
  v[1] = -v[1];
  return v;
}

INLINED Vec2d_t
vec2d_absub(Vec2d v, const Vec2d a, const Vec2d b) {
  v[0] = fabs(a[0] - b[0]);
  v[1] = fabs(a[1] - b[1]);
  return v;
}

INLINED int
vec2d_print(const Vec2d v, FILE *stream) {
  return (fprintf(stream, "%f %f", v[0], v[1]));
}

INLINED int
vec2d_scan(Vec2d v, FILE *stream) {
  return (fscanf(stream, "%lf %lf", &(v[0]), &(v[1])));
}

INLINED int
vec2d_cmp(const Vec2d a, const Vec2d b) {
  return (memcmp(a, b, sizeof(Vec2d)));
}

INLINED Vec2d_t
vec2d_min(Vec2d v, const Vec2d a, const Vec2d b) {
  v[0] = scald_min(a[0], b[0]);
  v[1] = scald_min(a[1], b[1]);
  return v;
}

INLINED Vec2d_t
vec2d_max(Vec2d v, const Vec2d a, const Vec2d b) {
  v[0] = scald_max(a[0], b[0]);
  v[1] = scald_max(a[1], b[1]);
  return v;
}

INLINED double
vec2d_sqnorm(const Vec2d a) {
  return ((a[0] * a[0]) + (a[1] * a[1]));
}

INLINED double
vec2d_norm(const Vec2d a) {
  return (sqrt(vec2d_sqnorm(a)));
}

INLINED Vec2d_t
vec2d_normalize(Vec2d v) {
  double nrm_inv = 1.0 / vec2d_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  return v;
}

INLINED double
vec2d_dot(const Vec2d a, const Vec2d b) {
  return ((a[0] * b[0]) + (a[1] * b[1]));
}

INLINED double
vec2d_cross(const Vec2d a, const Vec2d b) {
  return ((a[0] * b[1]) - (a[1] * b[0]));
}

INLINED double
vec2d_sqdist(const Vec2d a, const Vec2d b) {
  Vec2d d;
  vec2d_sub(d, a, b);
  return (vec2d_sqnorm(d));
}

INLINED double
vec2d_dist(const Vec2d a, const Vec2d b) {
  return (sqrt(vec2d_sqdist(a, b)));
}

INLINED double
vec2d_cosang(const Vec2d a, const Vec2d b) {
  return (vec2d_dot(a, b) / sqrt(vec2d_sqnorm(a) * vec2d_sqnorm(b)));
}

INLINED Vec2d_t
vec2d_perp(Vec2d v, const Vec2d a) {
  v[0] = - a[1];
  v[1] =   a[0];
  return v;
}

INLINED Vec2d_t
vec2d_lerp(Vec2d v, const Vec2d a, const Vec2d b, double t) {
  v[0] = (b[0] - a[0]) * t + a[0];
  v[1] = (b[1] - a[1]) * t + a[1];
  return v;
}

INLINED Vec2d_t
vec2d_slerp(Vec2d v, const Vec2d a, const Vec2d b, double t) {
  double alpha = acos(vec2d_dot(a, b)); /* unit vectors */
  double sin_alpha_inv = 1.0 / sin(alpha);
  double weight_a = sin((1.0 - t) * alpha) * sin_alpha_inv;
  double weight_b = sin(t * alpha) * sin_alpha_inv;
  assert(errno != EDOM); /* acos */
  v[0] = weight_a * a[0] + weight_b * b[0];
  v[1] = weight_a * a[1] + weight_b * b[1];
  return v;
}

/*
 * Vec2f definitions
 */
INLINED Vec2f_t
vec2f_set(Vec2f v, float x, float y) {
  v[0] = x;
  v[1] = y;
  return v;
}

INLINED Vec2f_t
vec2f_eq(Vec2f v, const Vec2f a) {
  v[0] = a[0];
  v[1] = a[1];
  return v;
}

INLINED Vec2f_t
vec2f_addeq(Vec2f v, const Vec2f a) {
  v[0] += a[0];
  v[1] += a[1];
  return v;
}

INLINED Vec2f_t
vec2f_subeq(Vec2f v, const Vec2f a) {
  v[0] -= a[0];
  v[1] -= a[1];
  return v;
}

INLINED Vec2f_t
vec2f_multeq(Vec2f v, float s) {
  v[0] *= s;
  v[1] *= s;
  return v;
}

INLINED Vec2f_t
vec2f_diveq(Vec2f v, float s) {
  v[0] /= s;
  v[1] /= s;
  return v;
}

INLINED Vec2f_t
vec2f_add(Vec2f v, const Vec2f a, const Vec2f b) {
  v[0] = a[0] + b[0];
  v[1] = a[1] + b[1];
  return v;
}

INLINED Vec2f_t
vec2f_sub(Vec2f v, const Vec2f a, const Vec2f b) {
  v[0] = a[0] - b[0];
  v[1] = a[1] - b[1];
  return v;
}

INLINED Vec2f_t
vec2f_mult(Vec2f v, const Vec2f a, float s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  return v;
}

INLINED Vec2f_t
vec2f_div(Vec2f v, const Vec2f a, float s) {
  v[0] = a[0] / s;
  v[1] = a[1] / s;
  return v;
}

INLINED Vec2f_t
vec2f_neg(Vec2f v) {
  v[0] = -v[0];
  v[1] = -v[1];
  return v;
}

INLINED Vec2f_t
vec2f_absub(Vec2f v, const Vec2f a, const Vec2f b) {
  v[0] = fabsf(a[0] - b[0]);
  v[1] = fabsf(a[1] - b[1]);
  return v;
}

INLINED int
vec2f_print(const Vec2f v, FILE *stream) {
  return (fprintf(stream, "%f %f", v[0], v[1]));
}

INLINED int
vec2f_scan(Vec2f v, FILE *stream) {
  return (fscanf(stream, "%f %f", &(v[0]), &(v[1])));
}

INLINED int
vec2f_cmp(const Vec2f a, const Vec2f b) {
  return (memcmp(a, b, sizeof(Vec2f)));
}

INLINED Vec2f_t
vec2f_min(Vec2f v, const Vec2f a, const Vec2f b) {
  v[0] = scalf_min(a[0], b[0]);
  v[1] = scalf_min(a[1], b[1]);
  return v;
}

INLINED Vec2f_t
vec2f_max(Vec2f v, const Vec2f a, const Vec2f b) {
  v[0] = scalf_max(a[0], b[0]);
  v[1] = scalf_max(a[1], b[1]);
  return v;
}

INLINED float
vec2f_sqnorm(const Vec2f a) {
  return ((a[0] * a[0]) + (a[1] * a[1]));
}

INLINED float
vec2f_norm(const Vec2f a) {
  return (sqrtf(vec2f_sqnorm(a)));
}

INLINED Vec2f_t
vec2f_normalize(Vec2f v) {
  float nrm_inv = 1.0f / vec2f_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  return v;
}

INLINED float
vec2f_dot(const Vec2f a, const Vec2f b) {
  return ((a[0] * b[0]) + (a[1] * b[1]));
}

INLINED float
vec2f_cross(const Vec2f a, const Vec2f b) {
  return ((a[0] * b[1]) - (a[1] * b[0]));
}

INLINED float
vec2f_sqdist(const Vec2f a, const Vec2f b) {
  Vec2f d;
  vec2f_sub(d, a, b);
  return (vec2f_sqnorm(d));
}

INLINED float
vec2f_dist(const Vec2f a, const Vec2f b) {
  return (sqrtf(vec2f_sqdist(a, b)));
}

INLINED float
vec2f_cosang(const Vec2f a, const Vec2f b) {
  return (vec2f_dot(a, b) / sqrtf(vec2f_sqnorm(a) * vec2f_sqnorm(b)));
}

INLINED Vec2f_t
vec2f_perp(Vec2f v, const Vec2f a) {
  v[0] = - a[1];
  v[1] =   a[0];
  return v;
}

INLINED Vec2f_t
vec2f_lerp(Vec2f v, const Vec2f a, const Vec2f b, float t) {
  v[0] = (b[0] - a[0]) * t + a[0];
  v[1] = (b[1] - a[1]) * t + a[1];
  return v;
}

INLINED Vec2f_t
vec2f_slerp(Vec2f v, const Vec2f a, const Vec2f b, double t) {
  double alpha = acos(vec2f_dot(a, b)); /* unit vectors */
  double sin_alpha_inv = 1.0 / sin(alpha);
  double weight_a = sin((1.0 - t) * alpha) * sin_alpha_inv;
  double weight_b = sin(t * alpha) * sin_alpha_inv;
  assert(errno != EDOM); /* acos */
  v[0] = (float) (weight_a * a[0] + weight_b * b[0]);
  v[1] = (float) (weight_a * a[1] + weight_b * b[1]);
  return v;
}

/*
 * Vec2i definitions
 */
INLINED Vec2i_t
vec2i_set(Vec2i v, int x, int y) {
  v[0] = x;
  v[1] = y;
  return v;
}

INLINED Vec2i_t
vec2i_eq(Vec2i v, const Vec2i a) {
  v[0] = a[0];
  v[1] = a[1];
  return v;
}

INLINED int
vec2i_print(const Vec2i v, FILE *stream) {
  return (fprintf(stream, "%i %i", v[0], v[1]));
}

#ifdef __cplusplus
}
#endif

#endif /* !__VEC2_H__ */
