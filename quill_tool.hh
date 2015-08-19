#ifndef __QUILL_TOOL_HH__
#define __QUILL_TOOL_HH__

#include <vec2.h>

#include "tool.hh"

class Quill_tool : public Tool {
public:
  Quill_tool(Toolbox *toolbox);
  virtual ~Quill_tool(void);
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
  
  bool _is_first;
  Vec2f _point_1, _point_2;
  Vec2f _vector_tangent, _vector_perpendicular;
  Vec2f _point_left_1, _point_right_1;
  Vec2f _point_left_2, _point_right_2;
  GLfloat _point_size;
  GLfloat _line_width;
  GLfloat _thickness;
  GLvecf _color;
};

#endif // __QUILL_TOOL_HH__
