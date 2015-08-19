#ifndef __GLUT_WIDGET_H__
#define __GLUT_WIDGET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <opengl_defs.h>

/*
 * GLUT widget declarations
 */
EXTERND void glut_widget_axes  (void);
EXTERND void glut_widget_string(const char *str, void *font,
                                const GLfloat color[4],
                                const GLclampf x, const GLclampf y);
EXTERND void glut_widget_menu  (const char **entries, const int nentries,
                                void *first_font, void *middle_font,
                                void *last_font, const GLfloat bg_color[4],
                                const GLfloat fg_color[4]);
EXTERND void glut_widget_teapot(GLdouble size);

#ifdef __cplusplus
}
#endif

#endif /* !__GLUT_WIDGET_H__ */
