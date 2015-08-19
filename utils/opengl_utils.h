#ifndef __OPENGL_UTILS_H__
#define __OPENGL_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <opengl_defs.h>
#include <GL/glu.h>
#include <trackdisk.h>
#include <trackball.h>

typedef GLdouble GLvecd[4];
typedef GLfloat  GLvecf[4];
typedef GLint    GLveci[4];
typedef GLubyte  GLvecub[4];
typedef GLdouble GLmatd[16];
typedef GLdouble *GLvecd_t;
typedef GLfloat  *GLvecf_t;
typedef GLint    *GLveci_t;
typedef GLdouble *GLmatd_t;
typedef const GLdouble *const_GLvecd_t;
typedef const GLfloat  *const_GLvecf_t;
typedef const GLint    *const_GLveci_t;
typedef const GLdouble *const_GLmatd_t;
typedef struct _GLtransf  GLtransf;
typedef struct _GLtexture GLtexture;
typedef struct _GLlist    GLlist;
typedef struct _GLerror   GLerror;

struct _GLtransf {
  /* Transformations */
  GLveci viewport;
  GLmatd projection_matrix, modelview_matrix;
  
  /* Precomputations */
  GLmatd projection_matrix_inverse, modelview_matrix_inverse;
  GLmatd final_matrix, final_matrix_inverse;
  GLboolean projection_changed, modelview_changed;
  GLboolean changed;
};

struct _GLtexture {
  GLuint name;
  GLenum target;
  GLsizei width, height, depth;
  GLint nlevels;
  GLenum format;
  /*
   * GLboolean setup_texture(void *data, GLboolean test_proxy)
   * will call one or more glTexParameter*() and only one gl*TexImage*D() at
   * the end. It will use an if-else construct to check the requested
   * texture state using a proxy and return the result.
   */
  GLboolean (*setup)(void *, GLboolean);
  void *setup_data;
};

struct _GLlist {
  GLuint name;
  /*
   * GLboolean display_list(void *data, GLboolean test_proxy)
   * will use an if-else construct to check the requested state using a
   * proxy and return the result. (Relevant for glColorTable(), etc.)
   */
  GLboolean (*display)(void *, GLboolean);
  void *display_data;
};

struct _GLerror {
  GLenum code;
  const GLubyte *report_string;
};

EXTERND const GLvecd GL_VECD_NULL;
EXTERND const GLvecf GL_VECF_NULL;
EXTERND const GLveci GL_VECI_NULL;
EXTERND const GLvecd GL_VECD_ORIGIN;
EXTERND const GLvecf GL_VECF_ORIGIN;
EXTERND const GLmatd GL_MATD_NULL;
EXTERND const GLmatd GL_MATD_ID;

EXTERND const GLubyte  GL_UBYTE_MIN;
EXTERND const GLubyte  GL_UBYTE_MAX;
EXTERND const GLushort GL_USHORT_MIN;
EXTERND const GLushort GL_USHORT_MAX;
EXTERND const GLuint   GL_UINT_MIN;
EXTERND const GLuint   GL_UINT_MAX;
EXTERND const GLclampf GL_CLAMPF_MIN;
EXTERND const GLclampf GL_CLAMPF_MAX;
EXTERND const GLclampd GL_CLAMPD_MIN;
EXTERND const GLclampd GL_CLAMPD_MAX;

EXTERND const GLfloat GL_DEPTH_NEAR;
EXTERND const GLfloat GL_DEPTH_FAR;

EXTERND const GLuint GL_TEXTURE_NULL_NAME;
EXTERND const GLuint GL_LIST_NULL_NAME;

EXTERND const GLvecf GL_PURE_BLACK;
EXTERND const GLvecf GL_PURE_WHITE;
EXTERND const GLvecf GL_PURE_GRAY;
EXTERND const GLvecf GL_PURE_RED;
EXTERND const GLvecf GL_PURE_GREEN;
EXTERND const GLvecf GL_PURE_BLUE;
EXTERND const GLvecf GL_PURE_CYAN;
EXTERND const GLvecf GL_PURE_MAGENTA;
EXTERND const GLvecf GL_PURE_YELLOW;

EXTERND const GLvecf GL_PASTEL_RED;
EXTERND const GLvecf GL_PASTEL_GREEN;
EXTERND const GLvecf GL_PASTEL_BLUE;
EXTERND const GLvecf GL_PASTEL_CYAN;
EXTERND const GLvecf GL_PASTEL_MAGENTA;
EXTERND const GLvecf GL_PASTEL_YELLOW;

EXTERND const GLvecf GL_TRNSP_BLACK;
EXTERND const GLvecf GL_TRNSP_WHITE;
EXTERND const GLvecf GL_TRNSP_GRAY;
EXTERND const GLvecf GL_TRNSP_RED;
EXTERND const GLvecf GL_TRNSP_GREEN;
EXTERND const GLvecf GL_TRNSP_BLUE;
EXTERND const GLvecf GL_TRNSP_CYAN;
EXTERND const GLvecf GL_TRNSP_MAGENTA;
EXTERND const GLvecf GL_TRNSP_YELLOW;

/*
 * GLvecd declarations
 */
INLINED GLvecd_t gl_vecd_set      (GLvecd v, GLdouble x, GLdouble y,
                                             GLdouble z, GLdouble w);
INLINED GLvecd_t gl_vecd_eq       (GLvecd v, const GLvecd a);
INLINED GLvecd_t gl_vecd_addeq    (GLvecd v, const GLvecd a);
INLINED GLvecd_t gl_vecd_subeq    (GLvecd v, const GLvecd a);
INLINED GLvecd_t gl_vecd_multeq   (GLvecd v, GLdouble s);
INLINED GLvecd_t gl_vecd_diveq    (GLvecd v, GLdouble s);
INLINED GLvecd_t gl_vecd_add      (GLvecd v, const GLvecd a, const GLvecd b);
INLINED GLvecd_t gl_vecd_sub      (GLvecd v, const GLvecd a, const GLvecd b);
INLINED GLvecd_t gl_vecd_mult     (GLvecd v, const GLvecd a, GLdouble s);
INLINED GLvecd_t gl_vecd_div      (GLvecd v, const GLvecd a, GLdouble s);
INLINED GLvecd_t gl_vecd_neg      (GLvecd v);
INLINED int      gl_vecd_print    (const GLvecd v, FILE *stream);
INLINED int      gl_vecd_scan     (GLvecd v, FILE *stream);
INLINED int      gl_vecd_cmp      (const GLvecd a, const GLvecd b);
INLINED GLdouble gl_vecd_sqnorm   (const GLvecd a);
INLINED GLdouble gl_vecd_norm     (const GLvecd a);
INLINED GLvecd_t gl_vecd_normalize(GLvecd v);
INLINED GLdouble gl_vecd_dot      (const GLvecd a, const GLvecd b);
INLINED GLdouble gl_vecd_sqdist   (const GLvecd a, const GLvecd b);
INLINED GLdouble gl_vecd_dist     (const GLvecd a, const GLvecd b);

/*
 * GLvecf declarations
 */
INLINED GLvecf_t gl_vecf_set      (GLvecf v, GLfloat x, GLfloat y,
                                             GLfloat z, GLfloat w);
INLINED GLvecf_t gl_vecf_eq       (GLvecf v, const GLvecf a);
INLINED GLvecf_t gl_vecf_addeq    (GLvecf v, const GLvecf a);
INLINED GLvecf_t gl_vecf_subeq    (GLvecf v, const GLvecf a);
INLINED GLvecf_t gl_vecf_multeq   (GLvecf v, GLfloat s);
INLINED GLvecf_t gl_vecf_diveq    (GLvecf v, GLfloat s);
INLINED GLvecf_t gl_vecf_add      (GLvecf v, const GLvecf a, const GLvecf b);
INLINED GLvecf_t gl_vecf_sub      (GLvecf v, const GLvecf a, const GLvecf b);
INLINED GLvecf_t gl_vecf_mult     (GLvecf v, const GLvecf a, GLfloat s);
INLINED GLvecf_t gl_vecf_div      (GLvecf v, const GLvecf a, GLfloat s);
INLINED GLvecf_t gl_vecf_neg      (GLvecf v);
INLINED int      gl_vecf_print    (const GLvecf v, FILE *stream);
INLINED int      gl_vecf_scan     (GLvecf v, FILE *stream);
INLINED int      gl_vecf_cmp      (const GLvecf a, const GLvecf b);
INLINED GLfloat  gl_vecf_sqnorm   (const GLvecf a);
INLINED GLfloat  gl_vecf_norm     (const GLvecf a);
INLINED GLvecf_t gl_vecf_normalize(GLvecf v);
INLINED GLfloat  gl_vecf_dot      (const GLvecf a, const GLvecf b);
INLINED GLfloat  gl_vecf_sqdist   (const GLvecf a, const GLvecf b);
INLINED GLfloat  gl_vecf_dist     (const GLvecf a, const GLvecf b);

/*
 * GLveci declarations
 */
INLINED GLveci_t gl_veci_set  (GLveci v, GLint x, GLint y, GLint z, GLint w);
INLINED GLveci_t gl_veci_eq   (GLveci v, const GLveci a);
INLINED int      gl_veci_print(const GLveci v, FILE *stream);

/*
 * GLmatd declarations
 */
INLINED GLmatd_t  gl_matd_eq            (GLmatd m, const GLmatd a);
INLINED GLvecd_t  gl_matd_multv         (GLvecd v,
                                         const GLmatd a, const GLvecd b);
INLINED GLmatd_t  gl_matd_multm         (GLmatd m,
                                         const GLmatd a, const GLmatd b);
INLINED int       gl_matd_print         (const GLmatd m, FILE *stream);
EXTERND GLboolean gl_matd_invert        (const GLmatd src, GLmatd inv);
EXTERND GLmatd_t  gl_matd_from_complxd  (GLmatd m, const Complxd c);
EXTERND GLmatd_t  gl_matd_from_quatd    (GLmatd m, const Quatd q);
INLINED GLmatd_t  gl_matd_from_trackdisk(GLmatd m, const Trackdisk *td);
INLINED GLmatd_t  gl_matd_from_trackball(GLmatd m, const Trackball *tb);

/*
 * GLtransf declarations
 */
INLINED GLtransf  *gl_transf_new            (void);
INLINED void       gl_transf_delete         (GLtransf *trsf);
INLINED GLtransf  *gl_transf_mv_changed     (GLtransf *trsf);
INLINED GLtransf  *gl_transf_proj_changed   (GLtransf *trsf);
INLINED GLtransf  *gl_transf_set_viewport   (GLtransf *trsf);
INLINED GLtransf  *gl_transf_set_proj_mat   (GLtransf *trsf);
INLINED GLtransf  *gl_transf_set_mv_mat     (GLtransf *trsf);
INLINED GLtransf  *gl_transf_eq             (GLtransf *trsf,
                                             const GLtransf *trsf_src);
INLINED GLtransf  *gl_transf_begin          (GLtransf *trsf);
INLINED GLtransf  *gl_transf_end            (GLtransf *trsf);
EXTERND GLboolean  gl_transf_invert         (GLtransf *trsf);
EXTERND GLboolean  gl_transf_unproject      (GLtransf *trsf,
                                             const GLvecd win, GLvecd obj);
EXTERND GLboolean  gl_transf_unproject4     (GLtransf *trsf,
                                             const GLvecd win,
                                             GLclampd z_near, GLclampd z_far,
                                             GLvecd obj);
EXTERND GLboolean  gl_transf_project        (GLtransf *trsf,
                                             const GLvecd obj, GLvecd win);
EXTERND GLtransf  *gl_transf_get_clip_dist  (GLtransf *trsf,
                                             const GLvecd min,
                                             const GLvecd max,
                                             GLdouble *z_near,
                                             GLdouble *z_far);
INLINED GLboolean  gl_transf_get_proj_center(GLtransf *trsf, GLvecd center);
INLINED GLboolean  gl_transf_get_view_vector(GLtransf *trsf, GLvecd view);
EXTERND int        gl_transf_print          (const GLtransf *trsf,
                                             FILE *stream);
INLINED GLtransf  *gl_transf_from_trackdisk (GLtransf *trsf,
                                             const Trackdisk *td);
INLINED GLtransf  *gl_transf_from_trackball (GLtransf *trsf,
                                             const Trackball *tb);

/*
 * GLtexture declarations
 */
INLINED GLtexture *gl_texture_new    (void);
INLINED void       gl_texture_delete (GLtexture *tex);
EXTERND GLboolean  gl_texture_set    (GLtexture *tex, GLenum target,
                                      GLboolean (*func)(void *, GLboolean),
                                      void *data);
INLINED GLtexture *gl_texture_clear  (GLtexture *tex);
INLINED GLboolean  gl_texture_bind   (const GLtexture *tex);
INLINED GLboolean  gl_texture_release(const GLtexture *tex);

/*
 * GLlist declarations
 */
INLINED GLlist    *gl_list_new   (void);
INLINED void       gl_list_delete(GLlist *lst);
INLINED GLboolean  gl_list_set   (GLlist *lst,
                                  GLboolean (*func)(void *, GLboolean),
                                  void *data, GLboolean do_it_now);
INLINED GLlist    *gl_list_clear (GLlist *lst);
INLINED GLboolean  gl_list_call  (GLlist *lst);

/*
 * GLerror declarations
 */
INLINED GLerror  *gl_error_new   (void);
INLINED void      gl_error_delete(GLerror *err);
INLINED GLboolean gl_error_report(GLerror *err);

/*
 * OpenGL utils declarations
 */
EXTERND GLboolean gl_utils_is_extension_supported      (const char *name);
EXTERND void      gl_utils_build_normalization_cube_map(GLfloat alpha,
                                                        GLint level,
                                                        GLenum internal_format,
                                                        GLsizei size);

/*
 * GLvecd definitions
 */
INLINED GLvecd_t
gl_vecd_set(GLvecd v, GLdouble x, GLdouble y, GLdouble z, GLdouble w) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = w;
  return v;
}

INLINED GLvecd_t
gl_vecd_eq(GLvecd v, const GLvecd a) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  v[3] = a[3];
  return v;
}

INLINED GLvecd_t
gl_vecd_addeq(GLvecd v, const GLvecd a) {
  v[0] = (v[0] * a[3]) + (a[0] * v[3]);
  v[1] = (v[1] * a[3]) + (a[1] * v[3]);
  v[2] = (v[2] * a[3]) + (a[2] * v[3]);
  v[3] *= a[3];
  return v;
}

INLINED GLvecd_t
gl_vecd_subeq(GLvecd v, const GLvecd a) {
  v[0] = (v[0] * a[3]) - (a[0] * v[3]);
  v[1] = (v[1] * a[3]) - (a[1] * v[3]);
  v[2] = (v[2] * a[3]) - (a[2] * v[3]);
  v[3] *= a[3];
  return v;
}

INLINED GLvecd_t
gl_vecd_multeq(GLvecd v, GLdouble s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  return v;
}

INLINED GLvecd_t
gl_vecd_diveq(GLvecd v, GLdouble s) {
  v[3] *= s;
  return v;
}

INLINED GLvecd_t
gl_vecd_add(GLvecd v, const GLvecd a, const GLvecd b) {
  v[0] = (a[0] * b[3]) + (b[0] * a[3]);
  v[1] = (a[1] * b[3]) + (b[1] * a[3]);
  v[2] = (a[2] * b[3]) + (b[2] * a[3]);
  v[3] = (a[3] * b[3]);
  return v;
}

INLINED GLvecd_t
gl_vecd_sub(GLvecd v, const GLvecd a, const GLvecd b) {
  v[0] = (a[0] * b[3]) - (b[0] * a[3]);
  v[1] = (a[1] * b[3]) - (b[1] * a[3]);
  v[2] = (a[2] * b[3]) - (b[2] * a[3]);
  v[3] = (a[3] * b[3]);
  return v;
}

INLINED GLvecd_t
gl_vecd_mult(GLvecd v, const GLvecd a, GLdouble s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  v[2] = a[2] * s;
  v[3] = a[3];
  return v;
}

INLINED GLvecd_t
gl_vecd_div(GLvecd v, const GLvecd a, GLdouble s) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  v[3] = a[3] * s;
  return v;
}

INLINED GLvecd_t
gl_vecd_neg(GLvecd v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
  return v;
}

INLINED int
gl_vecd_print(const GLvecd v, FILE *stream) {
  return (fprintf(stream, "%+5.5f\n%+5.5f\n%+5.5f\n%+5.5f\n",
                  v[0], v[1], v[2], v[3]));
}

INLINED int
gl_vecd_scan(GLvecd v, FILE *stream) {
  return (fscanf(stream, "%lf %lf %lf %lf",
                 &(v[0]), &(v[1]), &(v[2]), &(v[3])));
}

INLINED int
gl_vecd_cmp(const GLvecd a, const GLvecd b) {
  return (memcmp(a, b, sizeof(GLvecd)));
}

INLINED GLdouble
gl_vecd_sqnorm(const GLvecd a) {
  GLdouble x = a[0] / a[3];
  GLdouble y = a[1] / a[3];
  GLdouble z = a[2] / a[3];
  return ((x * x) + (y * y) + (z * z));
}

INLINED GLdouble
gl_vecd_norm(const GLvecd a) {
  return (sqrt(gl_vecd_sqnorm(a)));
}

INLINED GLvecd_t
gl_vecd_normalize(GLvecd v) {
  GLdouble nrm_inv = 1.0 / gl_vecd_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  v[2] *= nrm_inv;
  return v;
}

INLINED GLdouble
gl_vecd_dot(const GLvecd a, const GLvecd b) {
  return (((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2])) / (a[3] * b[3]));
}

INLINED GLdouble
gl_vecd_sqdist(const GLvecd a, const GLvecd b) {
  GLvecd d;
  gl_vecd_sub(d, a, b);
  return (gl_vecd_sqnorm(d));
}

INLINED GLdouble
gl_vecd_dist(const GLvecd a, const GLvecd b) {
  return (sqrt(gl_vecd_sqdist(a, b)));
}

/*
 * GLvecf definitions
 */
INLINED GLvecf_t
gl_vecf_set(GLvecf v, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = w;
  return v;
}

INLINED GLvecf_t
gl_vecf_eq(GLvecf v, const GLvecf a) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  v[3] = a[3];
  return v;
}

INLINED GLvecf_t
gl_vecf_addeq(GLvecf v, const GLvecf a) {
  v[0] = (v[0] * a[3]) + (a[0] * v[3]);
  v[1] = (v[1] * a[3]) + (a[1] * v[3]);
  v[2] = (v[2] * a[3]) + (a[2] * v[3]);
  v[3] *= a[3];
  return v;
}

INLINED GLvecf_t
gl_vecf_subeq(GLvecf v, const GLvecf a) {
  v[0] = (v[0] * a[3]) - (a[0] * v[3]);
  v[1] = (v[1] * a[3]) - (a[1] * v[3]);
  v[2] = (v[2] * a[3]) - (a[2] * v[3]);
  v[3] *= a[3];
  return v;
}

INLINED GLvecf_t
gl_vecf_multeq(GLvecf v, GLfloat s) {
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
  return v;
}

INLINED GLvecf_t
gl_vecf_diveq(GLvecf v, GLfloat s) {
  v[3] *= s;
  return v;
}

INLINED GLvecf_t
gl_vecf_add(GLvecf v, const GLvecf a, const GLvecf b) {
  v[0] = (a[0] * b[3]) + (b[0] * a[3]);
  v[1] = (a[1] * b[3]) + (b[1] * a[3]);
  v[2] = (a[2] * b[3]) + (b[2] * a[3]);
  v[3] = (a[3] * b[3]);
  return v;
}

INLINED GLvecf_t
gl_vecf_sub(GLvecf v, const GLvecf a, const GLvecf b) {
  v[0] = (a[0] * b[3]) - (b[0] * a[3]);
  v[1] = (a[1] * b[3]) - (b[1] * a[3]);
  v[2] = (a[2] * b[3]) - (b[2] * a[3]);
  v[3] = (a[3] * b[3]);
  return v;
}

INLINED GLvecf_t
gl_vecf_mult(GLvecf v, const GLvecf a, GLfloat s) {
  v[0] = a[0] * s;
  v[1] = a[1] * s;
  v[2] = a[2] * s;
  v[3] = a[3];
  return v;
}

INLINED GLvecf_t
gl_vecf_div(GLvecf v, const GLvecf a, GLfloat s) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  v[3] = a[3] * s;
  return v;
}

INLINED GLvecf_t
gl_vecf_neg(GLvecf v) {
  v[0] = -v[0];
  v[1] = -v[1];
  v[2] = -v[2];
  return v;
}

INLINED int
gl_vecf_print(const GLvecf v, FILE *stream) {
  return (fprintf(stream, "%+5.5f\n%+5.5f\n%+5.5f\n%+5.5f\n",
                  v[0], v[1], v[2], v[3]));
}

INLINED int
gl_vecf_scan(GLvecf v, FILE *stream) {
  return (fscanf(stream, "%f %f %f %f",
                 &(v[0]), &(v[1]), &(v[2]), &(v[3])));
}

INLINED int
gl_vecf_cmp(const GLvecf a, const GLvecf b) {
  return (memcmp(a, b, sizeof(GLvecf)));
}

INLINED GLfloat
gl_vecf_sqnorm(const GLvecf a) {
  GLfloat x = a[0] / a[3];
  GLfloat y = a[1] / a[3];
  GLfloat z = a[2] / a[3];
  return ((x * x) + (y * y) + (z * z));
}

INLINED GLfloat
gl_vecf_norm(const GLvecf a) {
  return (sqrtf(gl_vecf_sqnorm(a)));
}

INLINED GLvecf_t
gl_vecf_normalize(GLvecf v) {
  GLfloat nrm_inv = 1.0f / gl_vecf_norm(v);
  v[0] *= nrm_inv;
  v[1] *= nrm_inv;
  v[2] *= nrm_inv;
  return v;
}

INLINED GLfloat
gl_vecf_dot(const GLvecf a, const GLvecf b) {
  return (((a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2])) / (a[3] * b[3]));
}

INLINED GLfloat
gl_vecf_sqdist(const GLvecf a, const GLvecf b) {
  GLvecf d;
  gl_vecf_sub(d, a, b);
  return (gl_vecf_sqnorm(d));
}

INLINED GLfloat
gl_vecf_dist(const GLvecf a, const GLvecf b) {
  return (sqrtf(gl_vecf_sqdist(a, b)));
}

/*
 * GLveci definitions
 */
INLINED GLveci_t
gl_veci_set(GLveci v, GLint x, GLint y, GLint z, GLint w) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
  v[3] = w;
  return v;
}

INLINED GLveci_t
gl_veci_eq(GLveci v, const GLveci a) {
  v[0] = a[0];
  v[1] = a[1];
  v[2] = a[2];
  v[3] = a[3];
  return v;
}

INLINED int
gl_veci_print(const GLveci v, FILE *stream) {
  return (fprintf(stream, "%+5d\n%+5d\n%+5d\n%+5d\n",
                  v[0], v[1], v[2], v[3]));
}

/*
 * GLmatd definitions
 */
INLINED GLmatd_t
gl_matd_eq(GLmatd m, const GLmatd a) {
  memcpy(m, a, sizeof(GLmatd));
  return m;
}

INLINED GLvecd_t
gl_matd_multv(GLvecd v, const GLmatd a, const GLvecd b) {
  int i;
  
  for (i = 0; i < 4; i++) {
    v[i] =
      a[0*4+i]*b[0] +
      a[1*4+i]*b[1] +
      a[2*4+i]*b[2] +
      a[3*4+i]*b[3];
  }
  return v;
}

INLINED GLmatd_t
gl_matd_multm(GLmatd m, const GLmatd a, const GLmatd b) {
  int i, j;
  
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      m[j*4+i] =
        a[0*4+i]*b[j*4+0] +
        a[1*4+i]*b[j*4+1] +
        a[2*4+i]*b[j*4+2] +
        a[3*4+i]*b[j*4+3];
    }
  }
  return m;
}

INLINED int
gl_matd_print(const GLmatd m, FILE *stream) {
  return (fprintf(stream, "%+5.5f\t%+5.5f\t%+5.5f\t%+5.5f\n"
                          "%+5.5f\t%+5.5f\t%+5.5f\t%+5.5f\n"
                          "%+5.5f\t%+5.5f\t%+5.5f\t%+5.5f\n"
                          "%+5.5f\t%+5.5f\t%+5.5f\t%+5.5f\n",
                  m[0], m[4],  m[8], m[12],
                  m[1], m[5],  m[9], m[13],
                  m[2], m[6], m[10], m[14],
                  m[3], m[7], m[11], m[15]));
}

INLINED GLmatd_t
gl_matd_from_trackdisk(GLmatd m, const Trackdisk *td) {
  gl_matd_from_complxd(m, td->rotation);
  
  m[0]  *= td->scale;
  m[1]  *= td->scale;
  
  m[4]  *= td->scale;
  m[5]  *= td->scale;
  
  m[12] = td->translation[0];
  m[13] = td->translation[1];
  
  return m;
}

INLINED GLmatd_t
gl_matd_from_trackball(GLmatd m, const Trackball *tb) {
  gl_matd_from_quatd(m, tb->rotation);
  m[12] = tb->translation[0];
  m[13] = tb->translation[1];
  m[14] = tb->translation[2];
  return m;
}

/*
 * GLtransf definitions
 */
INLINED GLtransf *
gl_transf_new(void) {
  GLtransf *trsf = (GLtransf *) malloc(sizeof(GLtransf));
  assert(trsf != NULL);
  gl_veci_eq(trsf->viewport, GL_VECI_NULL);
  gl_matd_eq(trsf->projection_matrix, GL_MATD_ID);
  gl_matd_eq(trsf->modelview_matrix, GL_MATD_ID);
  gl_matd_eq(trsf->projection_matrix_inverse, GL_MATD_ID);
  gl_matd_eq(trsf->modelview_matrix_inverse, GL_MATD_ID);
  gl_matd_eq(trsf->final_matrix, GL_MATD_ID);
  gl_matd_eq(trsf->final_matrix_inverse, GL_MATD_ID);
  trsf->projection_changed = trsf->modelview_changed = GL_FALSE;
  trsf->changed = GL_FALSE;
  return trsf;
}

INLINED void
gl_transf_delete(GLtransf *trsf) {
  assert(trsf != NULL);
  free(trsf);
#if DEBUG
  trsf = NULL;
#endif
}

INLINED GLtransf  *
gl_transf_proj_changed(GLtransf *trsf) {
  trsf->projection_changed = GL_TRUE;
  trsf->changed = GL_TRUE;
  return trsf;
}

INLINED GLtransf  *
gl_transf_mv_changed(GLtransf *trsf) {
  trsf->modelview_changed = GL_TRUE;
  trsf->changed = GL_TRUE;
  return trsf;
}

INLINED GLtransf *
gl_transf_set_viewport(GLtransf *trsf) {
  glGetIntegerv(GL_VIEWPORT, trsf->viewport);
  /* No need to signal change */
  return trsf;
}

INLINED GLtransf *
gl_transf_set_proj_mat(GLtransf *trsf) {
  glGetDoublev(GL_PROJECTION_MATRIX, trsf->projection_matrix);
  gl_transf_proj_changed(trsf);
  return trsf;
}

INLINED GLtransf *
gl_transf_set_mv_mat(GLtransf *trsf) {
  glGetDoublev(GL_MODELVIEW_MATRIX, trsf->modelview_matrix);
  gl_transf_mv_changed(trsf);
  return trsf;
}

INLINED GLtransf *
gl_transf_eq(GLtransf *trsf, const GLtransf *trsf_src) {
  memcpy(trsf, trsf_src, sizeof(GLtransf));
  return trsf;
}

INLINED GLtransf *
gl_transf_begin(GLtransf *trsf) {
  glPushAttrib(GL_TRANSFORM_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixd(trsf->projection_matrix);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMultMatrixd(trsf->modelview_matrix);
  return trsf;
}

INLINED GLtransf *
gl_transf_end(GLtransf *trsf) {
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  return trsf;
}

INLINED GLboolean
gl_transf_get_proj_center(GLtransf *trsf, GLvecd center) {
  GLvecd in = {0.0, 0.0, 0.0, 1.0};
  GLvecd out;
  
  if (!gl_transf_invert(trsf)) {
    return GL_FALSE;
  }
  gl_matd_multv(out, trsf->modelview_matrix_inverse, in);
  gl_vecd_set(center, out[0], out[1], out[2], 1.0);
  return GL_TRUE;
}

INLINED GLboolean
gl_transf_get_view_vector(GLtransf *trsf, GLvecd view) {
  GLvecd O_in = {0.0, 0.0, 0.0, 1.0};
  GLvecd Z_in = {0.0, 0.0, 1.0, 1.0};
  GLvecd O_out, Z_out, ZO_out;
  
  if (!gl_transf_invert(trsf)) {
    return GL_FALSE;
  }
  gl_matd_multv(O_out, trsf->modelview_matrix_inverse, O_in);
  gl_matd_multv(Z_out, trsf->modelview_matrix_inverse, Z_in);
  gl_vecd_sub(ZO_out, O_out, Z_out);
  gl_vecd_set(view, ZO_out[0], ZO_out[1], ZO_out[2], 1.0);
  return GL_TRUE;
}

INLINED GLtransf *
gl_transf_from_trackdisk(GLtransf *trsf, const Trackdisk *td) {
  gl_matd_from_trackdisk(trsf->modelview_matrix, td);
  gl_transf_mv_changed(trsf);
  return trsf;
}

INLINED GLtransf *
gl_transf_from_trackball(GLtransf *trsf, const Trackball *tb) {
  gl_matd_from_trackball(trsf->modelview_matrix, tb);
  gl_transf_mv_changed(trsf);
  return trsf;
}

/*
 * GLtexture definitions
 */
INLINED GLtexture *
gl_texture_new(void) {
  GLtexture *tex = (GLtexture *) malloc(sizeof(GLtexture));
  assert(tex != NULL);
  tex->name = GL_TEXTURE_NULL_NAME;
  tex->target = GL_TEXTURE_2D;
  tex->width = 0;
  tex->height = tex->depth = 1;
  tex->nlevels = 1;
  tex->format = GL_RGB;
  tex->setup = NULL;
  tex->setup_data = NULL;
  return tex;
}

INLINED void
gl_texture_delete(GLtexture *tex) {
  assert(tex != NULL);
  gl_texture_clear(tex);
  free(tex);
#if DEBUG
  tex = NULL;
#endif
}

INLINED GLtexture *
gl_texture_clear(GLtexture *tex) {
  if (glIsTexture(tex->name)) {
    glDeleteTextures(1, &(tex->name));
    tex->name = GL_TEXTURE_NULL_NAME;
  }
  return tex;
}

INLINED GLboolean
gl_texture_bind(const GLtexture *tex) {
  if (glIsTexture(tex->name)) {
    glBindTexture(tex->target, tex->name);
    return GL_TRUE;
  } else {
    return GL_FALSE;
  }
}

INLINED GLboolean
gl_texture_release(const GLtexture *tex) {
  if (glIsTexture(tex->name)) {
    glBindTexture(tex->target, 0);
    return GL_TRUE;
  } else {
    return GL_FALSE;
  }
}

/*
 * GLlist definitions
 */
INLINED GLlist *
gl_list_new(void) {
  GLlist *lst = (GLlist *) malloc(sizeof(GLlist));
  assert(lst != NULL);
  lst->name = GL_LIST_NULL_NAME;
  lst->display = NULL;
  lst->display_data = NULL;
  return lst;
}

INLINED void
gl_list_delete(GLlist *lst) {
  assert(lst != NULL);
  gl_list_clear(lst);
  free(lst);
#if DEBUG
  lst = NULL;
#endif
}

INLINED GLboolean
gl_list_set(GLlist *lst, GLboolean (*func)(void *, GLboolean), void *data,
            GLboolean do_it_now) {
  gl_list_clear(lst);
  assert(func != NULL);
  lst->display = func;
  lst->display_data = data;
  if (do_it_now) {
    GLboolean is_proxy_supported = lst->display(lst->display_data, GL_TRUE);
    if (is_proxy_supported) {
      lst->name = glGenLists(1);
      if (lst->name != GL_LIST_NULL_NAME) {
        glNewList(lst->name, GL_COMPILE);
        lst->display(lst->display_data, GL_FALSE);
        glEndList();
      } else {
        return GL_FALSE;
      }
    } else {
      return GL_FALSE;
    }
  }
  return GL_TRUE;
}

INLINED GLlist *
gl_list_clear(GLlist *lst) {
  if (glIsList(lst->name)) {
    glDeleteLists(lst->name, 1);
    lst->name = GL_LIST_NULL_NAME;
  }
  return lst;
}

INLINED GLboolean
gl_list_call(GLlist *lst) {
  if (glIsList(lst->name)) {
    glCallList(lst->name);
  } else {
    GLboolean is_proxy_supported = lst->display(lst->display_data, GL_TRUE);
    if (is_proxy_supported) {
      lst->name = glGenLists(1);
      if (lst->name != GL_LIST_NULL_NAME) {
        glNewList(lst->name, GL_COMPILE_AND_EXECUTE);
        lst->display(lst->display_data, GL_FALSE);
        glEndList();
      } else {
        return GL_FALSE;
      }
    } else {
      return GL_FALSE;
    }
  }
  return GL_TRUE;
}

/*
 * GLerror definitions
 */
INLINED GLerror *
gl_error_new(void) {
  GLerror *err = (GLerror *) malloc(sizeof(GLerror));
  assert(err != NULL);
  err->code = 0;
  err->report_string = NULL;
  return err;
}

INLINED void
gl_error_delete(GLerror *err) {
  assert(err != NULL);
  free(err);
#if DEBUG
  err = NULL;
#endif
}

INLINED GLboolean
gl_error_report(GLerror *err) {
  if ((err->code = glGetError()) != GL_NO_ERROR) {
    err->report_string = gluErrorString(err->code);
    fprintf(stderr, "OpenGL Error: %s\n", err->report_string);
    return GL_TRUE;
  } else {
    return GL_FALSE;
  }
}

#ifdef __cplusplus
}
#endif

#endif /* !__OPENGL_UTILS_H__ */
