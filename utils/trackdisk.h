#ifndef __TRACKDISK_H__
#define __TRACKDISK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdlib.h>
#include <complx.h>

typedef struct _Trackdisk Trackdisk;

struct _Trackdisk {
  /* Current state */
  Complxd rotation;
  Vec2d translation;
  double scale;
  bool is_rotated;
  bool is_translated;
  bool is_scaled;
  
  /* Init state */
  Complxd rotation_init;
  Vec2d translation_init;
  double scale_init;
  
  /* Input coordinates info */
  int x_range, y_range;
  int last_x, last_y;
};

/*
 * Trackdisk declarations
 */
INLINED Trackdisk *trackdisk_new            (void);
INLINED void       trackdisk_delete         (Trackdisk *td);
INLINED Trackdisk *trackdisk_set_transf     (Trackdisk *td, const Complxd r,
                                             const Vec2d t, const double s);
INLINED Trackdisk *trackdisk_set_init_transf(Trackdisk *td, const Complxd r,
                                             const Vec2d t, const double s);
INLINED Trackdisk *trackdisk_init_transf    (Trackdisk *td);
INLINED Trackdisk *trackdisk_reshape        (Trackdisk *td,
                                             const int w, const int h);
INLINED void       trackdisk_start_rotation (Trackdisk *td,
                                             const int x, const int y);
INLINED void       trackdisk_start_transl   (Trackdisk *td,
                                             const int x, const int y);
INLINED void       trackdisk_start_scale    (Trackdisk *td,
                                             const int x, const int y);
EXTERND void       trackdisk_move           (Trackdisk *td,
                                             const int x, const int y);
INLINED void       trackdisk_stop_rotation  (Trackdisk *td);
INLINED void       trackdisk_stop_transl    (Trackdisk *td);
INLINED void       trackdisk_stop_scale     (Trackdisk *td);

/*
 * Trackdisk definitions
 */
INLINED Trackdisk *
trackdisk_new(void) {
  Trackdisk *td = (Trackdisk *) malloc(sizeof(Trackdisk));
  assert(td != NULL);
  complxd_eq(td->rotation, COMPLXD_ID);
  vec2d_eq(td->translation, VEC2D_NULL);
  td->scale = 1.0;
  td->is_rotated = td->is_translated = td->is_scaled = false;
  complxd_eq(td->rotation_init, COMPLXD_ID);
  vec2d_eq(td->translation_init, VEC2D_NULL);
  td->scale_init = 1.0;
  td->x_range = td->y_range = 0;
  td->last_x = td->last_y = 0;
  return td;
}

INLINED void
trackdisk_delete(Trackdisk *td) {
  assert(td != NULL);
  free(td);
#if DEBUG
  td = NULL;
#endif
}

INLINED Trackdisk *
trackdisk_set_transf(Trackdisk *td, const Complxd r, const Vec2d t,
                                    const double s) {
  complxd_eq(td->rotation, r);
  vec2d_eq(td->translation, t);
  td->scale = s;
  return td;
}

INLINED Trackdisk *
trackdisk_set_init_transf(Trackdisk *td, const Complxd r, const Vec2d t,
                                         const double s) {
  complxd_eq(td->rotation_init, r);
  vec2d_eq(td->translation_init, t);
  td->scale_init = s;
  return td;
}

INLINED Trackdisk *
trackdisk_init_transf(Trackdisk *td) {
  complxd_eq(td->rotation, td->rotation_init);
  vec2d_eq(td->translation, td->translation_init);
  td->scale = td->scale_init;
  return td;
}

INLINED Trackdisk *
trackdisk_reshape(Trackdisk *td, const int w, const int h) {
  td->x_range = w;
  td->y_range = h;
  return td;
}

INLINED void
trackdisk_start_rotation(Trackdisk *td, const int x, const int y) {
  td->is_rotated = true;
  td->last_x = x;
  td->last_y = y;
}

INLINED void
trackdisk_start_transl(Trackdisk *td, const int x, const int y) {
  td->is_translated = true;
  td->last_x = x;
  td->last_y = y;
}

INLINED void
trackdisk_start_scale(Trackdisk *td, const int x, const int y) {
  td->is_scaled = true;
  td->last_x = x;
  td->last_y = y;
}

INLINED void
trackdisk_stop_rotation(Trackdisk *td) {
  td->is_rotated = false;
}

INLINED void
trackdisk_stop_transl(Trackdisk *td) {
  td->is_translated = false;
}

INLINED void
trackdisk_stop_scale(Trackdisk *td) {
  td->is_scaled = false;
}

#ifdef __cplusplus
}
#endif

#endif /* !__TRACKDISK_H__ */
