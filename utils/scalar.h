#ifndef __SCALAR_H__
#define __SCALAR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdio.h>
#include <math.h>

/*
 * Function name     Hint
 * lerp              linearly interpolates between a and b
 *                   with t in the range 0..1 such as (1 - t)*a + t*b
 */

/*
 * Scald declarations
 */
INLINED double scald_min  (double a, double b);
INLINED double scald_max  (double a, double b);
INLINED double scald_clamp(double a, double min, double max);
INLINED double scald_lerp (double a, double b, double t);
INLINED double scald_log2 (double a);
INLINED double scald_read (FILE *stream);
INLINED int    scald_write(double a, FILE *stream);

/*
 * Scalf declarations
 */
INLINED float scalf_min  (float a, float b);
INLINED float scalf_max  (float a, float b);
INLINED float scalf_clamp(float a, float min, float max);
INLINED float scalf_lerp (float a, float b, float t);
INLINED float scalf_read (FILE *stream);
INLINED int   scalf_write(float a, FILE *stream);

/*
 * Scali declarations
 */
INLINED int scali_min                 (int a, int b);
INLINED int scali_max                 (int a, int b);
INLINED int scali_clamp               (int a, int min, int max);
INLINED int scali_is_power_of_two     (int a);
INLINED int scali_nearest_power_of_two(unsigned int a);
INLINED int scali_read                (FILE *stream);
INLINED int scali_write               (int a, FILE *stream);

/*
 * Scald definitions
 */
INLINED double
scald_min(double a, double b) {
  return ((a < b) ? a : b);
}

INLINED double
scald_max(double a, double b) {
  return ((a > b) ? a : b);
}

INLINED double
scald_clamp(double a, double min, double max) {
  return (scald_max(min, scald_min(max, a)));
}

INLINED double
scald_lerp(double a, double b, double t) {
  return ((b - a) * t + a);
}

INLINED double
scald_log2(double a) {
  return (log(a) / M_LN2);
}

INLINED double
scald_read(FILE *stream) {
  unsigned char buf[sizeof(double)];
  double *tmp = (double *) buf;
  
  fread(buf, sizeof(double), 1, stream);
  return (*tmp);
}

INLINED int
scald_write(double a, FILE *stream) {
  return (fwrite(&a, sizeof(double), 1, stream));
}

/*
 * Scalf definitions
 */
INLINED float
scalf_min(float a, float b) {
  return ((a < b) ? a : b);
}

INLINED float
scalf_max(float a, float b) {
  return ((a > b) ? a : b);
}

INLINED float
scalf_clamp(float a, float min, float max) {
  return (scalf_max(min, scalf_min(max, a)));
}

INLINED float
scalf_lerp(float a, float b, float t) {
  return ((b - a) * t + a);
}

INLINED float
scalf_read(FILE *stream) {
  unsigned char buf[sizeof(float)];
  float *tmp = (float *) buf;
  
  fread(buf, sizeof(float), 1, stream);
  return (*tmp);
}

INLINED int
scalf_write(float a, FILE *stream) {
  return (fwrite(&a, 4, 1, stream)); /* 32 bits float */
}

/*
 * Scali definitions
 */
INLINED int
scali_min(int a, int b) {
  return ((a < b) ? a : b);
}

INLINED int
scali_max(int a, int b) {
  return ((a > b) ? a : b);
}

INLINED int
scali_clamp(int a, int min, int max) {
  return (scali_max(min, scali_min(max, a)));
}

INLINED int
scali_is_power_of_two(int a) {
  return (a > 0 && (a & (a - 1)) == 0);
}

INLINED int
scali_nearest_power_of_two(unsigned int a) {
  /*
  ** Adapted from sgi opengl sample implementation nearestPower()
  */
  int i = 1;
  
  if (a == 0) { /* Error! */
    return -1;
  }
  for (;;) {
    if (a == 1) {
      return i;
    } else if (a == 3) {
      return i*4;
    }
    a = a >> 1;
    i *= 2;
  }
}

INLINED int
scali_read(FILE *stream) {
  unsigned char buf[sizeof(int)];
  int *tmp = (int *) buf;
  
  fread(buf, sizeof(int), 1, stream);
  return (*tmp);
}

INLINED int
scali_write(int a, FILE *stream) {
  return (fwrite(&a, sizeof(int), 1, stream));
}

#ifdef __cplusplus
}
#endif

#endif /* !__SCALAR_H__ */
