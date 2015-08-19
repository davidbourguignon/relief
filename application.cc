#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "toolbox.hh"
#include "file.hh"
#include "drawing.hh"
#include "triangulation.hh"
#include "meshing.hh"
#include "tetrahedrization.hh"
#include "viewer.hh"
#include "application.hh"

using namespace std;

Application::Application(int *pargc, char ***pargv) {
  _toolbox = NULL;
  _file = NULL;
  _drawing = NULL;
  _triangulation = NULL;
  _meshing = NULL;
  _tetrahedrization = NULL;
  _viewer = NULL;
  
  gtk_init(pargc, pargv);
  gtk_gl_init(pargc, pargv);
  _parse_command_line(*pargc, *pargv);
}

Application::~Application(void) {
  if (_toolbox != NULL) {
    delete _toolbox;
  }
  if (_file != NULL) {
    delete _file;
  }
  if (_drawing != NULL) {
    delete _drawing;
  }
  if (_triangulation != NULL) {
    delete _triangulation;
  }
  if (_meshing != NULL) {
    delete _meshing;
  }
  if (_tetrahedrization != NULL) {
    delete _tetrahedrization;
  }
  if (_viewer != NULL) {
    delete _viewer;
  }
}

int
Application::exec(void) {
  gtk_main();
  return EXIT_SUCCESS;
}

Toolbox *
Application::toolbox(void) {
  if (_toolbox == NULL) {
    _toolbox = new Toolbox(this);
    assert(_toolbox != NULL);
  }
  return _toolbox;
}

File *
Application::file(void) {
  if (_file == NULL) {
    _file = new File(this);
    assert(_file != NULL);
  }
  return _file;
}

Drawing *
Application::drawing(void) {
  if (_drawing == NULL) {
    _drawing = new Drawing(this);
    assert(_drawing != NULL);
  }
  return _drawing;
}

Triangulation *
Application::triangulation(void) {
  if (_triangulation == NULL) {
    _triangulation = new Triangulation(this);
    assert(_triangulation != NULL);
  }
  return _triangulation;
}

Meshing *
Application::meshing(void) {
  if (_meshing == NULL) {
    _meshing = new Meshing(this);
    assert(_meshing != NULL);
  }
  return _meshing;
}

Tetrahedrization *
Application::tetrahedrization(void) {
  if (_tetrahedrization == NULL) {
    _tetrahedrization = new Tetrahedrization(this);
    assert(_tetrahedrization != NULL);
  }
  return _tetrahedrization;
}

Viewer *
Application::viewer(void) {
  if (_viewer == NULL) {
    _viewer = new Viewer(this);
    assert(_viewer != NULL);
  }
  return _viewer;
}

void
Application::remove(Toolbox *toolbox) {
  if (toolbox == _toolbox) {
    if (_toolbox != NULL) {
      delete _toolbox;
      _toolbox = NULL;
    }
    _quit();
  }
}

void
Application::remove(File *file) {
  if (file == _file) {
    if (_file != NULL) {
      delete _file;
      _file = NULL;
    }
    _quit();
  }
}

void
Application::remove(Drawing *drawing) {
  if (drawing == _drawing) {
    if (_drawing != NULL) {
      delete _drawing;
      _drawing = NULL;
    }
    _quit();
  }
}

void
Application::remove(Triangulation *triangulation) {
  if (triangulation == _triangulation) {
    if (_triangulation != NULL) {
      delete _triangulation;
      _triangulation = NULL;
    }
    _quit();
  }
}

void
Application::remove(Meshing *meshing) {
  if (meshing == _meshing) {
    if (_meshing != NULL) {
      delete _meshing;
      _meshing = NULL;
    }
    _quit();
  }
}

void
Application::remove(Tetrahedrization *tetrahedrization) {
  if (tetrahedrization == _tetrahedrization) {
    if (_tetrahedrization != NULL) {
      delete _tetrahedrization;
      _tetrahedrization = NULL;
    }
    _quit();
  }
}

void
Application::remove(Viewer *viewer) {
  if (viewer == _viewer) {
    if (_viewer != NULL) {
      delete _viewer;
      _viewer = NULL;
    }
    _quit();
  }
}

void
Application::_parse_command_line(int argc, char **argv) {
  char *filename = NULL;
  bool has_read_file = false, is_file_rlf = false;
  
  for (int i = 1; i < argc; i++) {
    if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {
      printf("Usage: %s [OPTION]... [FILE]...\n", argv[0]);
      printf("\n");
      printf("Options\n");
      printf("  -f, --file\tinput .rlf file\n");
      printf("  -i, --import\timport .off file\n");
      exit(EXIT_SUCCESS);
    } else if ((!strcmp(argv[i], "-f")) || (!strcmp(argv[i], "--file")) ||
               (!strcmp(argv[i], "-i")) || (!strcmp(argv[i], "--import"))) {
      if (!has_read_file) {
        i++;
        if (i < argc) {
          filename = argv[i];
          has_read_file = true;
          if ((!strcmp(argv[i], "-f")) || (!strcmp(argv[i], "--file"))) {
            is_file_rlf = true;
          }
        } else {
          fprintf(stderr, "Error: missing file %s!\n", argv[i]);
          fprintf(stdout, "Try `%s --help' for more information.\n", argv[0]);
          exit(EXIT_FAILURE);
        }
      } else {
        fprintf(stderr, "Error: too many files!\n");
        fprintf(stdout, "Try `%s --help' for more information.\n", argv[0]);
        exit(EXIT_FAILURE);
      }
    } else {
      fprintf(stderr, "Error: invalid option %s!\n", argv[i]);
      fprintf(stdout, "Try `%s --help' for more information.\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  toolbox();
  if (has_read_file) {
    if (is_file_rlf) {
      file()->open(File::RLF, filename);
    } else {
      file()->open(File::OFF, filename);
    }
  }
}

void
Application::_quit(void) {
  if (_toolbox          == NULL &&
      _file             == NULL &&
      _drawing          == NULL &&
      _triangulation    == NULL &&
      _meshing          == NULL &&
      _tetrahedrization == NULL &&
      _viewer           == NULL) {
    gtk_main_quit();
  }
//   if (_toolbox != NULL) {
//     printf("toolbox\n");
//   }
//   if (_file != NULL) {
//     printf("file\n");
//   }
//   if (_drawing != NULL) {
//     printf("drawing\n");
//   }
//   if (_triangulation != NULL) {
//     printf("triangulation\n");
//   }
//   if (_meshing != NULL) {
//     printf("meshing\n");
//   }
//   if (_tetrahedrization != NULL) {
//     printf("tetrahedrization\n");
//   }
//   if (_viewer != NULL) {
//     printf("viewer\n");
//   }
//   printf("\n");
}
