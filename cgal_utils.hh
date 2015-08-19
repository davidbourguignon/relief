#ifndef __CGAL_UTILS_HH__
#define __CGAL_UTILS_HH__

#include <GL/gl.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Filtered_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_hierarchy_2.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>

typedef float NT;
typedef CGAL::Simple_cartesian<NT> C;
typedef CGAL::Filtered_kernel<C> Kernel;

inline void
glPoint2(const Kernel::Point_2& p) {
  glVertex2f(p[0], p[1]);
}

inline void
glSegment2(const Kernel::Segment_2& s) {
  glPoint2(s.source());
  glPoint2(s.target());
}

inline void
glTriangle2(const Kernel::Triangle_2& t) {
  glPoint2(t[0]);
  glPoint2(t[1]);
  glPoint2(t[2]);
}

inline void
glNormal3(const Kernel::Vector_3& v) {
  glNormal3f(v[0], v[1], v[2]);
}

inline void
glPoint3(const Kernel::Point_3& p) {
  glVertex3f(p[0], p[1], p[2]);
}

inline void
glSegment3(const Kernel::Segment_3& s) {
  glPoint3(s.source());
  glPoint3(s.target());
}

inline void
glTriangle3(const Kernel::Triangle_3& t) {
  glPoint3(t[0]);
  glPoint3(t[1]);
  glPoint3(t[2]);
}

template <typename V>
inline const V&
normalize(V& v) {
  v = v / std::sqrt(v*v);
  return v;
}

#endif // !__CGAL_UTILS_HH__
