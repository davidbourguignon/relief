#include <vec3.h>

static void
print_resultf(const Vec3f v, const char *name) {
  printf("%s = (", name);
  vec3f_print(v, stdout);
  printf(")\n");
}

static void
print_resultd(const Vec3d v, const char *name) {
  printf("%s = (", name);
  vec3d_print(v, stdout);
  printf(")\n");
}

int
main(void) {
  Vec3f Af, Bf, Cf;
  Vec3d Ad, Bd, Cd;
  
  printf("Computations made twice:\n");
  printf("Once with Vec3f and once with Vec3d.\n");
  printf("\n");
  printf("sizeof(Vec3f): %d bytes\n", sizeof(Vec3f));
  printf("sizeof(Vec3d): %d bytes\n", sizeof(Vec3d));
  
  printf("\nprint(A, stdout) uninitialized\n");
  print_resultf(Af, "A");
  print_resultd(Ad, "A");
  
  printf("\nset(A, 1.0, -2.0, -3.0)\n");
  print_resultf(vec3f_set(Af, 1.0f, -2.0f, -3.0f), "A");
  print_resultd(vec3d_set(Ad, 1.0, -2.0, -3.0), "A");
  
  printf("\neq(B, A)\n");
  print_resultf(vec3f_eq(Bf, Af), "B");
  print_resultd(vec3d_eq(Bd, Ad), "B");
  
  printf("\neq(C, VEC3_NULL) and addeq(C, A)\n");
  print_resultf(vec3f_addeq(vec3f_eq(Cf, VEC3F_NULL), Af), "C");
  print_resultd(vec3d_addeq(vec3d_eq(Cd, VEC3D_NULL), Ad), "C");
  
  printf("\nsubeq(C, A)\n");
  print_resultf(vec3f_subeq(Cf, Af), "C");
  print_resultd(vec3d_subeq(Cd, Ad), "C");
  
  printf("\nmulteq(A, 2.0)\n");
  print_resultf(vec3f_multeq(Af, 2.0f), "A");
  print_resultd(vec3d_multeq(Ad, 2.0), "A");
  
  printf("\ndiveq(A, 2.0)\n");
  print_resultf(vec3f_diveq(Af, 2.0f), "A");
  print_resultd(vec3d_diveq(Ad, 2.0), "A");
  
  printf("\nadd(C, B, A)\n");
  print_resultf(vec3f_add(Cf, Bf, Af), "C");
  print_resultd(vec3d_add(Cd, Bd, Ad), "C");
  
  printf("\nsub(C, B, A)\n");
  print_resultf(vec3f_sub(Cf, Bf, Af), "C");
  print_resultd(vec3d_sub(Cd, Bd, Ad), "C");
  
  printf("\nmult(A, B, 3.0)\n");
  print_resultf(vec3f_mult(Af, Bf, 3.0f), "A");
  print_resultd(vec3d_mult(Ad, Bd, 3.0), "A");
  
  printf("\ndiv(A, B, 3.0)\n");
  print_resultf(vec3f_div(Af, Bf, 3.0f), "A");
  print_resultd(vec3d_div(Ad, Bd, 3.0), "A");
  
  printf("\nneg(A)\n");
  print_resultf(vec3f_neg(Af), "A");
  print_resultd(vec3d_neg(Ad), "A");
  
  printf("\nscan(C) to 1.0 2.0 3.0 (enter vector twice)\n");
  vec3f_scan(Cf, stdin);
  vec3d_scan(Cd, stdin);
  print_resultf(Cf, "C");
  print_resultd(Cd, "C");
  
  printf("\nsqnorm(C) is 14.0\n");
  printf("%f\n", vec3f_sqnorm(Cf));
  printf("%f\n", vec3d_sqnorm(Cd));
  
  printf("\nnorm(C) is 3.7416574\n");
  printf("%f\n", vec3f_norm(Cf));
  printf("%f\n", vec3d_norm(Cd));
  
  printf("\nnormalize(C) is (0.26726124, 0.53452248, 0.80178373)\n");
  print_resultf(vec3f_normalize(Cf), "C");
  print_resultd(vec3d_normalize(Cd), "C");
  printf("norm(C) = %f\n", vec3f_norm(Cf));
  printf("norm(C) = %f\n", vec3d_norm(Cd));
  
  printf("\ndot(B, C) is -3.2071349\n");
  printf("%f\n", vec3f_dot(Bf, Cf));
  printf("%f\n", vec3d_dot(Bd, Cd));
  
  printf("\ncross(A, B, C) is (-2.0E-8, -1.6035674, 1.069045)\n");
  print_resultf(vec3f_cross(Af, Bf, Cf), "A");
  print_resultd(vec3d_cross(Ad, Bd, Cd), "A");
  
  printf("\nsqdist(B, C) is 21.41427\n");
  printf("%f\n", vec3f_sqdist(Bf, Cf));
  printf("%f\n", vec3d_sqdist(Bd, Cd));
  
  printf("\ndist(B, C) is 4.6275555\n");
  printf("%f\n", vec3f_dist(Bf, Cf));
  printf("%f\n", vec3d_dist(Bd, Cd));
  
  printf("\ncosang(B, C) is -0.85714285\n");
  printf("%f\n", vec3f_cosang(Bf, Cf));
  printf("%f\n", vec3d_cosang(Bd, Cd));
  
  return 0;
}
