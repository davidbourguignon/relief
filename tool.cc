#include <cassert>

#include "tool.hh"

using namespace std;

static const Kernel::FT SQUARED_DISTANCE_MIN = Kernel::FT(16.0);
static const Kernel::FT FOUR_TIMES_SQUARED_DISTANCE_MIN
  = Kernel::FT(4.0) * SQUARED_DISTANCE_MIN;

Tool::Tool(Toolbox *toolbox, GLboolean (*display_list_cb)(void *, GLboolean))
  : _toolbox(toolbox),
    _button(NULL),
    _points(),
    _points_buf(),
    _is_path_interpolated(false) {
  gl_veci_eq(_drawport, GL_VECI_NULL);
  _list = gl_list_new();
  gl_list_set(_list, display_list_cb, (void *) this, GL_FALSE);
}

Tool::~Tool(void) {
  gl_list_delete(_list);
}

void
Tool::clear(void) {
  gl_list_clear(_list);
}

void
Tool::set_button(GtkWidget *button) {
  assert(GTK_IS_WIDGET(button));
  _button = button;
}

void
Tool::active_button(gboolean is_active) const {
  assert(GTK_IS_WIDGET(_button));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_button), is_active);
}

void
Tool::start_recording_path(GdkInputSource source,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt) {
  _points.push_back(Point(x, y));
}

void
Tool::stop_recording_path(void) {
  _points_buf.swap(_points);
  _points.clear();
}

void
Tool::record_path(GdkInputSource source,
                  gdouble x, gdouble y, gdouble pressure,
                  gdouble xtilt, gdouble ytilt) {
  Point p_prev(_points.back());
  Point p_next(x, y);
  FT squared_distance = CGAL::squared_distance(p_prev, p_next);
  
  _is_path_interpolated = false;
  if (squared_distance > FOUR_TIMES_SQUARED_DISTANCE_MIN) {
    // linear interpolation of missing point
    Point p_midl = CGAL::midpoint(p_prev, p_next);
    _points.push_back(p_midl);
    _points.push_back(p_next);
    _is_path_interpolated = true;
  } else if (squared_distance > SQUARED_DISTANCE_MIN) {
    // stroke sampling at given granularity
    _points.push_back(p_next);
  } else {
    // too close to previous point, do not insert in point set
  }
}

const vector<Tool::Point>&
Tool::recorded_path(void) const {
  return _points_buf;
}

const_GLveci_t
Tool::drawn_pixels(void) const {
  return _drawport;
}
