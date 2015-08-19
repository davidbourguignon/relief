#ifndef __RECONSTRUCT_SURFACE_HH__
#define __RECONSTRUCT_SURFACE_HH__

#include <queue>

#include "tetrahedrization.hh"

class Reconstruct_surface {
public:
  Reconstruct_surface(Tetrahedrization& tetrahedrization);
  ~Reconstruct_surface(void);
  void operator()(Tetrahedrization::Finite_vertices_iterator vertices_begin,
                  Tetrahedrization::Finite_vertices_iterator vertices_end);
  
private:
  typedef Tetrahedrization::Geom_traits::Kernel::Sphere_3 Sphere;
  typedef Tetrahedrization::Point Point;
  typedef Tetrahedrization::Facet Facet;
  typedef Tetrahedrization::Vertex_handle Vertex_handle;
  typedef Tetrahedrization::Cell_handle Cell_handle;
  typedef Tetrahedrization::Facet_circulator Facet_circulator;
  typedef Tetrahedrization::Finite_vertices_iterator Finite_vertices_iterator;
  
  void _convect(const Facet& f_out);
  void _convect_thin_part(const Facet& f_out);
  
  Tetrahedrization& _tetrahedrization;
  std::queue<Facet> _convection_facets;
  std::queue<Facet> _surface_facets;
};

#endif // __RECONSTRUCT_SURFACE_HH__
