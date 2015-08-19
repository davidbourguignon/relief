#include <opengl_utils.h>

int
main(void) {
  GLmatd m, m_inv, m_i, m_copy;
  GLmatd MAd, MBd, Md;
  Quatd qAd, qBd;
  Complxd cAd, cBd;
  GLvecd u, v;
  Vec3d axisA = {+1.0, +2.0, +3.0};
  Vec3d axisB = {-1.0, -2.0, -3.0};
  GLboolean invertible = GL_FALSE;
  
  /* Initialization could have been done directly */
  m[0+4*0] = 1.0;   m[0+4*1] = 0.5;   m[0+4*2] = 0.5; m[0+4*3] = 0.0;
  m[1+4*0] = -0.25; m[1+4*1] = 1.0;   m[1+4*2] = 0.5; m[1+4*3] = 0.0;
  m[2+4*0] = -0.25; m[2+4*1] = -0.25; m[2+4*2] = 1.0; m[2+4*3] = 0.0;
  m[3+4*0] = 0.0;   m[3+4*1] = 0.0;   m[3+4*2] = 0.0; m[3+4*3] = 1.0;
  printf("Matrix\n");
  gl_matd_print(m, stdout);
  printf("\nInverse\n");
  invertible = gl_matd_invert(m, m_inv);
  assert(invertible);
  gl_matd_print(m_inv, stdout);
  printf("\nCheck Matrix*Inverse\n");
  gl_matd_multm(m_i, m, m_inv);
  gl_matd_print(m_i, stdout);
  printf("\nCopy\n");
  gl_matd_eq(m_copy, m);
  gl_matd_print(m_copy, stdout);
  
  /* Initialization could have been done directly */
  v[0] = 1.0;
  v[1] = 2.0;
  v[2] = 3.0;
  v[3] = 1.0;
  printf("\nVector\n");
  gl_vecd_print(v, stdout);
  printf("\nMult with Matrix\n");
  gl_matd_multv(u, m, v);
  gl_vecd_print(u, stdout);
  
  /* Complxd */
  printf("\nComplxd A and B\n");
  printf("complxd_set_angle(A, PI/8)\n");
  complxd_set_angle(cAd, M_PI_4/2);
  printf("complxd_set_angle(B, -PI/8)\n");
  complxd_set_angle(cBd, -M_PI_4/2);
  
  printf("\nfrom_complxd(MA, A)\n");
  gl_matd_print(gl_matd_from_complxd(MAd, cAd), stdout);
  
  printf("\nfrom_complxd(MB, B)\n");
  gl_matd_print(gl_matd_from_complxd(MBd, cBd), stdout);
  
  printf("\ngl_matd_multm(M, MA, MB)\n");
  gl_matd_print(gl_matd_multm(Md, MAd, MBd), stdout);
  
  /* Quatd */
  printf("\nQuatd A and B\n");
  printf("quatd_set_axis_angle(A, (+1.0, +2.0, +3.0), PI/2)\n");
  quatd_set_axis_angle(qAd, axisA, M_PI_2);
  printf("quatd_set_axis_angle(B, (-1.0, -2.0, -3.0), PI/2)\n");
  quatd_set_axis_angle(qBd, axisB, M_PI_2);
  
  printf("\nfrom_quatd(A, MA)\n");
  gl_matd_print(gl_matd_from_quatd(qAd, MAd), stdout);
  
  printf("\nfrom_quatd(B, MB)\n");
  gl_matd_print(gl_matd_from_quatd(qBd, MBd), stdout);
  
  printf("\ngl_matd_multm(M, MA, MB)\n");
  gl_matd_print(gl_matd_multm(Md, MAd, MBd), stdout);
  
  return 0;
}
