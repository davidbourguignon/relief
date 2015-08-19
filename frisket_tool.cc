#include "brush_tool.hh"
#include "frisket_tool.hh"

using namespace std;

static const GLfloat TRANSPARENCY = 0.1f;

Frisket_tool::Frisket_tool(Toolbox *toolbox)
  : Base(toolbox, _display_list_cb) {}

Frisket_tool::~Frisket_tool(void) {}

void
Frisket_tool::start_recording_path(GdkInputSource source,
                                   gdouble x, gdouble y, gdouble pressure,
                                   gdouble xtilt, gdouble ytilt) {}

void
Frisket_tool::stop_recording_path(void) {}

void
Frisket_tool::record_path(GdkInputSource source,
                          gdouble x, gdouble y, gdouble pressure,
                          gdouble xtilt, gdouble ytilt) {}

void
Frisket_tool::start_drawing_pixels(GdkInputSource source,
                                   gdouble x, gdouble y, gdouble pressure,
                                   gdouble xtilt, gdouble ytilt) {
  gl_vecf_eq(_color, _toolbox->background_color());
  _color[3] = TRANSPARENCY;
  draw_pixels(source, x, y, pressure, xtilt, ytilt);
}

GLboolean
Frisket_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x73); /* 0x1 + 0x2 + 0x10 + 0x20 + 0x40 */
  glStencilFunc(GL_ALWAYS, 0x12, 0x73); /* 0x2 + 0x10 */
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  return GL_TRUE;
}
