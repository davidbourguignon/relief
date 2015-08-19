#ifndef __BRUSH_TOOL_HH__
#define __BRUSH_TOOL_HH__

#include <vec2.h>

#include "tool.hh"

class Brush_tool : public Tool {
public:
  Brush_tool(Toolbox *toolbox, GLboolean (*display_list_cb)(void *, GLboolean)
               = _display_list_cb);
  virtual ~Brush_tool(void);
  virtual void start_drawing_pixels(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt);
  virtual void stop_drawing_pixels(void);
  virtual void draw_pixels(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  
protected:
  Vec2f _point;
  GLfloat _size;
  GLvecf _color;
  
private:
  typedef Tool Base;
  
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
};

#endif // __BRUSH_TOOL_HH__
