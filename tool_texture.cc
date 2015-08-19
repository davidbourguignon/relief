#include <iostream>

#include "images.h"
#include "tool_texture.hh"

using namespace std;

static const GLsizei DEFAULT_IMAGE_SIZE = 64;
static const GLsizei DEFAULT_GRADIENT_SIZE = 28;

Tool_texture::Tool_texture(void) {
  _image = gl_framebuf_new();
  _texture = gl_texture_new();
  _list = gl_list_new();
  _is_texture_set = false;
  
  _image->width = _image->height = DEFAULT_IMAGE_SIZE;
  gl_framebuf_set_format(_image, GL_ALPHA);
  _image->pixels = circle_gradient_image(DEFAULT_IMAGE_SIZE,
                                         DEFAULT_GRADIENT_SIZE,
                                         GL_UBYTE_MAX, GL_UBYTE_MIN);
  gl_list_set(_list, _display_list_cb, (void *) this, GL_FALSE);
}

Tool_texture::~Tool_texture(void) {
  gl_framebuf_delete(_image);
  gl_texture_delete(_texture);
  gl_list_delete(_list);
}

void
Tool_texture::clear(void) {
  _is_texture_set = false;
  gl_texture_clear(_texture);
  gl_list_clear(_list);
}

void
Tool_texture::bind(void) {
  if (!_is_texture_set) {
    _is_texture_set = true;
    gl_texture_set(_texture, GL_TEXTURE_2D, _setup_texture_cb, (void *) this);
  }
  gl_list_call(_list);
}

void
Tool_texture::release(void) {
  gl_texture_release(_texture);
}

GLboolean
Tool_texture::_setup_texture_cb(void *data, GLboolean test_proxy) {
  Tool_texture *tool_texture = (Tool_texture *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_PRIORITY, 1.0f);
  assert(tool_texture->_image->width  % 2 == 0 &&
         tool_texture->_image->height % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_ALPHA,
                 tool_texture->_image->width, tool_texture->_image->height,
                 0, tool_texture->_image->format, tool_texture->_image->type,
                 tool_texture->_image->pixels);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 tool_texture->_image->width, tool_texture->_image->height,
                 0, tool_texture->_image->format, tool_texture->_image->type,
                 tool_texture->_image->pixels);
    return GL_TRUE;
  }
}

GLboolean
Tool_texture::_display_list_cb(void *data, GLboolean test_proxy) {
  Tool_texture *tool_texture = (Tool_texture *) data;
  
  if (test_proxy) return GL_TRUE;
  glActiveTexture(GL_TEXTURE0);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gl_texture_bind(tool_texture->_texture);
  glEnable(GL_TEXTURE_2D);
  return GL_TRUE;
}
