#ifndef __TRIANGULATION_BASE_HH__
#define __TRIANGULATION_BASE_HH__

#include "cgal_utils.hh"

template < typename K,
           typename V = CGAL::Triangulation_vertex_base_2<K> >
class Triangulation_vertex : public V {
private:
  typedef V Base;
  
public:
  typedef typename K::FT            FT;
  
  typedef typename V::Point         Point;
  typedef typename V::Vertex_handle Vertex_handle;
  typedef typename V::Face_handle   Face_handle;
  
  template <typename TDS2>
  struct Rebind_TDS {
    typedef typename V::template Rebind_TDS<TDS2>::Other V2;
    typedef Triangulation_vertex<K, V2>                  Other;
  };
  
  Triangulation_vertex(void)
    : Base(),
      _granularity(0) {}
  Triangulation_vertex(const Point& p)
    : Base(p),
      _granularity(0) {}
  Triangulation_vertex(Face_handle f)
    : Base(f),
      _granularity(0) {}
  Triangulation_vertex(const Point& p, Face_handle f)
    : Base(p, f),
      _granularity(0) {}
  FT granularity(void) const { return _granularity; }
  FT& granularity(void) { return _granularity; }
  
private:
  FT _granularity;
};

template < typename K,
           typename F = CGAL::Triangulation_face_base_2<K> >
class Triangulation_face : public F {
private:
  typedef F Base;
  
public:
  typedef typename F::Vertex_handle Vertex_handle;
  typedef typename F::Face_handle   Face_handle;
  
  template <typename TDS2>
  struct Rebind_TDS {
    typedef typename F::template Rebind_TDS<TDS2>::Other F2;
    typedef Triangulation_face<K, F2>                    Other;
  };
  
  Triangulation_face(void)
    : Base() { reset(); }
  Triangulation_face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2)
    : Base(v0, v1, v2) { reset(); }
  Triangulation_face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2,
                     Face_handle   n0, Face_handle   n1, Face_handle   n2)
    : Base(v0, v1, v2, n0, n1, n2) { reset(); }
  Triangulation_face(Vertex_handle v0, Vertex_handle v1, Vertex_handle v2,
                     Face_handle   n0, Face_handle   n1, Face_handle   n2,
                     bool          c0, bool          c1, bool          c2)
    : Base(v0, v1, v2, n0, n1, n2, c0, c1, c2) { reset(); }
  bool is_outside(void) const { return _is_outside; }
  bool is_convection_edge(unsigned int i) const {
    assert(i < 3);
    return _is_convection_edge[i];
  }
  bool is_curve_edge(unsigned int i) const {
    assert(i < 3);
    return _is_curve_edge[i];
  }
  bool& is_outside(void) { return _is_outside; }
  bool& is_convection_edge(unsigned int i) {
    assert(i < 3);
    return _is_convection_edge[i];
  }
  bool& is_curve_edge(unsigned int i) {
    assert(i < 3);
    return _is_curve_edge[i];
  }
  void reset(void) {
    _is_outside = false;
    for (int i = 0; i < 3; i++) {
      _is_convection_edge[i] = false;
      _is_curve_edge[i] = false;
    }
  }
  
private:
  bool _is_outside;
  bool _is_convection_edge[3];
  bool _is_curve_edge[3];
};

typedef Triangulation_vertex<Kernel> Vb2;
typedef CGAL::Triangulation_hierarchy_vertex_base_2<Vb2> Vbh2;
typedef Triangulation_face<Kernel> Fb2;
typedef CGAL::Triangulation_data_structure_2<Vbh2, Fb2> Tds2;
typedef CGAL::Delaunay_triangulation_2<Kernel, Tds2> Dt2;
typedef CGAL::Triangulation_hierarchy_2<Dt2> Triangulation_base;

#endif // __TRIANGULATION_BASE_HH__
