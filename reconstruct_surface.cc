#include "reconstruct_surface.hh"

#include "debug.hh"//

using namespace std;

/*
** Raphaelle Chaine, A geometric convection approach of 3D reconstruction,
** Proceedings of the Eurographics symposium on geometry processing, 2003.
*/

const unsigned int GRANULARITY_RATIO = 1;
// Rationale:
// since 2 * radius > granularity is the lower bound
// we can also write radius^2 > 0.25 * granularity^2
// thus, the current ratio is four times the minimum ratio (0.25 * 4 = 1)

Reconstruct_surface::Reconstruct_surface(Tetrahedrization& tetrahedrization)
  : _tetrahedrization(tetrahedrization),
    _convection_facets(),
    _surface_facets() {}

Reconstruct_surface::~Reconstruct_surface(void) {}

void
Reconstruct_surface::operator()(Finite_vertices_iterator vertices_begin,
                                Finite_vertices_iterator vertices_end) {
  if (_tetrahedrization.number_of_vertices() == 0) {
    cerr << "Warning: empty tetrahedrization!" << endl;
    return;
  }
  
  _tetrahedrization.set_granularity(vertices_begin, vertices_end);
  
  /* initialization with convex hull facets */
  Vertex_handle vh_infinite = _tetrahedrization.infinite_vertex();
  vector<Cell_handle> infinite_cells;
  _tetrahedrization.incident_cells(vh_infinite, back_inserter(infinite_cells));
  for (vector<Cell_handle>::iterator chi = infinite_cells.begin();
       chi != infinite_cells.end(); chi++) {
    for (int i = 0; i < 4; i++) {
      if (_tetrahedrization.is_infinite((*chi)->vertex(i))) {
        _convect(Facet(*chi, i));
      }
    }
  }
  
  /* convection */
  while (!_convection_facets.empty()) {
    Facet f_out(_convection_facets.front());
    _convection_facets.pop();
    
    if (f_out.first->is_convection_facet(f_out.second)) {
      f_out.first->is_convection_facet(f_out.second) = false;
      Facet f_in(f_out.first->neighbor(f_out.second),
                 f_out.first->mirror_index(f_out.second));
      for (int i = 1; i < 4; i++) {
        _convect(Facet(f_in.first, (f_in.second + i)%4));
      }
    }
  }
  
  /* initialization with thin part facets */
  queue<Facet>::size_type surface_facets_size_init = _surface_facets.size();
  for (unsigned int i = 0; i < surface_facets_size_init; i++) {
    Facet f_out(_surface_facets.front());
    _surface_facets.pop();
    Facet f_in(f_out.first->neighbor(f_out.second),
               f_out.first->mirror_index(f_out.second));
    
    if (f_out.first->is_surface_facet(f_out.second)) {
      if (f_in.first->is_surface_facet(f_in.second)) {
        _convection_facets.push(f_out);
      } else {
        _surface_facets.push(f_out);
      }
    }
  }
  
  /* thin part convection */
  queue<Facet>::size_type convection_facets_size_previous = 0;
  queue<Facet>::size_type convection_facets_size_current
    = _convection_facets.size();
  while (convection_facets_size_current != convection_facets_size_previous) {
    for (unsigned int i = 0; i < convection_facets_size_current; i++) {
      Facet f(_convection_facets.front());
      _convection_facets.pop();
      _convect_thin_part(f);
    }
    convection_facets_size_previous = convection_facets_size_current;
    convection_facets_size_current = _convection_facets.size();
  }
}

void
Reconstruct_surface::_convect(const Facet& f_out) {
  // vertices and inside half facet
  assert(!f_out.first->is_convection_facet(f_out.second));
  Vertex_handle vh1 = f_out.first->vertex((f_out.second + 1)%4);
  Vertex_handle vh2 = f_out.first->vertex((f_out.second + 2)%4);
  Vertex_handle vh3 = f_out.first->vertex((f_out.second + 3)%4);
  Facet f_in(f_out.first->neighbor(f_out.second),
             f_out.first->mirror_index(f_out.second));
  
  // outside facet properties
  Point p1(vh1->point()), p2(vh2->point()), p3(vh3->point());
  debug.out() << "p1 " << p1 << endl;//
  debug.out() << "p2 " << p2 << endl;//
  debug.out() << "p3 " << p3 << endl;//
  bool are_collinear = collinear(p1, p2, p3);//
  if (are_collinear) debug.out() << "Points are collinear" << endl;//
  else debug.out() << "Points are not collinear" << endl;//
  typedef Tetrahedrization::Geom_traits::Kernel::Vector_3 Vector;//
  Vector v21 = p1 - p2;//
  Vector v23 = p3 - p2;//
  normalize(v21);//
  normalize(v23);//
  debug.out() << "Dot product v21 * v23 " << v21 * v23 << endl;//
  //CAVEAT:
  //apparement, meme si les points sont detectes comme non collineaires,
  //cela fait quand meme probleme...
  //utiliser un test avec un epsilon ?
  //Expr: den != FT(0)
  //File: /usr/local/CGAL/include/CGAL/constructions/kernel_ftC3.h
  //Line: 129
  Sphere sphere(p1, p2, p3);
  //Sphere sphere(vh1->point(), vh2->point(), vh3->point());
  bool is_not_gabriel
    = sphere.has_on_bounded_side(f_in.first->vertex(f_in.second)->point());
  assert(vh1->granularity() != 0 &&
         vh2->granularity() != 0 &&
         vh3->granularity() != 0 );
  bool is_hiding_cavity
    = (sphere.squared_radius()
       > GRANULARITY_RATIO * min(vh1->granularity(), min(vh2->granularity(),
                                                         vh3->granularity())));
  bool is_convectable = (is_not_gabriel || is_hiding_cavity);
  bool is_resting_on_surface
    = f_in.first->is_surface_facet(f_in.second);
  bool is_intersecting_convection
    = f_in.first->is_convection_facet(f_in.second);
  
  if (!is_intersecting_convection && is_convectable) {
    // evolve with convection
    f_out.first->is_convection_facet(f_out.second) = true;
    _convection_facets.push(f_out);
  } else if (is_intersecting_convection &&
             (is_convectable || !is_resting_on_surface)) {
    // collapse convection locally
    f_in.first->is_convection_facet(f_in.second) = false;
    f_in.first->is_surface_facet(f_in.second) = false;
  } else {
    // belong to surface
    f_out.first->is_convection_facet(f_out.second) = true;
    f_out.first->is_surface_facet(f_out.second) = true;
    _surface_facets.push(f_out);
  }
}

void
Reconstruct_surface::_convect_thin_part(const Facet& f_out) {
  if (!f_out.first->is_surface_facet(f_out.second)) return;////indent
  Facet f_in(f_out.first->neighbor(f_out.second),
             f_out.first->mirror_index(f_out.second));
  
  int index[3];
  bool is_convectable = true;
  for (int i = 0; i < 3; i++) { // 3 facet edges
    index[0] = (f_out.second + 1 + i)%4;
    index[1] = Tetrahedrization::next_around_edge(index[0], f_out.second);
    index[2] = Tetrahedrization::next_around_edge(index[1], f_out.second);
    assert(!_tetrahedrization.is_infinite(f_out.first->vertex(index[2])));
    
    bool is_not_gabriel
      = (CGAL::angle(f_out.first->vertex(index[0])->point(),
                     f_out.first->vertex(index[2])->point(),
                     f_out.first->vertex(index[1])->point())
         == CGAL::OBTUSE);
    
    if (is_not_gabriel) {
      bool is_border_edge = true;
      Facet_circulator
        fc = _tetrahedrization.incident_facets(f_out.first, index[0], index[1],
                                               f_out.first, f_out.second),
        done(fc);
      while (++fc != done) { // do not take f_out into account
        Facet f(*fc);
        if (f.first->is_surface_facet(f.second) ||
            f.first->neighbor(f.second)->is_surface_facet(
              f.first->mirror_index(f.second))) {
          is_border_edge = false;
          break;
        }
      }
      if (is_border_edge) {
        f_out.first->is_surface_facet(f_out.second) = false;
        f_in.first->is_surface_facet(f_in.second) = false;
        is_convectable = false;
        break;
      }
    }
  }
  if (is_convectable) {
    _convection_facets.push(f_out);
  }
}
