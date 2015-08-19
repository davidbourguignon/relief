#ifndef __TESSELATION_HH__
#define __TESSELATION_HH__

#include "tesselation_base.hh"

class Tesselation : public Tesselation_base {
public:
  Tesselation(void);
  ~Tesselation(void);
  void display(void) const;
  float known_neighbor_ratio(Vertex_handle v);
  void propagate_depth(void);
  
private:
  typedef Tesselation_base Base;
  
  struct Compare_known_neighbors_ratio {
    bool operator()(Vertex_handle v1, Vertex_handle v2) const {
      return v1->known_neighbors_ratio() < v2->known_neighbors_ratio();
    }
  };
};

#endif // __TESSELATION_HH__
