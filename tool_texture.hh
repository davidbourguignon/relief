#ifndef __TOOL_TEXTURE_HH__
#define __TOOL_TEXTURE_HH__

#include <opengl_utils.h>
#include <opengl_buffer.h>

class Tool_texture {
public:
  Tool_texture(void);
  ~Tool_texture(void);
  void clear(void);
  void bind(void);
  void release(void);
  
private:
  static GLboolean _setup_texture_cb(void *data, GLboolean test_proxy);
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
  
  GLframebuf *_image;
  GLtexture *_texture;
  GLlist *_list;
  bool _is_texture_set;
};

#endif // __TOOL_TEXTURE_HH__
