#include "brush_tool.hh"
#include "burnisher_tool.hh"

using namespace std;

static const GLfloat TRANSPARENCY = 0.1f;

Burnisher_tool::Burnisher_tool(Toolbox *toolbox)
  : Base(toolbox, _display_list_cb) {
  gl_vecf_eq(_color, GL_PURE_WHITE);
  _color[3] = TRANSPARENCY;
}

Burnisher_tool::~Burnisher_tool(void) {}

void
Burnisher_tool::start_recording_path(GdkInputSource source,
                                     gdouble x, gdouble y, gdouble pressure,
                                     gdouble xtilt, gdouble ytilt) {}

void
Burnisher_tool::stop_recording_path(void) {}

void
Burnisher_tool::record_path(GdkInputSource source,
                            gdouble x, gdouble y, gdouble pressure,
                            gdouble xtilt, gdouble ytilt) {}

void
Burnisher_tool::start_drawing_pixels(GdkInputSource source,
                                     gdouble x, gdouble y, gdouble pressure,
                                     gdouble xtilt, gdouble ytilt) {
  draw_pixels(source, x, y, pressure, xtilt, ytilt);
}

GLboolean
Burnisher_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x20);
  glStencilFunc(GL_ALWAYS, 0x20, 0x20);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  return GL_TRUE;
}
