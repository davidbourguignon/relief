#include "opengl_widget.h"

void
gl_widget_bbox_2(const GLfloat x_min, const GLfloat y_min,
                 const GLfloat x_max, const GLfloat y_max) {
  glBegin(GL_QUADS);
  glVertex2f(x_min, y_min);
  glVertex2f(x_max, y_min);
  glVertex2f(x_max, y_max);
  glVertex2f(x_min, y_max);
  glEnd();
}

void
gl_widget_bbox_3(const GLfloat x_min, const GLfloat y_min,
                 const GLfloat z_min, const GLfloat x_max,
                 const GLfloat y_max, const GLfloat z_max) {
  glBegin(GL_QUAD_STRIP);
  glVertex3f(x_min, y_min, z_max);
  glVertex3f(x_max, y_min, z_max);
  glVertex3f(x_min, y_max, z_max);
  glVertex3f(x_max, y_max, z_max);
  glVertex3f(x_min, y_max, z_min);
  glVertex3f(x_max, y_max, z_min);
  glVertex3f(x_min, y_min, z_min);
  glVertex3f(x_max, y_min, z_min);
  glEnd();
  
  glBegin(GL_QUAD_STRIP);
  glVertex3f(x_min, y_max, z_max);
  glVertex3f(x_min, y_max, z_min);
  glVertex3f(x_min, y_min, z_max);
  glVertex3f(x_min, y_min, z_min);
  glVertex3f(x_max, y_min, z_max);
  glVertex3f(x_max, y_min, z_min);
  glVertex3f(x_max, y_max, z_max);
  glVertex3f(x_max, y_max, z_min);
  glEnd();
}

void
gl_widget_grid(const GLfloat width, const GLfloat length,
               const GLint rows, const GLint columns) {
  GLfloat x = 0.5f * width;
  GLfloat z = 0.5f * length;
  GLfloat step_x = width/columns;
  GLfloat step_z = length/rows;
  int c, r;
  
  glPushAttrib(GL_POLYGON_BIT);
  
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
  for (c = 0; c < columns; c++) {
    GLfloat x_prev = -x +  c   *step_x;
    GLfloat x_next = -x + (c+1)*step_x;
    glBegin(GL_QUAD_STRIP);
    for (r = 0; r <= rows; r++) {
      GLfloat z_curr = z - r*step_z;
      glVertex3f(x_prev, 0.0, z_curr);
      glVertex3f(x_next, 0.0, z_curr);
    }
    glEnd();
  }
  
  glPopAttrib();
}
