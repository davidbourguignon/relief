#ifndef __TETRAHEDRIZATION_DISPLAY_HH__
#define __TETRAHEDRIZATION_DISPLAY_HH__

#include <opengl_utils.h>
#include <opengl_buffer.h>

#include "tetrahedrization.hh"

class Tetrahedrization_display {
public:
  typedef enum {
    POINTS,
    WIREFRAME,
    SOLID,
    FACET_NORMALS,
#if DEBUG
    VERTEX_NORMALS,
#endif
    NPR_GOOCH,
    NPR_RASKAR,
    DEPTH_BUFFER,
    ITEM_BUFFER_POINTS,
    ITEM_BUFFER_TRIANGLES
  } Style;
  
  Tetrahedrization_display(Tetrahedrization& tetrahedrization);
  ~Tetrahedrization_display(void);
  void push_style(Style style);
  void pop_style(void);
  GLfloat size(void) const;
  void display(void);
  void display_bbox(void);
  
private:
  typedef Tetrahedrization::Geom_traits::Kernel::Vector_3 Vector;
  typedef Tetrahedrization::Point Point;
  typedef Tetrahedrization::Triangle Triangle;
  typedef Tetrahedrization::Facet Facet;
  typedef Tetrahedrization::Vertex_handle Vertex_handle;
  typedef Tetrahedrization::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Tetrahedrization::All_cells_iterator All_cells_iterator;
  
  static GLboolean _setup_gooch_texture_cb(void *data, GLboolean test_proxy);
  static GLboolean _display_enable_lighting_list_cb(
    void *data, GLboolean test_proxy);
  static GLboolean _display_disable_lighting_list_cb(
    void *data, GLboolean test_proxy);
  static GLboolean _display_facets_with_facet_normals_list_cb(
    void *data, GLboolean test_proxy);
  static GLboolean _display_facet_normals_list_cb(
    void *data, GLboolean test_proxy);
  static GLboolean _display_gooch_list_cb(
    void *data, GLboolean test_proxy);
#if DEBUG
  static GLboolean _display_facets_with_vertex_normals_list_cb(
    void *data, GLboolean test_proxy);
  static GLboolean _display_vertex_normals_list_cb(
    void *data, GLboolean test_proxy);
#endif
  
  Tetrahedrization& _tetrahedrization;
  std::stack<Style> _style;
  GLframebuf *_gooch_image;
  GLtexture *_gooch_texture;
  GLlist *_enable_lighting_list, *_disable_lighting_list;
  GLlist *_facets_with_facet_normals_list, *_facet_normals_list;
  GLlist *_gooch_list;
#if DEBUG
  GLlist *_facets_with_vertex_normals_list, *_vertex_normals_list;
  CGAL::Unique_hash_map<Vertex_handle, Vector> _normals;
#endif
  GLfloat _size, _normal_display_scale;
  bool _is_first_npr_gooch_display;
};

#endif // __TETRAHEDRIZATION_DISPLAY_HH__
