#include <glib.h>
#include <opengl_utils.h>
#include <opengl_widget.h>

#include "triangulation_display.hh"

using namespace std;

Triangulation_display::Triangulation_display(Triangulation& triangulation)
  : _triangulation(triangulation),
    _style() {
  _style.push(STANDARD);
}

Triangulation_display::~Triangulation_display(void) {}

void
Triangulation_display::push_style(Style style) {
  _style.push(style);
}

void
Triangulation_display::pop_style(void) {
  _style.pop();
}

void
Triangulation_display::display(void) {
  if (_triangulation.number_of_vertices() == 0) return;
  assert(!_style.empty());
  
  switch (_style.top()) {
  case STANDARD: {
    /* Draw faces */
    glPushAttrib(GL_CURRENT_BIT);
    glBegin(GL_TRIANGLES);
    for (Finite_faces_iterator fi = _triangulation.finite_faces_begin();
         fi != _triangulation.finite_faces_end(); fi++) {
      if (fi->is_outside()) {
        glColor4fv(GL_PURE_WHITE);
      } else {
        glColor3ub(255, 255, 215); // R. Chaine colors
      }
      glTriangle2(_triangulation.triangle(fi));
    }
    glEnd();
    glPopAttrib();
    
    /* Draw edges */
    glPushAttrib(GL_CURRENT_BIT);
    glColor3ub(233, 202, 144); // R. Chaine colors
    glBegin(GL_LINES);
    for (Finite_edges_iterator ei = _triangulation.finite_edges_begin();
         ei != _triangulation.finite_edges_end(); ei++) {
      glSegment2(_triangulation.segment(ei));
    }
    glEnd();
    glPopAttrib();
    
    /* Draw curve edges */
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glColor3ub(2, 178, 2); // R. Chaine colors
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (All_faces_iterator fi = _triangulation.all_faces_begin();
         fi != _triangulation.all_faces_end(); fi++) {
      for (int i = 0; i < 3; i++) {
        if (fi->is_curve_edge(i)) {
          glSegment2(_triangulation.segment(fi, i));
        }
      }
    }
    glEnd();
    glPopAttrib();
    
    /* Draw vertices */
    glPushAttrib(GL_CURRENT_BIT | GL_POINT_BIT);
    glColor3ub(246, 109, 1); // R. Chaine colors
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (Point_iterator pi = _triangulation.points_begin();
         pi != _triangulation.points_end(); pi++) {
      glPoint2(*pi);
    }
    glEnd();
    glPopAttrib();
  } break;
  case THIN_MASK:
  case THICK_MASK: {
    /* Draw faces */
    glBegin(GL_TRIANGLES);
    for (Finite_faces_iterator fi = _triangulation.finite_faces_begin();
         fi != _triangulation.finite_faces_end(); fi++) {
      if (!fi->is_outside()) {
        glTriangle2(_triangulation.triangle(fi));
      }
    }
    glEnd();
    
    /* Draw thick edges to compensate faces rasterization errors */
    if (_style.top() == THICK_MASK) {
      glPushAttrib(GL_POLYGON_BIT | GL_LINE_BIT);
      
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glLineWidth(2.0f);
      glBegin(GL_TRIANGLES);
      for (Finite_faces_iterator fi = _triangulation.finite_faces_begin();
           fi != _triangulation.finite_faces_end(); fi++) {
        if (!fi->is_outside()) {
          glTriangle2(_triangulation.triangle(fi));
        }
      }
      glEnd();
      
      glPopAttrib();
    }
  } break;
  case ITEM_BUFFER_POINTS: {
    guint32 index = 0u;
    GLubyte *pindex = (GLubyte *) &index;
    
    glPushAttrib(GL_CURRENT_BIT);
    
    glBegin(GL_POINTS);
    for (Finite_vertices_iterator vi = _triangulation.finite_vertices_begin();
         vi != _triangulation.finite_vertices_end(); vi++) {
      glColor4ubv(pindex);
      glPoint2(vi->point());
      index++;
    }
    glEnd();
    
    glPopAttrib();
  } break;
  default:
    assert(false);
    break;
  }
}

void
Triangulation_display::display_bbox(void) {
  glPushAttrib(GL_CURRENT_BIT | GL_POLYGON_BIT);
  
  glColor4fv(GL_PURE_BLACK);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  gl_widget_bbox_2(_triangulation.bbox().xmin(), _triangulation.bbox().ymin(),
                   _triangulation.bbox().xmax(), _triangulation.bbox().ymax());
  
  glPopAttrib();
}
