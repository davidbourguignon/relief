#include "smooth_surface.hh"

using namespace std;

/*
** We used the following method:
** Gabriel Taubin, A Signal Processing Approach to Fair Surface Design,
** Proceedings of SIGGRAPH, 1995.
**
** The method below is also interesting, but is only meaningful for
** processing an entire mesh:
** J. Vollmer, R. Mencl and H. Muller, Improved Laplacian Smoothing of Noisy
** Surface Meshes, Proceedings of Eurographics, 1999.
*/

static const double DEFAULT_LAMBDA = 0.6307; // given in the paper
static const double DEFAULT_K_PB = 0.1;      //

Smooth_surface::Smooth_surface(Tetrahedrization& tetrahedrization, Type type)
  : _tetrahedrization(tetrahedrization),
    _type(type),
    _lambda(DEFAULT_LAMBDA),
    _k_pb(DEFAULT_K_PB),
    _neighborhoods(),
    _is_first_update(true) {
  _mu = _lambda / (_lambda * _k_pb - 1.0); // k_pb = 1/lambda + 1/mu
}

Smooth_surface::~Smooth_surface(void) {}

template <typename Vertex_handle_iterator>
void
Smooth_surface::operator()(Vertex_handle_iterator begin,
                           Vertex_handle_iterator end) {
  typename Vertex_handle_iterator::difference_type number_of_smoothed_vertices
    = distance(begin, end);
  if (number_of_smoothed_vertices == 0) return;
  
  _neighborhoods.reserve(number_of_smoothed_vertices);
  vector<Vertex_handle> neighborhood;
  for (Vertex_handle_iterator vhi = begin; vhi != end; vhi++) {
    Vertex_handle vh(*vhi);
    _tetrahedrization.incident_surface_vertices(vh,
                                                back_inserter(neighborhood));
    _neighborhoods.push_back(CGAL::make_triple(vh, neighborhood, Point()));
    neighborhood.clear();
  }
  switch (_type) {
  case SIMPLE_AVERAGE:
    _simple_average_step();
    _update();
    break;
  case TAUBIN:
    // first step: laplacian smoothing with positive scale factor (shrink)
    _taubin_step(_lambda);
    _update();
    // second step: laplacian smoothing with negative scale factor (expand)
    _taubin_step(_mu);
    _update();
    break;
  default:
    assert(false);
    break;
  }
  assert(_tetrahedrization.is_valid());
}

template void
Smooth_surface::operator()<
  vector<Tetrahedrization::Vertex_handle>::iterator>(
    vector<Tetrahedrization::Vertex_handle>::iterator begin,
    vector<Tetrahedrization::Vertex_handle>::iterator end);

void
Smooth_surface::_simple_average_step(void) {
  for (_Neighborhoods::iterator ni = _neighborhoods.begin();
       ni != _neighborhoods.end(); ni++) {
    Vector sum(CGAL::NULL_VECTOR);
    for (vector<Vertex_handle>::const_iterator vhi = ni->second.begin();
         vhi != ni->second.end(); vhi++) {
      sum = sum + ((*vhi)->point() - CGAL::ORIGIN);
    }
    if (!ni->second.empty()) {
      ni->third = CGAL::ORIGIN + sum / ni->second.size();
    } else {
      ni->third = ni->first->point();
    }
  }
}

void
Smooth_surface::_taubin_step(double scale_factor) {
  vector< pair<Vector, double> > diffs;
  
  // apply laplacian operator, store result in buffer
  for (_Neighborhoods::iterator ni = _neighborhoods.begin();
       ni != _neighborhoods.end(); ni++) {
    Point p(ni->first->point());
    double weight_sum = 0.0;
    for (vector<Vertex_handle>::const_iterator vhi = ni->second.begin();
         vhi != ni->second.end(); vhi++) {
      Vector diff((*vhi)->point() - p);
      double weight = 1.0 / sqrt(diff * diff);
      diffs.push_back(make_pair(diff, weight));
      weight_sum += weight;
    }
    Vector delta(CGAL::NULL_VECTOR);
    for (vector< pair<Vector, double> >::const_iterator di = diffs.begin();
         di != diffs.end(); di++) {
      delta = delta + di->first * (di->second / weight_sum);
    }
    diffs.clear();
    ni->third = p + scale_factor * delta;
  }
}

void
Smooth_surface::_update(void) {
  // update positions with buffer values
  if (_is_first_update) {
    _is_first_update = false;
    _Neighborhoods::const_iterator ni = _neighborhoods.begin();
    _tetrahedrization.move_first(ni->first, ni->third);
    for (ni++; ni != _neighborhoods.end(); ni++) {
      _tetrahedrization.move(ni->first, ni->third);
    }
  } else {
    _Neighborhoods::const_iterator ni = _neighborhoods.begin();
    _tetrahedrization.move_multipass_first(ni->first, ni->third);
    for (ni++; ni != _neighborhoods.end(); ni++) {
      _tetrahedrization.move_multipass(ni->first, ni->third);
    }
  }
}
