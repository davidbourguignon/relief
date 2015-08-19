#include <complx.h>

static void
print_resultd(const Complxd c, const char *name) {
  printf("%s = (", name);
  printf("%f %f", c[0], c[1]);
  printf(")\n");
}

int
main(void) {
  Complxd Ad, Bd, Cd, Dd;
  
  printf("sizeof(Complxd): %d bytes\n", sizeof(Complxd));
  
  printf("\nset_angle(A, PI/8)\n");
  print_resultd(complxd_set_angle(Ad, M_PI_4/2), "A");
  
  printf("\nset_angle(B, -PI/8)\n");
  print_resultd(complxd_set_angle(Bd, -M_PI_4/2), "B");
  
  printf("\neq(C, COMPLX_ID) and multeq(C, A)\n");
  print_resultd(complxd_multeq(complxd_eq(Cd, COMPLXD_ID), Ad), "C");
  
  printf("\neq(D, COMPLX_ID) and diveq(D, B)\n");
  print_resultd(complxd_diveq(complxd_eq(Dd, COMPLXD_ID), Bd), "D");
  
  printf("\nnormalize(A) and normalize(B)\n");
  print_resultd(complxd_normalize(Ad), "A");
  print_resultd(complxd_normalize(Bd), "B");
  
  printf("\nmult(C, A, B)\n");
  print_resultd(complxd_mult(Cd, Ad, Bd), "C");
  
  printf("\ndiv(D, A, B)\n");
  print_resultd(complxd_div(Dd, Ad, Bd), "D");
  
  return 0;
}
