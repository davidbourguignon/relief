#ifndef __TETRAHEDRIZATION_BASE_HH__
#define __TETRAHEDRIZATION_BASE_HH__

#include "cgal_utils.hh"

template < typename K,
           typename V = CGAL::Triangulation_vertex_base_3<K> >
class Tetrahedrization_vertex : public V {
private:
  typedef V Base;
  
public:
  typedef typename K::FT            FT;
  
  typedef typename V::Point         Point;
  typedef typename V::Vertex_handle Vertex_handle;
  typedef typename V::Cell_handle   Cell_handle;
  
  template <typename TDS2>
  struct Rebind_TDS {
    typedef typename V::template Rebind_TDS<TDS2>::Other V2;
    typedef Tetrahedrization_vertex<K, V2>               Other;
  };
  
  Tetrahedrization_vertex(void)
    : Base(),
      _granularity(0) {}
  Tetrahedrization_vertex(const Point& p)
    : Base(p),
      _granularity(0) {}
  Tetrahedrization_vertex(Cell_handle c)
    : Base(c),
      _granularity(0) {}
  Tetrahedrization_vertex(const Point& p, Cell_handle c)
    : Base(p, c),
      _granularity(0) {}
  FT granularity(void) const { return _granularity; }
  FT& granularity(void) { return _granularity; }
  
private:
  FT _granularity;
};

template < typename K,
           typename C = CGAL::Triangulation_cell_base_3<K> >
class Tetrahedrization_cell : public C {
private:
  typedef C Base;
  
public:
  typedef typename C::Vertex_handle Vertex_handle;
  typedef typename C::Cell_handle   Cell_handle;
  
  template <typename TDS2>
  struct Rebind_TDS {
    typedef typename C::template Rebind_TDS<TDS2>::Other C2;
    typedef Tetrahedrization_cell<K, C2>                 Other;
  };
  
  Tetrahedrization_cell(void)
    : Base() { reset(); }
  Tetrahedrization_cell(Vertex_handle v0, Vertex_handle v1,
                        Vertex_handle v2, Vertex_handle v3)
    : Base(v0, v1, v2, v3) { reset(); }
  Tetrahedrization_cell(Vertex_handle v0, Vertex_handle v1,
                        Vertex_handle v2, Vertex_handle v3,
                        Cell_handle   n0, Cell_handle   n1,
                        Cell_handle   n2, Cell_handle   n3)
    : Base(v0, v1, v2, v3, n0, n1, n2, n3) { reset(); }
  bool is_outside(void) const { return _is_outside; }
  bool is_convection_facet(unsigned int i) const {
    assert(i < 4);
    return _is_convection_facet[i];
  }
  bool is_surface_facet(unsigned int i) const {
    assert(i < 4);
    return _is_surface_facet[i];
  }
  bool& is_outside(void) { return _is_outside; }
  bool& is_convection_facet(unsigned int i) {
    assert(i < 4);
    return _is_convection_facet[i];
  }
  bool& is_surface_facet(unsigned int i) {
    assert(i < 4);
    return _is_surface_facet[i];
  }
  void reset(void) {
    _is_outside = false;
    for (int i = 0; i < 4; i++) {
      _is_convection_facet[i] = false;
      _is_surface_facet[i] = false;
    }
  }
  
private:
  bool _is_outside;
  bool _is_convection_facet[4];
  bool _is_surface_facet[4];
};

typedef Tetrahedrization_vertex<Kernel> Vb3;
typedef CGAL::Triangulation_hierarchy_vertex_base_3<Vb3> Vbh3;
typedef Tetrahedrization_cell<Kernel> Cb3;
typedef CGAL::Triangulation_data_structure_3<Vbh3, Cb3> Tds3;
typedef CGAL::Delaunay_triangulation_3<Kernel, Tds3> Dt3;
typedef CGAL::Triangulation_hierarchy_3<Dt3> Tetrahedrization_base;

#endif // __TETRAHEDRIZATION_BASE_HH__
