#ifndef __TOOLBOX_HH__
#define __TOOLBOX_HH__

#include <gtk/gtk.h>
#include <opengl_utils.h>

#include <vector>

class Application;
class Tool;
class Tool_texture;

class Toolbox {
public:
  Toolbox(Application *application);
  ~Toolbox(void);
  void clear(void);
  Tool *core_pointer_tool(void);
  Tool *stylus_pen_tool(void);
  Tool *stylus_eraser_tool(void);
  Tool_texture *tool_texture(void);
  const_GLvecf_t foreground_color(void) const;
  const_GLvecf_t background_color(void) const;
  void sync(GdkInputSource source);
  
private:
  typedef enum {
    _FILE_DEFAULT,
    _FILE_NEW,
    _FILE_OPEN,
    _FILE_IMPORT_OFF,
    _FILE_SAVE,
    _FILE_SAVE_AS,
    _FILE_EXPORT_OFF,
    _FILE_CLOSE,
    _FILE_QUIT
  } _FileMenuType;
  typedef enum {
    _PREFS_INPUT
  } _PrefsMenuType;
  typedef enum {
    _PENCIL_TOOL,
    _QUILL_TOOL,
    _BRUSH_TOOL,
    _SMUDGE_TOOL,
    _ERASER_TOOL,
    _FRISKET_TOOL,
    _BURNISHER_TOOL,
    _SCRAPER_TOOL
  } _ToolType;
  
  static void _item_factory_file_cb(Toolbox *toolbox, guint action,
                                    GtkWidget *widget);
  static void _item_factory_prefs_cb(Toolbox *toolbox, guint action,
                                     GtkWidget *widget);
  static void _item_factory_help_cb(Toolbox *toolbox, guint action,
                                    GtkWidget *widget);
  static void _realize_cb(GtkWidget *widget, gpointer data);
  static gboolean _pencil_button_press_event_cb(GtkWidget *widget,
                                                GdkEventButton *event,
                                                Toolbox *toolbox);
  static gboolean _quill_button_press_event_cb(GtkWidget *widget,
                                               GdkEventButton *event,
                                               Toolbox *toolbox);
  static gboolean _brush_button_press_event_cb(GtkWidget *widget,
                                               GdkEventButton *event,
                                               Toolbox *toolbox);
  static gboolean _smudge_button_press_event_cb(GtkWidget *widget,
                                                GdkEventButton *event,
                                                Toolbox *toolbox);
  static gboolean _eraser_button_press_event_cb(GtkWidget *widget,
                                                GdkEventButton *event,
                                                Toolbox *toolbox);
  static gboolean _frisket_button_press_event_cb(GtkWidget *widget,
                                                 GdkEventButton *event,
                                                 Toolbox *toolbox);
  static gboolean _burnisher_button_press_event_cb(GtkWidget *widget,
                                                   GdkEventButton *event,
                                                   Toolbox *toolbox);
  static gboolean _scraper_button_press_event_cb(GtkWidget *widget,
                                                 GdkEventButton *event,
                                                 Toolbox *toolbox);
  static gboolean _color_toggle_expose_event_cb(GtkWidget *widget,
                                                GdkEventExpose *event,
                                                Toolbox *toolbox);
  static gboolean _color_toggle_button_press_event_cb(GtkWidget *widget,
                                                      GdkEventButton *event,
                                                      Toolbox *toolbox);
  static gboolean _delete_event_cb(GtkWidget *widget,
                                   GdkEvent *event,
                                   Toolbox *toolbox);
  
  Tool *_tool(_ToolType tool_type);
  void _set_source_tool(GdkInputSource source, Tool *tool);
  void _create_window(void);
  void _set_window(GtkWidget *window);
  void _display_input_dialog(void);
  void _quit(void);
  
  Application *_application;
  Tool *_core_pointer_tool;
  Tool *_stylus_pen_tool;
  Tool *_stylus_eraser_tool;
  std::vector<Tool *> _tools;
  Tool_texture *_tool_texture;
  GLvecf _foreground_color, _background_color;
  GtkWidget *_window;
  bool _toggle_black_color;
};

#endif // __TOOLBOX_HH__
