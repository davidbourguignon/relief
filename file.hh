#ifndef __FILE_HH__
#define __FILE_HH__

#include <gtk/gtk.h>
#include <string>

class Application;
class Drawing;
class Meshing;
class Viewer;

class File {
public:
  typedef enum {
    RLF,
    OFF
  } Type;
  
  File(Application *application);
  ~File(void);
  void create(void);
  void open(Type type = RLF, const char *name = _NULL_NAME);
  void save(void);
  void save_as(Type type = RLF, const char *name = _NULL_NAME);
  bool close(void);
  const char *name(void);
  
private:
  static gint _file_selection_cb(GtkWidget *widget, File *file);
  
  gint _create_file_selection(const gchar *action, const gchar *extension);
  gint _display_close_message(GtkMessageType type,
                              const gchar *message,
                              const gchar *name) const;
  gint _display_ok_cancel_message(GtkMessageType type,
                                  const gchar *message,
                                  const gchar *name) const;
  void _set_file_selection(GtkWidget *file_selection);
  
  static const char *_NULL_NAME;
  static const char *_DEFAULT_NAME;
  static const std::string _RLF_EXTENSION; // .rlf file
  static const std::string _TR2_EXTENSION; // .tr2 CGAL::Triangulation_2 file
  static const std::string _TR3_EXTENSION; // .tr3 CGAL::Triangulation_3 file
  static const std::string _OFF_EXTENSION; // .off file
  
  Application *_application;
  Drawing *_drawing_proxy;
  Meshing *_meshing_proxy;
  Viewer *_viewer_proxy;
  GtkWidget *_file_selection;
  std::string _name, _name_selected;
};

#endif // __FILE_HH__
