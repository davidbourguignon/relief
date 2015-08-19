#ifndef __OPENGL_WIDGET_H__
#define __OPENGL_WIDGET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <opengl_defs.h>

/*
 * OpenGL widget declarations
 */
EXTERND void gl_widget_bbox_2(const GLfloat x_min, const GLfloat y_min,
                              const GLfloat x_max, const GLfloat y_max);
EXTERND void gl_widget_bbox_3(const GLfloat x_min, const GLfloat y_min,
                              const GLfloat z_min, const GLfloat x_max,
                              const GLfloat y_max, const GLfloat z_max);
INLINED void gl_widget_box   (const GLfloat width, const GLfloat height,
                              const GLfloat length);
EXTERND void gl_widget_grid  (const GLfloat width, const GLfloat length,
                              const GLint rows, const GLint columns);

/*
 * OpenGL widget definitions
 */
INLINED void
gl_widget_box(const GLfloat width, const GLfloat height,
              const GLfloat length) {
  GLfloat x = 0.5f * width;
  GLfloat y = 0.5f * height;
  GLfloat z = 0.5f * length;
  
  gl_widget_bbox_3(-x, -y, -z, x, y, z);
}

#ifdef __cplusplus
}
#endif

#endif /* !__OPENGL_WIDGET_H__ */
