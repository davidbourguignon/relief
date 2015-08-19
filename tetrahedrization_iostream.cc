#include <string>
// #include <CGAL/IO/File_scanner_OFF.h>

#include "tetrahedrization_iostream.hh"

using namespace std;

Tetrahedrization_iostream::Tetrahedrization_iostream(
  Tetrahedrization& tetrahedrization, Format format, bool verbose)
  : _tetrahedrization(tetrahedrization),
    _format(format),
    _verbose(verbose) {}

Tetrahedrization_iostream::~Tetrahedrization_iostream(void) {}

istream&
operator>>(istream& in, Tetrahedrization_iostream& tin) {
  switch (tin._format) {
  case Tetrahedrization_iostream::DEFAULT:
    tin._read(in);
    break;
  case Tetrahedrization_iostream::OFF:
    tin._read_off(in);
    break;
  default:
    assert(false);
    break;
  }
  assert(tin._tetrahedrization.is_valid());
  return in;
}

ostream&
operator<<(ostream& out, const Tetrahedrization_iostream& tout) {
  switch (tout._format) {
  case Tetrahedrization_iostream::DEFAULT:
    tout._write(out);
    break;
  case Tetrahedrization_iostream::OFF:
    tout._write_off(out);
    break;
  default:
    assert(false);
    break;
  }
  return out;
}

void
Tetrahedrization_iostream::_read(istream& in) {
  in >> _tetrahedrization;
}

static string&
first_line_not_pound_comment(istream& in, string& s) {
  const string::value_type BLANK_SPACE = string(" ")[0];
  const string::value_type POUND_SIGN = string("#")[0];
  
  bool is_comment_line = true;
  while (is_comment_line) {
    s.clear();
    getline(in, s);
    string::size_type pos = s.find_first_not_of(BLANK_SPACE);
    if (!s.empty() && s.find(POUND_SIGN, pos) != pos) {
      is_comment_line = false;
    }
  }
  return s;
}

void
Tetrahedrization_iostream::_read_off(istream& in) {
  string s;
  
  if (first_line_not_pound_comment(in, s).substr(0, 3) == "OFF") {
    unsigned int number_of_vertices = 0;
    unsigned int number_of_faces = 0;
    unsigned int number_of_edges = 0;
    in >> number_of_vertices >> number_of_faces >> number_of_edges;
    cout << number_of_vertices << " vertices and "
         << number_of_faces << " faces" << endl;
    
    double xd = 0.0, yd = 0.0, zd = 0.0;
    sscanf(first_line_not_pound_comment(in, s).c_str(),
           "%lf %lf %lf\n", &xd, &yd, &zd);
    _tetrahedrization.insert_first(Point(xd, yd, zd));
    FT x = 0.0, y = 0.0, z = 0.0;
    for (unsigned int i = 1; i < number_of_vertices; i++) {
      in >> x >> y >> z;
      _tetrahedrization.insert(Point(x, y, z));
    }
    assert(_tetrahedrization.is_valid());
  } else {
    in.setstate(istream::failbit);
  }
//   CGAL::File_scanner_OFF file_scanner(in, _verbose);
//   Point p;
//   file_scan_vertex(file_scanner, p);
//   _tetrahedrization.insert_first(p);
//   file_scanner.skip_to_next_vertex(0);
//   for (int i = 1; i < file_scanner.size_of_vertices() - 1; i++) {
//     file_scan_vertex(file_scanner, p);
//     _tetrahedrization.insert(p);
//     file_scanner.skip_to_next_vertex(i);
//   }
//   file_scan_vertex(file_scanner, p);
//   _tetrahedrization.insert_last(p);
//   file_scanner.skip_to_next_vertex(file_scanner.size_of_vertices() - 1);
}

void
Tetrahedrization_iostream::_write(ostream& out) const {
  out << _tetrahedrization;
}

void
Tetrahedrization_iostream::_write_off(ostream& out) const {
  const unsigned int NULL_INDEX = ~0u;
  
  /* set surface vertices and facets indices */
  
  CGAL::Unique_hash_map<Vertex_handle, unsigned int>
    indices(NULL_INDEX, _tetrahedrization.number_of_surface_vertices());
  vector<Vertex_handle> surface_vertices;
  vector< CGAL::Triple<unsigned int, unsigned int, unsigned int> >
    surface_facets;
  for (All_cells_iterator ci = _tetrahedrization.all_cells_begin();
       ci != _tetrahedrization.all_cells_end(); ci++) {
    for (int i = 0; i < 4; i++) {
      if (ci->is_surface_facet(i)) {
        Facet f(ci, i);
        unsigned int facet_indices[3];
        unsigned int k = (f.second + 1)%4;
        for (int j = 0; j < 3; j++) {
          Vertex_handle vh = f.first->vertex(k);
          assert(!_tetrahedrization.is_infinite(vh));
          if (indices[vh] == NULL_INDEX) {
            indices[vh] = surface_vertices.size();
            surface_vertices.push_back(vh);
          }
          facet_indices[j] = indices[vh];
          k = Tetrahedrization::next_around_edge(k, f.second);
        }
        surface_facets.push_back(CGAL::make_triple(facet_indices[0],
                                                   facet_indices[1],
                                                   facet_indices[2]));
      }
    }
  }
  
  /* write OFF file */
  
  // write header
  out << "OFF" << endl;
  
  // write number of vertices, facets, edges
  out << surface_vertices.size() << " "
      << surface_facets.size() << " "
      << 0 << endl; // no edge info
  cout << surface_vertices.size() << " vertices and "
       << surface_facets.size() << " faces" << endl;
  
  // write vertices coordinates
  for (vector<Vertex_handle>::const_iterator vhi = surface_vertices.begin();
       vhi != surface_vertices.end(); vhi++) {
    out << **vhi << endl;
  }
  
  // write faces with vertices indices oriented counterclockwise
  for (vector< CGAL::Triple<unsigned int, unsigned int,
         unsigned int> >::const_iterator ti = surface_facets.begin();
       ti != surface_facets.end(); ti++) {
    out << "3 " << ti->first << " " << ti->second << " " << ti->third
        << endl;
  }
}
