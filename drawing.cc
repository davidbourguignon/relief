#include <gtk/gtk.h>
#include <cassert>
#include <fstream>

#include "application.hh"
#include "toolbox.hh"
#include "triangulation.hh"
#include "triangulation_display.hh"
#include "reconstruct_curve.hh"
#include "drawing.hh"

using namespace std;

static const Vec2i DEFAULT_SIZE = {256, 256};
static const Vec2i SIZE_RANGE = {128, 1024};
static const int ERASER_THRESHOLD = 16;
static const GLfloat ALPHA_SCALE = 0.5f;

Drawing::Drawing(Application *application)
  : _application(application),
    _triangulation_proxy(NULL),
    _triangulation_display_ptr(NULL),
    _tool(NULL),
    _is_drawing_stroke(false),
    _is_marking_stroke(false),
    _has_just_been_cleared(true),
    _has_changed(false),
    _are_textures_and_lists_set(false),
    _marking_paths(),
    _back_first_type(_COLOR_TABLE_THRESHOLD),
    _back_second_type(_COLOR_TABLE_ABSOLUTE),
    _distance_type(_COLOR_TABLE_SPHERE_MAP),
    _euclidean_distance_max(0.0) {
  vec2i_eq(_size, DEFAULT_SIZE);
  gl_vecf_eq(_background_color, GL_VECF_NULL);
  gl_veci_eq(_viewport, GL_VECI_NULL);
  gl_veci_eq(_drawport, GL_VECI_NULL);
  gl_veci_eq(_drawbox, GL_VECI_NULL);
  
  _front_texture = gl_texture_new();
  _back_first_texture = gl_texture_new();
  _back_second_texture = gl_texture_new();
  _overlay_texture = gl_texture_new();
  _distance_texture = gl_texture_new();
  
  _first_pass_list = gl_list_new();
  _second_pass_list = gl_list_new();
  _back_first_color_table_list = gl_list_new();
  _back_second_color_table_list = gl_list_new();
  _distance_color_table_list = gl_list_new();
  _convolution_list = gl_list_new();
  _overlay_list = gl_list_new();
  
  _colorbuf = gl_framebuf_new();
  _back_colorbuf = gl_framebuf_new();
  _stencilbuf = gl_framebuf_new();
  _overlay_image = gl_framebuf_new();
  _itembuf = gl_itembuf_new(GL_ITEMBUF_1D);
  
  gl_framebuf_set_format(_colorbuf, GL_RED);
  gl_framebuf_set_format(_back_colorbuf, GL_RGBA);
  gl_framebuf_set_format(_stencilbuf, GL_STENCIL_INDEX);
  gl_framebuf_sread(_overlay_image, "rgb/overlay.bw", GL_FILE_SGI);
  gl_framebuf_set_format(_overlay_image, GL_ALPHA);
}

Drawing::~Drawing(void) {
  _application->remove(_triangulation_proxy);
  if (_triangulation_display_ptr != NULL) {
    delete _triangulation_display_ptr;
  }
  
  gl_texture_delete(_front_texture);
  gl_texture_delete(_back_first_texture);
  gl_texture_delete(_back_second_texture);
  gl_texture_delete(_overlay_texture);
  gl_texture_delete(_distance_texture);
  
  gl_list_delete(_first_pass_list);
  gl_list_delete(_second_pass_list);
  gl_list_delete(_back_first_color_table_list);
  gl_list_delete(_back_second_color_table_list);
  gl_list_delete(_distance_color_table_list);
  gl_list_delete(_convolution_list);
  gl_list_delete(_overlay_list);
  
  gl_framebuf_delete(_colorbuf);
  gl_framebuf_delete(_back_colorbuf);
  gl_framebuf_delete(_stencilbuf);
  gl_framebuf_delete(_overlay_image);
  gl_itembuf_delete(_itembuf);
}

const_Vec2i_t
Drawing::size(void) const {
  return _size;
}

const_GLvecf_t
Drawing::background_color(void) const {
  return _background_color;
}

const_GLveci_t
Drawing::drawbox(void) const {
  return _drawbox;
}

bool
Drawing::has_changed(void) const {
  return _has_changed;
}

double
Drawing::height_field_max(void) {
  double max = _euclidean_distance_max;
  _euclidean_distance_max = 0.0;
  return max;
}

bool
Drawing::init(void) {
  GtkWidget *dialog = gtk_dialog_new_with_buttons("New drawing",
                                                  NULL,
                                                  GTK_DIALOG_MODAL,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_OK,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_CANCEL,
                                                  NULL);
  
  GtkWidget *frame = gtk_frame_new("Size (in pixels)");
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), frame, TRUE, TRUE, 5);
  
  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
  gtk_container_add(GTK_CONTAINER(frame), hbox);
  
  GtkWidget *hbox1 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), hbox1, TRUE, TRUE, 5);
  
  GtkWidget *label1 = gtk_label_new("Width: ");
  gtk_misc_set_alignment(GTK_MISC(label1), 0.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox1), label1, FALSE, FALSE, 0);
  
  GtkAdjustment *adj1 = (GtkAdjustment *) gtk_adjustment_new(DEFAULT_SIZE[0],
                                                             SIZE_RANGE[0],
                                                             SIZE_RANGE[1],
                                                             1, 10, 0);
  
  GtkWidget *spinner1 = gtk_spin_button_new(adj1, 1.0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinner1), TRUE);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner1), FALSE);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spinner1),
                                    GTK_UPDATE_IF_VALID);
  gtk_box_pack_start(GTK_BOX(hbox1), spinner1, FALSE, FALSE, 0);
  
  GtkWidget *hbox2 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), hbox2, TRUE, TRUE, 5);
  
  GtkWidget *label2 = gtk_label_new("Height: ");
  gtk_misc_set_alignment(GTK_MISC(label2), 0.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox2), label2, FALSE, FALSE, 0);
  
  GtkAdjustment *adj2 = (GtkAdjustment *) gtk_adjustment_new(DEFAULT_SIZE[1],
                                                             SIZE_RANGE[0],
                                                             SIZE_RANGE[1],
                                                             1, 10, 0);
  
  GtkWidget *spinner2 = gtk_spin_button_new(adj2, 1.0, 0);
  gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spinner2), TRUE);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner2), FALSE);
  gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(spinner2),
                                    GTK_UPDATE_IF_VALID);
  gtk_box_pack_start(GTK_BOX(hbox2), spinner2, FALSE, FALSE, 0);
  
  gtk_widget_show_all(dialog);
  gint response = gtk_dialog_run(GTK_DIALOG(dialog));
  bool result = false;
  switch (response) {
  case GTK_RESPONSE_OK: {
    _triangulation_proxy = _application->triangulation();
    result = (_triangulation_proxy != NULL);
    int width
      = scali_nearest_power_of_two(gtk_spin_button_get_value_as_int(
                                     GTK_SPIN_BUTTON(spinner1)));
    int height
      = scali_nearest_power_of_two(gtk_spin_button_get_value_as_int(
                                     GTK_SPIN_BUTTON(spinner2)));
    assert(width >= 0 && height >= 0);
    vec2i_set(_size, width, height);
    gl_vecf_eq(_background_color, _application->toolbox()->background_color());
  } break;
  case GTK_RESPONSE_CANCEL:
  case GTK_RESPONSE_DELETE_EVENT:
    result = false;
    break;
  default:
    assert(false);
    break;
  }
  gtk_widget_destroy(dialog);
  return result;
}

bool
Drawing::read(ifstream& fin, File::Type file_type) {
  if (!init()) {
    return false;
  } else {
    switch (file_type) {
    case File::RLF:
      fin >> *_triangulation_proxy;
      break;
    case File::OFF:
      // nothing to do
      break;
    default:
      assert(false);
      break;
    }
    if (fin.fail()) {
      fin.clear();
      return false;
    } else {
      vector<Vertex_handle> triangulation_vertices;
      triangulation_vertices.reserve(
        _triangulation_proxy->number_of_vertices());
      for (Finite_vertices_iterator vi
             = _triangulation_proxy->finite_vertices_begin();
           vi != _triangulation_proxy->finite_vertices_end(); vi++) {
        triangulation_vertices.push_back(vi);
      }
      Reconstruct_curve reconstruct_curve(*_triangulation_proxy);
      reconstruct_curve(triangulation_vertices);
      return true;
    }
  }
}

bool
Drawing::write(ofstream& fout, File::Type file_type) const {
  switch (file_type) {
  case File::RLF:
    fout << *_triangulation_proxy;
    break;
  case File::OFF:
    // nothing to do
    break;
  default:
    assert(false);
    break;
  }
  if (fout.fail()) {
    fout.clear();
    return false;
  } else {
    return true;
  }
}

void
Drawing::start_drawing_stroke(GdkInputSource source, guint state,
                              gdouble x, gdouble y, gdouble pressure,
                              gdouble xtilt, gdouble ytilt) {
  _is_drawing_stroke = true;
  if (state & GDK_SHIFT_MASK) { // hole mark mode if shift pressed
    _is_marking_stroke = true;
  }
  switch (source) {
  case GDK_SOURCE_MOUSE:
    _tool = _application->toolbox()->core_pointer_tool();
    break;
  case GDK_SOURCE_PEN:
    _tool = _application->toolbox()->stylus_pen_tool();
    break;
  case GDK_SOURCE_ERASER:
    _tool = _application->toolbox()->stylus_eraser_tool();
    break;
  case GDK_SOURCE_CURSOR:
    cerr << "Error: Cursor is an unknown input device source!\n" << endl;
    break;
  default:
    assert(false);
    break;
  }
  _tool->start_recording_path(source, x, y, pressure, xtilt, ytilt);
  _tool->start_drawing_pixels(source, x, y, pressure, xtilt, ytilt);
  gl_veci_eq(_drawport, _tool->drawn_pixels());
}

void
Drawing::stop_drawing_stroke(GdkInputSource source, guint state,
                             gdouble x, gdouble y, gdouble pressure,
                             gdouble xtilt, gdouble ytilt) {
  _is_drawing_stroke = true;
  _tool->stop_recording_path();
  _tool->stop_drawing_pixels();
  gl_veci_eq(_drawport, _tool->drawn_pixels());
  
  if (_is_marking_stroke) {
    _is_marking_stroke = false;
    // some tools, such as the eraser, do not record path
    if (!_tool->recorded_path().empty()) {
      _marking_paths.push_back(_tool->recorded_path());
    }
  } else if (!_tool->recorded_path().empty()) {
#if DEBUG
    vector<Vertex_handle> inserted_vertices;
#endif
    Vertex_handle vh;
    bool is_first = true;
    for (vector<Tool::Point>::const_iterator pi
           = _tool->recorded_path().begin();
         pi != _tool->recorded_path().end(); pi++) {
      if (is_first) {
        is_first = false;
        vh = _triangulation_proxy->insert_first(*pi);
      } else {
        vh = _triangulation_proxy->insert(*pi);
      }
#if DEBUG
      inserted_vertices.push_back(vh);
#endif
    }
    assert(_triangulation_proxy->is_valid());
#if DEBUG
    if (_triangulation_proxy->dimension() == 2) {
      assert(!inserted_vertices.empty());
      for (All_faces_iterator fi = _triangulation_proxy->all_faces_begin();
           fi != _triangulation_proxy->all_faces_end(); fi++) {
        fi->reset();
      }
      vector<Vertex_handle> triangulation_vertices;
      triangulation_vertices.reserve(
        _triangulation_proxy->number_of_vertices());
      for (Finite_vertices_iterator vi
             = _triangulation_proxy->finite_vertices_begin();
           vi != _triangulation_proxy->finite_vertices_end(); vi++) {
        triangulation_vertices.push_back(vi);
      }
      Reconstruct_curve reconstruct_curve(*_triangulation_proxy);
      reconstruct_curve(triangulation_vertices);//inserted_vertices);
      //TODO: use partial update of the curve edges using inserted vertices
    } else if (_triangulation_proxy->number_of_vertices() != 0) {
      cerr << "Warning: triangulation convex hull is not two-dimensional!"
           << endl;
    }
#endif
  }
}

void
Drawing::draw_stroke(GdkInputSource source, guint state,
                     gdouble x, gdouble y, gdouble pressure,
                     gdouble xtilt, gdouble ytilt) {
  _is_drawing_stroke = true;
  _tool->record_path(source, x, y, pressure, xtilt, ytilt);
  _tool->draw_pixels(source, x, y, pressure, xtilt, ytilt);
  gl_veci_eq(_drawport, _tool->drawn_pixels());
}

void
Drawing::draw_height_field(void) {
  if (_triangulation_proxy->number_of_vertices() == 0) {
    return;
  } else if (_triangulation_proxy->number_of_inside_faces() == 0) {
    cerr << "Warning: unable to reconstruct drawn shape!" << endl;
    cerr << "Hint: close strokes tighter!" << endl;
    return;
  }
  
  // remove erased vertices and gather marked faces
  gl_framebuf_set_port(_back_colorbuf, _viewport);
  gl_framebuf_read(_back_colorbuf, GL_BACK);
  _remove_erased_triangulation_vertices();
  _reconstruct_curve();
  gl_framebuf_draw(_back_colorbuf, GL_BACK);
  _gather_marked_triangulation_faces();
  
  // set port and tex coords
  _is_drawing_stroke = true;
  gl_veci_set(_drawport,
              _drawbox[0],
              _drawbox[1],
              _drawbox[2] - _drawbox[0],
              _drawbox[3] - _drawbox[1]);
  
  GLvecf drawtex;
  gl_vecf_set(drawtex,
              (GLfloat) _drawbox[0] / (GLfloat) _viewport[2],
              (GLfloat) _drawbox[1] / (GLfloat) _viewport[3],
              (GLfloat) _drawbox[2] / (GLfloat) _viewport[2],
              (GLfloat) _drawbox[3] / (GLfloat) _viewport[3]);
  
  // render triangulation mask
  gl_framebuf_set_port(_stencilbuf, _drawport);
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glStencilMask(0x80);
  glStencilFunc(GL_ALWAYS, 0x80, 0x80);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glEnable(GL_STENCIL_TEST);
  display_triangulation(Triangulation_display::THIN_MASK, false);
  _is_drawing_stroke = true; // HACK: because the display reset this flag
  
  glPopAttrib();
  
  // convert stencil buffer to monochrome image
  gl_framebuf_read(_stencilbuf, GL_BACK);
  GLubyte *stencilbuf_pixels = (GLubyte *) _stencilbuf->pixels;
  int size = _stencilbuf->width * _stencilbuf->height;
  for (int i = 0; i < size; i++) {
    stencilbuf_pixels[i]
      = (stencilbuf_pixels[i] & 0x80) ? GL_UBYTE_MAX : GL_UBYTE_MIN;
  }
  
  // compute euclidean distance transform
  gl_framebuf_edt(_stencilbuf, &_euclidean_distance_max);
#if DEBUG
  gl_framebuf_swrite(_stencilbuf, "debug_000.bw", GL_FILE_SGI, GL_FALSE);
#endif
  
  // render the height field
  glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT |
               GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glActiveTexture(GL_TEXTURE0);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gl_texture_bind(_distance_texture);
  glEnable(GL_TEXTURE_2D);
  /*
   * Distance field to height field remapping. See the following reference.
   *
   * Byong Mok Oh, Max Chen, Julie Dorsey and Fredo Durand, Image-based
   * modeling and photo editing, Proceedings of ACM SIGGRAPH 2001.
   */
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  gl_list_call(_distance_color_table_list);
  gl_list_call(_convolution_list);
  glTexSubImage2D(GL_TEXTURE_2D, 0,
                  _drawport[0], _drawport[1], _drawport[2], _drawport[3],
                  GL_ALPHA, _stencilbuf->type, _stencilbuf->pixels);
  glPopAttrib();
  glPopClientAttrib();
  
#if DEBUG
  GLframebuf *distance_buf = gl_framebuf_new();
  gl_framebuf_tread(distance_buf, _distance_texture, 0);
  gl_framebuf_swrite(distance_buf, "debug_001.bw", GL_FILE_SGI, GL_FALSE);
  gl_framebuf_delete(distance_buf);
#endif
  
  glColor4fv(_application->toolbox()->foreground_color());
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilFunc(GL_EQUAL, 0x80, 0x80);
  glAlphaFunc(GL_NOTEQUAL, 0);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_ALPHA_TEST);
  
  glBegin(GL_QUADS);
  glTexCoord2f(drawtex[0], drawtex[1]);
  glVertex2i(_drawbox[0], _drawbox[1]);
  glTexCoord2f(drawtex[2], drawtex[1]);
  glVertex2i(_drawbox[2], _drawbox[1]);
  glTexCoord2f(drawtex[2], drawtex[3]);
  glVertex2i(_drawbox[2], _drawbox[3]);
  glTexCoord2f(drawtex[0], drawtex[3]);
  glVertex2i(_drawbox[0], _drawbox[3]);
  glEnd();
  
  glPopAttrib();
  
  draw(); // HACK: because the popup menu needs a non drawing stroke redisplay
}

void
Drawing::start_drawing(const GLveci viewport) {
  gl_veci_eq(_viewport, viewport);
  
  if (!_are_textures_and_lists_set) {
    _are_textures_and_lists_set = true;
    gl_list_set(_back_first_color_table_list,
                _display_color_table_list_cb, (void *) &_back_first_type,
                GL_FALSE);
    gl_list_set(_back_second_color_table_list,
                _display_color_table_list_cb, (void *) &_back_second_type,
                GL_FALSE);
    gl_list_set(_distance_color_table_list,
                _display_color_table_list_cb, (void *) &_distance_type,
                GL_FALSE);
    gl_list_set(_convolution_list,
                _display_convolution_list_cb, NULL, GL_FALSE);
    gl_texture_set(_overlay_texture, GL_TEXTURE_2D,
                   _setup_overlay_texture_cb, (void *) _overlay_image);
    gl_list_set(_overlay_list,
                _display_overlay_list_cb, (void *) this, GL_FALSE);
  }
  
  glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT);
  
  glClearColor(_background_color[0],
               _background_color[1],
               _background_color[2],
               _background_color[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glReadBuffer(GL_FRONT);
  gl_texture_set(_front_texture, GL_TEXTURE_2D,
                 _setup_non_overlay_texture_cb, (void *) this);
  
  glReadBuffer(GL_BACK);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  gl_list_call(_back_first_color_table_list);
  gl_texture_set(_back_first_texture, GL_TEXTURE_2D,
                 _setup_non_overlay_texture_cb, (void *) this);
  gl_list_call(_back_second_color_table_list);
  gl_texture_set(_back_second_texture, GL_TEXTURE_2D,
                 _setup_non_overlay_texture_cb, (void *) this);
  glPopAttrib();
  gl_texture_set(_distance_texture, GL_TEXTURE_2D,
                 _setup_distance_texture_cb, (void *) this);
  
  glPopAttrib();
  
  gl_list_set(_first_pass_list,
              _display_first_pass_list_cb, (void *) this, GL_FALSE);
  gl_list_set(_second_pass_list,
              _display_second_pass_list_cb, (void *) this, GL_FALSE);
  
  // reset
  _triangulation_proxy->clear();
  _remove(_triangulation_display_ptr);
  _has_just_been_cleared = true;
  _marking_paths.clear();
}

void
Drawing::stop_drawing(void) {
  if (_triangulation_proxy->dimension() == 2) {
    _remove_erased_triangulation_vertices();
    _reconstruct_curve();
    _gather_marked_triangulation_faces();
    
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilMask(0x5); /* 0x1 + 0x4 */
    glStencilFunc(GL_ALWAYS, 0x5, 0x5);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glEnable(GL_STENCIL_TEST);
    display_triangulation(Triangulation_display::THICK_MASK, false);
    
    glPopAttrib();
  } else if (_triangulation_proxy->number_of_vertices() != 0) {
#if DEBUG
    cerr << "Warning: triangulation convex hull is not two-dimensional!"
         << endl;
#endif
  }
  
#if DEBUG
  GLframebuf *back_first_buf = gl_framebuf_new();
  gl_framebuf_tread(back_first_buf, _back_first_texture, 0);
  gl_framebuf_swrite(back_first_buf, "debug_02.rgb", GL_FILE_SGI, GL_FALSE);
  gl_framebuf_delete(back_first_buf);
  
  GLframebuf *back_second_buf = gl_framebuf_new();
  gl_framebuf_tread(back_second_buf, _back_second_texture, 0);
  gl_framebuf_swrite(back_second_buf, "debug_03.rgb", GL_FILE_SGI, GL_FALSE);
  gl_framebuf_delete(back_second_buf);
  
  GLframebuf *stencilbuf = gl_framebuf_new();
  gl_framebuf_set_format(stencilbuf, GL_STENCIL_INDEX);
  gl_framebuf_set_port(stencilbuf, _viewport);
  gl_framebuf_read(stencilbuf, GL_BACK);
  gl_framebuf_swrite(stencilbuf, "debug_04.bw", GL_FILE_SGI, GL_FALSE);
  gl_framebuf_delete(stencilbuf);
#endif
}

void
Drawing::draw(void) {
  GLveci quadi;
  GLvecf quadf;
  
  if (_is_drawing_stroke) {
    GLveci quadi_lim;
    gl_veci_set(quadi_lim,
                _viewport[0],
                _viewport[1],
                _viewport[0] + _viewport[2],
                _viewport[1] + _viewport[3]);
    
    gl_veci_set(quadi,
                _drawport[0],
                _drawport[1],
                _drawport[0] + _drawport[2],
                _drawport[1] + _drawport[3]);
    quadi[0] = scali_clamp(quadi[0], quadi_lim[0], quadi_lim[2]);
    quadi[1] = scali_clamp(quadi[1], quadi_lim[1], quadi_lim[3]);
    quadi[2] = scali_clamp(quadi[2], quadi_lim[0], quadi_lim[2]);
    quadi[3] = scali_clamp(quadi[3], quadi_lim[1], quadi_lim[3]);
    
    gl_vecf_set(quadf,
                (GLfloat) quadi[0] / (GLfloat) (quadi_lim[2] - quadi_lim[0]),
                (GLfloat) quadi[1] / (GLfloat) (quadi_lim[3] - quadi_lim[1]),
                (GLfloat) quadi[2] / (GLfloat) (quadi_lim[2] - quadi_lim[0]),
                (GLfloat) quadi[3] / (GLfloat) (quadi_lim[3] - quadi_lim[1]));
    
    if (_has_just_been_cleared) {
      _has_just_been_cleared = false;
      gl_veci_eq(_drawbox, quadi);
    } else {
      _drawbox[0] = scali_min(_drawbox[0], quadi[0]);
      _drawbox[1] = scali_min(_drawbox[1], quadi[1]);
      _drawbox[2] = scali_max(_drawbox[2], quadi[2]);
      _drawbox[3] = scali_max(_drawbox[3], quadi[3]);
    }
  } else {
    gl_veci_set(quadi,
                _viewport[0],
                _viewport[1],
                _viewport[0] + _viewport[2],
                _viewport[1] + _viewport[3]);
    gl_vecf_set(quadf, 0.0f, 0.0f, 1.0f, 1.0f);
  }
  
  /* draw non-overlay */
  glPushAttrib(GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);
  
  gl_list_call(_first_pass_list);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  gl_list_call(_back_second_color_table_list);
  _draw_quad(quadi, quadf);
  glPopAttrib();
  
  gl_list_call(_second_pass_list);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  gl_list_call(_back_first_color_table_list);
  _draw_quad(quadi, quadf);
  glPopAttrib();
  
  glPopAttrib();
  
  gl_vecf_set(quadf,
              (GLfloat) quadi[0] / (GLfloat) _overlay_texture->width,
              (GLfloat) quadi[1] / (GLfloat) _overlay_texture->height,
              (GLfloat) quadi[2] / (GLfloat) _overlay_texture->width,
              (GLfloat) quadi[3] / (GLfloat) _overlay_texture->height);
  
  /* draw overlay */
  glPushAttrib(GL_CURRENT_BIT | GL_TEXTURE_BIT |
               GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  gl_list_call(_overlay_list);
  glBegin(GL_QUADS);
  glTexCoord2f(quadf[0], quadf[1]);
  glVertex2i(quadi[0], quadi[1]);
  glTexCoord2f(quadf[2], quadf[1]);
  glVertex2i(quadi[2], quadi[1]);
  glTexCoord2f(quadf[2], quadf[3]);
  glVertex2i(quadi[2], quadi[3]);
  glTexCoord2f(quadf[0], quadf[3]);
  glVertex2i(quadi[0], quadi[3]);
  glEnd();
  glFinish();
  
  glPopAttrib();
  
  if (_is_drawing_stroke) {
    _is_drawing_stroke = false;
  }
}

void
Drawing::display_triangulation(Triangulation_display::Style style,
                               bool display_bbox, bool display_marks) {
  _is_drawing_stroke = false;
  if (display_marks) {
    _reconstruct_curve();
    _gather_marked_triangulation_faces();
  }
  _triangulation_display()->push_style(style);
  _triangulation_display()->display();
  _triangulation_display()->pop_style();
  if (display_bbox) {
    _triangulation_display()->display_bbox();
  }
}

GLboolean
Drawing::_setup_non_overlay_texture_cb(void *data, GLboolean test_proxy) {
  Drawing *drawing = (Drawing *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  assert(drawing->_viewport[2] % 2 == 0 && drawing->_viewport[3] % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_RGB,
                 drawing->_viewport[2], drawing->_viewport[3], 0,
                 GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     drawing->_viewport[0], drawing->_viewport[1],
                     drawing->_viewport[2], drawing->_viewport[3], 0);
    return GL_TRUE;
  }
}

GLboolean
Drawing::_setup_overlay_texture_cb(void *data, GLboolean test_proxy) {
  GLframebuf *image = (GLframebuf *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(image->width % 2 == 0 && image->height % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_ALPHA,
                 image->width, image->height, 0,
                 image->format, image->type, image->pixels);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                 image->width, image->height, 0,
                 image->format, image->type, image->pixels);
    return GL_TRUE;
  }
}

GLboolean
Drawing::_setup_distance_texture_cb(void *data, GLboolean test_proxy) {
  Drawing *drawing = (Drawing *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(drawing->_viewport[2] % 2 == 0 && drawing->_viewport[3] % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_ALPHA,
                 drawing->_viewport[2], drawing->_viewport[3], 0,
                 GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA,
                     drawing->_viewport[0], drawing->_viewport[1],
                     drawing->_viewport[2], drawing->_viewport[3], 0);
    return GL_TRUE;
  }
}

GLboolean
Drawing::_display_first_pass_list_cb(void *data, GLboolean test_proxy) {
  Drawing *drawing = (Drawing *) data;
  
  if (test_proxy) return GL_TRUE;
  glDrawBuffer(GL_FRONT);
  glActiveTexture(GL_TEXTURE0);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gl_texture_bind(drawing->_front_texture);
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE1);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
  gl_texture_bind(drawing->_back_second_texture);
  glEnable(GL_TEXTURE_2D);
  return GL_TRUE;
}

GLboolean
Drawing::_display_second_pass_list_cb(void *data, GLboolean test_proxy) {
  Drawing *drawing = (Drawing *) data;
  
  if (test_proxy) return GL_TRUE;
  glActiveTexture(GL_TEXTURE1);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glActiveTexture(GL_TEXTURE0);
  gl_texture_release(drawing->_front_texture);
  gl_texture_bind(drawing->_back_first_texture);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);
  return GL_TRUE;
}

GLboolean
Drawing::_display_color_table_list_cb(void *data, GLboolean test_proxy) {
  _ColorTableType type = *((_ColorTableType *) data);
  
  GLubyte color_table_RGB[256][3];
  GLubyte color_table_A[256];
  //TODO: should be L instead of RGB...
  switch (type) {
  case _COLOR_TABLE_THRESHOLD:
    for (int i = 0; i < 256; i++) {
      GLubyte c = (GLubyte) (i > 127 ? 255 : 0);
      for (int j = 0; j < 3; j++) {
        color_table_RGB[i][j] = c;
      }
    }
    break;
  case _COLOR_TABLE_ABSOLUTE:
    for (int i = 0; i < 256; i++) {
      GLubyte c = (GLubyte) abs(2*i - 255);
      for (int j = 0; j < 3; j++) {
        color_table_RGB[i][j] = c;
      }
    }
    break;
  case _COLOR_TABLE_SPHERE_MAP:
    for (int i = 0; i < 256; i++) {
      double distance = (1.0 - (double) i / 255.0);
      double height = sqrt(1.0 - distance*distance);
      GLubyte c = (GLubyte) (height * 255.0);
      color_table_A[i] = c;
    }
    break;
  default:
    assert(false);
    break;
  }
  if (test_proxy) {
    GLint proxy_table_width = 0;
    switch (type) {
    case _COLOR_TABLE_THRESHOLD:
    case _COLOR_TABLE_ABSOLUTE:
      glColorTable(GL_PROXY_COLOR_TABLE, GL_RGB, 256, GL_RGB,
                   GL_UNSIGNED_BYTE, color_table_RGB);
      break;
    case _COLOR_TABLE_SPHERE_MAP:
      glColorTable(GL_PROXY_COLOR_TABLE, GL_ALPHA, 256, GL_ALPHA,
                   GL_UNSIGNED_BYTE, color_table_A);
      break;
    default:
      assert(false);
      break;
    }
    glGetColorTableParameteriv(GL_PROXY_COLOR_TABLE, GL_COLOR_TABLE_WIDTH,
                               &proxy_table_width);
    if (proxy_table_width == 0) {
      cerr << "Error: unsupported color table!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    switch (type) {
    case _COLOR_TABLE_THRESHOLD:
    case _COLOR_TABLE_ABSOLUTE:
      glColorTable(GL_COLOR_TABLE, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE,
                   color_table_RGB);
      break;
    case _COLOR_TABLE_SPHERE_MAP:
      glColorTable(GL_COLOR_TABLE, GL_ALPHA, 256, GL_ALPHA, GL_UNSIGNED_BYTE,
                   color_table_A);
      break;
    default:
      assert(false);
      break;
    }
    glEnable(GL_COLOR_TABLE);
    return GL_TRUE;
  }
}

GLboolean
Drawing::_display_convolution_list_cb(void *data, GLboolean test_proxy) {
  GLfloat gaussian_kernel[7]
    = {0.006f, 0.061f, 0.242f, 0.383f, 0.242f, 0.061f, 0.006f};
  // discrete approximation to Gaussian function with sigma equal to 1.0
  
  if (test_proxy) return GL_TRUE;
  glConvolutionParameteri(GL_SEPARABLE_2D, GL_CONVOLUTION_BORDER_MODE,
                          GL_REPLICATE_BORDER_HP);
  glSeparableFilter2D(GL_SEPARABLE_2D, GL_ALPHA, 7, 7,
                      GL_ALPHA, GL_FLOAT,
                      gaussian_kernel, gaussian_kernel);
  glEnable(GL_SEPARABLE_2D);
  return GL_TRUE;
}

GLboolean
Drawing::_display_overlay_list_cb(void *data, GLboolean test_proxy) {
  Drawing *drawing = (Drawing *) data;
  
  if (test_proxy) return GL_TRUE;
  glColor4f(GL_PURE_RED[0],
            GL_PURE_RED[1],
            GL_PURE_RED[2],
            GL_PURE_RED[3] * ALPHA_SCALE);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gl_texture_bind(drawing->_overlay_texture);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
  glDrawBuffer(GL_FRONT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glStencilFunc(GL_EQUAL, 0x10, 0x10);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glEnable(GL_STENCIL_TEST);
  return GL_TRUE;
}

Triangulation_display *
Drawing::_triangulation_display(void) {
  if (_triangulation_display_ptr == NULL) {
    _triangulation_display_ptr
      = new Triangulation_display(*(_application->triangulation()));
    assert(_triangulation_display_ptr != NULL);
  }
  return _triangulation_display_ptr;
}

void
Drawing::_remove(Triangulation_display *triangulation_display) {
  if (triangulation_display == _triangulation_display_ptr) {
    if (_triangulation_display_ptr != NULL) {
      delete _triangulation_display_ptr;
      _triangulation_display_ptr = NULL;
    }
  }
}

void
Drawing::_draw_quad(const GLveci quadi, const GLvecf quadf) const {
  if (_is_drawing_stroke) {
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0,
                        quadi[0], quadi[1], quadi[0], quadi[1],
                        quadi[2] - quadi[0], quadi[3] - quadi[1]);
  }
  
  glBegin(GL_QUADS);
  glMultiTexCoord2f(GL_TEXTURE0, quadf[0], quadf[1]);
  glMultiTexCoord2f(GL_TEXTURE1, quadf[0], quadf[1]);
  glVertex2i(quadi[0], quadi[1]);
  glMultiTexCoord2f(GL_TEXTURE0, quadf[2], quadf[1]);
  glMultiTexCoord2f(GL_TEXTURE1, quadf[2], quadf[1]);
  glVertex2i(quadi[2], quadi[1]);
  glMultiTexCoord2f(GL_TEXTURE0, quadf[2], quadf[3]);
  glMultiTexCoord2f(GL_TEXTURE1, quadf[2], quadf[3]);
  glVertex2i(quadi[2], quadi[3]);
  glMultiTexCoord2f(GL_TEXTURE0, quadf[0], quadf[3]);
  glMultiTexCoord2f(GL_TEXTURE1, quadf[0], quadf[3]);
  glVertex2i(quadi[0], quadi[3]);
  glEnd();
  glFinish();
}

void
Drawing::_remove_erased_triangulation_vertices(void) {
  gl_veci_set(_drawport,
              _drawbox[0],
              _drawbox[1],
              _drawbox[2] - _drawbox[0],
              _drawbox[3] - _drawbox[1]);
  
  /* get color and item buffers */
  gl_framebuf_set_port(_colorbuf, _drawport);
  gl_framebuf_read(_colorbuf, GL_BACK);
#if DEBUG
  gl_framebuf_swrite(_colorbuf, "debug_00.bw", GL_FILE_SGI, GL_FALSE);
#endif
  
  gl_itembuf_set_port(_itembuf, _drawport);
  unsigned int npoint_items = _triangulation_proxy->number_of_vertices();
  gl_itembuf_set_items(_itembuf, &npoint_items);
  gl_itembuf_render_begin(_itembuf, GL_TRUE);
  
  glPushAttrib(GL_STENCIL_BUFFER_BIT);
  
  glStencilFunc(GL_EQUAL, 0x2, 0x2);
  glEnable(GL_STENCIL_TEST);
  display_triangulation(Triangulation_display::ITEM_BUFFER_POINTS, false);
  
  glPopAttrib();
  
  gl_itembuf_render_end(_itembuf);
  // gl_itembuf_simple_lookup(_itembuf); // not necessary
#if DEBUG
  gl_framebuf_swrite(_itembuf->gl_framebuf, "debug_01.rgb", GL_FILE_SGI,
                     GL_FALSE);
#endif
  
  /* remove erased vertices */
  const GLubyte BACKGROUND_COLOR_RED
    = (GLubyte) (_background_color[0] * (GLfloat) GL_UBYTE_MAX);
  vector<bool> is_visible(_itembuf->nitems._1D, false);
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  guint32 *itembuf_pixels = (guint32 *) _itembuf->gl_framebuf->pixels;
  int size = _itembuf->gl_framebuf->width * _itembuf->gl_framebuf->height;
  for (int i = 0; i < size; i++) {
    guint32 id = itembuf_pixels[i];
    if (id != GL_ITEMBUF_NULL_ID) {
      assert(id < _itembuf->nitems._1D);
      is_visible[id]
        = (abs(colorbuf_pixels[i] - BACKGROUND_COLOR_RED) < ERASER_THRESHOLD);
    }
  }
  int number_of_removed_vertices = 0;
  Finite_vertices_iterator vi = _triangulation_proxy->finite_vertices_begin();
  for (guint32 i = 0; i < _itembuf->nitems._1D; i++, vi++) {
    if (is_visible[i]) {
      _triangulation_proxy->remove(vi);
      number_of_removed_vertices++;
    }
  }
  assert(vi == _triangulation_proxy->finite_vertices_end());
#if DEBUG
  if (number_of_removed_vertices != 0) {
    cout << "Erasing " << number_of_removed_vertices << " vertices." << endl;
  }
#endif
}

void
Drawing::_reconstruct_curve(void) {
  for (All_faces_iterator fi = _triangulation_proxy->all_faces_begin();
       fi != _triangulation_proxy->all_faces_end(); fi++) {
    fi->reset();
  }
  vector<Vertex_handle> triangulation_vertices;
  triangulation_vertices.reserve(_triangulation_proxy->number_of_vertices());
  for (Finite_vertices_iterator vi
         = _triangulation_proxy->finite_vertices_begin();
       vi != _triangulation_proxy->finite_vertices_end(); vi++) {
    triangulation_vertices.push_back(vi);
  }
  Reconstruct_curve reconstruct_curve(*_triangulation_proxy);
  reconstruct_curve(triangulation_vertices);
  _has_changed = true;
}

void
Drawing::_gather_marked_triangulation_faces(void) {
  // CAVEAT: the current algorithm do not handle more than one stroke per
  // marked area, e.g., it will crash if the user inputs several marks in
  // a hole to be emptied.
  //
  // To solve this problem, we could change the interface: instead of a
  // simple press on the SHIFT key, the user define the "boundaries" of its
  // mark strokes by pressing a first time on M (F1?) and a second time
  // on N (F2?). This way, we will be able to gather faces in a consistent
  // way.
  //
  for (vector< vector<Tool::Point> >::const_iterator vpi
         = _marking_paths.begin(); vpi != _marking_paths.end(); vpi++) {
    set<Face_handle> marked_faces;
    vector<Tool::Point>::const_iterator pi_begin = vpi->begin();
    vector<Tool::Point>::const_iterator pi_end   = vpi->end();
    vector<Tool::Point>::const_iterator pi_next = pi_begin;
    vector<Tool::Point>::const_iterator pi_prev = pi_next++;
    Face_handle fh_next = _triangulation_proxy->locate(*pi_prev);
    assert(fh_next != NULL);
    Face_handle fh_prev = fh_next;
    
    for (; pi_next != pi_end; pi_prev = pi_next++) {
      fh_prev = fh_next;
      fh_next = _triangulation_proxy->locate(*pi_next, fh_prev);
      assert(fh_next != NULL);
      Line_face_circulator
        lfc = _triangulation_proxy->line_walk(*pi_prev, *pi_next, fh_prev),
        done(lfc);
      assert(lfc != NULL);
      do {
        if (!_triangulation_proxy->is_infinite(lfc)) {
          marked_faces.insert(lfc);
        }
        if (lfc == fh_next) {
          break;
        }
      } while (++lfc != done);
    }
    if (!marked_faces.empty()) {
      Reconstruct_curve reconstruct_curve(*_triangulation_proxy);
      reconstruct_curve(marked_faces);
    }
  }
}
