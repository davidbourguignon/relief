#include "pencil_tool.hh"

using namespace std;

static const GLfloat DEFAULT_POINT_SIZE = 0.250f;
static const GLfloat DEFAULT_LINE_WIDTH = 0.375f;

Pencil_tool::Pencil_tool(Toolbox *toolbox) : Base(toolbox, _display_list_cb) {
  vec2f_eq(_point_1, VEC2F_NULL);
  vec2f_eq(_point_2, VEC2F_NULL);
  _point_size = DEFAULT_POINT_SIZE;
  _line_width = DEFAULT_LINE_WIDTH;
  gl_vecf_eq(_color, GL_PURE_BLACK);
//   GLfloat range = 0.0f;
//   glGetFloatv(GL_SMOOTH_POINT_SIZE_RANGE, &range);
//   glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &range);
//   glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, &range);
//   glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &range);
//   GLfloat granularity = 0.0f;
//   glGetFloatv(GL_SMOOTH_POINT_SIZE_GRANULARITY, &granularity);
//   glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &granularity);
}

Pencil_tool::~Pencil_tool(void) {}

void
Pencil_tool::start_drawing_pixels(GdkInputSource source,
                                  gdouble x, gdouble y, gdouble pressure,
                                  gdouble xtilt, gdouble ytilt) {
  gl_vecf_eq(_color, _toolbox->foreground_color());
  vec2f_set(_point_1, x, y);
  
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
               GL_HINT_BIT | GL_LINE_BIT | GL_POINT_BIT);
  
  glColor4fv(_color);
  gl_list_call(_list);
  glBegin(GL_POINTS);
  glVertex2fv(_point_1);
  glEnd();
  
  glPopAttrib();
  
  GLfloat safe_radius = 2.0f * scalf_max(_point_size, _line_width);
  gl_veci_set(_drawport,
              (GLint) (_point_1[0] - safe_radius),
              (GLint) (_point_1[1] - safe_radius),
              (GLint) (2.0f * safe_radius) + 1,
              (GLint) (2.0f * safe_radius) + 1);
  // we add 1 to compensate the cast from float to int
}

void
Pencil_tool::stop_drawing_pixels(void) {}

void
Pencil_tool::draw_pixels(GdkInputSource source,
                         gdouble x, gdouble y, gdouble pressure,
                         gdouble xtilt, gdouble ytilt) {
  vec2f_set(_point_2, x, y);
  
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
               GL_HINT_BIT | GL_LINE_BIT | GL_POINT_BIT);
  
  glColor4fv(_color);
  gl_list_call(_list);
  glBegin(GL_LINES);
  glVertex2fv(_point_1);
  glVertex2fv(_point_2);
  glEnd();
  glBegin(GL_POINTS);
  glVertex2fv(_point_2);
  glEnd();
  
  glPopAttrib();
  
  GLfloat safe_radius = 2.0f * scalf_max(_point_size, _line_width);
  Vec2f delta;
  vec2f_absub(delta, _point_1, _point_2);
  gl_veci_set(_drawport,
              (GLint) (_point_1[0] - safe_radius),
              (GLint) (_point_1[1] - safe_radius),
              (GLint) (delta[0] + 2.0f * safe_radius) + 1,
              (GLint) (delta[1] + 2.0f * safe_radius) + 1);
  
  vec2f_eq(_point_1, _point_2);
}

GLboolean
Pencil_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  Pencil_tool *pencil_tool = (Pencil_tool *) data;
  
  if (test_proxy) return GL_TRUE;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x1);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glLineWidth(pencil_tool->_line_width);
  glPointSize(pencil_tool->_point_size);
  return GL_TRUE;
}
