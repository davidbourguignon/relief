#include <cassert>

#include "images.h"
#include "application.hh"
#include "file.hh"
#include "pencil_tool.hh"
#include "quill_tool.hh"
#include "brush_tool.hh"
#include "smudge_tool.hh"
#include "eraser_tool.hh"
#include "frisket_tool.hh"
#include "burnisher_tool.hh"
#include "scraper_tool.hh"
#include "tool_texture.hh"
#include "toolbox.hh"

using namespace std;

static const guint16 G_MAXUINT16 = 65535;
static const unsigned int TOOL_TYPE_SIZE = 8u;

Toolbox::Toolbox(Application *application)
  : _application(application),
    _core_pointer_tool(NULL),
    _stylus_pen_tool(NULL),
    _stylus_eraser_tool(NULL),
    _tools(TOOL_TYPE_SIZE, (Tool *) 0),
    _tool_texture(NULL),
    _window(NULL),
    _toggle_black_color(false) {
  _core_pointer_tool = _tool(_PENCIL_TOOL);
  _stylus_pen_tool = _tool(_BRUSH_TOOL);
  _stylus_eraser_tool = _tool(_ERASER_TOOL);
  gl_vecf_eq(_foreground_color, GL_PURE_BLACK);
  gl_vecf_eq(_background_color, GL_PURE_GRAY);
  _create_window();
}

Toolbox::~Toolbox(void) {
  for (vector<Tool *>::iterator pti = _tools.begin();
       pti != _tools.end(); pti++) {
    Tool *pt(*pti);
    if (pt != NULL) {
      delete pt;
    }
  }
  if (_tool_texture != NULL) {
    delete _tool_texture;
  }
  if (GTK_IS_WIDGET(_window)) {
    gtk_widget_destroy(_window);
  }
}

void
Toolbox::clear(void) {
  for (vector<Tool *>::iterator pti = _tools.begin();
       pti != _tools.end(); pti++) {
    Tool *pt(*pti);
    if (pt != NULL) {
      pt->clear();
    }
  }
  if (_tool_texture != NULL) {
    _tool_texture->clear();
  }
}

Tool *
Toolbox::core_pointer_tool(void) {
  assert(_core_pointer_tool != NULL);
  return _core_pointer_tool;
}

Tool *
Toolbox::stylus_pen_tool(void) {
  assert(_stylus_pen_tool != NULL);
  return _stylus_pen_tool;
}

Tool *
Toolbox::stylus_eraser_tool(void) {
  assert(_stylus_eraser_tool != NULL);
  return _stylus_eraser_tool;
}

Tool_texture *
Toolbox::tool_texture(void) {
  if (_tool_texture == NULL) {
    _tool_texture = new Tool_texture();
    assert(_tool_texture != NULL);
  }
  return _tool_texture;
}

const_GLvecf_t
Toolbox::foreground_color(void) const {
  return _foreground_color;
}

const_GLvecf_t
Toolbox::background_color(void) const {
  return _background_color;
}

void
Toolbox::sync(GdkInputSource source) {
  static GdkInputSource source_prev = (GdkInputSource) -1;
  
  if (source != source_prev) {
    Tool *tool = NULL;
    
    switch (source) {
    case GDK_SOURCE_MOUSE:
      tool = _core_pointer_tool;
      break;
    case GDK_SOURCE_PEN:
      tool = _stylus_pen_tool;
      break;
    case GDK_SOURCE_ERASER:
      tool = _stylus_eraser_tool;
      break;
    case GDK_SOURCE_CURSOR:
      cerr << "Warning: cursor is an unknown input device source!" << endl;
      break;
    default:
      assert(false);
      break;
    }
    tool->active_button(TRUE);
    source_prev = source;
  }
}

void
Toolbox::_item_factory_file_cb(Toolbox *toolbox, guint action,
                               GtkWidget *widget) {
  switch (action) {
  case _FILE_NEW:
    toolbox->_application->file()->create();
    break;
  case _FILE_OPEN:
    toolbox->_application->file()->open();
    break;
  case _FILE_IMPORT_OFF:
    toolbox->_application->file()->open(File::OFF);
    break;
  case _FILE_SAVE:
    toolbox->_application->file()->save();
    break;
  case _FILE_SAVE_AS:
    toolbox->_application->file()->save_as();
    break;
  case _FILE_EXPORT_OFF:
    toolbox->_application->file()->save_as(File::OFF);
    break;
  case _FILE_CLOSE:
    toolbox->_application->file()->close();
    break;
  case _FILE_QUIT:
    toolbox->_quit();
    break;
  default:
    assert(false);
    break;
  }
}

void
Toolbox::_item_factory_prefs_cb(Toolbox *toolbox, guint action,
                                GtkWidget *widget) {
  switch (action) {
  case _PREFS_INPUT:
    toolbox->_display_input_dialog();
    break;
  default:
    assert(false);
    break;
  }
}

void
Toolbox::_item_factory_help_cb(Toolbox *toolbox, guint action,
                               GtkWidget *widget) {
  GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(toolbox->_window),
                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                             GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_CLOSE,
    "Relief: A Modeling by Drawing Tool\n"
    "Copyright (c) 2004 INRIA\n");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

void
Toolbox::_realize_cb(GtkWidget *widget, gpointer data) {
  gdk_input_set_extension_events(GTK_BUTTON(widget)->event_window,
                                 GDK_BUTTON_PRESS_MASK |
                                 GDK_BUTTON_RELEASE_MASK,
                                 GDK_EXTENSION_EVENTS_CURSOR);
}

gboolean
Toolbox::_pencil_button_press_event_cb(GtkWidget *widget,
                                       GdkEventButton *event,
                                       Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_PENCIL_TOOL));
  return FALSE;
}

gboolean
Toolbox::_quill_button_press_event_cb(GtkWidget *widget, GdkEventButton *event,
                                      Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_QUILL_TOOL));
  return FALSE;
}

gboolean
Toolbox::_brush_button_press_event_cb(GtkWidget *widget, GdkEventButton *event,
                                      Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_BRUSH_TOOL));
  return FALSE;
}

gboolean
Toolbox::_smudge_button_press_event_cb(GtkWidget *widget,
                                       GdkEventButton *event,
                                       Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_SMUDGE_TOOL));
  return FALSE;
}

gboolean
Toolbox::_eraser_button_press_event_cb(GtkWidget *widget,
                                       GdkEventButton *event,
                                       Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_ERASER_TOOL));
  return FALSE;
}

gboolean
Toolbox::_frisket_button_press_event_cb(GtkWidget *widget,
                                        GdkEventButton *event,
                                        Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_FRISKET_TOOL));
  return FALSE;
}

gboolean
Toolbox::_burnisher_button_press_event_cb(GtkWidget *widget,
                                          GdkEventButton *event,
                                          Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_BURNISHER_TOOL));
  return FALSE;
}

gboolean
Toolbox::_scraper_button_press_event_cb(GtkWidget *widget,
                                        GdkEventButton *event,
                                        Toolbox *toolbox) {
  toolbox->_set_source_tool(event->device->source,
                            toolbox->_tool(_SCRAPER_TOOL));
  return FALSE;
}

gboolean
Toolbox::_color_toggle_expose_event_cb(GtkWidget *widget,
                                       GdkEventExpose *event,
                                       Toolbox *toolbox) {
  if (widget->window) {
    GtkStyle *style = gtk_widget_get_style(widget);
    gdk_draw_rectangle(widget->window,
                       style->bg_gc[GTK_STATE_NORMAL],
                       TRUE,
                       event->area.x, event->area.y,
                       event->area.width, event->area.height);
  }
  return TRUE;
}

gboolean
Toolbox::_color_toggle_button_press_event_cb(GtkWidget *widget,
                                             GdkEventButton *event,
                                             Toolbox *toolbox) {
  static GdkColor color;
  if (toolbox->_toggle_black_color) {
    toolbox->_toggle_black_color = false;
    gl_vecf_eq(toolbox->_foreground_color, GL_PURE_BLACK);
  } else {
    toolbox->_toggle_black_color = true;
    gl_vecf_eq(toolbox->_foreground_color, GL_PURE_WHITE);
  }
  color.red   = (guint16) toolbox->_foreground_color[0] * G_MAXUINT16;
  color.blue  = (guint16) toolbox->_foreground_color[1] * G_MAXUINT16;
  color.green = (guint16) toolbox->_foreground_color[2] * G_MAXUINT16;
  gtk_widget_modify_bg(widget, GTK_STATE_NORMAL, &color);
  return TRUE;
}

gboolean
Toolbox::_delete_event_cb(GtkWidget *widget, GdkEvent *event,
                          Toolbox *toolbox) {
  toolbox->_quit();
  return TRUE;
}

Tool *
Toolbox::_tool(_ToolType tool_type) {
  if (_tools[tool_type] == NULL) {
    switch (tool_type) {
    case _PENCIL_TOOL:
      _tools[_PENCIL_TOOL] = new Pencil_tool(this);
      break;
    case _QUILL_TOOL:
      _tools[_QUILL_TOOL] = new Quill_tool(this);
      break;
    case _BRUSH_TOOL:
      _tools[_BRUSH_TOOL] = new Brush_tool(this);
      break;
    case _SMUDGE_TOOL:
      _tools[_SMUDGE_TOOL] = new Smudge_tool(this);
      break;
    case _ERASER_TOOL:
      _tools[_ERASER_TOOL] = new Eraser_tool(this);
      break;
    case _FRISKET_TOOL:
      _tools[_FRISKET_TOOL] = new Frisket_tool(this);
      break;
    case _BURNISHER_TOOL:
      _tools[_BURNISHER_TOOL] = new Burnisher_tool(this);
      break;
    case _SCRAPER_TOOL:
      _tools[_SCRAPER_TOOL] = new Scraper_tool(this);
      break;
    default:
      assert(false);
      break;
    }
    assert(_tools[tool_type] != NULL);
  }
  return _tools[tool_type];
}

void
Toolbox::_set_source_tool(GdkInputSource source, Tool *tool) {
  switch (source) {
  case GDK_SOURCE_MOUSE:
    _core_pointer_tool = tool;
    break;
  case GDK_SOURCE_PEN:
    _stylus_pen_tool = tool;
    break;
  case GDK_SOURCE_ERASER:
    _stylus_eraser_tool = tool;
    break;
  case GDK_SOURCE_CURSOR:
    cerr << "Warning: cursor is an unknown input device source!" << endl;
    break;
  default:
    assert(false);
    break;
  }
}

void
Toolbox::_create_window(void) {
  void (*f)(void) = (void (*)(void)) _item_factory_file_cb;
  void (*p)(void) = (void (*)(void)) _item_factory_prefs_cb;
  void (*h)(void) = (void (*)(void)) _item_factory_help_cb;
  
  GtkItemFactoryEntry items[] = {
    {"/_File",            "<CTRL>F", NULL, 0,                "<Branch>"},
    {"/File/_New",        "<CTRL>N", f,    _FILE_NEW,        "<Item>"},
#if DEBUG // still under development
    {"/File/_Open",       "<CTRL>O", f,    _FILE_OPEN,       "<Item>"},
#endif
    {"/File/_Import OFF", "<CTRL>I", f,    _FILE_IMPORT_OFF, "<Item>"},
#if DEBUG // still under development
    {"/File/Separator1",  NULL,      NULL, 0,                "<Separator>"},
    {"/File/_Save",       "<CTRL>S", f,    _FILE_SAVE,       "<Item>"},
    {"/File/Save _As",    "<CTRL>A", f,    _FILE_SAVE_AS,    "<Item>"},
#endif
    {"/File/_Export OFF", "<CTRL>E", f,    _FILE_EXPORT_OFF, "<Item>"},
    {"/File/Separator2",  NULL,      NULL, 0,                "<Separator>"},
    {"/File/Close",       "<CTRL>W", f,    _FILE_CLOSE,      "<Item>"},
    {"/File/_Quit",       "<CTRL>Q", f,    _FILE_QUIT,       "<Item>"},
    
    {"/_Preferences",              "<CTRL>P", NULL, 0,           "<Branch>"},
    {"/Preferences/Input devices", NULL,      p,    _PREFS_INPUT, "<Item>"},
    
    {"/_Help",                     "<CTRL>H", NULL, 0, "<LastBranch>"},
    {"/Help/About",                NULL,      h,    0, "<Item>"},
  };
  
  int nitems = sizeof(items) / sizeof(GtkItemFactoryEntry);
  
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Relief");
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_delete_event_cb), this);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &_window);
  
//   gtk_widget_show(window);
  
  GtkWidget *table = gtk_table_new(3, 5, TRUE);
  gtk_container_add(GTK_CONTAINER(window), table);
  
  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  
  GtkItemFactory *item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR,
                                                      "<ToolboxMain>",
                                                      accel_group);
  gtk_item_factory_create_items(item_factory, nitems, items, this);
  gtk_table_attach_defaults(GTK_TABLE(table),
                            gtk_item_factory_get_widget(item_factory,
                                                        "<ToolboxMain>"),
                            0, 5, 0, 1);
  
  GtkTooltips *tooltips = gtk_tooltips_new();
  
//   GtkStyle *style = gtk_widget_get_style(window);
//   GdkBitmap *mask = NULL;
//   GdkPixmap *pixmap
//     = gdk_pixmap_create_from_xpm_d(window->window,
//                                 &mask, &style->bg[GTK_STATE_NORMAL],
//                                    (gchar **) BRUSH_XPM);
//   GtkWidget *pencil_image = gtk_image_new_from_pixmap(pixmap, mask);
  
  vector<GtkWidget *> buttons(_tools.size(), (GtkWidget *) 0);
  
  GtkWidget *pencil_image = gtk_image_new_from_file("xpm/pencil.xpm");
  GtkWidget *pencil_button
    = gtk_radio_button_new(NULL);
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(pencil_button), FALSE);
  g_signal_connect(G_OBJECT(pencil_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(pencil_button), "button_press_event",
                   G_CALLBACK(_pencil_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(pencil_button), pencil_image);
  gtk_table_attach_defaults(GTK_TABLE(table), pencil_button, 0, 1, 1, 2);
  gtk_tooltips_set_tip(tooltips, pencil_button, "Pencil", NULL);
  buttons[_PENCIL_TOOL] = pencil_button;
  
  GtkWidget *quill_image = gtk_image_new_from_file("xpm/quill.xpm");
  GtkWidget *quill_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(quill_button), FALSE);
  g_signal_connect(G_OBJECT(quill_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(quill_button), "button_press_event",
                   G_CALLBACK(_quill_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(quill_button), quill_image);
  gtk_table_attach_defaults(GTK_TABLE(table), quill_button, 1, 2, 1, 2);
  gtk_tooltips_set_tip(tooltips, quill_button, "Quill", NULL);
  buttons[_QUILL_TOOL] = quill_button;
  
  GtkWidget *brush_image = gtk_image_new_from_file("xpm/brush.xpm");
  GtkWidget *brush_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(brush_button), FALSE);
  g_signal_connect(G_OBJECT(brush_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(brush_button), "button_press_event",
                   G_CALLBACK(_brush_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(brush_button), brush_image);
  gtk_table_attach_defaults(GTK_TABLE(table), brush_button, 2, 3, 1, 2);
  gtk_tooltips_set_tip(tooltips, brush_button, "Brush", NULL);
  buttons[_BRUSH_TOOL] = brush_button;
  
  GtkWidget *smudge_image = gtk_image_new_from_file("xpm/smudge.xpm");
  GtkWidget *smudge_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(smudge_button), FALSE);
  g_signal_connect(G_OBJECT(smudge_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(smudge_button), "button_press_event",
                   G_CALLBACK(_smudge_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(smudge_button), smudge_image);
  gtk_table_attach_defaults(GTK_TABLE(table), smudge_button, 3, 4, 1, 2);
  gtk_tooltips_set_tip(tooltips, smudge_button, "Smudge", NULL);
  buttons[_SMUDGE_TOOL] = smudge_button;
  
  GtkWidget *eraser_image = gtk_image_new_from_file("xpm/eraser.xpm");
  GtkWidget *eraser_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(eraser_button), FALSE);
  g_signal_connect(G_OBJECT(eraser_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(eraser_button), "button_press_event",
                   G_CALLBACK(_eraser_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(eraser_button), eraser_image);
  gtk_table_attach_defaults(GTK_TABLE(table), eraser_button, 4, 5, 1, 2);
  gtk_tooltips_set_tip(tooltips, eraser_button, "Eraser", NULL);
  buttons[_ERASER_TOOL] = eraser_button;
  
  GtkWidget* event_box = gtk_event_box_new();
  gtk_table_attach_defaults(GTK_TABLE(table), event_box, 0, 2, 2, 3);
  gtk_tooltips_set_tip(tooltips, event_box, "Toggle Color", NULL);
  
  GtkWidget *vbox = gtk_vbox_new(FALSE, 4);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 4);
  gtk_container_add(GTK_CONTAINER(event_box), vbox);
  
  GtkWidget *frame = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);
  
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_container_add(GTK_CONTAINER(frame), drawing_area);
  gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
  g_signal_connect(drawing_area, "expose_event",
                   G_CALLBACK(_color_toggle_expose_event_cb), this);
  g_signal_connect(drawing_area, "button_press_event",
                   G_CALLBACK(_color_toggle_button_press_event_cb), this);
  GdkColor color;
  color.red   = (guint16) _foreground_color[0] * G_MAXUINT16;
  color.blue  = (guint16) _foreground_color[1] * G_MAXUINT16;
  color.green = (guint16) _foreground_color[2] * G_MAXUINT16;
  gtk_widget_modify_bg(drawing_area, GTK_STATE_NORMAL, &color);
  
  GtkWidget *frisket_image = gtk_image_new_from_file("xpm/frisket.xpm");
  GtkWidget *frisket_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(frisket_button), FALSE);
  g_signal_connect(G_OBJECT(frisket_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(frisket_button), "button_press_event",
                   G_CALLBACK(_frisket_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(frisket_button), frisket_image);
  gtk_table_attach_defaults(GTK_TABLE(table), frisket_button, 2, 3, 2, 3);
  gtk_tooltips_set_tip(tooltips, frisket_button, "Frisket", NULL);
  buttons[_FRISKET_TOOL] = frisket_button;
  
  GtkWidget *burnisher_image = gtk_image_new_from_file("xpm/burnisher.xpm");
  GtkWidget *burnisher_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(burnisher_button), FALSE);
  g_signal_connect(G_OBJECT(burnisher_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(burnisher_button), "button_press_event",
                   G_CALLBACK(_burnisher_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(burnisher_button), burnisher_image);
  gtk_table_attach_defaults(GTK_TABLE(table), burnisher_button, 3, 4, 2, 3);
  gtk_tooltips_set_tip(tooltips, burnisher_button, "Burnisher", NULL);
  buttons[_BURNISHER_TOOL] = burnisher_button;
  
  GtkWidget *scraper_image = gtk_image_new_from_file("xpm/scraper.xpm");
  GtkWidget *scraper_button
    = gtk_radio_button_new_from_widget(GTK_RADIO_BUTTON(pencil_button));
  gtk_toggle_button_set_mode(GTK_TOGGLE_BUTTON(scraper_button), FALSE);
  g_signal_connect(G_OBJECT(scraper_button), "realize",
                   G_CALLBACK(_realize_cb), NULL);
  g_signal_connect(G_OBJECT(scraper_button), "button_press_event",
                   G_CALLBACK(_scraper_button_press_event_cb), this);
  gtk_container_add(GTK_CONTAINER(scraper_button), scraper_image);
  gtk_table_attach_defaults(GTK_TABLE(table), scraper_button, 4, 5, 2, 3);
  gtk_tooltips_set_tip(tooltips, scraper_button, "Scraper", NULL);
  buttons[_SCRAPER_TOOL] = scraper_button;
  
  gtk_widget_show_all(window);
  
  for (unsigned int i = 0; i < _tools.size(); i++) {
    _tool((_ToolType) i)->set_button(buttons[i]);
  }
  _set_window(window);
}

void
Toolbox::_set_window(GtkWidget *window) {
  if (GTK_IS_WIDGET(_window)) {
    gtk_widget_destroy(_window);
  }
  _window = window;
  assert(_window != NULL);
}

void
Toolbox::_quit(void) {
//   if (_application->file()->close()) {
  //TODO: change the way this application works...
  _application->file()->close();
  _application->remove(this);
//   }
}

void
Toolbox::_display_input_dialog(void) {
  GtkWidget *inputd = gtk_input_dialog_new();
  g_signal_connect(G_OBJECT(inputd), "delete_event",
                   G_CALLBACK(gtk_widget_destroy), inputd);
//   g_signal_connect(G_OBJECT(inputd), "destroy",
//                 G_CALLBACK(gtk_widget_destroyed), &inputd);
  g_signal_connect_swapped(G_OBJECT(GTK_INPUT_DIALOG(inputd)->close_button),
                           "clicked", G_CALLBACK(gtk_widget_destroy),
                           G_OBJECT(inputd));
  gtk_widget_hide(GTK_INPUT_DIALOG(inputd)->save_button);
  gtk_widget_show(inputd);
}
