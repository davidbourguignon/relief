#ifndef __TESSELATION_BASE_HH__
#define __TESSELATION_BASE_HH__

#include <opengl_utils.h>

#include "tetrahedrization.hh"

template < typename K,
           typename V = CGAL::Triangulation_vertex_base_2<K> >
class Tesselation_vertex : public V {
private:
  typedef V Base;
  
public:
  typedef typename K::Vector_3      Vector_3;
  
  typedef typename V::Vertex_handle Vertex_handle;
  typedef typename V::Face_handle   Face_handle;
  typedef typename V::Point         Point;
  
  typedef Tetrahedrization::Vertex_handle Vertex_handle_3;
  
  template <typename TDS2>
  struct Rebind_TDS {
    typedef typename V::template Rebind_TDS<TDS2>::Other V2;
    typedef Tesselation_vertex<K, V2>                    Other;
  };
  
  Tesselation_vertex(void)
    : Base(),
      _depth(GL_DEPTH_FAR),
      _offset(0u),
      _normal(CGAL::NULL_VECTOR),
      _projected_vertex(NULL),
      _known_neighbors_ratio(1.0f) {}
  Tesselation_vertex(const Point& p)
    : Base(p),
      _depth(GL_DEPTH_FAR),
      _offset(0u),
      _normal(CGAL::NULL_VECTOR),
      _projected_vertex(NULL),
      _known_neighbors_ratio(1.0f) {}
  Tesselation_vertex(Face_handle f)
    : Base(f),
      _depth(GL_DEPTH_FAR),
      _offset(0u),
      _normal(CGAL::NULL_VECTOR),
      _projected_vertex(NULL),
      _known_neighbors_ratio(1.0f) {}
  Tesselation_vertex(const Point& p, Face_handle f)
    : Base(p, f),
      _depth(GL_DEPTH_FAR),
      _offset(0u),
      _normal(CGAL::NULL_VECTOR),
      _projected_vertex(NULL),
      _known_neighbors_ratio(1.0f) {}
  GLfloat depth(void) const { return _depth; }
  GLubyte offsetub(void) const { return _offset; }
  GLfloat offsetf(void) const {
    const GLubyte MIDDLE_INF = 127u;
    const GLubyte MIDDLE_SUP = 128u;
    if (_offset == MIDDLE_INF || _offset == MIDDLE_SUP) {
      return 0.0f;
    } else {
      return (2.0f * ((GLfloat) _offset / 255.0f) - 1.0f);
    }
  }
  const Vector_3& normal(void) const { return _normal; }
  Vertex_handle_3 projected_vertex(void) const { return _projected_vertex; }
  float known_neighbors_ratio(void) const { return _known_neighbors_ratio; }
  GLfloat& depth(void) { return _depth; }
  GLubyte& offsetub(void) { return _offset; }
  Vector_3& normal(void) { return _normal; }
  Vertex_handle_3& projected_vertex(void) { return _projected_vertex; }
  float& known_neighbors_ratio(void) { return _known_neighbors_ratio; }
  
private:
  GLfloat _depth;
  GLubyte _offset;
  Vector_3 _normal;
  Vertex_handle_3 _projected_vertex;
  float _known_neighbors_ratio;
};

typedef Tesselation_vertex<Kernel> Vb;
typedef CGAL::Triangulation_hierarchy_vertex_base_2<Vb> Vbh;
typedef CGAL::Triangulation_data_structure_2<Vbh> Tds;
typedef CGAL::Delaunay_triangulation_2<Kernel, Tds> Dt;
typedef CGAL::Triangulation_hierarchy_2<Dt> Tesselation_base;

#endif // __TESSELATION_BASE_HH__
