#include <vec2.h>

static void
print_resultf(const Vec2f v, const char *name) {
  printf("%s = (", name);
  vec2f_print(v, stdout);
  printf(")\n");
}

static void
print_resultd(const Vec2d v, const char *name) {
  printf("%s = (", name);
  vec2d_print(v, stdout);
  printf(")\n");
}

int
main(void) {
  Vec2f Af, Bf, Cf;
  Vec2d Ad, Bd, Cd;
  
  printf("Computations made twice:\n");
  printf("Once with Vec2f and once with Vec2d.\n");
  printf("\n");
  printf("sizeof(Vec2f): %d bytes\n", sizeof(Vec2f));
  printf("sizeof(Vec2d): %d bytes\n", sizeof(Vec2d));
  
  printf("\nprint(A, stdout) uninitialized\n");
  print_resultf(Af, "A");
  print_resultd(Ad, "A");
  
  printf("\nset(A, 1.0, -2.0)\n");
  print_resultf(vec2f_set(Af, 1.0f, -2.0f), "A");
  print_resultd(vec2d_set(Ad, 1.0, -2.0), "A");
  
  printf("\neq(B, A)\n");
  print_resultf(vec2f_eq(Bf, Af), "B");
  print_resultd(vec2d_eq(Bd, Ad), "B");
  
  printf("\neq(C, VEC2_NULL) and addeq(C, A)\n");
  print_resultf(vec2f_addeq(vec2f_eq(Cf, VEC2F_NULL), Af), "C");
  print_resultd(vec2d_addeq(vec2d_eq(Cd, VEC2D_NULL), Ad), "C");
  
  printf("\nsubeq(C, A)\n");
  print_resultf(vec2f_subeq(Cf, Af), "C");
  print_resultd(vec2d_subeq(Cd, Ad), "C");
  
  printf("\nmulteq(A, 2.0)\n");
  print_resultf(vec2f_multeq(Af, 2.0f), "A");
  print_resultd(vec2d_multeq(Ad, 2.0), "A");
  
  printf("\ndiveq(A, 2.0)\n");
  print_resultf(vec2f_diveq(Af, 2.0f), "A");
  print_resultd(vec2d_diveq(Ad, 2.0), "A");
  
  printf("\nadd(C, B, A)\n");
  print_resultf(vec2f_add(Cf, Bf, Af), "C");
  print_resultd(vec2d_add(Cd, Bd, Ad), "C");
  
  printf("\nsub(C, B, A)\n");
  print_resultf(vec2f_sub(Cf, Bf, Af), "C");
  print_resultd(vec2d_sub(Cd, Bd, Ad), "C");
  
  printf("\nmult(A, B, 3.0)\n");
  print_resultf(vec2f_mult(Af, Bf, 3.0f), "A");
  print_resultd(vec2d_mult(Ad, Bd, 3.0), "A");
  
  printf("\ndiv(A, B, 3.0)\n");
  print_resultf(vec2f_div(Af, Bf, 3.0f), "A");
  print_resultd(vec2d_div(Ad, Bd, 3.0), "A");
  
  printf("\nneg(A)\n");
  print_resultf(vec2f_neg(Af), "A");
  print_resultd(vec2d_neg(Ad), "A");
  
  printf("\nscan(C) to 1.0 2.0 (enter vector twice)\n");
  vec2f_scan(Cf, stdin);
  vec2d_scan(Cd, stdin);
  print_resultf(Cf, "C");
  print_resultd(Cd, "C");
  
  printf("\nsqnorm(C) is 5.0\n");
  printf("%f\n", vec2f_sqnorm(Cf));
  printf("%f\n", vec2d_sqnorm(Cd));
  
  printf("\nnorm(C) is 2.236068\n");
  printf("%f\n", vec2f_norm(Cf));
  printf("%f\n", vec2d_norm(Cd));
  
  printf("\nnormalize(C) is (0.4472136, 0.89442719)\n");
  print_resultf(vec2f_normalize(Cf), "C");
  print_resultd(vec2d_normalize(Cd), "C");
  printf("norm(C) = %f\n", vec2f_norm(Cf));
  printf("norm(C) = %f\n", vec2d_norm(Cd));
  
  printf("\ndot(B, C) is -1.3416408\n");
  printf("%f\n", vec2f_dot(Bf, Cf));
  printf("%f\n", vec2d_dot(Bd, Cd));
  
  printf("\ncross(B, C) is 1.7888544\n");
  printf("%f\n", vec2f_cross(Bf, Cf));
  printf("%f\n", vec2d_cross(Bd, Cd));
  
  printf("\nsqdist(B, C) is 8.6832816\n");
  printf("%f\n", vec2f_sqdist(Bf, Cf));
  printf("%f\n", vec2d_sqdist(Bd, Cd));
  
  printf("\ndist(B, C) is 2.9467408\n");
  printf("%f\n", vec2f_dist(Bf, Cf));
  printf("%f\n", vec2d_dist(Bd, Cd));
  
  printf("\ncosang(B, C) is -0.60000001\n");
  printf("%f\n", vec2f_cosang(Bf, Cf));
  printf("%f\n", vec2d_cosang(Bd, Cd));
  
  printf("\nperp(A, C) is (-0.89442719, 0.4472136)\n");
  print_resultf(vec2f_perp(Af, Cf), "A");
  print_resultd(vec2d_perp(Ad, Cd), "A");
  printf("and cross(C, A) = %f\n", vec2f_cross(Cf, Af));
  printf("and cross(C, A) = %f\n", vec2d_cross(Cd, Ad));
  
  return 0;
}
