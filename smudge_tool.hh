#ifndef __SMUDGE_TOOL_HH__
#define __SMUDGE_TOOL_HH__

#include "tool.hh"

class Smudge_tool : public Tool {
public:
  Smudge_tool(Toolbox *toolbox);
  virtual ~Smudge_tool(void);
  virtual void clear(void);
  virtual void start_recording_path(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt);
  virtual void stop_recording_path(void);
  virtual void record_path(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  virtual void start_drawing_pixels(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt);
  virtual void stop_drawing_pixels(void);
  virtual void draw_pixels(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  
private:
  typedef Tool Base;
  
  static GLboolean _setup_texture_cb(void *data, GLboolean test_proxy);
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
  
  GLfloat _rate;
  GLtexture *_texture;
};

#endif // __SMUDGE_TOOL_HH__
