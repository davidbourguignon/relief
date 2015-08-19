#ifndef __GLUT_UTILS_H__
#define __GLUT_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <opengl_utils.h>
#include <GL/glut.h>

typedef struct _GLUTwin GLUTwin;
typedef struct _GLUTfps GLUTfps;

struct _GLUTwin {
  int id;
  
  /* Current state */
  int x, y;
  int width, height;
  GLtransf *gl_transf;
  
  /* Init state */
  int x_init, y_init;
  int width_init, height_init;
};

struct _GLUTfps {
  int begin, end;
  char report_string[32];
};

/*
 * GLUTwin declarations
 */
INLINED GLUTwin  *glut_win_new        (void);
INLINED void      glut_win_delete     (GLUTwin *win);
INLINED GLUTwin  *glut_win_create     (GLUTwin *win, char *name);
INLINED GLUTwin  *glut_win_init_pos   (GLUTwin *win,
                                       const int x, const int y);
INLINED GLUTwin  *glut_win_init_size  (GLUTwin *win,
                                       const int w, const int h);
INLINED GLUTwin  *glut_win_reinit_pos (GLUTwin *win);
INLINED GLUTwin  *glut_win_reinit_size(GLUTwin *win);
INLINED GLUTwin  *glut_win_set_pos    (GLUTwin *win,
                                       const int x, const int y);
INLINED GLUTwin  *glut_win_set_size   (GLUTwin *win,
                                       const int w, const int h);
INLINED GLboolean glut_win_unproject  (GLUTwin *win,
                                       const int x, const int y,
                                       const double z, GLvecd obj);
INLINED GLboolean glut_win_unproject4 (GLUTwin *win,
                                       const int x, const int y,
                                       const double z, const double w,
                                       GLclampd z_near, GLclampd z_far,
                                       GLvecd obj);

/*
 * GLUTfps declarations
 */
INLINED GLUTfps *glut_fps_new   (void);
INLINED void     glut_fps_delete(GLUTfps *f);
INLINED void     glut_fps_start (GLUTfps *f);
INLINED void     glut_fps_stop  (GLUTfps *f);

/*
 * GLUT utils declarations
 */
EXTERND void glut_utils_bitmap_string       (const char *str, void *font,
                                             GLclampf x, GLclampf y);
EXTERND void glut_utils_stroke_string       (const char *str, void *font,
                                             GLfloat x, GLfloat y);
EXTERND int  glut_utils_extensions_supported(const char **entries,
                                             const int nentries);
EXTERND int  glut_utils_gl_itembuf_supported(void);

/*
 * GLUTwin definitions
 */
INLINED GLUTwin *
glut_win_new(void) {
  GLUTwin *win = (GLUTwin *) malloc(sizeof(GLUTwin));
  assert(win != NULL);
  win->id = 0;
  win->x = win->y = win->width = win->height = 0;
  win->gl_transf = gl_transf_new();
  win->x_init = win->y_init = win->width_init = win->height_init = 0;
  return win;
}

INLINED void
glut_win_delete(GLUTwin *win) {
  assert(win != NULL);
  gl_transf_delete(win->gl_transf);
  free(win);
#if DEBUG
  win = NULL;
#endif
}

INLINED GLUTwin  *
glut_win_create(GLUTwin *win, char *name) {
  win->id = glutCreateWindow(name);
  return win;
}

INLINED GLUTwin *
glut_win_init_pos(GLUTwin *win, const int x, const int y) {
  win->x_init = x;
  win->y_init = y;
  glutInitWindowPosition(win->x_init, win->y_init);
  return win;
}

INLINED GLUTwin *
glut_win_init_size(GLUTwin *win, const int w, const int h) {
  win->width_init = w;
  win->height_init = h;
  glutInitWindowSize(win->width_init, win->height_init);
  return win;
}

INLINED GLUTwin *
glut_win_reinit_pos(GLUTwin *win) {
  win->x = win->x_init;
  win->y = win->y_init;
  glutPositionWindow(win->x_init, win->y_init);
  return win;
}

INLINED GLUTwin *
glut_win_reinit_size(GLUTwin *win) {
  win->width  = win->width_init;
  win->height = win->height_init;
  glutReshapeWindow(win->width_init, win->height_init);
  return win;
}

INLINED GLUTwin *
glut_win_set_pos(GLUTwin *win, const int x, const int y) {
  win->x = x;
  win->y = y;
  return win;
}

INLINED GLUTwin *
glut_win_set_size(GLUTwin *win, const int w, const int h) {
  win->width = w;
  win->height = h;
  return win;
}

INLINED GLboolean
glut_win_unproject(GLUTwin *win, const int x, const int y, const double z,
                   GLvecd obj) {
  GLvecd wincoord;
  gl_vecd_set(wincoord, x, win->gl_transf->viewport[3] - 1 - y, z, 1.0);
  return gl_transf_unproject(win->gl_transf, wincoord, obj);
}

INLINED GLboolean
glut_win_unproject4(GLUTwin *win,
                    const int x, const int y, const double z, const double w,
                    GLclampd z_near, GLclampd z_far, GLvecd obj) {
  GLvecd wincoord;
  gl_vecd_set(wincoord, x, win->gl_transf->viewport[3] - 1 - y, z, w);
  return gl_transf_unproject4(win->gl_transf, wincoord, z_near, z_far, obj);
}

/*
 * GLUTfps declarations
 */
INLINED GLUTfps *
glut_fps_new(void) {
  GLUTfps *f = (GLUTfps *) malloc(sizeof(GLUTfps));
  assert(f != NULL);
  f->begin = f->end = 0;
  return f;
}

INLINED void
glut_fps_delete(GLUTfps *f) {
  assert(f != NULL);
  free(f);
#if DEBUG
  f = NULL;
#endif
}

INLINED void
glut_fps_start(GLUTfps *f) {
  glFinish();
  f->begin = glutGet(GLUT_ELAPSED_TIME);
}

INLINED void
glut_fps_stop(GLUTfps *f) {
  int elapsed_time = 0;
  
  glFinish();
  f->end = glutGet(GLUT_ELAPSED_TIME);
  if ((elapsed_time = f->end - f->begin) > 0) {
    sprintf(f->report_string, "%3.1f Hz (%d ms)",
            1000.0 / (double) elapsed_time, elapsed_time);
  } else {
    sprintf(f->report_string, "> 1 kHz (< 1 ms)");
  }
}

#ifdef __cplusplus
}
#endif

#endif /* !__GLUT_UTILS_H__ */
