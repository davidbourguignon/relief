#include <quat.h>

static void
print_resultd(const Quatd q, const char *name) {
  printf("%s = (", name);
  printf("%f %f %f %f", q[0], q[1], q[2], q[3]);
  printf(")\n");
}

int
main(void) {
  Quatd Ad, Bd, Cd;
  Vec3d axisA = {+1.0, +2.0, +3.0};
  Vec3d axisB = {-1.0, -2.0, -3.0};
  
  printf("sizeof(Quatd): %d bytes\n", sizeof(Quatd));
  
  printf("\nset_axis_angle(A, (+1.0, +2.0, +3.0), PI/2)\n");
  print_resultd(quatd_set_axis_angle(Ad, axisA, M_PI_2), "A");
  
  printf("\nset_axis_angle(B, (-1.0, -2.0, -3.0), PI/2)\n");
  print_resultd(quatd_set_axis_angle(Bd, axisB, M_PI_2), "B");
  
  printf("\neq(C, QUAT_ID) and multeq(C, A)\n");
  print_resultd(quatd_multeq(quatd_eq(Cd, QUATD_ID), Ad), "C");
  
  printf("\nnormalize(A) and normalize(B)\n");
  print_resultd(quatd_normalize(Ad), "A");
  print_resultd(quatd_normalize(Bd), "B");
  
  printf("\nmult(C, A, B)\n");
  print_resultd(quatd_mult(Cd, Ad, Bd), "C");
  
  return 0;
}
