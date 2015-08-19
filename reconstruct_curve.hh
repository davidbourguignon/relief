#ifndef __RECONSTRUCT_CURVE_HH__
#define __RECONSTRUCT_CURVE_HH__

#include <queue>

#include "triangulation.hh"

class Reconstruct_curve {
public:
  Reconstruct_curve(Triangulation& triangulation);
  ~Reconstruct_curve(void);
  void operator()(std::vector<Triangulation::Vertex_handle>& vertices);
  void operator()(std::set<Triangulation::Face_handle>& marked_faces);
  
private:
  typedef Triangulation::Geom_traits::Kernel::Circle_2 Circle;
  typedef Triangulation::Edge Edge;
  typedef Triangulation::Vertex_handle Vertex_handle;
  typedef Triangulation::Face_handle Face_handle;
  typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Triangulation::Vertex_circulator Vertex_circulator;
  typedef Triangulation::Face_circulator Face_circulator;
  
  void _convect(const Edge& e_out);
  void _start_convection(void);
  template <typename Edge_container>
  void _stop_convection(Edge_container& edges);
  
  Triangulation& _triangulation;
  std::queue<Edge> _convection_edges;
  std::queue<Edge> _curve_edges;
};

#endif // __RECONSTRUCT_CURVE_HH__
