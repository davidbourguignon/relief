#ifndef __SMOOTH_SURFACE_HH__
#define __SMOOTH_SURFACE_HH__

#include "tetrahedrization.hh"

class Smooth_surface {
public:
  typedef enum {
    SIMPLE_AVERAGE,
    TAUBIN
  } Type;
  
  Smooth_surface(Tetrahedrization& tetrahedrization, Type type = TAUBIN);
  ~Smooth_surface(void);
  template <typename Vertex_handle_iterator>
  void operator()(Vertex_handle_iterator begin, Vertex_handle_iterator end);
  
private:
  typedef Tetrahedrization::Geom_traits::Kernel::Vector_3 Vector;
  typedef Tetrahedrization::Point Point;
  typedef Tetrahedrization::Vertex_handle Vertex_handle;
  
  typedef std::vector< CGAL::Triple<Vertex_handle,
                                    std::vector<Vertex_handle>,
                                    Point> > _Neighborhoods;
  
  void _simple_average_step(void);
  void _taubin_step(double scale_factor);
  void _update(void);
  
  Tetrahedrization& _tetrahedrization;
  Type _type;
  double _lambda, _mu, _k_pb; // Taubin
  _Neighborhoods _neighborhoods;
  bool _is_first_update;
};

#endif // __SMOOTH_SURFACE_HH__
