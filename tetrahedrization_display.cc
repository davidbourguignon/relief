#include <glib.h>
#include <opengl_widget.h>

#include "images.h"
#include "tetrahedrization_display.hh"

using namespace std;

static const GLfloat UNIT_NORMAL_DISPLAY_SCALE = 0.01f;
static const GLfloat RASKAR_SIZE = 8.0f;
static const GLfloat DEPTH_LINE_SIZE = 10.0f;

Tetrahedrization_display::Tetrahedrization_display(Tetrahedrization&
                                                     tetrahedrization)
  : _tetrahedrization(tetrahedrization),
    _style(),
    _is_first_npr_gooch_display(true) {
  _style.push(SOLID);
  _gooch_image = gl_framebuf_new();
  _gooch_texture = gl_texture_new();
  _enable_lighting_list = gl_list_new();
  _disable_lighting_list = gl_list_new();
  _facets_with_facet_normals_list = gl_list_new();
  _facet_normals_list = gl_list_new();
  _gooch_list = gl_list_new();
#if DEBUG
  _facets_with_vertex_normals_list = gl_list_new();
  _vertex_normals_list = gl_list_new();
#endif
  
  gl_framebuf_sread(_gooch_image, "rgb/gooch.bw", GL_FILE_SGI);
  gl_list_set(_enable_lighting_list,
              _display_enable_lighting_list_cb, NULL, GL_FALSE);
  gl_list_set(_disable_lighting_list,
              _display_disable_lighting_list_cb, NULL, GL_FALSE);
  gl_list_set(_facets_with_facet_normals_list,
              _display_facets_with_facet_normals_list_cb, (void *) this,
              GL_FALSE);
  gl_list_set(_facet_normals_list,
              _display_facet_normals_list_cb, (void *) this, GL_FALSE);
  gl_list_set(_gooch_list,
              _display_gooch_list_cb, (void *) this, GL_FALSE);
#if DEBUG
  gl_list_set(_facets_with_vertex_normals_list,
              _display_facets_with_vertex_normals_list_cb, (void *) this,
              GL_FALSE);
  gl_list_set(_vertex_normals_list,
              _display_vertex_normals_list_cb, (void *) this, GL_FALSE);
  
  _normals = CGAL::Unique_hash_map<Vertex_handle, Vector>(
    CGAL::NULL_VECTOR, tetrahedrization.number_of_surface_vertices());
  for (Finite_vertices_iterator vi = _tetrahedrization.finite_vertices_begin();
       vi != _tetrahedrization.finite_vertices_end(); vi++) {
    _normals[vi] = _tetrahedrization.approximate_normal(vi);
  }
#endif
  
  CGAL::Bbox_3 bbox = tetrahedrization.bbox();
  GLvecf min, max, diag;
  gl_vecf_set(min, bbox.xmin(), bbox.ymin(), bbox.zmin(), 1.0f);
  gl_vecf_set(max, bbox.xmax(), bbox.ymax(), bbox.zmax(), 1.0f);
  _size = gl_vecf_norm(gl_vecf_sub(diag, min, max));
  _normal_display_scale = UNIT_NORMAL_DISPLAY_SCALE * _size;
}

Tetrahedrization_display::~Tetrahedrization_display(void) {
  gl_framebuf_delete(_gooch_image);
  gl_texture_delete(_gooch_texture);
  gl_list_delete(_enable_lighting_list);
  gl_list_delete(_disable_lighting_list);
  gl_list_delete(_facets_with_facet_normals_list);
  gl_list_delete(_facet_normals_list);
  gl_list_delete(_gooch_list);
#if DEBUG
  gl_list_delete(_facets_with_vertex_normals_list);
  gl_list_delete(_vertex_normals_list);
#endif
}

void
Tetrahedrization_display::push_style(Style style) {
  _style.push(style);
}

void
Tetrahedrization_display::pop_style(void) {
  _style.pop();
  assert(!_style.empty());
}

GLfloat
Tetrahedrization_display::size(void) const {
  return _size;
}

void
Tetrahedrization_display::display(void) {
  if (_tetrahedrization.number_of_vertices() == 0) return;
  
  switch (_style.top()) {
  case POINTS: {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case WIREFRAME: {
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case SOLID: {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    gl_list_call(_enable_lighting_list);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case FACET_NORMALS: {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT |
                 GL_LIGHTING_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    gl_list_call(_enable_lighting_list);
    gl_list_call(_facets_with_facet_normals_list);
    gl_list_call(_disable_lighting_list);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    gl_list_call(_facets_with_facet_normals_list);
    
    glColor4fv(GL_PURE_RED);
    gl_list_call(_facet_normals_list);
    
    glPopAttrib();
  } break;
#if DEBUG
  case VERTEX_NORMALS: {
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT |
                 GL_LIGHTING_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    gl_list_call(_enable_lighting_list);
    gl_list_call(_facets_with_vertex_normals_list);
    gl_list_call(_disable_lighting_list);
    
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    gl_list_call(_facets_with_vertex_normals_list);
    
    glColor4fv(GL_PURE_RED);
    gl_list_call(_vertex_normals_list);
    
    glPopAttrib();
  } break;
#endif
  case NPR_GOOCH: {
    /*
     * Amy Gooch, Bruce Gooch, Peter Shirley and Elaine Cohen,
     * A Non-Photorealistic Lighting Model for Automatic Technical
     * Illustration, Proceedings of ACM SIGGRAPH, pp. 447-452, 1998.
     */
    if (_is_first_npr_gooch_display) {
      _is_first_npr_gooch_display = false;
      gl_texture_set(_gooch_texture,
                     GL_TEXTURE_2D, _setup_gooch_texture_cb, (void *) this);
    }
    
    glPushAttrib(GL_ENABLE_BIT | GL_HINT_BIT | GL_TEXTURE_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    gl_list_call(_gooch_list);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case NPR_RASKAR: {
    /*
     * Ramesh Raskar and Michael Cohen, Image Precision Silhouette Edges,
     * Proceedings of the Symposium on Interactive 3D Graphics, pp. 135-140,
     * 1999.
     *
     * Paul Rademacher, View-Dependent Geometry, Proceedings of ACM
     * SIGGRAPH, pp. 439-446, 1999.
     */    
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                 GL_POLYGON_BIT | GL_LINE_BIT | GL_POINT_BIT);
    
    // Raskar 1st pass
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    gl_list_call(_facets_with_facet_normals_list);
    
    // Raskar 2nd pass */
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);
    glPolygonMode(GL_BACK, GL_LINE);
    glLineWidth(RASKAR_SIZE);
    gl_list_call(_facets_with_facet_normals_list);
    
    // Rademacher 3rd pass
    glPolygonMode(GL_BACK, GL_POINT);
    glPointSize(0.5f*RASKAR_SIZE);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case DEPTH_BUFFER: {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT |
                 GL_POLYGON_BIT | GL_LINE_BIT);
    
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilMask(0x8);
    glStencilFunc(GL_NOTEQUAL, 0x18, 0x10); /* 0x8 + 0x10 */
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_DEPTH_TEST);
    gl_list_call(_facets_with_facet_normals_list);
    
    glPopAttrib();
  } break;
  case ITEM_BUFFER_POINTS: {
    guint32 index = 0u;
    GLubyte *pindex = (GLubyte *) &index;
    
    glPushAttrib(GL_CURRENT_BIT);
    
    glBegin(GL_POINTS);
    for (Finite_vertices_iterator vi
           = _tetrahedrization.finite_vertices_begin();
         vi != _tetrahedrization.finite_vertices_end(); vi++) {
      glColor4ubv(pindex);
      glPoint3(vi->point());
      index++;
    }
    glEnd();
    
    glPopAttrib();
  } break;
  case ITEM_BUFFER_TRIANGLES: {
    guint32 index = 0u;
    GLubyte *pindex = (GLubyte *) &index;
    
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBegin(GL_TRIANGLES);
    for (All_cells_iterator ci = _tetrahedrization.all_cells_begin();
         ci != _tetrahedrization.all_cells_end(); ci++) {
      for (int i = 0; i < 4; i++) {
        if (ci->is_surface_facet(i)) {
          glColor4ubv(pindex);
          glTriangle3(_tetrahedrization.triangle(ci, i));
          index++;
        }
      }
    }
    glEnd();
    
    glPopAttrib();
  } break;
  default:
    assert(false);
    break;
  }
}

void
Tetrahedrization_display::display_bbox(void) {
  if (_tetrahedrization.number_of_vertices() == 0) return;
  
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT);
  glColor4fv(GL_PURE_BLACK);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  gl_widget_bbox_3(_tetrahedrization.bbox().xmin(),
                   _tetrahedrization.bbox().ymin(),
                   _tetrahedrization.bbox().zmin(),
                   _tetrahedrization.bbox().xmax(),
                   _tetrahedrization.bbox().ymax(),
                   _tetrahedrization.bbox().zmax());
  glPopAttrib();
}

GLboolean
Tetrahedrization_display::_setup_gooch_texture_cb(void *data,
                                                  GLboolean test_proxy) {
  Tetrahedrization_display *display = (Tetrahedrization_display *) data;
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  assert(display->_gooch_image->width  % 2 == 0 &&
         display->_gooch_image->height % 2 == 0);
  if (test_proxy) {
    GLint proxy_texture_width = 0;
    glTexImage2D(GL_PROXY_TEXTURE_2D, 0, GL_LUMINANCE,
                 display->_gooch_image->width, display->_gooch_image->height,
                 0, display->_gooch_image->format, display->_gooch_image->type,
                 display->_gooch_image->pixels);
    glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                             &proxy_texture_width);
    if (proxy_texture_width == 0) {
      cerr << "Error: unsupported texture!" << endl;
      return GL_FALSE;
    } else {
      return GL_TRUE;
    }
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE,
                 display->_gooch_image->width, display->_gooch_image->height,
                 0, display->_gooch_image->format, display->_gooch_image->type,
                 display->_gooch_image->pixels);
    return GL_TRUE;
  }
}

GLboolean
Tetrahedrization_display::_display_enable_lighting_list_cb(
  void *data, GLboolean test_proxy) {
  const GLfloat mat_ambient[]   = {0.1f, 0.1f, 0.1f, 1.0f};
  const GLfloat mat_diffuse[]   = {0.7f, 0.7f, 0.7f, 1.0f};
  const GLfloat mat_specular[]  = {0.2f, 0.2f, 0.2f, 1.0f};
  const GLfloat mat_shininess[] = {5.0f};
  
  if (test_proxy) return GL_TRUE;
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  return GL_TRUE;
}

GLboolean
Tetrahedrization_display::_display_disable_lighting_list_cb(
  void *data, GLboolean test_proxy) {
  if (test_proxy) return GL_TRUE;
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  return GL_TRUE;
}

GLboolean
Tetrahedrization_display::_display_facets_with_facet_normals_list_cb(
  void *data, GLboolean test_proxy) {
  Tetrahedrization& tetrahedrization
    = ((Tetrahedrization_display *) data)->_tetrahedrization;
  
  if (test_proxy) return GL_TRUE;
  glBegin(GL_TRIANGLES);
  for (All_cells_iterator ci = tetrahedrization.all_cells_begin();
       ci != tetrahedrization.all_cells_end(); ci++) {
    for (int i = 0; i < 4; i++) {
      if (ci->is_surface_facet(i)) {
        Triangle triangle(tetrahedrization.triangle(ci, i));
        Vector v = triangle.supporting_plane().orthogonal_vector();
        glNormal3(normalize(v));
        glTriangle3(triangle);
      }
    }
  }
  glEnd();
  return GL_TRUE;
}

GLboolean
Tetrahedrization_display::_display_facet_normals_list_cb(
  void *data, GLboolean test_proxy) {
  Tetrahedrization_display *display = (Tetrahedrization_display *) data;
  Tetrahedrization& tetrahedrization = display->_tetrahedrization;
  
  if (test_proxy) return GL_TRUE;
  glBegin(GL_LINES);
  for (All_cells_iterator ci = tetrahedrization.all_cells_begin();
       ci != tetrahedrization.all_cells_end(); ci++) {
    for (int i = 0; i < 4; i++) {
      if (ci->is_surface_facet(i)) {
        Tetrahedrization::Triangle triangle(tetrahedrization.triangle(ci, i));
        Vector v1 = triangle.vertex(1) - CGAL::ORIGIN;
        Vector v2 = triangle.vertex(2) - CGAL::ORIGIN;
        Vector v3 = triangle.vertex(3) - CGAL::ORIGIN;
        Point p1 = CGAL::ORIGIN + (v1 + v2 + v3)/3;
        Vector n = triangle.supporting_plane().orthogonal_vector();
        Point p2 = p1 + display->_normal_display_scale * normalize(n);
        glPoint3(p1);
        glPoint3(p2);
      }
    }
  }
  glEnd();
  return GL_TRUE;
}

#if DEBUG
GLboolean
Tetrahedrization_display::_display_facets_with_vertex_normals_list_cb(
  void *data, GLboolean test_proxy) {
  Tetrahedrization_display *display = (Tetrahedrization_display *) data;
  Tetrahedrization& tetrahedrization = display->_tetrahedrization;
  
  if (test_proxy) return GL_TRUE;
  glBegin(GL_TRIANGLES);
  for (All_cells_iterator ci = tetrahedrization.all_cells_begin();
       ci != tetrahedrization.all_cells_end(); ci++) {
    for (int i = 0; i < 4; i++) {
      if (ci->is_surface_facet(i)) {
        Facet f(ci, i);
        int k = (f.second + 1)%4;
        for (int j = 0; j < 3; j++) {
          Vertex_handle vh = f.first->vertex(k);
          assert(!tetrahedrization.is_infinite(vh));
          glNormal3(display->_normals[vh]);
          glPoint3(vh->point());
          k = Tetrahedrization::next_around_edge(k, f.second);
        }
      }
    }
  }
  glEnd();
  return GL_TRUE;
}
#endif

#if DEBUG
GLboolean
Tetrahedrization_display::_display_vertex_normals_list_cb(
  void *data, GLboolean test_proxy) {
  Tetrahedrization_display *display = (Tetrahedrization_display *) data;
  Tetrahedrization& tetrahedrization = display->_tetrahedrization;
  
  if (test_proxy) return GL_TRUE;
  glBegin(GL_LINES);
  for (Finite_vertices_iterator vi = tetrahedrization.finite_vertices_begin();
       vi != tetrahedrization.finite_vertices_end(); vi++) {
    Point p1 = vi->point();
    Point p2 = p1 + display->_normal_display_scale * display->_normals[vi];
    glPoint3(p1);
    glPoint3(p2);
  }
  glEnd();
  return GL_TRUE;
}
#endif

GLboolean
Tetrahedrization_display::_display_gooch_list_cb(void *data,
                                                 GLboolean test_proxy) {
  Tetrahedrization_display *display = (Tetrahedrization_display *) data;
  
  if (test_proxy) return GL_TRUE;
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  gl_texture_bind(display->_gooch_texture);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_2D);
  return GL_TRUE;
}
