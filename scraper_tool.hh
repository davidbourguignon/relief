#ifndef __SCRAPER_TOOL_HH__
#define __SCRAPER_TOOL_HH__

class Brush_tool;

class Scraper_tool : public Brush_tool {
public:
  Scraper_tool(Toolbox *toolbox);
  virtual ~Scraper_tool(void);
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
  
private:
  typedef Brush_tool Base;
  
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
};

#endif // __SCRAPER_TOOL_HH__
