#ifndef __TETRAHEDRIZATION_HH__
#define __TETRAHEDRIZATION_HH__

#include "tetrahedrization_base.hh"

class Application;

class Tetrahedrization : public Tetrahedrization_base {
public:
  Tetrahedrization(Application *application);
  ~Tetrahedrization(void);
  const CGAL::Bbox_3& bbox(void) const;
  void clear(void);
  int number_of_surface_vertices(void) const;
  int number_of_surface_facets(void) const;
  int number_of_inside_cells(void) const;
  template <typename Output_iterator>
  Output_iterator incident_surface_vertices(Vertex_handle v,
                                            Output_iterator vertices) const;
  // incident surface facets are ordered with the manifold facets first
  // the number of manifold surface facets is equal to n
  template <typename Output_iterator>
  Output_iterator incident_surface_facets(Vertex_handle v,
                                          Output_iterator facets,
                                          int& n) const;
  Geom_traits::Kernel::Vector_3 approximate_normal(Vertex_handle v) const;
  Vertex_handle insert_first(const Point& p,
                             Cell_handle start = Cell_handle(NULL));
  Vertex_handle insert(const Point& p, Cell_handle start = Cell_handle(NULL));
  Vertex_handle move_first(Vertex_handle v, const Point& p);
  Vertex_handle move(Vertex_handle v, const Point& p);
  Vertex_handle move_multipass_first(Vertex_handle v, const Point& p);
  Vertex_handle move_multipass(Vertex_handle v, const Point& p);
  void remove_first(Vertex_handle v);
  void remove(Vertex_handle v);
  void undo_last_changes(void);
  int set_granularity(Finite_vertices_iterator begin,
                      Finite_vertices_iterator end);
  
private:
  typedef Tetrahedrization_base Base;
  typedef Geom_traits::Kernel::Vector_3 Vector;
  typedef Geom_traits::FT FT;
  
  Application *_application;
  CGAL::Bbox_3 _bbox;
  std::vector<Point> _old_points;
  std::vector<Vertex_handle> _new_vertices;
};

#endif // __TETRAHEDRIZATION_HH__
