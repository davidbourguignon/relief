#include <gtk/gtkgl.h>
#include <gdk/gdkkeysyms.h>
#include <cassert>

#include "application.hh"
#include "toolbox.hh"
#include "file.hh"
#include "drawing.hh"
#include "meshing.hh"
#include "viewer.hh"

using namespace std;

static const GLdouble DEFAULT_FOVY = 45.0;
static const GLdouble DEFAULT_Z_NEAR = 2.0;
static const GLdouble DEFAULT_Z_FAR = 6.0;

Viewer::Viewer(Application *application) {
  _application = application;
  _drawing = application->drawing();
  _meshing = application->meshing();
  _window = NULL;
  _menu = NULL;
  _error = gl_error_new();
  _timer = g_timer_new();
  _transf_ortho = gl_transf_new();
  _transf_persp = gl_transf_new();
#if DEBUG
  _trackdisk = trackdisk_new();
#endif
  _trackball = trackball_new();
  _axes_bbox = CGAL::Bbox_3(-1.05, -1.05, -1.05, 1.05, 1.05, 1.05);
  _fovy = DEFAULT_FOVY;
  _z_near = DEFAULT_Z_NEAR;
  _z_far = DEFAULT_Z_FAR;
  _axes_list = gl_list_new();
  _keyboard_mode = _NO_KEYBOARD;
  _button_mode = _NO_BUTTON;
  _display_mode = _DISPLAY_DOUBLE_BUFFER;
  _tetrahedrization_display_style = Tetrahedrization_display::SOLID;
  
  _create_window();
  _create_menu();
  g_timer_start(_timer);
  
  Quatd q, q1, q2;
  Vec3d u, v;
#if DEBUG
  trackdisk_set_init_transf(_trackdisk,
                            COMPLXD_ID, vec2d_set(u, 0.0, 0.0), 1.0);
  trackdisk_init_transf(_trackdisk);
  gl_transf_from_trackdisk(_transf_ortho, _trackdisk);
#endif
  quatd_set_axis_angle(q1, vec3d_set(v, 1.0, 0.0, 0.0), -M_PI_4 / 4);
  quatd_set_axis_angle(q2, vec3d_set(v, 0.0, 1.0, 0.0), +M_PI_4 / 2);
  trackball_set_init_transf(_trackball, quatd_mult(q, q1, q2),
                            vec3d_set(v, 0.0, 0.0, -4.0));
  trackball_init_transf(_trackball);
  gl_transf_from_trackball(_transf_persp, _trackball);
  
  gl_list_set(_axes_list, _display_axes_list_cb, NULL, GL_FALSE);
}

Viewer::~Viewer(void) {
  gl_list_delete(_axes_list);
  if (GTK_IS_WIDGET(_window)) {
    gtk_widget_destroy(_window);
  }
  if (GTK_IS_WIDGET(_menu)) {
    gtk_widget_destroy(_menu);
  }
  gl_error_delete(_error);
  g_timer_destroy(_timer);
  gl_transf_delete(_transf_ortho);
  gl_transf_delete(_transf_persp);
#if DEBUG
  trackdisk_delete(_trackdisk);
#endif
  trackball_delete(_trackball);
}

void
Viewer::sync(void) {
  gtk_window_set_title(GTK_WINDOW(_window), _application->file()->name());
}

gboolean
Viewer::_delete_event_cb(GtkWidget *widget, GdkEvent *event, Viewer *viewer) {
  viewer->_application->toolbox()->clear();
  viewer->_application->file()->close();
  return TRUE;
}

void
Viewer::_realize_cb(GtkWidget *widget, Viewer *viewer) {
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
  
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return;
  }
  // nothing to do
  gdk_gl_drawable_gl_end(gldrawable);
  return;
}

gboolean
Viewer::_configure_event_cb(GtkWidget *widget, GdkEventConfigure *event,
                            Viewer *viewer) {
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
  const GLsizei w = widget->allocation.width;
  const GLsizei h = widget->allocation.height;
  
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  
  glViewport(0, 0, w, h);
  gl_transf_set_viewport(viewer->_transf_ortho);
  gl_transf_set_viewport(viewer->_transf_persp);
  
  glPushAttrib(GL_TRANSFORM_BIT);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glPushMatrix();
  gluOrtho2D(0, w, 0, h);
  gl_transf_set_proj_mat(viewer->_transf_ortho);
  glPopMatrix();
  glPushMatrix();
  gluPerspective(viewer->_fovy, (GLdouble) w / (GLdouble) h,
                 viewer->_z_near, viewer->_z_far);
  gl_transf_set_proj_mat(viewer->_transf_persp);
  glPopMatrix();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glPopAttrib();
  
#if DEBUG
  trackdisk_reshape(viewer->_trackdisk, w, h);
#endif
  trackball_reshape(viewer->_trackball, w, h);
  
  gdk_gl_drawable_gl_end(gldrawable);
  return TRUE;
}

gboolean
Viewer::_expose_event_cb(GtkWidget *widget, GdkEventExpose *event,
                         Viewer *viewer) {
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
  
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  
  if (viewer->_display_mode & _DISPLAY_DOUBLE_BUFFER) {
    gl_transf_begin(viewer->_transf_persp);
    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    
    glColor4fv(viewer->_application->toolbox()->foreground_color());
    glClearColor(viewer->_drawing->background_color()[0],
                 viewer->_drawing->background_color()[1],
                 viewer->_drawing->background_color()[2],
                 viewer->_drawing->background_color()[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    viewer->_meshing->display_tetrahedrization(
      viewer->_tetrahedrization_display_style,
      viewer->_display_mode & _DISPLAY_BBOX);
    if (viewer->_display_mode & _DISPLAY_AXES) {
      gl_list_call(viewer->_axes_list);
    }
    
    glPopAttrib();
    gl_transf_end(viewer->_transf_persp);
  } else {
#if DEBUG
    if (viewer->_display_mode & _DISPLAY_TRIANGULATION) {
      glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
      glDrawBuffer(GL_FRONT);
      glColor4fv(viewer->_application->toolbox()->foreground_color());
      glClearColor(viewer->_drawing->background_color()[0],
                   viewer->_drawing->background_color()[1],
                   viewer->_drawing->background_color()[2],
                   viewer->_drawing->background_color()[3]);
      glClear(GL_COLOR_BUFFER_BIT);
      viewer->_drawing->display_triangulation(
        Triangulation_display::STANDARD,
        viewer->_display_mode & _DISPLAY_BBOX,
        true);
      glPopAttrib();
    } else {
#endif
      viewer->_drawing->draw();
#if DEBUG
    }
#endif
  }
  
#if DEBUG
  gl_error_report(viewer->_error);
#endif
  
  if (viewer->_display_mode & _DISPLAY_DOUBLE_BUFFER) {
    gdk_gl_drawable_swap_buffers(gldrawable);
  } else {
    glFinish();
  }
  
  gdk_gl_drawable_gl_end(gldrawable);
  
  viewer->_measure_fps();
  return TRUE;
}

gboolean
Viewer::_key_press_event_cb(GtkWidget *widget, GdkEventKey *event,
                            Viewer *viewer) {
  switch (event->keyval) {
  case GDK_Shift_L:
  case GDK_Shift_R:
  case GDK_Shift_Lock:
    // nothing to do
    break;
  case GDK_space:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_SPACE;
    }
    break;
  case GDK_c:
  case GDK_C:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_C;
    }
    break;
  case GDK_x:
  case GDK_X:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_X;
    }
    break;
  case GDK_z:
  case GDK_Z:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_Z;
    }
    break;
#if DEBUG
  case GDK_e:
  case GDK_E:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_E;
    }
    break;
  case GDK_w:
  case GDK_W:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_W;
    }
    break;
  case GDK_q:
  case GDK_Q:
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _KEYBOARD_Q;
    }
    break;
#endif
  case GDK_h:
  case GDK_H:
    printf("K e y b o a r d  H e l p\n");
    printf("\n");
    printf("Shift - set marking stroke\n");
    printf("Space - apply changes\n");
    printf("c     - activate trackball rotation\n");
    printf("x     - activate trackball translation Xy\n");
    printf("z     - activate trackball translation Z\n");
    printf("h     - print keyboard help\n");
    printf("All keys except 'h' must be used in combination "
           "with the left mouse button\n");
    break;
  default:
    fprintf(stderr, "Error: unknown key value!\n");
    break;
  }
  return TRUE;
}

gboolean
Viewer::_key_release_event_cb(GtkWidget *widget, GdkEventKey *event,
                              Viewer *viewer) {
  switch (event->keyval) {
  case GDK_Shift_L:
  case GDK_Shift_R:
  case GDK_Shift_Lock:
    // nothing to do
    break;
  case GDK_space:
  case GDK_c:
  case GDK_C:
  case GDK_x:
  case GDK_X:
  case GDK_z:
  case GDK_Z:
#if DEBUG
  case GDK_e:
  case GDK_E:
  case GDK_w:
  case GDK_W:
  case GDK_q:
  case GDK_Q:
#endif
    if (viewer->_button_mode == _NO_BUTTON) {
      viewer->_keyboard_mode = _NO_KEYBOARD;
    }
    break;
  case GDK_h:
  case GDK_H:
    // nothing to do
    break;
  default:
    break;
  }
  return TRUE;
}

gboolean
Viewer::_button_press_event_cb(GtkWidget *widget, GdkEventButton *event,
                               Viewer *viewer) {
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
  
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  
  switch (event->button) {
  case _BUTTON_1: {
    viewer->_button_mode = _BUTTON_1;
    switch (viewer->_keyboard_mode) {
    case _NO_KEYBOARD:
      if (viewer->_display_mode & _DISPLAY_DOUBLE_BUFFER) {
        viewer->_display_mode -= _DISPLAY_DOUBLE_BUFFER;
        viewer->_meshing->stop_meshing();
        gl_transf_begin(viewer->_transf_ortho);
        viewer->_drawing->start_drawing(viewer->_transf_ortho->viewport);
      }
      break;
    case _KEYBOARD_SPACE:
    case _KEYBOARD_C:
    case _KEYBOARD_X:
    case _KEYBOARD_Z:
      if (!(viewer->_display_mode & _DISPLAY_DOUBLE_BUFFER)) {
        viewer->_display_mode += _DISPLAY_DOUBLE_BUFFER;
        viewer->_meshing->start_meshing(viewer->_drawing->drawbox());
        viewer->_drawing->stop_drawing();
        gl_transf_end(viewer->_transf_ortho);
        viewer->_meshing->mesh(viewer->_transf_ortho, viewer->_transf_persp);
        //viewer->_meshing->stop_meshing();
      }
      break;
    default:
      break;
    }
    switch (viewer->_keyboard_mode) {
    case _NO_KEYBOARD: {
      gdouble pressure;
      gdouble xtilt, ytilt;
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_PRESSURE, &pressure);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_XTILT, &xtilt);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_YTILT, &ytilt);
      viewer->_drawing->start_drawing_stroke(
        event->device->source, event->state,
        event->x, widget->allocation.height - 1 - event->y,
        pressure, xtilt, ytilt);
    } break;
    case _KEYBOARD_SPACE:
      // nothing to do
      break;
    case _KEYBOARD_C:
      trackball_start_rotation(viewer->_trackball,
                               (int) event->x, (int) event->y);
      break;
    case _KEYBOARD_X:
      trackball_start_transl_xy(viewer->_trackball,
                                (int) event->x, (int) event->y);
      break;
    case _KEYBOARD_Z:
      trackball_start_transl_z(viewer->_trackball,
                               (int) event->x, (int) event->y);
      break;
#if DEBUG
    case _KEYBOARD_E:
      trackdisk_start_rotation(viewer->_trackdisk,
                               (int) event->x, (int) event->y);
      break;
    case _KEYBOARD_W:
      trackdisk_start_transl(viewer->_trackdisk,
                             (int) event->x, (int) event->y);
      break;
    case _KEYBOARD_Q:
      trackdisk_start_scale(viewer->_trackdisk,
                            (int) event->x, (int) event->y);
      break;
#endif
    default:
      assert(false);
      break;
    }
  } break;
  case _BUTTON_2:
    viewer->_button_mode = _BUTTON_2;
    break;
  case _BUTTON_3:
    viewer->_button_mode = _BUTTON_3;
    gtk_menu_popup(GTK_MENU(viewer->_menu), NULL, NULL, NULL, NULL,
                   event->button, event->time);
    // CAVEAT: after a popup, the button release callback is sometimes not
    // called!
    viewer->_button_mode = _NO_BUTTON;
    break;
  default:
    assert(false);
    break;
  }
  
  gdk_gl_drawable_gl_end(gldrawable);
  gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);
  gdk_window_process_updates(widget->window, FALSE);
  return TRUE;
}

gboolean
Viewer::_button_release_event_cb(GtkWidget *widget, GdkEventButton *event,
                                 Viewer *viewer) {
  GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
  
  if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
    return FALSE;
  }
  
  switch (event->button) {
  case _BUTTON_1:
    viewer->_button_mode = _NO_BUTTON;
    switch (viewer->_keyboard_mode) {
    case _NO_KEYBOARD: {
      gdouble pressure;
      gdouble xtilt, ytilt;
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_PRESSURE, &pressure);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_XTILT, &xtilt);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_YTILT, &ytilt);
      viewer->_drawing->stop_drawing_stroke(
        event->device->source, event->state,
        event->x, widget->allocation.height - 1 - event->y,
        pressure, xtilt, ytilt);
    } break;
    case _KEYBOARD_SPACE:
      // nothing to do
      break;
    case _KEYBOARD_C:
      trackball_stop_rotation(viewer->_trackball);
      break;
    case _KEYBOARD_X:
      trackball_stop_transl_xy(viewer->_trackball);
      break;
    case _KEYBOARD_Z:
      trackball_stop_transl_z(viewer->_trackball);
      break;
#if DEBUG
    case _KEYBOARD_E:
      trackdisk_stop_rotation(viewer->_trackdisk);
      break;
    case _KEYBOARD_W:
      trackdisk_stop_transl(viewer->_trackdisk);
      break;
    case _KEYBOARD_Q:
      trackdisk_stop_scale(viewer->_trackdisk);
      break;
#endif
    default:
      assert(false);
      break;
    }
    break;
  case _BUTTON_2:
    viewer->_button_mode = _NO_BUTTON;
    break;
  case _BUTTON_3:
    viewer->_button_mode = _NO_BUTTON;
    gtk_menu_popdown(GTK_MENU(viewer->_menu));
    break;
  default:
    assert(false);
    break;
  }
  // Take into account premature key release
  viewer->_keyboard_mode = _NO_KEYBOARD;
  
  gdk_gl_drawable_gl_end(gldrawable);
  gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);
  gdk_window_process_updates(widget->window, FALSE);
  return TRUE;
}

gboolean
Viewer::_motion_notify_event_cb(GtkWidget *widget, GdkEventMotion *event,
                                Viewer *viewer) {
  if (viewer->_button_mode == _BUTTON_1) {
    assert(event->state & GDK_BUTTON1_MASK); // fool-proof
    
    GdkGLContext  *glcontext  = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);
    
    if (!gdk_gl_drawable_gl_begin(gldrawable, glcontext)) {
      return FALSE;
    }
    
    switch (viewer->_keyboard_mode) {
    case _NO_KEYBOARD: {
      gdouble x, y;
      gdouble pressure;
      gdouble xtilt, ytilt;
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_PRESSURE, &pressure);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_XTILT, &xtilt);
      gdk_event_get_axis((GdkEvent *) event, GDK_AXIS_YTILT, &ytilt);
      viewer->_drawing->draw_stroke(
        event->device->source, event->state,
        event->x, widget->allocation.height - 1 - event->y,
        pressure, xtilt, ytilt);
    } break;
    case _KEYBOARD_SPACE:
      // nothing to do
      break;
    case _KEYBOARD_C:
    case _KEYBOARD_X:
    case _KEYBOARD_Z:
      trackball_move(viewer->_trackball, (int) event->x, (int) event->y);
      gl_transf_from_trackball(viewer->_transf_persp, viewer->_trackball);
      viewer->_set_perspective_projection_matrix();
      break;
#if DEBUG
    case _KEYBOARD_E:
    case _KEYBOARD_W:
    case _KEYBOARD_Q:
      trackdisk_move(viewer->_trackdisk, (int) event->x, (int) event->y);
      gl_transf_from_trackdisk(viewer->_transf_ortho, viewer->_trackdisk);
      break;
#endif
    default:
      assert(false);
      break;
    }
    
    gdk_gl_drawable_gl_end(gldrawable);
    gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);
    gdk_window_process_updates(widget->window, FALSE);
    return TRUE;
  } else {
    viewer->_application->toolbox()->sync(event->device->source);
    return FALSE;
  }
}

void
Viewer::_item_factory_edit_cb(Viewer *viewer, guint action,
                              GtkWidget *widget) {
  switch (action) {
  case _EDIT_BLOB:
    viewer->_drawing->draw_height_field();
    break;
  case _EDIT_UNDO:
    viewer->_meshing->unmesh();
    break;
  case _EDIT_AT_DEPTH:
    viewer->_meshing->is_at_depth() = !viewer->_meshing->is_at_depth();
    break;
  default:
    assert(false);
    break;
  }
}

void
Viewer::_item_factory_style_cb(Viewer *viewer,
                               Tetrahedrization_display::Style style,
                               GtkWidget *widget) {
  viewer->_tetrahedrization_display_style = style;
}

void
Viewer::_item_factory_display_cb(Viewer *viewer, guint action,
                                 GtkWidget *widget) {
  if (viewer->_display_mode & action) {
    viewer->_display_mode -= action;
  } else {
    viewer->_display_mode += action;
  }
}

void
Viewer::_item_factory_misc_cb(Viewer *viewer, guint action,
                              GtkWidget *widget) {
  switch (action) {
  case _MISC_REINIT:
    trackball_init_transf(viewer->_trackball);
    gl_transf_from_trackball(viewer->_transf_persp, viewer->_trackball);
    viewer->_set_perspective_projection_matrix();
    break;
  default:
    assert(false);
    break;
  }
}

GLboolean
Viewer::_display_axes_list_cb(void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TRANSFORM_BIT);
  
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  
  glColor4fv(GL_PURE_BLACK);
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(1.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 1.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glVertex3f(0.0f, 0.0f, 1.0f);
  glEnd();
  
  glColor4fv(GL_PURE_YELLOW);
  gdk_gl_draw_cube(TRUE, 0.075);
  
  glMatrixMode(GL_MODELVIEW);
  
  glPushMatrix();
  glTranslatef(1.0f, 0.0f, 0.0f);
  glColor4fv(GL_PURE_RED);
  gdk_gl_draw_cube(TRUE, 0.075);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0f, 1.0f, 0.0f);
  glColor4fv(GL_PURE_GREEN);
  gdk_gl_draw_cube(TRUE, 0.075);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 1.0f);
  glColor4fv(GL_PURE_BLUE);
  gdk_gl_draw_cube(TRUE, 0.075);
  glPopMatrix();
  
  glPopAttrib();
  return GL_TRUE;
}

void
Viewer::_create_window(void) {
  /* glconfig */
  int major = 0, minor = 0;
  gdk_gl_query_version(&major, &minor);
  g_print("OpenGL %d.%d\n", major, minor);
  GdkGLConfig *glconfig = gdk_gl_config_new_by_mode((GdkGLConfigMode)
                                                    (GDK_GL_MODE_RGB     |
                                                     GDK_GL_MODE_ALPHA   |
                                                     GDK_GL_MODE_DEPTH   |
                                                     GDK_GL_MODE_STENCIL |
                                                     GDK_GL_MODE_DOUBLE));
  if (glconfig == NULL) {
    cerr << "Error: no appropriate frame buffer configuration found!" << endl;
    exit(EXIT_FAILURE);
  }
  /*
   * The 8 (eight) bits per RGBA component are useful for 32-bit indices
   * in itembuffers and 8-bit grayscale values (see meshing.cc).
   *
   * The 8 (eight) bits of stencil are necessary for masking:
   * 0x1  is the drawing (and triangulation) mask (see for example
   *      pencil_tool.cc for the former and drawing.cc for the latter)
   * 0x2  is the eraser mask (see eraser_tool.cc and smudge_tool.cc)
   * 0x4  is the triangulation mask (see drawing.cc)
   * 0x8  is the depth mask (see tetrahedrization_display.cc)
   * 0x10 is the frisket mask (see frisket_tool.cc)
   * 0x20 is the burnisher mask (see burnisher_tool.cc)
   * 0x40 is the scraper mask (see scraper_tool.cc)
   * 0x80 is the garbage mask, available for any purpose, such as creating a
   *      mask for computing an euclidean distance transform (see drawing.cc)
   */
  int red_size = 0, green_size = 0, blue_size = 0;
  int alpha_size = 0, stencil_size = 0;
  gdk_gl_config_get_attrib(glconfig, GDK_GL_RED_SIZE, &red_size);
  gdk_gl_config_get_attrib(glconfig, GDK_GL_GREEN_SIZE, &green_size);
  gdk_gl_config_get_attrib(glconfig, GDK_GL_BLUE_SIZE, &blue_size);
  gdk_gl_config_get_attrib(glconfig, GDK_GL_ALPHA_SIZE, &alpha_size);
  gdk_gl_config_get_attrib(glconfig, GDK_GL_STENCIL_SIZE, &stencil_size);
  if (!(red_size == 8 && green_size == 8 && blue_size == 8 &&
        alpha_size == 8 && stencil_size == 8)) {
    cerr << "Error: no appropriate frame buffer configuration found!" << endl;
    exit(EXIT_FAILURE);
  }
  
  /* window */
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(window),
                              _drawing->size()[0], _drawing->size()[1]);
  GdkGeometry hints;
  hints.min_width  = _drawing->size()[0];
  hints.min_height = _drawing->size()[1];
  hints.max_width  = _drawing->size()[0];
  hints.max_height = _drawing->size()[1];
  gtk_window_set_geometry_hints(GTK_WINDOW(window), NULL, &hints,
                                (GdkWindowHints) (GDK_HINT_MIN_SIZE  |
                                                  GDK_HINT_MAX_SIZE));
  gtk_window_set_title(GTK_WINDOW(window),
                       _application->file()->name());
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
#ifndef G_OS_WIN32
  gtk_container_set_resize_mode(GTK_CONTAINER(window), GTK_RESIZE_IMMEDIATE);
#endif
  gtk_container_set_reallocate_redraws(GTK_CONTAINER(window), TRUE);
  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(_delete_event_cb), this);
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(gtk_widget_destroyed), &_window);
  g_signal_connect(G_OBJECT(window), "key_press_event",
                   G_CALLBACK(_key_press_event_cb), this);
  g_signal_connect(G_OBJECT(window), "key_release_event",
                   G_CALLBACK(_key_release_event_cb), this);
  
  /* vbox */
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);
  
  /* drawing_area */
  GtkWidget *drawing_area = gtk_drawing_area_new();
  gboolean success = gtk_widget_set_gl_capability(drawing_area, glconfig,
                                                  NULL, TRUE,
                                                  GDK_GL_RGBA_TYPE);
  assert(success);
  gtk_widget_add_events(drawing_area, GDK_KEY_PRESS_MASK      |
                                      GDK_KEY_RELEASE_MASK    |
                                      GDK_BUTTON_PRESS_MASK   |
                                      GDK_BUTTON_RELEASE_MASK |
                                      GDK_POINTER_MOTION_MASK);
  gtk_widget_set_extension_events(drawing_area, GDK_EXTENSION_EVENTS_CURSOR);
  gtk_box_pack_start(GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
  g_signal_connect_after(G_OBJECT(drawing_area), "realize",
                         G_CALLBACK(_realize_cb), this);
  g_signal_connect(G_OBJECT(drawing_area), "configure_event",
                   G_CALLBACK(_configure_event_cb), this);
  g_signal_connect(G_OBJECT(drawing_area), "expose_event",
                   G_CALLBACK(_expose_event_cb), this);
  g_signal_connect(G_OBJECT(drawing_area), "button_press_event",
                   G_CALLBACK(_button_press_event_cb), this);
  g_signal_connect(G_OBJECT(drawing_area), "button_release_event",
                   G_CALLBACK(_button_release_event_cb), this);
  g_signal_connect(G_OBJECT(drawing_area), "motion_notify_event",
                   G_CALLBACK(_motion_notify_event_cb), this);
  
  gtk_widget_show_all(window);
  
  _set_window(window);
  
  /* extensions check */
  static bool check_gl_extensions = true;
  if (check_gl_extensions) {
    check_gl_extensions = false;
    if (!gdk_gl_query_gl_extension("GL_ARB_imaging")) {
      cerr << "Error: GL_ARB_imaging extension not supported!" << endl;
      exit(EXIT_FAILURE);
    }
    if (!gdk_gl_query_gl_extension("GL_ARB_multitexture")) {
      cerr << "Error: GL_ARB_multitexture extension not supported!" << endl;
      exit(EXIT_FAILURE);
    }
  }
}

void
Viewer::_create_menu(void) {
  void (*e)(void) = (void (*)(void)) _item_factory_edit_cb;
  void (*s)(void) = (void (*)(void)) _item_factory_style_cb;
  void (*d)(void) = (void (*)(void)) _item_factory_display_cb;
  void (*m)(void) = (void (*)(void)) _item_factory_misc_cb;
  
  GtkItemFactoryEntry items[] = {
    {"/Tearoff", NULL, NULL, 0, "<Tearoff>"},
    
    {"/Edit",               NULL, NULL, 0,              "<Branch>"},
    {"/Edit/Blob it!",      NULL, e,    _EDIT_BLOB,     "<Item>"},
    {"/Edit/Undo!",         NULL, e,    _EDIT_UNDO,     "<Item>"},
    {"/Edit/Separator",     NULL, NULL, 0,              "<Separator>"},
    {"/Edit/Draw at depth", NULL, e,    _EDIT_AT_DEPTH, "<CheckItem>"},
    
    {"/Style",            NULL, NULL, 0,                 "<Branch>"},
    {"/Style/Solid",      NULL, s,    _STYLE_SOLID,      "<RadioItem>"},
#if DEBUG
    {"/Style/Facet normals",  NULL, s, _STYLE_FACET_NORMALS,  "/Style/Solid"},
    {"/Style/Vertex normals", NULL, s, _STYLE_VERTEX_NORMALS, "/Style/Solid"},
#else
    {"/Style/Normals",        NULL, s, _STYLE_FACET_NORMALS,  "/Style/Solid"},
#endif
    {"/Style/Wireframe",  NULL, s,    _STYLE_WIREFRAME,  "/Style/Solid"},
    {"/Style/Points",     NULL, s,    _STYLE_POINTS,     "/Style/Solid"},
#if DEBUG
    {"/Style/NPR Gooch",  NULL, s,    _STYLE_NPR_GOOCH,  "/Style/Solid"},
    {"/Style/NPR Raskar", NULL, s,    _STYLE_NPR_RASKAR, "/Style/Solid"},
#else
    {"/Style/NPR",        NULL, s,    _STYLE_NPR_GOOCH,  "/Style/Solid"},
#endif
    
    {"/Display",                   NULL, NULL, 0,             "<LastBranch>"},
    {"/Display/Frames per second", NULL, d,    _DISPLAY_FPS,  "<CheckItem>"},
    {"/Display/Axes",              NULL, d,    _DISPLAY_AXES, "<CheckItem>"},
    {"/Display/Bounding box",      NULL, d,    _DISPLAY_BBOX, "<CheckItem>"},
#if DEBUG
    {"/Display/Triangulation", NULL, d, _DISPLAY_TRIANGULATION, "<CheckItem>"},
#endif
    {"/Display/Separator",         NULL, NULL, 0,             "<Separator>"},
    {"/Display/Reinit trackball",  NULL, m,    _MISC_REINIT,  "<Item>"},
  };
  
  int nitems = sizeof(items) / sizeof(GtkItemFactoryEntry);
  
  GtkItemFactory *item_factory = gtk_item_factory_new(GTK_TYPE_MENU,
                                                      "<ViewerMain>",
                                                      NULL);
  gtk_item_factory_create_items(item_factory, nitems, items, this);
  GtkWidget *menu = gtk_item_factory_get_widget(item_factory, "<ViewerMain>");
  _set_menu(menu);
}

void
Viewer::_set_window(GtkWidget *window) {
  if (GTK_IS_WIDGET(_window)) {
    gtk_widget_destroy(_window);
  }
  _window = window;
  assert(_window != NULL);
}

void
Viewer::_set_menu(GtkWidget *menu) {
  if (GTK_IS_WIDGET(_menu)) {
    gtk_widget_destroy(_menu);
  }
  _menu = menu;
  assert(_menu != NULL);
}

void
Viewer::_set_perspective_projection_matrix(void) {
  // get clipping distance
  CGAL::Bbox_3 bbox = _application->tetrahedrization()->bbox();
  bbox = bbox + _axes_bbox;
  GLvecd bbox_min, bbox_max;
  gl_vecd_set(bbox_min, bbox.xmin(), bbox.ymin(), bbox.zmin(), 1.0);
  gl_vecd_set(bbox_max, bbox.xmax(), bbox.ymax(), bbox.zmax(), 1.0);
  gl_transf_get_clip_dist(_transf_persp,
                          bbox_min, bbox_max, &_z_near, &_z_far);
  
  // set projection matrix
  glPushAttrib(GL_TRANSFORM_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  gluPerspective(_fovy,
                 (GLdouble) _transf_persp->viewport[2] /
                 (GLdouble) _transf_persp->viewport[3],
                 _z_near, _z_far);
  gl_transf_set_proj_mat(_transf_persp);
  glPopMatrix();
  glPopAttrib();
}

void
Viewer::_measure_fps(void) {
  const gdouble FPS_MEASUREMENT_PERIOD = 5.0;
  static int frames = 0;
  
  frames++;
  if (_display_mode & _DISPLAY_FPS) {
    gdouble seconds = g_timer_elapsed(_timer, NULL);
    if (seconds >= FPS_MEASUREMENT_PERIOD) {
      gdouble fps = frames/seconds;
      g_print("%i frames in %6.3f seconds = %6.3f fps\n",
              frames, seconds, fps);
      g_timer_reset(_timer);
      frames = 0;
    }
  }
}
