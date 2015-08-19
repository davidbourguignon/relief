#include "brush_tool.hh"
#include "scraper_tool.hh"

using namespace std;

static const GLfloat TRANSPARENCY = 0.1f;

Scraper_tool::Scraper_tool(Toolbox *toolbox)
  : Base(toolbox, _display_list_cb) {
  gl_vecf_eq(_color, GL_PURE_BLACK);
  _color[3] = TRANSPARENCY;
}

Scraper_tool::~Scraper_tool(void) {}

void
Scraper_tool::start_recording_path(GdkInputSource source,
                                   gdouble x, gdouble y, gdouble pressure,
                                   gdouble xtilt, gdouble ytilt) {}

void
Scraper_tool::stop_recording_path(void) {}

void
Scraper_tool::record_path(GdkInputSource source,
                          gdouble x, gdouble y, gdouble pressure,
                          gdouble xtilt, gdouble ytilt) {}

void
Scraper_tool::start_drawing_pixels(GdkInputSource source,
                                   gdouble x, gdouble y, gdouble pressure,
                                   gdouble xtilt, gdouble ytilt) {
  draw_pixels(source, x, y, pressure, xtilt, ytilt);
}

GLboolean
Scraper_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x40);
  glStencilFunc(GL_ALWAYS, 0x40, 0x40);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  return GL_TRUE;
}
