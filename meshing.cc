#include <fstream>

#include "application.hh"
#include "drawing.hh"
#include "tetrahedrization_iostream.hh"
#include "tetrahedrization_display.hh"
#include "smooth_surface.hh"
#include "reconstruct_surface.hh"
#include "meshing.hh"

using namespace std;

static const float HALFTONING_SCALE = 0.1f;
static const GLfloat ABSOLUTE_UNIT_OFFSET_SCALE = 0.05f;
static const GLfloat RELATIVE_UNIT_OFFSET_SCALE = 0.15f;
static const int BACKGROUND_ERROR = 45;

Meshing::Meshing(Application *application)
  : _application(application),
    _tetrahedrization_proxy(NULL),
    _tetrahedrization_display_ptr(NULL),
    _tesselation(),
    _transf_ortho(NULL),
    _transf_persp(NULL),
    _smoothed_vertices(),
    _removed_vertices(),
    _visible_vertices(),
    _visible_facets(),
    _integer_positions(),
    _has_changed(false),
    _is_at_depth(false),
    _offset_scale(0.0f) {
  gl_veci_eq(_drawbox, GL_VECI_NULL);
  gl_veci_eq(_drawport, GL_VECI_NULL);
  _stencilbuf = gl_framebuf_new();
  _depthbuf = gl_framebuf_new();
  _colorbuf = gl_framebuf_new();
  _errorbuf = gl_framebuf_new();
  _itembuf = gl_itembuf_new(GL_ITEMBUF_1D);
  _list = gl_list_new();
  
  gl_framebuf_set_format(_stencilbuf, GL_STENCIL_INDEX);
  gl_framebuf_set_format(_depthbuf, GL_DEPTH_COMPONENT);
  _depthbuf->type = GL_FLOAT;
  gl_framebuf_set_format(_colorbuf, GL_RED);
  gl_framebuf_set_format(_errorbuf, GL_RED);
  gl_list_set(_list, _display_list_cb, NULL, GL_FALSE);
}

Meshing::~Meshing(void) {
  _application->remove(_tetrahedrization_proxy);
  if (_tetrahedrization_display_ptr != NULL) {
    delete _tetrahedrization_display_ptr;
  }
  gl_framebuf_delete(_stencilbuf);
  gl_framebuf_delete(_depthbuf);
  gl_framebuf_delete(_colorbuf);
  gl_framebuf_delete(_errorbuf);
  gl_itembuf_delete(_itembuf);
  gl_list_delete(_list);
}

bool
Meshing::has_changed(void) const {
  return _has_changed;
}

bool
Meshing::is_at_depth(void) const {
  return _is_at_depth;
}

bool&
Meshing::is_at_depth(void) {
  return _is_at_depth;
}

bool
Meshing::init(void) {
  _tetrahedrization_proxy = _application->tetrahedrization();
  return (_tetrahedrization_proxy != NULL);
}

bool
Meshing::read(ifstream& fin, File::Type file_type) {
  if (!init()) {
    return false;
  } else {
    Tetrahedrization_iostream::Format stream_format;
    switch (file_type) {
    case File::RLF:
      stream_format = Tetrahedrization_iostream::DEFAULT;
      break;
    case File::OFF:
      stream_format = Tetrahedrization_iostream::OFF;
      break;
    default:
      assert(false);
      break;
    }
    Tetrahedrization_iostream tin(*_tetrahedrization_proxy,
                                  stream_format, true);
    fin >> tin;
    if (fin.fail()) {
      fin.clear();
      return false;
    } else {
      Reconstruct_surface reconstruct_surface(*_tetrahedrization_proxy);
      reconstruct_surface(_tetrahedrization_proxy->finite_vertices_begin(),
                          _tetrahedrization_proxy->finite_vertices_end());
      return true;
    }
  }
}

bool
Meshing::write(ofstream& fout, File::Type file_type) const {
  Tetrahedrization_iostream::Format stream_format;
  switch (file_type) {
  case File::RLF:
    stream_format = Tetrahedrization_iostream::DEFAULT;
    break;
  case File::OFF:
    stream_format = Tetrahedrization_iostream::OFF;
    break;
  default:
    assert(false);
    break;
  }
  Tetrahedrization_iostream tout(*_tetrahedrization_proxy,
                                 stream_format, true);
  fout << tout;
  if (fout.fail()) {
    fout.clear();
    return false;
  } else {
    return true;
  }
}

void
Meshing::start_meshing(const GLveci drawbox) {
  gl_veci_eq(_drawbox, drawbox);
  gl_veci_set(_drawport,
              drawbox[0],
              drawbox[1],
              drawbox[2] - drawbox[0],
              drawbox[3] - drawbox[1]);
  
  if (_application->triangulation()->number_of_vertices() == 0) {
    return;
  } else if (_application->triangulation()->number_of_inside_faces() == 0) {
    cerr << "Warning: unable to reconstruct drawn shape!" << endl;
    cerr << "Hint: close strokes tighter!" << endl;
    return;
  }
  
  glPushAttrib(GL_PIXEL_MODE_BIT);
  
  gl_list_call(_list);
  gl_framebuf_set_port(_colorbuf, _drawport);
  gl_framebuf_read(_colorbuf, GL_BACK);
#if DEBUG
  gl_framebuf_swrite(_colorbuf, "debug_05.bw", GL_FILE_SGI, GL_FALSE);
#endif
  
  glPopAttrib();
  
  // reset
  _tesselation.clear();
}

void
Meshing::stop_meshing(void) {}

void
Meshing::mesh(GLtransf *ortho, GLtransf *persp) {
  _transf_ortho = ortho;
  _transf_persp = persp;
  
  /* burnisher and scraper tools */
  _get_item_buffers();
  _smooth_or_remove_tetrahedrization_points();
  if (!_smoothed_vertices.empty() || !_removed_vertices.empty()) {
    _reconstruct_surface();
    return;
  }
  
  if (_application->triangulation()->number_of_inside_faces() == 0) return;
  
  GLvecd view_vector;
  if (!gl_transf_get_view_vector(_transf_persp, view_vector)) {
    cerr << "Error: gl_transf_get_view_vector() failure!" << endl;
    assert(false);
  }
  Vector view_vector_reversed(-view_vector[0],
                              -view_vector[1],
                              -view_vector[2]);
  
  GLvecd proj_center;
  if (!gl_transf_get_proj_center(_transf_persp, proj_center)) {
    cerr << "Error: gl_transf_get_proj_center() failure!" << endl;
    assert(false);
  }
  Point projection_center(proj_center[0],
                          proj_center[1],
                          proj_center[2]);
  
  /* pencil, quill, brush, smudge and frisket tools */
  _get_stencil_depth_and_item_buffers();
  if (_visible_vertices.empty()) {
    // drawing either not on surface or at depth: use default depth value
    _evaluate_tesselation_error();
    if (_is_at_depth && _tetrahedrization_proxy->number_of_vertices() != 0) {
      _insert_new_triangulation_points_in_tesselation_at_depth(
        view_vector_reversed, projection_center);
    } else {
      _insert_new_triangulation_points_in_tesselation(
        view_vector_reversed, projection_center);
    }
    _unproject_new_triangulation_points();
  } else {
    // drawing on surface: use existing depth values
    Point mean_point(CGAL::ORIGIN);
    _render_tesselation_with_tetrahedrization_points(projection_center,
                                                     mean_point);
    _evaluate_tesselation_error();
    _insert_new_tetrahedrization_points_in_tesselation(view_vector_reversed,
                                                       projection_center,
                                                       mean_point);
    _unproject_new_tetrahedrization_points();
  }
  _reconstruct_surface();
}

void
Meshing::unmesh(void) {
  _tetrahedrization_proxy->undo_last_changes();
  _reconstruct_surface();
}

void
Meshing::display_tetrahedrization(Tetrahedrization_display::Style style,
                                  bool display_bbox) {
  _tetrahedrization_display()->push_style(style);
  _tetrahedrization_display()->display();
  _tetrahedrization_display()->pop_style();
  if (display_bbox) {
    _tetrahedrization_display()->display_bbox();
  }
}

GLboolean
Meshing::_display_list_cb(void *data, GLboolean test_proxy) {
  GLfloat gaussian_kernel[7]
    = {0.006f, 0.061f, 0.242f, 0.383f, 0.242f, 0.061f, 0.006f};
  // discrete approximation to Gaussian function with sigma equal to 1.0
  
  if (test_proxy) return GL_TRUE;
  glConvolutionParameteri(GL_SEPARABLE_2D, GL_CONVOLUTION_BORDER_MODE,
                          GL_REPLICATE_BORDER_HP);
  glSeparableFilter2D(GL_SEPARABLE_2D, GL_LUMINANCE, 7, 7,
                      GL_LUMINANCE, GL_FLOAT,
                      gaussian_kernel, gaussian_kernel);
  glEnable(GL_SEPARABLE_2D);
  return GL_TRUE;
}

Tetrahedrization_display *
Meshing::_tetrahedrization_display(void) {
  if (_tetrahedrization_display_ptr == NULL) {
    _tetrahedrization_display_ptr
      = new Tetrahedrization_display(*(_application->tetrahedrization()));
    assert(_tetrahedrization_display_ptr != NULL);
  }
  return _tetrahedrization_display_ptr;
}

void
Meshing::_remove(Tetrahedrization_display *tetrahedrization_display) {
  if (tetrahedrization_display == _tetrahedrization_display_ptr) {
    if (_tetrahedrization_display_ptr != NULL) {
      delete _tetrahedrization_display_ptr;
      _tetrahedrization_display_ptr = NULL;
    }
  }
}

void
Meshing::_get_item_buffers(void) {
  if (_tetrahedrization_proxy->number_of_vertices() == 0) return;
  gl_transf_begin(_transf_persp);
  
  gl_itembuf_set_port(_itembuf, _drawport);
  unsigned int npoint_items = _tetrahedrization_proxy->number_of_vertices();
  gl_itembuf_set_items(_itembuf, &npoint_items);
  gl_itembuf_render_begin(_itembuf, GL_TRUE);
  
  glPushAttrib(GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);
  
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glPolygonOffset(1.0, 1.0);
  glEnable(GL_POLYGON_OFFSET_FILL);
  
  display_tetrahedrization(Tetrahedrization_display::SOLID, false);
  
  glDisable(GL_POLYGON_OFFSET_FILL);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);
  glStencilFunc(GL_EQUAL, 0x20, 0x30); /* 0x10 + 0x20 */
  glEnable(GL_STENCIL_TEST);
  
  display_tetrahedrization(Tetrahedrization_display::ITEM_BUFFER_POINTS,
                           false);
  
  glPopAttrib();
  
  gl_itembuf_render_end(_itembuf);
  gl_itembuf_simple_lookup(_itembuf);
#if DEBUG
  gl_framebuf_swrite(_itembuf->gl_framebuf, "debug_06.rgb", GL_FILE_SGI,
                     GL_FALSE);
#endif
  
  _smoothed_vertices.clear();
  _smoothed_vertices.reserve(_itembuf->nitems._1D);
  Finite_vertices_iterator vi
    = _tetrahedrization_proxy->finite_vertices_begin();
  for (guint32 i = 0; i < _itembuf->nitems._1D; i++, vi++) {
    if (_itembuf->items._1D[i] != 0) {
      _smoothed_vertices.push_back(vi);
    }
  }
#if DEBUG
  cout << "Smoothing " << _smoothed_vertices.size() << " vertices" << endl;
#endif
  
  // burnisher *or* scraper to avoid problems in determining first vertex
  // inserted or removed (useful for undo purposes)
  if (!_smoothed_vertices.empty()) {
    gl_transf_end(_transf_persp);
    return;
  }
  
  gl_itembuf_reset_items(_itembuf);
  gl_itembuf_render_begin(_itembuf, GL_FALSE);
  
  glPushAttrib(GL_STENCIL_BUFFER_BIT);
  
  glDepthFunc(GL_LESS);
  glStencilFunc(GL_EQUAL, 0x40, 0x50); /* 0x10 + 0x40 */
  glEnable(GL_STENCIL_TEST);
  
  display_tetrahedrization(Tetrahedrization_display::ITEM_BUFFER_POINTS,
                           false);
  
  glPopAttrib();
  
  gl_itembuf_render_end(_itembuf);
  gl_itembuf_simple_lookup(_itembuf);
#if DEBUG
  gl_framebuf_swrite(_itembuf->gl_framebuf, "debug_07.rgb", GL_FILE_SGI,
                     GL_FALSE);
#endif
  
  _removed_vertices.clear();
  _removed_vertices.reserve(_itembuf->nitems._1D);
  vi = _tetrahedrization_proxy->finite_vertices_begin();
  for (guint32 i = 0; i < _itembuf->nitems._1D; i++, vi++) {
    if (_itembuf->items._1D[i] != 0) {
      _removed_vertices.push_back(vi);
    }
  }
#if DEBUG
  cout << "Removing " << _removed_vertices.size() << " vertices" << endl;
#endif
  
  gl_transf_end(_transf_persp);
}

void
Meshing::_smooth_or_remove_tetrahedrization_points(void) {
  if (!_smoothed_vertices.empty()) {
    Smooth_surface smooth_surface(*_tetrahedrization_proxy);
    smooth_surface(_smoothed_vertices.begin(), _smoothed_vertices.end());
  } else if (!_removed_vertices.empty()) {
    vector<Vertex_handle>::iterator vhi = _removed_vertices.begin();
    _tetrahedrization_proxy->remove_first(*vhi);
    for (vhi++; vhi != _removed_vertices.end(); vhi++) {
      _tetrahedrization_proxy->remove(*vhi);
    }
  }
}

void
Meshing::_evaluate_tesselation_error(void) {
  glPushAttrib(GL_PIXEL_MODE_BIT);
  
  gl_list_call(_list);
  gl_framebuf_set_port(_errorbuf, _drawport);
  gl_framebuf_read(_errorbuf, GL_BACK);
#if DEBUG
  gl_framebuf_swrite(_errorbuf, "debug_08.bw", GL_FILE_SGI, GL_FALSE);
#endif
  
  glPopAttrib();
  
  GLubyte *stencilbuf_pixels = (GLubyte *) _stencilbuf->pixels;
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  GLubyte *errorbuf_pixels = (GLubyte *) _errorbuf->pixels;
  int size = _colorbuf->width * _colorbuf->height;
  unsigned int mask = (_is_at_depth ? 0x4 : 0xC); /* 0x4 + 0x8 */
  int error = 0;
  for (int i = 0; i < size; i++) {
    if ((stencilbuf_pixels[i] & mask) == 0x4) {
      error = BACKGROUND_ERROR;
    } else {
      error = abs(errorbuf_pixels[i] - colorbuf_pixels[i]);
    }
    error = (int) roundf(HALFTONING_SCALE * (float) error);
    errorbuf_pixels[i] = (GLubyte) (255 - error);
  }
#if DEBUG
  gl_framebuf_swrite(_errorbuf, "debug_09.bw", GL_FILE_SGI, GL_FALSE);
#endif
  
  /* Avoiding the "dead zone" problem. Solution adapted from:
   * Pierre Alliez, Mark Meyer and Mathieu Desbrun, Interactive Geometry
   * Remeshing, Proceedings of ACM SIGGRAPH, 2002.
   */
  GLveci port;
  gl_veci_set(port,
    _errorbuf->x, _errorbuf->y, _errorbuf->width, 2 * _errorbuf->height);
  gl_framebuf_set_port(_errorbuf, port);
  size_t nbytes = size * sizeof(GLubyte);
  GLubyte *src  = (GLubyte *) _errorbuf->pixels;
  GLubyte *dest = src + nbytes;
  memcpy(dest, src, nbytes);
  gl_framebuf_ht(_errorbuf);
  memcpy(src, dest, nbytes);
  gl_veci_set(port,
    _errorbuf->x, _errorbuf->y, _errorbuf->width, _errorbuf->height / 2);
  gl_framebuf_set_port(_errorbuf, port);
#if DEBUG
  gl_framebuf_swrite(_errorbuf, "debug_10.bw", GL_FILE_SGI, GL_FALSE);
#endif
}

void
Meshing::_insert_new_triangulation_points_in_tesselation(
  const Vector& normal, const Point& eye) {
  GLvecd obj, win;
  gl_vecd_set(obj, 0.0, 0.0, 0.0, 1.0);
  if (!gl_transf_project(_transf_persp, obj, win)) {
    cerr << "Error: gl_transf_project() failure!" << endl;
    assert(false);
  }
  GLdouble origin_depth = win[2];
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  GLubyte *errorbuf_pixels = (GLubyte *) _errorbuf->pixels;
  int width  = _errorbuf->width;
  int height = _errorbuf->height;
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int index = j * width + i;
      if (errorbuf_pixels[index] == GL_UBYTE_MIN) {
        Tesselation::Vertex_handle vh
          = _tesselation.insert(Tesselation::Point(i + _errorbuf->x,
                                                   j + _errorbuf->y));
        vh->depth() = origin_depth;
        vh->offsetub() = colorbuf_pixels[index];
        vh->normal() = normal;
      } else {
        assert(errorbuf_pixels[index] == GL_UBYTE_MAX);
      }
    }
  }
  assert(_tesselation.is_valid());
  
  double height_field_max = _application->drawing()->height_field_max();
  if (height_field_max == 0) {
    Point center(CGAL::ORIGIN);
    _offset_scale
      = ABSOLUTE_UNIT_OFFSET_SCALE * sqrt(CGAL::squared_distance(eye, center));
    //cout << "dist " << sqrt(CGAL::squared_distance(eye, center)) << endl;////
  } else {
    _set_height_field_offset_scale(height_field_max, origin_depth);
  }
}

void
Meshing::_insert_new_triangulation_points_in_tesselation_at_depth(
  const Vector& normal, const Point& eye) {
  GLfloat *depthbuf_pixels = (GLfloat *) _depthbuf->pixels;
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  GLubyte *errorbuf_pixels = (GLubyte *) _errorbuf->pixels;
  GLfloat depth_min = GL_DEPTH_FAR;
  Tesselation::Point center_2D(CGAL::ORIGIN);
  Point center_3D(CGAL::ORIGIN);
  int width  = _errorbuf->width;
  int height = _errorbuf->height;
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int index = j * width + i;
      if (errorbuf_pixels[index] == GL_UBYTE_MIN) {
        Tesselation::Vertex_handle vh
          = _tesselation.insert(Tesselation::Point(i + _errorbuf->x,
                                                   j + _errorbuf->y));
        GLfloat depth = depthbuf_pixels[index];
        if (depth < depth_min) {
          depth_min = depth;
          center_2D = vh->point();
        }
        vh->offsetub() = colorbuf_pixels[index];
        vh->normal() = normal;
      } else {
        assert(errorbuf_pixels[index] == GL_UBYTE_MAX);
      }
    }
  }
  assert(_tesselation.is_valid());
  for (Tesselation::Finite_vertices_iterator vi
         = _tesselation.vertices_begin();
       vi != _tesselation.vertices_end(); vi++) {
    vi->depth() = depth_min;
  }
  
  GLvecd win, obj;
  gl_vecd_set(win, center_2D.x(), center_2D.y(), depth_min, 1.0);
  if (gl_transf_unproject(_transf_persp, win, obj)) {
    center_3D = Point(obj[0], obj[1], obj[2]);
  } else {
    cerr << "Error: gl_transf_unproject() failure!" << endl;
    assert(false);
  }
  
  double height_field_max = _application->drawing()->height_field_max();
  if (height_field_max == 0) {
    _offset_scale
      = RELATIVE_UNIT_OFFSET_SCALE
          * (sqrt(CGAL::squared_distance(eye, center_3D))
               / _tetrahedrization_display()->size());
    //cout << "dist " << sqrt(CGAL::squared_distance(eye, center_3D))
    //     << endl;//
    //cout << "dist rel " << (sqrt(CGAL::squared_distance(eye, center_3D))
    //                       / _tetrahedrization_display()->size()) << endl;///
  } else {
    _set_height_field_offset_scale(height_field_max, depth_min);
  }
}

void
Meshing::_unproject_new_triangulation_points(void) {
  GLvecd win, obj;
  bool is_first = true;
  
  for (Tesselation::Finite_vertices_iterator vi
         = _tesselation.vertices_begin();
       vi != _tesselation.vertices_end(); vi++) {
    Tesselation::Point p_2D(vi->point());
    
    gl_vecd_set(win, p_2D.x(), p_2D.y(), vi->depth(), 1.0);
    if (gl_transf_unproject(_transf_persp, win, obj)) {
      Point p_3D(obj[0], obj[1], obj[2]);
      
      p_3D = p_3D + _offset_scale * vi->offsetf() * vi->normal();
      if (is_first) {
        is_first = false;
        vi->projected_vertex() = _tetrahedrization_proxy->insert_first(p_3D);
      } else {
        vi->projected_vertex() = _tetrahedrization_proxy->insert(p_3D);
      }
    } else {
      cerr << "Error: gl_transf_unproject() failure!" << endl;
      assert(false);
    }
  }
  assert(_tetrahedrization_proxy->is_valid());
}

void
Meshing::_get_stencil_depth_and_item_buffers(void) {
  if (_tetrahedrization_proxy->number_of_vertices() == 0) {
    _read_stencil_buffer();
    _clear_color_buffer();
    return;
  }
  
  _visible_vertices.clear();
  _visible_facets.clear();
  
  gl_transf_begin(_transf_persp);
  
  glClear(GL_DEPTH_BUFFER_BIT);
  gl_framebuf_set_port(_depthbuf, _drawport);
  display_tetrahedrization(Tetrahedrization_display::DEPTH_BUFFER, false);
  gl_framebuf_read(_depthbuf, GL_BACK);
  _read_stencil_buffer();
  
#if DEBUG
  GLframebuf *depthbuf = gl_framebuf_new();
  gl_framebuf_set_format(depthbuf, GL_DEPTH_COMPONENT);
  gl_framebuf_set_port(depthbuf, _drawport);
  gl_framebuf_read(depthbuf, GL_BACK);
  gl_framebuf_swrite(depthbuf, "debug_11.bw", GL_FILE_SGI, GL_FALSE);
  gl_framebuf_delete(depthbuf);
#endif
  
  if (_is_at_depth) {
    gl_transf_end(_transf_persp);
    _clear_color_buffer();
    return;
  }
  
  /* get visible vertices */
  gl_itembuf_set_port(_itembuf, _drawport);
  unsigned int npoint_items = _tetrahedrization_proxy->number_of_vertices();
  gl_itembuf_set_items(_itembuf, &npoint_items);
  gl_itembuf_render_begin(_itembuf, GL_TRUE);
  
  glPushAttrib(GL_POLYGON_BIT | GL_STENCIL_BUFFER_BIT);
  
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glPolygonOffset(1.0, 1.0);
  glEnable(GL_POLYGON_OFFSET_FILL);
  
  display_tetrahedrization(Tetrahedrization_display::SOLID, false);
  
  glDisable(GL_POLYGON_OFFSET_FILL);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);
  glStencilFunc(GL_NOTEQUAL, 0x10, 0x10);
  glEnable(GL_STENCIL_TEST);
  
  display_tetrahedrization(Tetrahedrization_display::ITEM_BUFFER_POINTS,
                           false);
  
  glPopAttrib();
  
  gl_itembuf_render_end(_itembuf);
  gl_itembuf_simple_lookup(_itembuf);
#if DEBUG
  gl_framebuf_swrite(_itembuf->gl_framebuf, "debug_13.rgb", GL_FILE_SGI,
                     GL_FALSE);
#endif
  
  Finite_vertices_iterator vi
    = _tetrahedrization_proxy->finite_vertices_begin();
  for (guint32 i = 0; i < _itembuf->nitems._1D; i++, vi++) {
    if (_itembuf->items._1D[i] != 0) {
      _visible_vertices[i] = vi;
    }
  }
#if DEBUG
  cout << _visible_vertices.size() << " visible vertices" << endl;
#endif
  
  /* get visible facets */
  unsigned int ntriangle_items
    = _tetrahedrization_proxy->number_of_surface_facets();
  gl_itembuf_set_items(_itembuf, &ntriangle_items);
  gl_itembuf_render_begin(_itembuf, GL_TRUE);
  
  glPushAttrib(GL_STENCIL_BUFFER_BIT);
  
  glStencilFunc(GL_NOTEQUAL, 0x10, 0x10);
  glEnable(GL_STENCIL_TEST);
  
  display_tetrahedrization(Tetrahedrization_display::ITEM_BUFFER_TRIANGLES,
                           false);
  
  glPopAttrib();
  
  gl_itembuf_render_end(_itembuf);
  gl_itembuf_simple_lookup(_itembuf);
#if DEBUG
  gl_framebuf_swrite(_itembuf->gl_framebuf, "debug_14.rgb", GL_FILE_SGI,
                     GL_FALSE);
#endif
  
  guint32 index = 0u;
  for (All_cells_iterator ci = _tetrahedrization_proxy->all_cells_begin();
       ci != _tetrahedrization_proxy->all_cells_end(); ci++) {
    for (int i = 0; i < 4; i++) {
      if (ci->is_surface_facet(i)) {
        assert(index < _itembuf->nitems._1D);
        if (_itembuf->items._1D[index] != 0) {
          _visible_facets[index] = Facet(ci, i);
        }
        index++;
      }
    }
  }
#if DEBUG
  cout << _visible_facets.size() << " visible facets" << endl;
#endif
  
  gl_transf_end(_transf_persp);
  _clear_color_buffer();
}

void
Meshing::_render_tesselation_with_tetrahedrization_points(const Point& eye,
                                                          Point& center) {
  Vector mean_vector(CGAL::NULL_VECTOR);
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  GLvecd obj, win;
  
  _integer_positions.clear();
  for (map<guint32, Vertex_handle>::iterator vhi = _visible_vertices.begin();
       vhi != _visible_vertices.end(); vhi++) {
    Vertex_handle vh_3D = vhi->second;
    Point p_3D = vh_3D->point();
    
    mean_vector = mean_vector + (p_3D - CGAL::ORIGIN);
    gl_vecd_set(obj, p_3D.x(), p_3D.y(), p_3D.z(), 1.0);
    if (gl_transf_project(_transf_persp, obj, win)) {
      Tesselation::Point p_2D(win[0], win[1]);
      Tesselation::Vertex_handle vh_2D = _tesselation.insert(p_2D);
      
      vh_2D->depth() = win[2];
      pair<int, int> integer_position((int) win[0], (int) win[1]);
      integer_position.first
        = scali_clamp(integer_position.first,  _drawbox[0], _drawbox[2] - 1);
      integer_position.second
        = scali_clamp(integer_position.second, _drawbox[1], _drawbox[3] - 1);
      // we subtract 1 to take into account the cast from double to int
      _integer_positions.insert(integer_position);
      GLuint index = gl_framebuf_index(_colorbuf, integer_position.first,
                                                  integer_position.second);
      assert(index != GL_FRAMEBUF_NULL_INDEX);
      vh_2D->offsetub() = colorbuf_pixels[index];
      vh_2D->normal() = _tetrahedrization_proxy->approximate_normal(vh_3D);
      /*
       * Lee Markosian and Michael A. Kowalski and Samuel J. Trychin and
       * Lubomir D. Bourdev and Daniel Goldstein and John F. Hughes,
       * Real-Time Nonphotorealistic Rendering, Proceedings of ACM SIGGRAPH,
       * pp. 415-420, 1997.
       *
       * "Definition 1: A polygon is front-facing if the dot product of its
       * outward normal and a vector from a point on the polygon to the
       * camera position is positive. Otherwise the polygon is
       * back-facing. A silhouette edge is an edge adjacent to one
       * front-facing and one back-facing polygon." (p. 416)
       */
      FT eye_dot_normal = (eye - p_3D) * vh_2D->normal();
      if (eye_dot_normal < 0) {
        vh_2D->normal() = - vh_2D->normal();
      } else if (eye_dot_normal == 0) {
#if DEBUG
        cerr << "Warning: eye * normal equal zero!" << endl;
#endif
      }
      vh_2D->projected_vertex() = vh_3D;
    } else {
      cerr << "Error: gl_transf_project() failure!" << endl;
      assert(false);
    }
  }
  assert(_tesselation.is_valid());
  assert(!_visible_vertices.empty());
  mean_vector = mean_vector / _visible_vertices.size();
  center = CGAL::ORIGIN + mean_vector;
  
  // render drawing approximation using projected points only
  gl_transf_begin(_transf_ortho);
  _tesselation.display();
  gl_transf_end(_transf_ortho);
}

void
Meshing::_insert_new_tetrahedrization_points_in_tesselation(
  const Vector& normal, const Point& eye, const Point& center) {
  GLfloat *depthbuf_pixels = (GLfloat *) _depthbuf->pixels;
  GLubyte *colorbuf_pixels = (GLubyte *) _colorbuf->pixels;
  GLubyte *errorbuf_pixels = (GLubyte *) _errorbuf->pixels;
  guint32 *itembuf_pixels = (guint32 *) _itembuf->gl_framebuf->pixels;
  int width  = _errorbuf->width;
  int height = _errorbuf->height;
  int number_of_inserted_points = 0;
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int index = j * width + i;
      if (errorbuf_pixels[index] == GL_UBYTE_MIN) {
        pair<int, int> position(i + _errorbuf->x, j + _errorbuf->y);
        if (_integer_positions.find(position) == _integer_positions.end()) {
          Tesselation::Vertex_handle vh
            = _tesselation.insert(Tesselation::Point(position.first,
                                                     position.second));
          number_of_inserted_points++;
          vh->depth() = depthbuf_pixels[index];
          vh->offsetub() = colorbuf_pixels[index];
          guint32 id = itembuf_pixels[index];
          if (id != GL_ITEMBUF_NULL_ID) {
            assert(id < _itembuf->nitems._1D);
            vh->normal()
              = _tetrahedrization_proxy->triangle(_visible_facets[id])
                  .supporting_plane().orthogonal_vector();
          } else {
            vh->normal() = normal;
          }
        } else {
#if DEBUG
          cerr << "Warning: point position already occupied!" << endl;
#endif
        }
      } else {
        assert(errorbuf_pixels[index] == GL_UBYTE_MAX);
      }
    }
  }
  
  assert(_tesselation.is_valid());
  if (number_of_inserted_points > 0) {
    _tesselation.propagate_depth();
  }
  
  double height_field_max = _application->drawing()->height_field_max();
  if (height_field_max == 0) {
    _offset_scale
      = RELATIVE_UNIT_OFFSET_SCALE
          * (sqrt(CGAL::squared_distance(eye, center))
               / _tetrahedrization_display()->size());
    //cout << "dist " << sqrt(CGAL::squared_distance(eye, center_3D))
    //     << endl;//
    //cout << "dist rel " << (sqrt(CGAL::squared_distance(eye, center_3D))
    //                       / _tetrahedrization_display()->size()) << endl;///
  } else {
    GLvecd obj, win;
    gl_vecd_set(obj, center.x(), center.y(), center.z(), 1.0);
    if (!gl_transf_project(_transf_persp, obj, win)) {
      cerr << "Error: gl_transf_project() failure!" << endl;
      assert(false);
    }
    GLdouble mean_depth = win[2];
    _set_height_field_offset_scale(height_field_max, mean_depth);
  }
}

void
Meshing::_unproject_new_tetrahedrization_points(void) {
  GLvecd win, obj;
  bool is_first = true;
  
  for (Tesselation::Finite_vertices_iterator vi
         = _tesselation.vertices_begin();
       vi != _tesselation.vertices_end(); vi++) {
    Vertex_handle projected_vertex(vi->projected_vertex());
    if (projected_vertex == NULL) {
      Tesselation::Point p_2D(vi->point());
      
      gl_vecd_set(win, p_2D.x(), p_2D.y(), vi->depth(), 1.0);
      if (gl_transf_unproject(_transf_persp, win, obj)) {
        Point p_3D(obj[0], obj[1], obj[2]);
        
        p_3D = p_3D + _offset_scale * vi->offsetf() * vi->normal();
        if (is_first) {
          is_first = false;
          vi->projected_vertex() = _tetrahedrization_proxy->insert_first(p_3D);
        } else {
          vi->projected_vertex() = _tetrahedrization_proxy->insert(p_3D);
        }
      } else {
        cerr << "Error: gl_transf_unproject() failure!" << endl;
        assert(false);
      }
    } else {
      GLfloat offset = vi->offsetf();
      if (offset != 0.0f) {
        Point p_3D(projected_vertex->point());
        
        p_3D = p_3D + _offset_scale * offset * vi->normal();
        if (is_first) {
          is_first = false;
          vi->projected_vertex()
            = _tetrahedrization_proxy->move_first(projected_vertex, p_3D);
        } else {
          vi->projected_vertex()
            = _tetrahedrization_proxy->move(projected_vertex, p_3D);
        }
      }
    }
  }
  assert(_tetrahedrization_proxy->is_valid());
}

void
Meshing::_read_stencil_buffer(void) {
  gl_framebuf_set_port(_stencilbuf, _drawport);
  gl_framebuf_read(_stencilbuf, GL_BACK);
#if DEBUG
  gl_framebuf_swrite(_stencilbuf, "debug_12.bw", GL_FILE_SGI, GL_FALSE);
#endif
}

void
Meshing::_clear_color_buffer(void) {
  GLvecf clear_color;
  gl_vecf_eq(clear_color, _application->drawing()->background_color());
  glPushAttrib(GL_COLOR_BUFFER_BIT);
  
  glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
  glClear(GL_COLOR_BUFFER_BIT);
  
  glPopAttrib();
}

void
Meshing::_reconstruct_surface(void) {
  assert(_tetrahedrization_proxy->dimension() == 3);
  cout << "Reconstruct surface" << endl;
  for (All_cells_iterator ci = _tetrahedrization_proxy->all_cells_begin();
       ci != _tetrahedrization_proxy->all_cells_end(); ci++) {
    ci->reset();
  }
  Reconstruct_surface reconstruct_surface(*_tetrahedrization_proxy);
  reconstruct_surface(_tetrahedrization_proxy->finite_vertices_begin(),
                      _tetrahedrization_proxy->finite_vertices_end());
  //TODO: perform partial reconstruction?
  cout << _tetrahedrization_proxy->number_of_surface_vertices()
       << " vertices and "
       << _tetrahedrization_proxy->number_of_surface_facets()
       << " faces" << endl;
  _remove(_tetrahedrization_display_ptr);
  cout << "done." << endl;
  _has_changed = true;
}

void
Meshing::_set_height_field_offset_scale(double height_field_max,
                                        GLdouble depth) {
  // p is the triangulation bbox center
  CGAL::Bbox_2 triangulation_bbox = _application->triangulation()->bbox();
  Triangulation::Point p_2D(
    0.5 * (triangulation_bbox.xmin() + triangulation_bbox.xmax()),
    0.5 * (triangulation_bbox.ymin() + triangulation_bbox.ymax()));
  
  // q is a point at height_field_max distance from p
  Triangulation::Geom_traits::Kernel::Vector_2 translation(height_field_max,
                                                           0.0);
  Triangulation::Point q_2D(p_2D + translation);
  
  // unproject them to compute their distance in 3D
  GLvecd win, obj;
  Point p_3D(CGAL::ORIGIN);
  gl_vecd_set(win, p_2D.x(), p_2D.y(), depth, 1.0);
  if (gl_transf_unproject(_transf_persp, win, obj)) {
    p_3D = Point(obj[0], obj[1], obj[2]);
  } else {
    cerr << "Error: gl_transf_unproject() failure!" << endl;
    assert(false);
  }
  Point q_3D(CGAL::ORIGIN);
  gl_vecd_set(win, q_2D.x(), q_2D.y(), depth, 1.0);
  if (gl_transf_unproject(_transf_persp, win, obj)) {
    q_3D = Point(obj[0], obj[1], obj[2]);
  } else {
    cerr << "Error: gl_transf_unproject() failure!" << endl;
    assert(false);
  }
  
  _offset_scale = sqrt(CGAL::squared_distance(p_3D, q_3D));
}
