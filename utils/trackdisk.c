#include "trackdisk.h"

static const int    RENORMALIZE_COUNT       = 128;
static const double TRANSLATION_SENSITIVITY = 512.0;
static const double SCALE_SENSITIVITY       = 2.0;
static const double SCALE_MIN               = 1.0E-6;

/*
 * Local functions declarations
 */
INLINED void _trackdisk_move_rotation(Trackdisk *td,
                                      const int x, const int y);
INLINED void _trackdisk_move_transl  (Trackdisk *td,
                                      const int x, const int y);
INLINED void _trackdisk_move_scale   (Trackdisk *td,
                                      const int x, const int y);
INLINED void build_complxd           (Complxd c,
                                      const double p1_x, const double p1_y,
                                      const double p2_x, const double p2_y);

/*
 * Local functions definitions
 */
INLINED void
_trackdisk_move_rotation(Trackdisk *td, const int x, const int y) {
  static int count = 0;
  static Complxd c;
  /*
   * Points coordinates are reported in the new 2D frame:
   * origin at screen center, x (respectively y) unit vector length equal to
   * half screen width (respectively height).
   */
  build_complxd(
    c,
    (double) (2*td->last_x   -   td->x_range) / (double) td->x_range,
    (double) (  td->y_range  - 2*td->last_y)  / (double) td->y_range,
    (double) (2*x            -   td->x_range) / (double) td->x_range,
    (double) (  td->y_range  - 2*y)           / (double) td->y_range
  );
  /*
   * Note: This routine normalizes the result every RENORMALIZE_COUNT times
   * it is called, to keep error from creeping in.
   */
  if (++count > RENORMALIZE_COUNT) {
    count = 0;
    complxd_normalize(complxd_multeq(td->rotation, c));
  } else {
    complxd_multeq(td->rotation, c);
  }
}

INLINED void
_trackdisk_move_transl(Trackdisk *td, const int x, const int y) {
  td->translation[0] += ((double) (x - td->last_x) / (double) td->x_range) *
    TRANSLATION_SENSITIVITY;
  td->translation[1] += ((double) (td->last_y - y) / (double) td->y_range) *
    TRANSLATION_SENSITIVITY;
}

INLINED void
_trackdisk_move_scale(Trackdisk *td, const int x, const int y) {
  td->scale += ((double) (y - td->last_y) / (double) td->y_range) *
    SCALE_SENSITIVITY;
  /*
   * Note: If scale is equal to zero, the transformation matrix is no longer
   * invertible! Thus we clamp the result to a minimum scale value.
   */
  if (td->scale < SCALE_MIN) {
    td->scale = SCALE_MIN;
  }
}

INLINED void
build_complxd(Complxd c, const double p1_x, const double p1_y,
                         const double p2_x, const double p2_y) {
  Complxd p1, p2;
  /*
   * Ok, simulate a trackdisk. Use the points P1 and P2 of the complex plane
   * to figure out the angle of rotation. The disk is located at (0,0).
   *
   * It is assumed that the arguments to this routine are in the
   * range [-1.0, 1.0]
   */
  if ((p1_x == p2_x) && (p1_y == p2_y)) {
    complxd_eq(c, COMPLXD_ID);
    return;
  }
  complxd_div(c, complxd_normalize(complxd_set(p2, p2_x, p2_y)),
                 complxd_normalize(complxd_set(p1, p1_x, p1_y)));
}

/*
 * Trackdisk definitions
 */
void
trackdisk_move(Trackdisk *td, const int x, const int y) {
  if (td->is_rotated) {
    _trackdisk_move_rotation(td, x, y);
  }
  if (td->is_translated) {
    _trackdisk_move_transl(td, x, y);
  }
  if (td->is_scaled) {
    _trackdisk_move_scale(td, x, y);
  }
  td->last_x = x;
  td->last_y = y;
}
