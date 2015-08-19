#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdlib.h>
#include <quat.h>

typedef struct _Trackball Trackball;

struct _Trackball {
  /* Current state */
  Quatd rotation;
  Vec3d translation;
  bool is_rotated;
  bool is_translated_xy, is_translated_z;
  
  /* Init state */
  Quatd rotation_init;
  Vec3d translation_init;
  
  /* Input coordinates info */
  int x_range, y_range;
  int last_x, last_y;
};

/*
 * Trackball declarations
 */
INLINED Trackball *trackball_new            (void);
INLINED void       trackball_delete         (Trackball *tb);
INLINED Trackball *trackball_set_transf     (Trackball *tb,
                                             const Quatd r, const Vec3d t);
INLINED Trackball *trackball_set_init_transf(Trackball *tb,
                                             const Quatd r, const Vec3d t);
INLINED Trackball *trackball_init_transf    (Trackball *tb);
INLINED Trackball *trackball_reshape        (Trackball *tb,
                                             const int w, const int h);
INLINED void       trackball_start_rotation (Trackball *tb,
                                             const int x, const int y);
INLINED void       trackball_start_transl_xy(Trackball *tb,
                                             const int x, const int y);
INLINED void       trackball_start_transl_z (Trackball *tb,
                                             const int x, const int y);
EXTERND void       trackball_move           (Trackball *tb,
                                             const int x, const int y);
INLINED void       trackball_stop_rotation  (Trackball *tb);
INLINED void       trackball_stop_transl_xy (Trackball *tb);
INLINED void       trackball_stop_transl_z  (Trackball *tb);

/*
 * Trackball definitions
 */
INLINED Trackball *
trackball_new(void) {
  Trackball *tb = (Trackball *) malloc(sizeof(Trackball));
  assert(tb != NULL);
  quatd_eq(tb->rotation, QUATD_ID);
  vec3d_eq(tb->translation, VEC3D_NULL);
  tb->is_rotated = tb->is_translated_xy = tb->is_translated_z = false;
  quatd_eq(tb->rotation_init, QUATD_ID);
  vec3d_eq(tb->translation_init, VEC3D_NULL);
  tb->x_range = tb->y_range = 0;
  tb->last_x = tb->last_y = 0;
  return tb;
}

INLINED void
trackball_delete(Trackball *tb) {
  assert(tb != NULL);
  free(tb);
#if DEBUG
  tb = NULL;
#endif
}

INLINED Trackball *
trackball_set_transf(Trackball *tb, const Quatd r, const Vec3d t) {
  quatd_eq(tb->rotation, r);
  vec3d_eq(tb->translation, t);
  return tb;
}

INLINED Trackball *
trackball_set_init_transf(Trackball *tb, const Quatd r, const Vec3d t) {
  quatd_eq(tb->rotation_init, r);
  vec3d_eq(tb->translation_init, t);
  return tb;
}

INLINED Trackball *
trackball_init_transf(Trackball *tb) {
  quatd_eq(tb->rotation, tb->rotation_init);
  vec3d_eq(tb->translation, tb->translation_init);
  return tb;
}

INLINED Trackball *
trackball_reshape(Trackball *tb, const int w, const int h) {
  tb->x_range = w;
  tb->y_range = h;
  return tb;
}

INLINED void
trackball_start_rotation(Trackball *tb, const int x, const int y) {
  tb->is_rotated = true;
  tb->last_x = x;
  tb->last_y = y;
}

INLINED void
trackball_start_transl_xy(Trackball *tb, const int x, const int y) {
  tb->is_translated_xy = true;
  tb->last_x = x;
  tb->last_y = y;
}

INLINED void
trackball_start_transl_z(Trackball *tb, const int x, const int y) {
  tb->is_translated_z = true;
  tb->last_x = x;
  tb->last_y = y;
}

INLINED void
trackball_stop_rotation(Trackball *tb) {
  tb->is_rotated = false;
}

INLINED void
trackball_stop_transl_xy(Trackball *tb) {
  tb->is_translated_xy = false;
}

INLINED void
trackball_stop_transl_z(Trackball *tb) {
  tb->is_translated_z = false;
}

#ifdef __cplusplus
}
#endif

#endif /* !__TRACKBALL_H__ */
