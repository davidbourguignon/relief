/*
 ** Adapted from sgi virtual trackball implementation, November 1988.
 */

#include "trackball.h"

static const int    RENORMALIZE_COUNT       = 128;
static const double TRANSLATION_SENSITIVITY = 4.0;
static const double TRACKBALL_SIZE          = 0.8;

/*
 * Local functions declarations
 */
INLINED void  _trackball_move_rotation  (Trackball *tb,
                                         const int x, const int y);
INLINED void  _trackball_move_transl_xy (Trackball *tb,
                                         const int x, const int y);
INLINED void  _trackball_move_transl_z  (Trackball *tb,
                                         const int x, const int y);
static  void  build_quatd               (Quatd q,
                                         const double p1_x,
                                         const double p1_y,
                                         const double p2_x,
                                         const double p2_y);
static double project_to_deformed_sphere(const double x, const double y);

/*
 * Local functions definitions
 */
INLINED void
_trackball_move_rotation(Trackball *tb, const int x, const int y) {
  static int count = 0;
  static Quatd q;
  /*
   * Points coordinates are reported in the new 2D frame:
   * origin at screen center, x (respectively y) unit vector length equal to
   * half screen width (respectively height).
   */
  build_quatd(
    q,
    (double) (2*tb->last_x   -   tb->x_range) / (double) tb->x_range,
    (double) (  tb->y_range  - 2*tb->last_y)  / (double) tb->y_range,
    (double) (2*x            -   tb->x_range) / (double) tb->x_range,
    (double) (  tb->y_range  - 2*y)           / (double) tb->y_range
  );
  /*
   * Note: This routine normalizes the result every RENORMALIZE_COUNT times
   * it is called, to keep error from creeping in.
   */
  if (++count > RENORMALIZE_COUNT) {
    count = 0;
    quatd_normalize(quatd_multeq(tb->rotation, q));
  } else {
    quatd_multeq(tb->rotation, q);
  }
}

INLINED void
_trackball_move_transl_xy(Trackball *tb, const int x, const int y) {
  tb->translation[0] += ((double) (x - tb->last_x) / (double) tb->x_range) *
    TRANSLATION_SENSITIVITY;
  tb->translation[1] += ((double) (tb->last_y - y) / (double) tb->y_range) *
    TRANSLATION_SENSITIVITY;
}

INLINED void
_trackball_move_transl_z(Trackball *tb, const int x, const int y) {
  tb->translation[2] += ((double) (y - tb->last_y) / (double) tb->y_range) *
    TRANSLATION_SENSITIVITY;
}

static void
build_quatd(Quatd q, const double p1_x, const double p1_y,
                     const double p2_x, const double p2_y) {
  Vec3d p1_projected, p2_projected, axis;
  double sin_half_angle, cos_half_angle;
  /*
   * Ok, simulate a trackball. Project the points P1 and P2 onto the virtual
   * trackball, to obtain P1p and P2p, then figure out the axis of rotation,
   * which is the cross product of O P1 and O P2, where O is the center of the
   * ball located at (0,0,0).
   * Note: This is a deformed trackball -- it is a trackball in the center,
   * but is deformed into a hyperbolic sheet of rotation away from the center.
   * This particular function was chosen after trying out several variations.
   *
   * It is assumed that the arguments to this routine are in the
   * range [-1.0, 1.0]
   */
  if ((p1_x == p2_x) && (p1_y == p2_y)) {
    quatd_eq(q, QUATD_ID);
    return;
  }
  
  /* Axis */
  vec3d_set(p1_projected, p1_x, p1_y, project_to_deformed_sphere(p1_x, p1_y));
  vec3d_set(p2_projected, p2_x, p2_y, project_to_deformed_sphere(p2_x, p2_y));
  vec3d_normalize(vec3d_cross(axis, p2_projected, p1_projected));
  
  /* Angle */
  sin_half_angle
    = vec3d_dist(p1_projected, p2_projected) / (2.0 * TRACKBALL_SIZE);
  if (sin_half_angle > 1.0) {
    sin_half_angle = 1.0;
  } else if (sin_half_angle < -1.0) {
    sin_half_angle = -1.0;
  }
  cos_half_angle = sqrt(1.0 - (sin_half_angle * sin_half_angle));
  
  q[0] =           cos_half_angle;
  q[1] = axis[0] * sin_half_angle;
  q[2] = axis[1] * sin_half_angle;
  q[3] = axis[2] * sin_half_angle;
}

static double
project_to_deformed_sphere(const double x, const double y) {
  const double dd = x*x + y*y;
  const double d  = sqrt(dd);
  /*
   * Project an x, y pair onto a sphere of radius r OR a hyperbolic sheet
   * if we are away from the center of the sphere.
   */
  if (d < TRACKBALL_SIZE * M_SQRT1_2) {
    /*
     * Inside sphere:
     * TRACKBALL_SIZE^2 = dd + z^2
     */
    return (sqrt(TRACKBALL_SIZE * TRACKBALL_SIZE - dd));
  } else {
    /* On hyperbola */
    return ((TRACKBALL_SIZE * TRACKBALL_SIZE) / (2.0 * d));
  }
}

/*
 * Trackball definitions
 */
void
trackball_move(Trackball *tb, const int x, const int y) {
  if (tb->is_rotated) {
    _trackball_move_rotation(tb, x, y);
  }
  if (tb->is_translated_xy) {
    _trackball_move_transl_xy(tb, x, y);
  }
  if (tb->is_translated_z) {
    _trackball_move_transl_z(tb, x, y);
  }
  tb->last_x = x;
  tb->last_y = y;
}
