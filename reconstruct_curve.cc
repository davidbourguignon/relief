#include "reconstruct_curve.hh"

using namespace std;

/*
** Inspired by:
** Raphaelle Chaine, A geometric convection approach of 3D reconstruction,
** Proceedings of the Eurographics symposium on geometry processing, 2003.
*/

const int GRANULARITY_RATIO = 1;
// Rationale:
// since 2 * radius > granularity is the lower bound
// we can also write radius^2 > 0.25 * granularity^2
// thus, the current ratio is four times the minimum ratio (0.25 * 4 = 1)

Reconstruct_curve::Reconstruct_curve(Triangulation& triangulation)
  : _triangulation(triangulation),
    _convection_edges(),
    _curve_edges() {}

Reconstruct_curve::~Reconstruct_curve(void) {}

void
Reconstruct_curve::operator()(vector<Vertex_handle>& vertices) {
  if (_triangulation.number_of_vertices() == 0) {
    cerr << "Warning: empty triangulation!" << endl;
    return;
  }
  
  /* initialization */
  vector<Edge> convex_hull_edges;
  if (distance(vertices.begin(), vertices.end())
        == _triangulation.number_of_vertices()) {
    // full reconstruction of the point set
    // assuming all faces of the triangulation have been reset
    _triangulation.set_granularity(vertices.begin(), vertices.end());
    Face_circulator
      fc = _triangulation.incident_faces(_triangulation.infinite_vertex()),
      done(fc);
    do {
      fc->is_outside() = true;
      for (int i = 0; i < 3; i++) {
        if (_triangulation.is_infinite(fc->vertex(i))) {
          convex_hull_edges.push_back(Edge(fc, i));
          _convect(convex_hull_edges.back());
        }
      }
    } while (++fc != done);
  } else {
#if 0
    // partial reconstruction of the point set
    set<Vertex_handle> adjacent_vertices(vertices.begin(), vertices.end());
    for (vector<Vertex_handle>::iterator vhi = vertices.begin();
         vhi != vertices.end(); vhi++) {
      Vertex_circulator vc = _triangulation.incident_vertices(*vhi), done(vc);
      do {
        adjacent_vertices.insert(vc);
      } while (++vc != done);
    }
    _triangulation.set_granularity(adjacent_vertices.begin(),
                                   adjacent_vertices.end());
    Vertex_handle infinite_vertex = _triangulation.infinite_vertex();
    set<Face_handle> incident_faces;
    for (vector<Vertex_handle>::iterator vhi = vertices.begin();
         vhi != vertices.end(); vhi++) {
      Face_circulator fc = _triangulation.incident_faces(*vhi), done(fc);
      do {
        if (fc->has_vertex(infinite_vertex)) {
          incident_faces.insert(fc);
        }
      } while (++fc != done);
    }
    for (set<Face_handle>::const_iterator fhi = incident_faces.begin();
         fhi != incident_faces.end(); fhi++) {
      for (int i = 0; i < 3; i++) {
        Face_handle fh(*fhi);
        if (_triangulation.is_infinite(fh->vertex(i))) {
          convex_hull_edges.push_back(Edge(fh, i));
          _convect(convex_hull_edges.back());
        }
      }
    }
#else
    assert(false);
#endif
  }
  
  /* convection */
  _start_convection();
  _stop_convection(convex_hull_edges);
}

void
Reconstruct_curve::operator()(set<Face_handle>& marked_faces) {
  if (_triangulation.number_of_vertices() == 0) {
    cerr << "Warning: empty triangulation!" << endl;
    return;
  }
  
  /* initialization */
  set<Edge> marked_faces_edges;
  for (set<Face_handle>::iterator fhi = marked_faces.begin();
       fhi != marked_faces.end(); fhi++) {
    Face_handle fh(*fhi);
    assert(!_triangulation.is_infinite(fh));
    fh->is_outside() = true;
    for (int i = 0; i < 3; i++) { // for each face edge
      if (marked_faces.find(fh->neighbor(i)) == marked_faces.end()) {
        // boundary edge
        pair<set<Edge>::iterator, bool> result
          = marked_faces_edges.insert(Edge(fh, i));
        assert(result.second);
        _convect(*result.first);
      }
    }
  }
  
  /* convection */
  _start_convection();
  _stop_convection(marked_faces_edges);
}

void
Reconstruct_curve::_convect(const Edge& e_out) {
  // vertices and inside half edge
  assert(!e_out.first->is_convection_edge(e_out.second));
  Vertex_handle vh1 = e_out.first->vertex((e_out.second + 1)%3);
  Vertex_handle vh2 = e_out.first->vertex((e_out.second + 2)%3);
  Edge e_in(e_out.first->neighbor(e_out.second),
            e_out.first->mirror_index(e_out.second));
  
  // outside edge properties
  Circle circle(vh1->point(), vh2->point());
  bool is_not_gabriel
    = circle.has_on_bounded_side(e_in.first->vertex(e_in.second)->point());
  assert(vh1->granularity() != 0 && vh2->granularity() != 0);
  bool is_hiding_cavity
    = (circle.squared_radius()
       > GRANULARITY_RATIO * min(vh1->granularity(), vh2->granularity()));
  bool is_convectable = (is_not_gabriel || is_hiding_cavity);
  bool is_resting_on_curve
    = e_in.first->is_curve_edge(e_in.second);
  bool is_intersecting_convection
    = e_in.first->is_convection_edge(e_in.second);
  
  if (!is_intersecting_convection && is_convectable) {
    // evolve with convection
    e_out.first->is_convection_edge(e_out.second) = true;
    _convection_edges.push(e_out);
  } else if (is_intersecting_convection &&
             (is_convectable || !is_resting_on_curve)) {
    // collapse convection locally
    e_in.first->is_convection_edge(e_in.second) = false;
    e_in.first->is_curve_edge(e_in.second) = false;
  } else {
    // belong to curve
    e_out.first->is_convection_edge(e_out.second) = true;
    e_out.first->is_curve_edge(e_out.second) = true;
    _curve_edges.push(e_out);
  }
}

void
Reconstruct_curve::_start_convection(void) {
  while (!_convection_edges.empty()) { // convection
    Edge e_out(_convection_edges.front());
    _convection_edges.pop();
    
    if (e_out.first->is_convection_edge(e_out.second)) {
      assert(e_out.first->is_outside());
      e_out.first->is_convection_edge(e_out.second) = false;
      Edge e_in(e_out.first->neighbor(e_out.second),
                e_out.first->mirror_index(e_out.second));
      e_in.first->is_outside() = true;
      for (int i = 1; i < 3; i++) {
        _convect(Edge(e_in.first, (e_in.second + i)%3));
      }
    }
  }
}

template <typename Edge_container>
void
Reconstruct_curve::_stop_convection(Edge_container& edges) {
  while (!_curve_edges.empty()) { // remove thin part edges
    Edge e_out(_curve_edges.front());
    _curve_edges.pop();
    Edge e_in(e_out.first->neighbor(e_out.second),
              e_out.first->mirror_index(e_out.second));
    
    if (e_out.first->is_curve_edge(e_out.second) &&
        e_in.first->is_curve_edge(e_in.second)) {
      e_out.first->is_convection_edge(e_out.second) = false;
      e_out.first->is_curve_edge(e_out.second) = false;
      e_in.first->is_convection_edge(e_in.second) = false;
      e_in.first->is_curve_edge(e_in.second) = false;
    }
  }
}

template void
Reconstruct_curve::_stop_convection< vector<Triangulation::Edge> >(
  vector<Triangulation::Edge>& edges);

template void
Reconstruct_curve::_stop_convection< set<Triangulation::Edge> >(
  set<Triangulation::Edge>& edges);
