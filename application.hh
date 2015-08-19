#ifndef __APPLICATION_HH__
#define __APPLICATION_HH__

#include <platform_defs.h>

class Toolbox;
class File;
class Drawing;
class Triangulation;
class Meshing;
class Tetrahedrization;
class Viewer;

class Application {
public:
  Application(int *pargc, char ***pargv);
  ~Application(void);
  int exec(void);
  Toolbox *toolbox(void);
  File *file(void);
  Drawing *drawing(void);
  Triangulation *triangulation(void);
  Meshing *meshing(void);
  Tetrahedrization *tetrahedrization(void);
  Viewer *viewer(void);
  void remove(Toolbox *toolbox);
  void remove(File *file);
  void remove(Drawing *drawing);
  void remove(Triangulation *triangulation);
  void remove(Meshing *meshing);
  void remove(Tetrahedrization *tetrahedrization);
  void remove(Viewer *viewer);
  
private:
  void _parse_command_line(int argc, char **argv);
  void _quit(void);
  
  Toolbox *_toolbox;
  File *_file;
  Drawing *_drawing;
  Triangulation *_triangulation;
  Meshing *_meshing;
  Tetrahedrization *_tetrahedrization;
  Viewer *_viewer;
};

#endif // __APPLICATION_HH__
