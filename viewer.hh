#ifndef __VIEWER_HH__
#define __VIEWER_HH__

#include <gtk/gtk.h>
#include <opengl_utils.h>
#if DEBUG
#include <trackdisk.h>
#endif
#include <trackball.h>

#include "tetrahedrization_display.hh"
#include "cgal_utils.hh"

class Application;
class Drawing;
class Meshing;

class Viewer {
public:
  Viewer(Application *application);
  ~Viewer(void);
  void sync(void);
  
private:
  typedef Kernel::Point_3 Point_3;
  typedef enum {
    _NO_KEYBOARD,
    _KEYBOARD_SPACE,
    _KEYBOARD_C,
    _KEYBOARD_X,
    _KEYBOARD_Z,
#if DEBUG
    _KEYBOARD_E,
    _KEYBOARD_W,
    _KEYBOARD_Q
#endif
  } _KeyboardMode;
  typedef enum {
    _NO_BUTTON,
    _BUTTON_1, // left mouse button and pen tip
    _BUTTON_2, // middle mouse button and pen bottom button
    _BUTTON_3  // right mouse button and pen top button
  } _ButtonMode;
  typedef enum {
    _NO_DISPLAY             = 0,
    _DISPLAY_FPS            = 1 << 0,
    _DISPLAY_AXES           = 1 << 1,
    _DISPLAY_BBOX           = 1 << 2,
#if DEBUG
    _DISPLAY_TRIANGULATION  = 1 << 3,
#endif
    _DISPLAY_DOUBLE_BUFFER  = 1 << 4
  } _DisplayMode;
  typedef enum {
    _EDIT_BLOB,
    _EDIT_UNDO,
    _EDIT_AT_DEPTH
  } _EditMenuType;
  typedef enum {
    _STYLE_POINTS         = Tetrahedrization_display::POINTS,
    _STYLE_WIREFRAME      = Tetrahedrization_display::WIREFRAME,
    _STYLE_SOLID          = Tetrahedrization_display::SOLID,
    _STYLE_FACET_NORMALS  = Tetrahedrization_display::FACET_NORMALS,
#if DEBUG
    _STYLE_VERTEX_NORMALS = Tetrahedrization_display::VERTEX_NORMALS,
#endif
    _STYLE_NPR_GOOCH      = Tetrahedrization_display::NPR_GOOCH,
    _STYLE_NPR_RASKAR     = Tetrahedrization_display::NPR_RASKAR
  } _StyleMenuType;
  typedef enum {
    _MISC_REINIT
  } _MiscMenuType;
  
  static gboolean _delete_event_cb(GtkWidget *widget,
                                   GdkEvent *event,
                                   Viewer *viewer);
  static void _realize_cb(GtkWidget *widget,
                          Viewer *viewer);
  static gboolean _configure_event_cb(GtkWidget *widget,
                                      GdkEventConfigure *event,
                                      Viewer *viewer);
  static gboolean _expose_event_cb(GtkWidget *widget,
                                   GdkEventExpose *event,
                                   Viewer *viewer);
  static gboolean _key_press_event_cb(GtkWidget *widget,
                                      GdkEventKey *event,
                                      Viewer *viewer);
  static gboolean _key_release_event_cb(GtkWidget *widget,
                                        GdkEventKey *event,
                                        Viewer *viewer);
  static gboolean _button_press_event_cb(GtkWidget *widget,
                                         GdkEventButton *event,
                                         Viewer *viewer);
  static gboolean _button_release_event_cb(GtkWidget *widget,
                                           GdkEventButton *event,
                                           Viewer *viewer);
  static gboolean _motion_notify_event_cb(GtkWidget *widget,
                                          GdkEventMotion *event,
                                          Viewer *viewer);
  static void _item_factory_edit_cb(Viewer *viewer,
                                    guint action,
                                    GtkWidget *widget);
  static void _item_factory_style_cb(Viewer *viewer,
                                     Tetrahedrization_display::Style style,
                                     GtkWidget *widget);
  static void _item_factory_display_cb(Viewer *viewer,
                                       guint action,
                                       GtkWidget *widget);
  static void _item_factory_misc_cb(Viewer *viewer,
                                    guint action,
                                    GtkWidget *widget);
  static GLboolean _display_axes_list_cb(void *data, GLboolean test_proxy);
  
  void _create_window(void);
  void _create_menu(void);
  void _set_window(GtkWidget *window);
  void _set_menu(GtkWidget *menu);
  void _set_perspective_projection_matrix(void);
  void _measure_fps(void);
  
  Application *_application;
  Drawing *_drawing;
  Meshing *_meshing;
  GtkWidget *_window;
  GtkWidget *_menu;
  GLerror *_error;
  GTimer *_timer;
  GLtransf *_transf_ortho, *_transf_persp;
#if DEBUG
  Trackdisk *_trackdisk;
#endif
  Trackball *_trackball;
  CGAL::Bbox_3 _axes_bbox;
  GLdouble _fovy, _z_near, _z_far;
  GLlist *_axes_list;
  _KeyboardMode _keyboard_mode;
  _ButtonMode _button_mode;
  guint _display_mode;
  Tetrahedrization_display::Style _tetrahedrization_display_style;
};

#endif // __VIEWER_HH__
