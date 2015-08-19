#ifndef __TETRAHEDRIZATION_IOSTREAM_HH__
#define __TETRAHEDRIZATION_IOSTREAM_HH__

#include "tetrahedrization.hh"

class Tetrahedrization_iostream {
public:
  typedef enum {
    DEFAULT,
    OFF
  } Format;
  
  Tetrahedrization_iostream(Tetrahedrization& tetrahedrization,
                            Format format = DEFAULT,
                            bool verbose = false);
  ~Tetrahedrization_iostream(void);
  friend std::istream& operator>>(std::istream& in,
                                  Tetrahedrization_iostream& tin);
  friend std::ostream& operator<<(std::ostream& out,
                                  const Tetrahedrization_iostream& tout);
  
private:
  typedef Tetrahedrization::Geom_traits::Kernel::FT FT;
  typedef Tetrahedrization::Point Point;
  typedef Tetrahedrization::Facet Facet;
  typedef Tetrahedrization::Vertex_handle Vertex_handle;
  typedef Tetrahedrization::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Tetrahedrization::All_cells_iterator All_cells_iterator;
  
  void _read(std::istream& in);
  void _read_off(std::istream& in);
  void _write(std::ostream& out) const;
  void _write_off(std::ostream& out) const;
  
  Tetrahedrization& _tetrahedrization;
  Format _format;
  bool _verbose;
};

#endif // __TETRAHEDRIZATION_IOSTREAM_HH__
