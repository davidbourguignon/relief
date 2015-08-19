#ifndef __TRIANGULATION_HH__
#define __TRIANGULATION_HH__

#include "triangulation_base.hh"

class Application;

class Triangulation : public Triangulation_base {
public:
  Triangulation(Application *application);
  ~Triangulation(void);
  const CGAL::Bbox_2& bbox(void) const;
  void clear(void);
  int number_of_curve_vertices(void) const;
  int number_of_curve_edges(void) const;
  int number_of_inside_faces(void) const;
  Vertex_handle insert_first(const Point& p,
                             Face_handle start = Face_handle(NULL));
  Vertex_handle insert(const Point& p, Face_handle start = Face_handle(NULL));
  template <typename Vertex_handle_iterator>
  int set_granularity(Vertex_handle_iterator begin,
                      Vertex_handle_iterator end);
  
private:
  typedef Triangulation_base Base;
  typedef Geom_traits::FT FT;
  
  Application *_application;
  CGAL::Bbox_2 _bbox;
};

#endif // __TRIANGULATION_HH__
