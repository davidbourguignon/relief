#ifndef __DRAWING_HH__
#define __DRAWING_HH__

#include <gtk/gtk.h>
#include <vec2.h>
#include <opengl_buffer.h>

#include "file.hh"
#include "tool.hh"
#include "triangulation_display.hh"

class Application;

class Drawing {
public:
  Drawing(Application *application);
  ~Drawing(void);
  const_Vec2i_t size(void) const;
  const_GLvecf_t background_color(void) const;
  const_GLveci_t drawbox(void) const;
  bool has_changed(void) const;
  double height_field_max(void);
  bool init(void);
  bool read(std::ifstream& fin, File::Type file_type);
  bool write(std::ofstream& fout, File::Type file_type) const;
  void start_drawing_stroke(GdkInputSource source, guint state,
                            gdouble x, gdouble y, gdouble pressure,
                            gdouble xtilt, gdouble ytilt);
  void stop_drawing_stroke(GdkInputSource source, guint state,
                           gdouble x, gdouble y, gdouble pressure,
                           gdouble xtilt, gdouble ytilt);
  void draw_stroke(GdkInputSource source, guint state,
                   gdouble x, gdouble y, gdouble pressure,
                   gdouble xtilt, gdouble ytilt);
  void draw_height_field(void);
  void start_drawing(const GLveci viewport);
  void stop_drawing(void);
  void draw(void);
  void display_triangulation(Triangulation_display::Style style,
                             bool display_bbox = false,
                             bool display_marks = false);
  
private:
  typedef Triangulation::Vertex_handle Vertex_handle;
  typedef Triangulation::Face_handle Face_handle;
  typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
  typedef Triangulation::Line_face_circulator Line_face_circulator;
  typedef Triangulation::All_faces_iterator All_faces_iterator;
  typedef enum {
    _COLOR_TABLE_THRESHOLD,
    _COLOR_TABLE_ABSOLUTE,
    _COLOR_TABLE_SPHERE_MAP
  } _ColorTableType;
  
  static GLboolean _setup_non_overlay_texture_cb(void *data,
                                                 GLboolean test_proxy);
  static GLboolean _setup_overlay_texture_cb(void *data,
                                             GLboolean test_proxy);
  static GLboolean _setup_distance_texture_cb(void *data,
                                              GLboolean test_proxy);
  static GLboolean _display_first_pass_list_cb(void *data,
                                               GLboolean test_proxy);
  static GLboolean _display_second_pass_list_cb(void *data,
                                                GLboolean test_proxy);
  static GLboolean _display_color_table_list_cb(void *data,
                                                GLboolean test_proxy);
  static GLboolean _display_convolution_list_cb(void *data,
                                                GLboolean test_proxy);
  static GLboolean _display_overlay_list_cb(void *data,
                                            GLboolean test_proxy);
  
  Triangulation_display *_triangulation_display(void);
  void _remove(Triangulation_display *triangulation_display);
  void _draw_quad(const GLveci quadi, const GLvecf quadf) const;
  void _remove_erased_triangulation_vertices(void);
  void _reconstruct_curve(void);
  void _gather_marked_triangulation_faces(void);
  
  Application *_application;
  Triangulation *_triangulation_proxy;
  Triangulation_display *_triangulation_display_ptr;
  Tool *_tool;
  Vec2i _size;
  GLvecf _background_color;
  GLveci _viewport, _drawport, _drawbox;
  bool _is_drawing_stroke, _is_marking_stroke;
  bool _has_just_been_cleared, _has_changed;
  bool _are_textures_and_lists_set;
  GLtexture *_front_texture, *_back_first_texture, *_back_second_texture;
  GLtexture *_overlay_texture, *_distance_texture;
  GLlist *_first_pass_list, *_second_pass_list;
  GLlist *_back_first_color_table_list, *_back_second_color_table_list;
  GLlist *_distance_color_table_list, *_convolution_list;
  GLlist *_overlay_list;
  std::vector< std::vector<Tool::Point> > _marking_paths;
  GLframebuf *_colorbuf, *_back_colorbuf, *_stencilbuf, *_overlay_image;
  GLitembuf *_itembuf;
  _ColorTableType _back_first_type, _back_second_type, _distance_type;
  double _euclidean_distance_max;
};

#endif // __DRAWING_HH__
