#include "tool_texture.hh"
#include "brush_tool.hh"

// FIXME: Problems encountered with linuxwacom under mdk-9.2
#define LINUXWACOM_RH_8_0 0

using namespace std;

static const GLfloat DEFAULT_SIZE = 10.0f;
#if LINUXWACOM_RH_8_0
static const gdouble PRESSURE_SIZE_MAX =  8.0;
static const gdouble TILT_SIZE_MAX     = 14.0;
#else
static const gdouble SIZE_MIN =  6.0;
static const gdouble SIZE_MAX = 12.0;
#endif

Brush_tool::Brush_tool(Toolbox *toolbox,
                       GLboolean (*display_list_cb)(void *, GLboolean))
  : Base(toolbox, display_list_cb) {
  vec2f_eq(_point, VEC2F_NULL);
  _size = 0.0f;
  gl_vecf_eq(_color, GL_PURE_BLACK);
}

Brush_tool::~Brush_tool(void) {}

void
Brush_tool::start_drawing_pixels(GdkInputSource source,
                                 gdouble x, gdouble y, gdouble pressure,
                                 gdouble xtilt, gdouble ytilt) {
  gl_vecf_eq(_color, _toolbox->foreground_color());
  draw_pixels(source, x, y, pressure, xtilt, ytilt);
}

void
Brush_tool::stop_drawing_pixels(void) {}

void
Brush_tool::draw_pixels(GdkInputSource source,
                        gdouble x, gdouble y, gdouble pressure,
                        gdouble xtilt, gdouble ytilt) {
  vec2f_set(_point, x, y);
  
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
               GL_TEXTURE_BIT);
  
  switch (source) {
  case GDK_SOURCE_MOUSE:
    _size = DEFAULT_SIZE;
    glColor4fv(_color);
    break;
  case GDK_SOURCE_PEN:
  case GDK_SOURCE_ERASER:
#if LINUXWACOM_RH_8_0
    _size = scald_max(PRESSURE_SIZE_MAX * pressure,
                      TILT_SIZE_MAX * sqrt(xtilt*xtilt + ytilt*ytilt));
#else
    _size = scald_max(SIZE_MIN, SIZE_MAX * pressure);
#endif
    glColor4f(_color[0], _color[1], _color[2], _color[3] * pressure);
    break;
  case GDK_SOURCE_CURSOR:
    cerr << "Warning: cursor is an unknown input device source!" << endl;
    break;
  default:
    assert(false);
    break;
  }
  _toolbox->tool_texture()->bind();
  gl_list_call(_list);
  glBegin(GL_QUADS);
  glTexCoord2i(0, 0);
  glVertex2f(_point[0] - _size, _point[1] - _size);
  glTexCoord2i(1, 0);
  glVertex2f(_point[0] + _size, _point[1] - _size);
  glTexCoord2i(1, 1);
  glVertex2f(_point[0] + _size, _point[1] + _size);
  glTexCoord2i(0, 1);
  glVertex2f(_point[0] - _size, _point[1] + _size);
  glEnd();
  
  glPopAttrib();
  
  gl_veci_set(_drawport,
              (GLint) (_point[0] - _size) - 1,
              (GLint) (_point[1] - _size) - 1,
              (GLint) (2.0f * _size) + 1 + 2,
              (GLint) (2.0f * _size) + 1 + 2);
  // our safe radius is equal to 1 (-1 bottom, +2 top)
}

GLboolean
Brush_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x1);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  return GL_TRUE;
}
