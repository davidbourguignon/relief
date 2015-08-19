#ifndef __MESHING_HH__
#define __MESHING_HH__

#include <opengl_buffer.h>

//#include <hash_map>
//#include <hash_set>

#include "file.hh"
#include "tetrahedrization_display.hh"
#include "tesselation.hh"

class Application;

class Meshing {
public:
  Meshing(Application *application);
  ~Meshing(void);
  bool has_changed(void) const;
  bool is_at_depth(void) const;
  bool& is_at_depth(void);
  bool init(void);
  bool read(std::ifstream& fin, File::Type file_type);
  bool write(std::ofstream& fout, File::Type file_type) const;
  void start_meshing(const GLveci drawbox);
  void stop_meshing(void);
  void mesh(GLtransf *ortho, GLtransf *persp);
  void unmesh(void);
  void display_tetrahedrization(Tetrahedrization_display::Style style,
                                bool display_bbox);
  
private:
  typedef Tetrahedrization::Geom_traits::Kernel::Vector_3 Vector;
  typedef Tetrahedrization::Geom_traits::FT FT;
  typedef Tetrahedrization::Point Point;
  typedef Tetrahedrization::Facet Facet;
  typedef Tetrahedrization::Vertex_handle Vertex_handle;
  typedef Tetrahedrization::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Tetrahedrization::All_cells_iterator All_cells_iterator;
  
  static GLboolean _display_list_cb(void *data, GLboolean test_proxy);
  Tetrahedrization_display *_tetrahedrization_display(void);
  void _remove(Tetrahedrization_display *tetrahedrization_display);
  void _get_item_buffers(void);
  void _smooth_or_remove_tetrahedrization_points(void);
  void _evaluate_tesselation_error(void);
  void _insert_new_triangulation_points_in_tesselation(
    const Vector& normal, const Point& eye);
  void _insert_new_triangulation_points_in_tesselation_at_depth(
    const Vector& normal, const Point& eye);
  void _unproject_new_triangulation_points(void);
  void _get_stencil_depth_and_item_buffers(void);
  void _render_tesselation_with_tetrahedrization_points(const Point& eye,
                                                        Point& center);
  void _insert_new_tetrahedrization_points_in_tesselation(
    const Vector& normal, const Point& eye, const Point& center);
  void _unproject_new_tetrahedrization_points(void);
  void _read_stencil_buffer(void);
  void _clear_color_buffer(void);
  void _reconstruct_surface(void);
  void _set_height_field_offset_scale(double height_field_max, GLdouble depth);
  
  Application *_application;
  Tetrahedrization *_tetrahedrization_proxy;
  Tetrahedrization_display *_tetrahedrization_display_ptr;
  GLveci _drawbox, _drawport;
  GLframebuf *_stencilbuf, *_depthbuf, *_colorbuf, *_errorbuf;
  GLitembuf *_itembuf;
  GLlist *_list;
  Tesselation _tesselation;
  GLtransf *_transf_ortho, *_transf_persp;
  std::vector<Vertex_handle> _smoothed_vertices, _removed_vertices;
  std::map<guint32, Vertex_handle> _visible_vertices;
  std::map<guint32, Facet> _visible_facets;
  std::set< std::pair<int, int> > _integer_positions;
  bool _has_changed, _is_at_depth;
  GLfloat _offset_scale;
};

#endif // __MESHING_HH__
