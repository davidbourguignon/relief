#include <string.h>
#include "glut_utils.h"

/*
 * GLUT utils definitions
 */
void
glut_utils_bitmap_string(const char *str, void *font, GLclampf x, GLclampf y) {
  int len, i;
  
  glRasterPos2f(x, y);
  len = (int) strlen(str);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, str[i]);
  }
}

void
glut_utils_stroke_string(const char *str, void *font, GLfloat x, GLfloat y) {
  const char *p;
  
#if ANTIALIAS
  glPushAttrib(GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT | GL_LINE_BIT);
  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLineWidth(2.0);
#else
  glPushAttrib(GL_TRANSFORM_BIT);
#endif
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(x, y, 0.0);
  for (p = str; *p; p++) {
    glutStrokeCharacter(font, *p);
  }
  glPopMatrix();
  
  glPopAttrib();
}

int
glut_utils_extensions_supported(const char **entries, const int nentries) {
  int i;
  int not_supported = 0;
  
  for (i = 0; i < nentries; i++) {
    not_supported = (not_supported || (!glutExtensionSupported(entries[i])));
  }
  if (not_supported) {
    fprintf(stderr, "This program is so georgeous with the following "
                    "extensions:\n");
    for (i = 0; i < nentries; i++) {
      fprintf(stderr, entries[i]);
      fprintf(stderr, "\n");
    }
    fprintf(stderr, "But one or more of these extensions is not supported by "
                    "your driver... :-(\n");
  }
  return (!not_supported);
}

int
glut_utils_gl_itembuf_supported(void) {
  int not_supported = 0;
  
  if (!(glutGet(GLUT_WINDOW_RGBA)            &&
        glutGet(GLUT_WINDOW_RED_SIZE)   == 8 &&
        glutGet(GLUT_WINDOW_GREEN_SIZE) == 8 &&
        glutGet(GLUT_WINDOW_BLUE_SIZE)  == 8 &&
        glutGet(GLUT_WINDOW_ALPHA_SIZE) == 8 &&
        glutGet(GLUT_WINDOW_DOUBLEBUFFER))) {
    fprintf(stderr, "GLitembuf not supported!\n"
                    "No visual available for RGBA mode"
                    "and 8 bits per components...\n");
    not_supported = 1;
  }
  return (!not_supported);
}
