#ifndef __PENCIL_TOOL_HH__
#define __PENCIL_TOOL_HH__

#include <vec2.h>

#include "tool.hh"

class Pencil_tool : public Tool {
public:
  Pencil_tool(Toolbox *toolbox);
  virtual ~Pencil_tool(void);
  virtual void start_drawing_pixels(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt);
  virtual void stop_drawing_pixels(void);
  virtual void draw_pixels(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  
private:
  typedef Tool Base;
  
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
  
  Vec2f _point_1, _point_2;
  GLfloat _point_size;
  GLfloat _line_width;
  GLvecf _color;
};

#endif // __PENCIL_TOOL_HH__
