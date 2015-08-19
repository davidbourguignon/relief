#include "tool_texture.hh"
#include "smudge_tool.hh"

using namespace std;

static const GLint DEFAULT_SIZE = 8;
static const GLfloat DEFAULT_RATE = 0.5f;

Smudge_tool::Smudge_tool(Toolbox *toolbox) : Base(toolbox, _display_list_cb) {
  gl_veci_set(_drawport, 0, 0, 2*DEFAULT_SIZE, 2*DEFAULT_SIZE);
  _rate = DEFAULT_RATE;
  _texture = gl_texture_new();
}

Smudge_tool::~Smudge_tool(void) {
  gl_texture_delete(_texture);
}

void
Smudge_tool::clear(void) {
  Base::clear();
  gl_texture_clear(_texture);
}

void
Smudge_tool::start_recording_path(GdkInputSource source,
                                  gdouble x, gdouble y, gdouble pressure,
                                  gdouble xtilt, gdouble ytilt) {}

void
Smudge_tool::stop_recording_path(void) {}

void
Smudge_tool::record_path(GdkInputSource source,
                         gdouble x, gdouble y, gdouble pressure,
                         gdouble xtilt, gdouble ytilt) {}

void
Smudge_tool::start_drawing_pixels(GdkInputSource source,
                                  gdouble x, gdouble y, gdouble pressure,
                                  gdouble xtilt, gdouble ytilt) {
  _drawport[0] = (GLint) x - DEFAULT_SIZE;
  _drawport[1] = (GLint) y - DEFAULT_SIZE;
  gl_texture_set(_texture, GL_TEXTURE_2D, _setup_texture_cb, (void *) this);
}

void
Smudge_tool::stop_drawing_pixels(void) {}

void
Smudge_tool::draw_pixels(GdkInputSource source,
                         gdouble x, gdouble y, gdouble pressure,
                         gdouble xtilt, gdouble ytilt) {
  _drawport[0] = (GLint) x - DEFAULT_SIZE;
  _drawport[1] = (GLint) y - DEFAULT_SIZE;
  
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
               GL_TEXTURE_BIT);
  
  _toolbox->tool_texture()->bind();
  gl_list_call(_list);
  gl_texture_bind(_texture); // Dynamic! Cannot put this in display list!
  glEnable(GL_TEXTURE_2D);   //
  
  glBegin(GL_QUADS);
  glMultiTexCoord2i(GL_TEXTURE0, 0, 0);
  glMultiTexCoord2i(GL_TEXTURE1, 0, 0);
  glVertex2i(_drawport[0],                _drawport[1]);
  glMultiTexCoord2i(GL_TEXTURE0, 1, 0);
  glMultiTexCoord2i(GL_TEXTURE1, 1, 0);
  glVertex2i(_drawport[0] + _drawport[2], _drawport[1]);
  glMultiTexCoord2i(GL_TEXTURE0, 1, 1);
  glMultiTexCoord2i(GL_TEXTURE1, 1, 1);
  glVertex2i(_drawport[0] + _drawport[2], _drawport[1] + _drawport[3]);
  glMultiTexCoord2i(GL_TEXTURE0, 0, 1);
  glMultiTexCoord2i(GL_TEXTURE1, 0, 1);
  glVertex2i(_drawport[0],                _drawport[1] + _drawport[3]);
  glEnd();
  glFinish();
  glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                      _drawport[0], _drawport[1], _drawport[2], _drawport[3]);
  
  glPopAttrib();
}

GLboolean
Smudge_tool::_setup_texture_cb(void *data, GLboolean test_proxy) {
  Smudge_tool *smudge_tool = (Smudge_tool *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1.0f);
  assert(smudge_tool->_drawport[2] % 2 == 0 &&
         smudge_tool->_drawport[3] % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGB,
                 smudge_tool->_drawport[2], smudge_tool->_drawport[3], 0,
                 GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     smudge_tool->_drawport[0], smudge_tool->_drawport[1],
                     smudge_tool->_drawport[2], smudge_tool->_drawport[3], 0);
    return GL_TRUE;
  }
}

GLboolean
Smudge_tool::_display_list_cb(void *data, GLboolean test_proxy) {
  Smudge_tool *smudge_tool = (Smudge_tool *) data;
  
  if (test_proxy) return GL_TRUE;
  glColor4f(1.0f, 1.0f, 1.0f, smudge_tool->_rate);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilMask(0x3); /* 0x1 + 0x2 */
  glStencilFunc(GL_ALWAYS, 0x3, 0x3);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  glActiveTexture(GL_TEXTURE1);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  return GL_TRUE;
}
