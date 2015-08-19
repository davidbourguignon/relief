#include "glut_widget.h"
#include <glut_utils.h>

void
glut_widget_axes(void) {
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT |
               GL_TRANSFORM_BIT | GL_LIGHTING_BIT);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  
  /* We don't set line color to allow user setting */
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(1.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0, 1.0);
  glEnd();
  
  glColor3f(1.0, 1.0, 0.0);
  glutSolidCube(0.075);
  
  glMatrixMode(GL_MODELVIEW);
  
  glPushMatrix();
  glTranslatef(1.0, 0.0, 0.0);
  glColor3f(1.0, 0.0, 0.0);
  glutSolidCube(0.075);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0, 1.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glutSolidCube(0.075);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0, 0.0, 1.0);
  glColor3f(0.0, 0.0, 1.0);
  glutSolidCube(0.075);
  glPopMatrix();
  
  glPopAttrib();
}

void
glut_widget_string(const char *str, void *font, const GLfloat color[4],
                   const GLclampf x, const GLclampf y) {
  glPushAttrib(GL_CURRENT_BIT | GL_TRANSFORM_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 0.0, 1.0);
  glColor4fv(color);
  glut_utils_bitmap_string(str, font, x, y);
  glPopMatrix();
  
  glPopAttrib();
}

void
glut_widget_menu(const char **entries, const int nentries,
                 void *first_font, void *middle_font, void *last_font,
                 const GLfloat bg_color[4], const GLfloat fg_color[4]) {
  int i;
  GLclampf x, y, spacing;
  
  glPushAttrib(GL_CURRENT_BIT | GL_TRANSFORM_BIT | GL_COLOR_BUFFER_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 0.0, 1.0);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv(bg_color);
  glRectf(0.075, 0.075, 0.925, 0.925);
  
  glColor4fv(fg_color);
  /* Title */
  glut_utils_bitmap_string(entries[0], first_font, 0.125, 0.850);
  /* Menu entries */
  if (nentries - 2 > 14) { /* 14 = (0.800 - 0.100) / 0.050 */
    spacing = (0.800 - 0.100) / (float) nentries;
  } else {
    spacing = 0.050;
  }
  for (i = 1, x = 0.150, y = 0.800; i < nentries - 1; i++, y -= spacing) {
    glut_utils_bitmap_string(entries[i], middle_font, x, y);
  }
  /* Colophon */
  glut_utils_bitmap_string(entries[nentries - 1], last_font, 0.100, 0.100);
  
  glPopMatrix();
  glPopAttrib();
}

void
glut_widget_teapot(GLdouble size) {
  /* Light */
  const GLfloat lKa[4] = {0.25, 0.15, 0.15, 1.0};
  const GLfloat lKd[4] = {1.00, 0.85, 0.85, 1.0};
  const GLfloat lKs[4] = {1.00, 0.99, 0.99, 1.0};
  
  /* Nate Robins' Polished_Copper material */
  const GLfloat mKa[4] = {0.229500, 0.088250, 0.027500, 1.000000};
  const GLfloat mKd[4] = {0.550800, 0.211800, 0.066000, 1.000000};
  const GLfloat mKs[4] = {0.580594, 0.223257, 0.069570, 1.000000};
  const GLfloat S      = 51.200001;
  
  glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, lKa);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lKd);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lKs);
  
  glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
  glMaterialf(GL_FRONT, GL_SHININESS, S);
  
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glutSolidTeapot(size);
  
  glPopAttrib();
}
