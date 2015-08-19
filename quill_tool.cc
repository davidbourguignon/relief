#include "quill_tool.hh"

using namespace std;

static const float DEFAULT_THICKNESS = 2.0f;
static const float PRESSURE_THICKNESS_MAX = 2.0f;
static const GLfloat DEFAULT_POINT_SIZE = 0.5f;
static const GLfloat DEFAULT_LINE_WIDTH = 1.0f;

Quill_tool::Quill_tool(Toolbox *toolbox) : Base(toolbox, _display_list_cb) {
  _is_first = false;
  vec2f_eq(_point_1, VEC2F_NULL);
  vec2f_eq(_point_2, VEC2F_NULL);
  vec2f_eq(_vector_tangent, VEC2F_NULL);
  vec2f_eq(_vector_perpendicular, VEC2F_NULL);
  vec2f_eq(_point_left_1, VEC2F_NULL);
  vec2f_eq(_point_right_1, VEC2F_NULL);
  vec2f_eq(_point_left_2, VEC2F_NULL);
  vec2f_eq(_point_right_2, VEC2F_NULL);
  _point_size = DEFAULT_POINT_SIZE;
  _line_width = DEFAULT_LINE_WIDTH;
  _thickness = DEFAULT_THICKNESS;
  gl_vecf_eq(_color, GL_PURE_BLACK);
}

Quill_tool::~Quill_tool(void) {}

void
Quill_tool::start_drawing_pixels(GdkInputSource source,
                                 gdouble x, gdouble y, gdouble pressure,
                                 gdouble xtilt, gdouble ytilt) {
  gl_vecf_eq(_color, _toolbox->foreground_color());
  vec2f_set(_point_1, x, y);
  _is_first = true;
}

void
Quill_tool::stop_drawing_pixels(void) {
  if (!_is_first) {
    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
                 GL_HINT_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_POINT_BIT);
    
    glColor4fv(_color);
    gl_list_call(_list);
    glBegin(GL_LINES);
    glVertex2fv(_point_right_2);
    glVertex2fv(_point_left_2);
    glEnd();
    
    glPopAttrib();
  }
}

void
Quill_tool::draw_pixels(GdkInputSource source,
                        gdouble x, gdouble y, gdouble pressure,
                        gdouble xtilt, gdouble ytilt) {
  switch (source) {
  case GDK_SOURCE_MOUSE:
    _thickness = DEFAULT_THICKNESS;
    break;
  case GDK_SOURCE_PEN:
  case GDK_SOURCE_ERASER:
    _thickness = PRESSURE_THICKNESS_MAX * pressure * pressure;
    break;
  case GDK_SOURCE_CURSOR:
    cerr << "Warning: cursor is an unknown input device source!" << endl;
    break;
  default:
    assert(false);
    break;
  }
  
  vec2f_set(_point_2, x, y);
  vec2f_sub(_vector_tangent, _point_2, _point_1);
  if (vec2f_cmp(_vector_tangent, VEC2F_NULL) != 0) {
    vec2f_normalize(vec2f_perp(_vector_perpendicular, _vector_tangent));
    vec2f_multeq(_vector_perpendicular, _thickness);
    if (_is_first) {
      vec2f_add(_point_left_1, _point_1, _vector_perpendicular);
      vec2f_sub(_point_right_1, _point_1, _vector_perpendicular);
    }
    vec2f_add(_point_left_2, _point_2, _vector_perpendicular);
    vec2f_sub(_point_right_2, _point_2, _vector_perpendicular);
    
    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
                 GL_HINT_BIT | GL_POLYGON_BIT | GL_LINE_BIT | GL_POINT_BIT);
    
    glColor4fv(_color);
    gl_list_call(_list);
    glBegin(GL_QUADS);
    glVertex2fv(_point_right_1);
    glVertex2fv(_point_right_2);
    glVertex2fv(_point_left_2);
    glVertex2fv(_point_left_1);
    glEnd();
    glBegin(GL_LINES);
    if (_is_first) {
      _is_first = false;
      glVertex2fv(_point_right_1);
      glVertex2fv(_point_left_1);
    }
    glVertex2fv(_point_right_1);
    glVertex2fv(_point_right_2);
    glVertex2fv(_point_left_1);
    glVertex2fv(_point_left_2);
    glEnd();
    glBegin(GL_POINTS);
    glVertex2fv(_point_right_1);
    glVertex2fv(_point_right_2);
    glVertex2fv(_point_left_1);
    glVertex2fv(_point_left_2);
    glEnd();
    
    glPopAttrib();
    
    Vec2f point_min, point_max;
    vec2f_min(point_min,
              vec2f_min(point_min,
                        vec2f_min(point_min,
                                  _point_right_1,
                                  _point_left_1),
                        _point_right_2),
              _point_left_2);
    vec2f_max(point_max,
              vec2f_max(point_max,
                        vec2f_max(point_max,
                                  _point_right_1,
                                  _point_left_1),
                        _point_right_2),
              _point_left_2);
    Vec2f delta;
    vec2f_sub(delta, point_max, point_min);
    GLfloat safe_radius = 4.0f * scalf_max(_point_size, _line_width);
    gl_veci_set(_drawport,
                (GLint) (point_min[0] - safe_radius),
                (GLint) (point_min[1] - safe_radius),
                (GLint) (delta[0] + 2.0f * safe_radius) + 1,
                (GLint) (delta[1] + 2.0f * safe_radius) + 1);
    
    vec2f_eq(_point_1, _point_2);
    vec2f_eq(_point_left_1, _point_left_2);
    vec2f_eq(_point_right_1, _point_right_2);
  }
}

GLboolean
Quill_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  Quill_tool *quill_tool = (Quill_tool *) data;
  
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
  glLineWidth(quill_tool->_line_width);
  glPointSize(quill_tool->_point_size);
  return GL_TRUE;
}
