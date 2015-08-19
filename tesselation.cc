#include <queue>

#include "tesselation.hh"

using namespace std;

Tesselation::Tesselation(void) : Base() {}

Tesselation::~Tesselation(void) {}

void Tesselation::display(void) const {
  glPushAttrib(GL_CURRENT_BIT | GL_STENCIL_BUFFER_BIT);
  
  glStencilFunc(GL_EQUAL, 0xC, 0xC); /* 0x4 + 0x8 */
  glEnable(GL_STENCIL_TEST);
  glBegin(GL_TRIANGLES);
  for (Finite_faces_iterator fi = finite_faces_begin();
       fi != finite_faces_end(); fi++) {
    for (int i = 0; i < 3; i++) {
      Vertex_handle vh = fi->vertex(i);
      glColor3ub(vh->offsetub(), vh->offsetub(), vh->offsetub());
      glPoint2(vh->point());
    }
  }
  glEnd();
  
  glPopAttrib();
}

void
Tesselation::propagate_depth(void) {
  /*
   * We use the distance to neighbors for evaluating depth, with a weighting
   * scheme similar to the one suggested by Taubin. Since in most cases, we
   * evaluate depth for unknown points that are not located inside a polygon
   * of known points, it is impossible to apply barycentric coordinates
   * schemes, such as the one presented by Meyer et al.
   *
   * Gabriel Taubin, A Signal Processing Approach to Fair Surface Design,
   * Proceedings of SIGGRAPH, 1995.
   *
   * Mark Meyer, Haeyoung Lee, Alan Barr and Mathieu Desbrun, Generalized
   * Barycentric Coordinates on Irregular Polygons, Journal of Graphics
   * Tools, 7(1):13-22, 2002.
   */
  
  /* seed with vertices having known neighborhoods */
  priority_queue<Vertex_handle,
                 vector<Vertex_handle>,
                 Compare_known_neighbors_ratio> vertices;
  for (Finite_vertices_iterator vi = finite_vertices_begin();
       vi != finite_vertices_end(); vi++) {
    if (vi->projected_vertex() == NULL) {
      vi->known_neighbors_ratio() = known_neighbor_ratio(vi);
      if (vi->known_neighbors_ratio() != 0.0f) {
        vertices.push(vi);
      }
    }
  }
  assert(!vertices.empty());
  
  /* best-first search */
  typedef vector< pair<Vertex_handle, double> > Known_neighbors;
  typedef vector<Vertex_handle> Unknown_neighbors;
  Known_neighbors known_neighbors;
  Unknown_neighbors unknown_neighbors;
  
  while (!vertices.empty()) {
    Vertex_handle vh(vertices.top());
    vertices.pop();
    
    // sort known and unknown neighbors
    Vertex_circulator vc = incident_vertices(vh), done(vc);
    assert (vc != NULL);
    do {
      if (vc->depth() != GL_DEPTH_FAR) {
        known_neighbors.push_back(make_pair(vc, 0.0));
      } else {
        unknown_neighbors.push_back(vc);
      }
    } while (++vc != done);
    assert(!known_neighbors.empty());
    
    // evaluate depth using known neighbors
    double depth = 0.0;
    if (known_neighbors.size() > 1) {
      Point p(vh->point());
      double weight_sum = 0.0;
      for (Known_neighbors::iterator kni = known_neighbors.begin();
           kni != known_neighbors.end(); kni++) {
        double weight = 1.0 / sqrt(squared_distance(p, kni->first->point()));
        kni->second = weight;
        weight_sum += weight;
      }
      for (Known_neighbors::const_iterator kni = known_neighbors.begin();
           kni != known_neighbors.end(); kni++) {
        depth += kni->first->depth() * (kni->second / weight_sum);
      }
    } else if (known_neighbors.size() == 1) {
      cerr << "Warning: only one known neighbor!" << endl;
      depth = known_neighbors.begin()->first->depth();
    } else {
      cerr << "Error: no known neighbors!" << endl;
    }
    assert(depth > GL_DEPTH_NEAR && depth < GL_DEPTH_FAR);
    known_neighbors.clear();
    vh->depth() = depth;
    
    // insert unknown neighbors in the queue
    for (Unknown_neighbors::iterator uni = unknown_neighbors.begin();
         uni != unknown_neighbors.end(); uni++) {
      Vertex_handle un(*uni);
      un->known_neighbors_ratio() = known_neighbor_ratio(un);
      vertices.push(un);
    }
    unknown_neighbors.clear();
  }
}

float
Tesselation::known_neighbor_ratio(Vertex_handle v) {
  float ratio = 1.0f;
  int number_of_neighbors = 0;
  int number_of_known_neighbors = 0;
  
  Vertex_circulator vc = incident_vertices(v), done(vc);
  assert (vc != NULL);
  do {
    number_of_neighbors++;
    if (vc->depth() != GL_DEPTH_FAR) {
      number_of_known_neighbors++;
    }
  } while (++vc != done);
  ratio = (float) number_of_known_neighbors / (float) number_of_neighbors;
  assert(ratio >= 0.0f && ratio <= 1.0f);
  return ratio;
}
