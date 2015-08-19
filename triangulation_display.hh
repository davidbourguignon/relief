#ifndef __TRIANGULATION_DISPLAY_HH__
#define __TRIANGULATION_DISPLAY_HH__

#include "triangulation.hh"

class Triangulation_display {
public:
  typedef enum {
    STANDARD,
    THIN_MASK,
    THICK_MASK,
    ITEM_BUFFER_POINTS
  } Style;
  
  Triangulation_display(Triangulation& triangulation);
  ~Triangulation_display(void);
  void push_style(Style style);
  void pop_style(void);
  void display(void);
  void display_bbox(void);
  
private:
  typedef Triangulation::Segment Segment;
  typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Triangulation::Finite_edges_iterator Finite_edges_iterator;
  typedef Triangulation::Finite_faces_iterator Finite_faces_iterator;
  typedef Triangulation::Point_iterator Point_iterator;
  typedef Triangulation::All_faces_iterator All_faces_iterator;
  
  Triangulation& _triangulation;
  std::stack<Style> _style;
};

#endif // __TRIANGULATION_DISPLAY_HH__
