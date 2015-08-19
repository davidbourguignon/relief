#include "application.hh"
#include "tetrahedrization.hh"

using namespace std;

static const CGAL::Bbox_3 BBOX_NULL = CGAL::Bbox_3(0.0, 0.0, 0.0,
                                                   0.0, 0.0, 0.0);
const unsigned int NEAREST_NEIGHBOR_RANK = 5;
// Rationale:
// the mean number of neighbors in a 3D triangulation is between 12 and 18
// in our experiments, we observed values around 14.5
// thus, to keep the same ratio as in 2D, we should choose 4.83.. ~ 5

Tetrahedrization::Tetrahedrization(Application *application)
  : Base(),
    _application(application),
    _bbox(BBOX_NULL),
    _old_points(),
    _new_vertices() {}

Tetrahedrization::~Tetrahedrization(void) {}

const CGAL::Bbox_3&
Tetrahedrization::bbox(void) const {
  return _bbox;
}

void
Tetrahedrization::clear(void) {
  _bbox = BBOX_NULL;
  Base::clear();
}

int
Tetrahedrization::number_of_surface_vertices(void) const {
  if (dimension() < 3) {
    return number_of_vertices();
  } else {
    int n = 0;
    vector<Cell_handle> cells;
    for (Finite_vertices_iterator vi = finite_vertices_begin();
         vi != finite_vertices_end(); vi++) {
      incident_cells(vi, back_inserter(cells));
      bool is_surface_vertex = false;
      for (vector<Cell_handle>::const_iterator chi = cells.begin();
           chi != cells.end() && !is_surface_vertex; chi++) {
        Cell_handle ch(*chi);
        int vertex_index = ch->index(vi);
        for (int i = 0; i < 4 && !is_surface_vertex; i++) {
          if (ch->is_surface_facet(i) && i != vertex_index) {
            is_surface_vertex = true;
            n++;
          }
        }
      }
      cells.clear();
    }
    return n;
  }
}

int
Tetrahedrization::number_of_surface_facets(void) const {
  if (dimension() < 3) {
    return number_of_finite_facets();
  } else {
    int n = 0;
    for (All_cells_iterator ci = all_cells_begin();
         ci != all_cells_end(); ci++) {
      for (int i = 0; i < 4; i++) {
        if (ci->is_surface_facet(i)) {
          n++;
        }
      }
    }
    return n;
  }
}

int
Tetrahedrization::number_of_inside_cells(void) const {
  int n = 0;
  for (Finite_cells_iterator ci = finite_cells_begin();
       ci != finite_cells_end(); ci++) {
    if (!ci->is_outside()) {
      n++;
    }
  }
  return n;
}

template <typename Output_iterator>
Output_iterator
Tetrahedrization::incident_surface_vertices(Vertex_handle v,
                                            Output_iterator vertices) const {
  if (dimension() < 3) {
    list<Vertex_handle> ivertices;
    incident_vertices(v, back_inserter(ivertices));
    for (list<Vertex_handle>::iterator vhi = ivertices.begin();
         vhi != ivertices.end(); vhi++) {
      if (is_infinite(*vhi)) {
        ivertices.erase(vhi);
      }
    }
    return copy(ivertices.begin(), ivertices.end(), vertices);
  } else {
    static vector<Cell_handle> icells;
    set<Vertex_handle> ivertices;
    incident_cells(v, back_inserter(icells));
    for (vector<Cell_handle>::const_iterator chi = icells.begin();
         chi != icells.end(); chi++) {
      Cell_handle ch(*chi);
      int vertex_index = ch->index(v);
      for (int i = 0; i < 4; i++) {
        if (ch->is_surface_facet(i) && i != vertex_index) {
          Facet f(ch, i);
          Vertex_handle vh1
            = f.first->vertex(next_around_edge(vertex_index, f.second));
          Vertex_handle vh2
            = f.first->vertex(next_around_edge(f.second, vertex_index));
          assert(!(is_infinite(vh1) || is_infinite(vh2)));
          ivertices.insert(vh1);
          ivertices.insert(vh2);
        }
      }
    }
    icells.clear();
    return copy(ivertices.begin(), ivertices.end(), vertices);
  }
}

template back_insert_iterator< vector<Tetrahedrization::Vertex_handle> >
Tetrahedrization::incident_surface_vertices<
  back_insert_iterator< vector<Tetrahedrization::Vertex_handle> > >(
  Vertex_handle v,
  back_insert_iterator< vector<Tetrahedrization::Vertex_handle> > vertices)
  const;

template <typename Output_iterator>
Output_iterator
Tetrahedrization::incident_surface_facets(Vertex_handle v,
                                          Output_iterator facets,
                                          int& n) const {
  if (dimension() < 2) {
    n = 0;
    return facets;
  } else if (dimension() < 3) {
    static vector<Facet> ifacets;
    for (Finite_facets_iterator fi = finite_facets_begin();
         fi != finite_facets_end(); fi++) {
      Facet f(*fi);
      if (f.first->is_surface_facet(f.second) && has_vertex(f, v)) {
        ifacets.push_back(f);
      }
    }
    Output_iterator oit = copy(ifacets.begin(), ifacets.end(), facets);
    n = ifacets.size();
    ifacets.clear();
    return oit;
  } else {
    static vector<Cell_handle> icells;
    static deque<Facet> ifacets;
    set<Facet> ifacets_unsorted;
    incident_cells(v, back_inserter(icells));
    for (vector<Cell_handle>::const_iterator chi = icells.begin();
         chi != icells.end(); chi++) {
      Cell_handle ch(*chi);
      int vertex_index = ch->index(v);
      for (int i = 0; i < 4; i++) {
        if (ch->is_surface_facet(i) && i != vertex_index) {
          Facet f(ch, i);
          assert(!is_infinite(f));
          ifacets_unsorted.insert(f);
        }
      }
    }
    icells.clear();
    n = 0;
    for (set<Facet>::iterator fi = ifacets_unsorted.begin();
         fi != ifacets_unsorted.end(); fi++) {
      Facet f_out(*fi);
      Facet f_in(f_out.first->neighbor(f_out.second),
                 f_out.first->mirror_index(f_out.second));
      set<Facet>::iterator fi_found = ifacets_unsorted.find(f_in);
      if (fi_found == ifacets_unsorted.end()) {
        // manifold facet
        ifacets.push_front(f_out);
        n++;
      } else {
        // non manifold facet
        ifacets.push_back(f_out);
        ifacets.push_back(f_in);
        ifacets_unsorted.erase(fi_found);
      }
    }
    assert(n == 0 || (n%2 == 0 && ifacets.size()%2 == 0) ||
                     (n%2 != 0 && ifacets.size()%2 != 0));
    Output_iterator oit = copy(ifacets.begin(), ifacets.end(), facets);
    ifacets.clear();
    return oit;
  }
}

template back_insert_iterator< vector<Tetrahedrization::Facet> >
Tetrahedrization::incident_surface_facets<
  back_insert_iterator< vector<Tetrahedrization::Facet> > >(
  Vertex_handle v,
  back_insert_iterator< vector<Tetrahedrization::Facet> > facets, int& n)
  const;

Tetrahedrization::Vector
Tetrahedrization::approximate_normal(Vertex_handle v) const {
  if (dimension() < 3) {
    return Vector(CGAL::NULL_VECTOR);
  } else {
    /*
     * We approximate the normal at a vertex as the mean of the normals of
     * its incident surface facets. A better approximation is given in the
     * following reference.
     *
     * Nelson Max, Weights for Computing Vertex Normals from Facet Normals,
     * Journal of Graphics Tools, 4(2):1-6, 1999.
     */
    static vector<Facet> facets;
    int number_of_manifold_facets;
    incident_surface_facets(v, back_inserter(facets),
                            number_of_manifold_facets);
    Vector n(CGAL::NULL_VECTOR);
    if (facets.empty()) {
      cerr << "Warning: no incident surface facets!" << endl;
    } else {
      Vector normal_manifold_plus(CGAL::NULL_VECTOR);
      Vector normal_manifold_minus(CGAL::NULL_VECTOR);
      Vector normal_non_manifold(CGAL::NULL_VECTOR);
      int number_of_manifold_facets_plus = 0;
      int number_of_manifold_facets_minus = 0;
      Vector normal_manifold_first
        = triangle(*facets.begin()).supporting_plane().orthogonal_vector();
      int i = 0;
      for (vector<Facet>::const_iterator fi = facets.begin();
           fi != facets.end(); fi++, i++) {
        Vector normal_current
          = triangle(*fi).supporting_plane().orthogonal_vector();
        if (i < number_of_manifold_facets) {
          // manifold facets
          if (normal_current * normal_manifold_first > 0) {
            normal_manifold_plus = normal_manifold_plus + normal_current;
            number_of_manifold_facets_plus++;
          } else {
            normal_manifold_minus = normal_manifold_minus + normal_current;
            number_of_manifold_facets_minus++;
          }
        } else {
          // pairs of non manifold facets
          if (normal_current * normal_manifold_first > 0) {
            normal_non_manifold = normal_non_manifold + normal_current;
          } else {
            normal_non_manifold = normal_non_manifold - normal_current;
          }
          fi++;
          i++;
        }
      }
      assert(number_of_manifold_facets_plus + number_of_manifold_facets_minus
               == number_of_manifold_facets);
#if DEBUG
      if (number_of_manifold_facets_minus > 0) {
        cerr << "Warning: vertex is singular! "
             << "(Inconsistent manifold facets.)" << endl;
      }
#endif
      if (number_of_manifold_facets_minus > number_of_manifold_facets_plus) {
        n = normal_manifold_minus - normal_non_manifold;
      } else {
        n = normal_manifold_plus + normal_non_manifold;
      }
      facets.clear();
      normalize(n);
    }
    return n;
  }
}

Tetrahedrization::Vertex_handle
Tetrahedrization::insert_first(const Point& p, Cell_handle start) {
  if (number_of_vertices() == 0) {
    _bbox = p.bbox();
  }
  _old_points.clear();
  _new_vertices.clear();
  return insert(p, start);
}

Tetrahedrization::Vertex_handle
Tetrahedrization::insert(const Point& p, Cell_handle start) {
  _bbox = _bbox + p.bbox();
  Vertex_handle vh(Base::insert(p, start));
  _new_vertices.push_back(vh);
  return vh;
}

Tetrahedrization::Vertex_handle
Tetrahedrization::move_first(Vertex_handle v, const Point& p) {
  assert(number_of_vertices() != 0);
  _old_points.clear();
  _new_vertices.clear();
  remove(v);
  return insert(p);
}

Tetrahedrization::Vertex_handle
Tetrahedrization::move(Vertex_handle v, const Point& p) {
  remove(v);
  return insert(p);
}

Tetrahedrization::Vertex_handle
Tetrahedrization::move_multipass_first(Vertex_handle v, const Point& p) {
  assert(number_of_vertices() != 0);
  _new_vertices.clear();
  Base::remove(v);
  return insert(p);
}

Tetrahedrization::Vertex_handle
Tetrahedrization::move_multipass(Vertex_handle v, const Point& p) {
  Base::remove(v);
  return insert(p);
}

void
Tetrahedrization::remove_first(Vertex_handle v) {
  //CAVEAT: the bbox is not updated!
  _old_points.clear();
  _new_vertices.clear();
  remove(v);
}

void
Tetrahedrization::remove(Vertex_handle v) {
  //CAVEAT: the bbox is not updated!
  _old_points.push_back(v->point());
  Base::remove(v);
}

void
Tetrahedrization::undo_last_changes(void) {
  for (vector<Vertex_handle>::const_iterator vhi = _new_vertices.begin();
       vhi != _new_vertices.end(); vhi++) {
    Base::remove(*vhi);
  }
  _new_vertices.clear();
  for (vector<Point>::const_iterator pi = _old_points.begin();
       pi != _old_points.end(); pi++) {
    Base::insert(*pi);
  }
  _old_points.clear();
}

int
Tetrahedrization::set_granularity(Finite_vertices_iterator begin,
                                  Finite_vertices_iterator end) {
  vector<Vertex_handle> vertices;
  multiset<FT> squared_distances;
  int n = 0;
  
  for (Finite_vertices_iterator vi = begin; vi != end; vi++) {
    incident_vertices(vi, back_inserter(vertices));
    for (vector<Vertex_handle>::const_iterator vhi = vertices.begin();
         vhi != vertices.end(); vhi++) {
      Vertex_handle vh(*vhi);
      if (!is_infinite(vh)) {
        squared_distances.insert(CGAL::squared_distance(vi->point(),
                                                        vh->point()));
      }
    }
    if (squared_distances.size() < NEAREST_NEIGHBOR_RANK) {
      assert(!squared_distances.empty());
      vi->granularity() = *(squared_distances.rbegin());
    } else {
      multiset<FT>::const_iterator fti = squared_distances.begin();
      advance(fti, NEAREST_NEIGHBOR_RANK - 1);
      vi->granularity() = *fti;
    }
    squared_distances.clear();
    vertices.clear();
    n++;
  }
  return n;
}
