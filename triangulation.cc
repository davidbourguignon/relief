#include "application.hh"
#include "triangulation.hh"

using namespace std;

static const CGAL::Bbox_2 BBOX_NULL = CGAL::Bbox_2(0.0, 0.0, 0.0, 0.0);
static const unsigned int NEAREST_NEIGHBOR_RANK = 2;
// Rationale:
// the mean number of neighbors in a 2D triangulation is 6 (see Chaine, 2003)

Triangulation::Triangulation(Application *application) : Base() {
  _application = application;
  _bbox = BBOX_NULL;
}

Triangulation::~Triangulation(void) {}

const CGAL::Bbox_2&
Triangulation::bbox(void) const {
  return _bbox;
}

void
Triangulation::clear(void) {
  _bbox = BBOX_NULL;
  Base::clear();
}

int
Triangulation::number_of_curve_vertices(void) const {
  if (dimension() < 2) {
    return 0;
  } else {
    int n = 0;
    for (Finite_vertices_iterator vi = finite_vertices_begin();
         vi != finite_vertices_end(); vi++) {
      Edge_circulator ec = incident_edges(vi), done(ec);
      assert(ec != NULL);
      do {
        Edge e_out(*ec);
        Edge e_in(e_out.first->neighbor(e_out.second),
                  e_out.first->mirror_index(e_out.second));
        if (e_out.first->is_curve_edge(e_out.second) ||
            e_in.first->is_curve_edge(e_in.second)) {
          n++;
          break;
        }
      } while (++ec != done);
    }
    return n;
  }
}

int
Triangulation::number_of_curve_edges(void) const {
  if (dimension() < 2) {
    return 0;
  } else {
    int n = 0;
    for (All_faces_iterator fi = all_faces_begin();
         fi != all_faces_end(); fi++) {
      for (int i = 0; i < 3; i++) {
        if (fi->is_curve_edge(i)) {
          n++;
        }
      }
    }
    return n;
  }
}

int
Triangulation::number_of_inside_faces(void) const {
  int n = 0;
  for (Finite_faces_iterator fi = finite_faces_begin();
       fi != finite_faces_end(); fi++) {
    if (!fi->is_outside()) {
      n++;
    }
  }
  return n;
}

Triangulation::Vertex_handle
Triangulation::insert_first(const Point& p, Face_handle start) {
  if (number_of_vertices() == 0) {
    _bbox = p.bbox();
  }
  return insert(p, start);
}

Triangulation::Vertex_handle
Triangulation::insert(const Point& p, Face_handle start) {
  _bbox = _bbox + p.bbox();
  return Base::insert(p, start);
}

template <typename Vertex_handle_iterator>
int
Triangulation::set_granularity(Vertex_handle_iterator begin,
                               Vertex_handle_iterator end) {
  multiset<FT> squared_distances;
  int n = 0;
  
  for (Vertex_handle_iterator vhi = begin; vhi != end; vhi++) {
    Vertex_handle vh(*vhi);
    Vertex_circulator vc = incident_vertices(vh), done(vc);
    do {
      if (!is_infinite(vc)) {
        squared_distances.insert(CGAL::squared_distance(vh->point(),
                                                        vc->point()));
      }
    } while (++vc != done);
    if (squared_distances.size() < NEAREST_NEIGHBOR_RANK) {
      assert(!squared_distances.empty());
      vh->granularity() = *(squared_distances.rbegin());
    } else {
      multiset<FT>::const_iterator fti = squared_distances.begin();
      advance(fti, NEAREST_NEIGHBOR_RANK - 1);
      vh->granularity() = *fti;
    }
    squared_distances.clear();
    n++;
  }
  return n;
}

template int
Triangulation::set_granularity<vector<Triangulation::Vertex_handle>::iterator>(
  vector<Triangulation::Vertex_handle>::iterator begin,
  vector<Triangulation::Vertex_handle>::iterator end);

template int
Triangulation::set_granularity<set<Triangulation::Vertex_handle>::iterator>(
  set<Triangulation::Vertex_handle>::iterator begin,
  set<Triangulation::Vertex_handle>::iterator end);
