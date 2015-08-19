#ifndef __TOOL_HH__
#define __TOOL_HH__

#include <gtk/gtk.h>
#include <opengl_utils.h>

#include "cgal_utils.hh"
#include "toolbox.hh"

class Tool {
public:
  typedef Kernel::Point_2 Point;
  
  Tool(Toolbox *toolbox, GLboolean (*display_list_cb)(void *, GLboolean));
  virtual ~Tool(void);
  virtual void clear(void);
  virtual void set_button(GtkWidget *button);
  virtual void active_button(gboolean is_active) const;
  virtual void start_recording_path(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt);
  virtual void stop_recording_path(void);
  virtual void record_path(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  virtual const std::vector<Point>& recorded_path(void) const;
  virtual void start_drawing_pixels(GdkInputSource source,
                                    gdouble x, gdouble y, gdouble pressure,
                                    gdouble xtilt, gdouble ytilt) = 0;
  virtual void stop_drawing_pixels(void) = 0;
  virtual void draw_pixels(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt) = 0;
  virtual const_GLveci_t drawn_pixels(void) const;
  
protected:
  typedef Kernel::FT FT;
  typedef Kernel::Vector_2 Vector;
  
  Toolbox *_toolbox;
  GtkWidget *_button;
  std::vector<Point> _points, _points_buf;
  bool _is_path_interpolated;
  GLveci _drawport;
  GLlist *_list;
};

#endif // __TOOL_HH__
