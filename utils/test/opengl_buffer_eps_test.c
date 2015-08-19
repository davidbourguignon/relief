#include <opengl_buffer.h>
#include <glut_utils.h>
#include <glut_widget.h>

typedef enum {
  TRIANGLE,
  SPHERE,
  TORUS,
  TEAPOT
} ModelType;
#define MODEL_TYPE_SIZE 4

typedef enum {
  POINT,
  LINE,
  FILL
} StyleType;
#define STYLE_TYPE_SIZE 3

/*
 * Data
 */
static const GLfloat HELP_TEXT_LIGHT_BG_COLOR[4] = {1.0, 1.0, 1.0, 0.1};
static const GLfloat HELP_TEXT_DARK_BG_COLOR[4]  = {0.0, 0.0, 0.0, 0.1};
static const GLfloat HELP_TEXT_COLOR[4]          = {1.0, 0.0, 0.0, 1.0};
static const char *HELP_TEXT[]
  = {"H e l p",
     "p - write out Encapsulated PostScript (unsorted)",
     "P - write out Encapsulated PostScript (sorted)",
     "g - spawn Ghostview to view eps file test.eps",
     "v - toggle Verbose feedback buffer",
     "",
     "l - toggle Lighting",
     "m - switch Model",
     "y - switch polYgon rasterization mode (point, line, fill)",
     "z - switch point and line siZe (from 1 to 5)",
     "",
     "a - toggle Axes",
     "b - toggle black/white Background",
     "f - toggle Full screen",
     "h - toggle Help",
     "i - reInitialize trackball",
     "s - toggle display Speed",
     "q - Quit"};
static const int HELP_TEXT_SIZE = sizeof(HELP_TEXT)/sizeof(char *);
static const int NUMBER_OF_VERTICES[MODEL_TYPE_SIZE]
  = {3,     /* For triangle... */
     100,   /* 10*10 = 100 for sphere, see below */
     225,   /* 15*15 = 225 for torus, see below */
     4096}; /* A lot for teapot... */
static const int CONNECTIVITY_OF_VERTICES[MODEL_TYPE_SIZE]
  = {2,  /* For triangle in line mode */
     4,  /* For sphere in fill mode */
     4,  /* For torus in fill mode */
     4}; /* For teapot in fill mode */
#if DEBUG
static GLerror *err   = NULL;
#endif
static GLUTwin *win   = NULL;
static GLUTfps *fps   = NULL;
static Trackball *tb  = NULL;
static GLfeedbuf *fbb = NULL;
static GLuint light_help_list = 0, dark_help_list = 0;
static GLuint axes_list = 0, light_list = 0;
static GLboolean white_background = GL_TRUE;
static GLboolean display_help     = GL_FALSE;
static GLboolean full_screen      = GL_FALSE;
static GLboolean report_speed     = GL_FALSE;
static GLboolean display_axes     = GL_FALSE;
static GLboolean lighting         = GL_FALSE;
static GLboolean verbose          = GL_FALSE;
static ModelType model = TRIANGLE;
static StyleType style = FILL;
static int       size  = 1; /* Size of lines and points. */

/*
 * Functions declarations
 */
static void init    (int argc, char **argv);
static void reshape (int w, int h);
static void display (void);
static void keyboard(unsigned char key, int x, int y);
static void mouse   (int button, int state, int x, int y);
static void motion  (int x, int y);

static void draw_geometry(void);

/*
 * Functions definitions
 */
static void
init(int argc, char **argv) {
  Vec3d v;
  GLfloat light_diffuse[]  = {0.0, 1.0, 0.0, 1.0};
  GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
  
#if DEBUG
  err = gl_error_new();
#endif
  win = glut_win_new();
  fps = glut_fps_new();
  tb = trackball_new();
  fbb = gl_feedbuf_new();
  
  glut_win_init_pos(win, 200, 200);
  glut_win_init_size(win, 512, 512);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE |
                      GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
  win->id = glutCreateWindow(argv[0]);
  
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  
  light_help_list = glGenLists(1);
  glNewList(light_help_list, GL_COMPILE);
  glut_widget_menu(HELP_TEXT, HELP_TEXT_SIZE,
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_12,
                   GLUT_BITMAP_HELVETICA_10,
                   HELP_TEXT_LIGHT_BG_COLOR, HELP_TEXT_COLOR);
  glEndList();
  
  dark_help_list = glGenLists(1);
  glNewList(dark_help_list, GL_COMPILE);
  glut_widget_menu(HELP_TEXT, HELP_TEXT_SIZE,
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_12,
                   GLUT_BITMAP_HELVETICA_10,
                   HELP_TEXT_DARK_BG_COLOR, HELP_TEXT_COLOR);
  glEndList();
  
  axes_list = glGenLists(1);
  glNewList(axes_list, GL_COMPILE);
  glut_widget_axes();
  glEndList();
  
  light_list = glGenLists(1);
  glNewList(light_list, GL_COMPILE);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEndList();
  
  trackball_set_init_transf(tb, QUATD_ID, vec3d_set(v, 0.0, 0.0, -5.0));
  trackball_init_transf(tb);
  gl_transf_from_trackball(win->gl_transf, tb);
  
  gl_feedbuf_set_vertices(fbb, NUMBER_OF_VERTICES[model], GL_3D_COLOR,
                               CONNECTIVITY_OF_VERTICES[model]);
}

static void
reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  gl_transf_set_viewport(win->gl_transf);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLdouble) w / (GLdouble) h, 1.0, 10.0);
  gl_transf_set_proj_mat(win->gl_transf);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glut_win_set_size(win, w, h);
  trackball_reshape(tb, w, h);
}

static void
display(void) {
  if (report_speed) {
    glut_fps_start(fps);
  }
  if (white_background) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
  } else {
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }
  glDisable(GL_DITHER);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DITHER);
  
  glPushMatrix();
  glMultMatrixd(win->gl_transf->modelview_matrix);
  
  /* Render in color buffer */
  draw_geometry();
  if (white_background) {
    glColor3f(0.0, 0.0, 0.0);
  } else {
    glColor3f(1.0, 1.0, 1.0);
  }
  if (display_axes) {
    glCallList(axes_list);
  }
  
  /* Render in feedback buffer */
  gl_feedbuf_render_begin(fbb);
  draw_geometry();
  gl_feedbuf_render_end(fbb);
  if (verbose) {
    fprintf(stdout, "Results for feedback buffer:\n");
    fprintf(stdout, "Size %i Effective size %i\n", fbb->size,
            fbb->effective_size);
    gl_feedbuf_print(fbb, stdout);
    fprintf(stdout, "\n");
  }
  
  glPopMatrix();
  
  if (report_speed) {
    glut_fps_stop(fps);
    glut_widget_string(fps->report_string,
                       GLUT_BITMAP_HELVETICA_18, HELP_TEXT_COLOR,
                       0.025, 0.025);
  }
  if (display_help) {
    if (white_background) {
      glCallList(dark_help_list);
    } else {
      glCallList(light_help_list);
    }
  }
  
#if DEBUG
  gl_error_report(err);
#endif
  
  glutSwapBuffers();
}

static void
keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'a':
      display_axes = (!display_axes);
      break;
    case 'b':
      white_background = (!white_background);
      break;
    case 'f':
      if (full_screen) {
        full_screen = GL_FALSE;
        glut_win_reinit_pos(win);
        glut_win_reinit_size(win);
      } else {
        full_screen = GL_TRUE;
        glutFullScreen();
      }
      break;
    case 'g':
      if (system("ghostview out.eps &") == -1) {
        fprintf(stderr, "Error: Unable to start GNU ghostview to preview "
                        "eps file out.eps!\n");
      }
      break;
    case 'h':
      display_help = (!display_help);
      break;
    case 'i':
      trackball_init_transf(tb);
      gl_transf_from_trackball(win->gl_transf, tb);
      break;
    case 'l':
      lighting = (!lighting);
      break;
    case 'm':
      model = (model + 1) % MODEL_TYPE_SIZE;
      gl_feedbuf_set_vertices(fbb, NUMBER_OF_VERTICES[model], GL_3D_COLOR,
                              CONNECTIVITY_OF_VERTICES[model]);
      break;
    case 'p':
      glutSetCursor(GLUT_CURSOR_WAIT);
      if (!gl_feedbuf_write_eps(fbb, "out.eps", GL_FALSE)) {
        fprintf(stderr, "Error: Unable to write eps file out.eps!\n");
      }
      glutSetCursor(GLUT_CURSOR_INHERIT);
      break;
    case 'P':
      glutSetCursor(GLUT_CURSOR_WAIT);
      if (!gl_feedbuf_write_eps(fbb, "out.eps", GL_TRUE)) {
        fprintf(stderr, "Error: Unable to write eps file out.eps!\n");
      }
      glutSetCursor(GLUT_CURSOR_INHERIT);
      break;
    case 'q':
    case 27:
      exit(0);
      break;
    case 's':
      report_speed = (!report_speed);
      break;
    case 'v':
      verbose = (!verbose);
      break;
    case 'y':
      style = (style + 1) % STYLE_TYPE_SIZE;
      break;
    case 'z':
      size = (size % 5) + 1;
      break;
    default:
      fprintf(stderr, "Error: Unknown option !\n");
      break;
  }
  glutPostRedisplay();
}

static void
mouse(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        trackball_start_rotation(tb, x, y);
      } else {
        trackball_stop_rotation(tb);
      }
      break;
    case GLUT_MIDDLE_BUTTON:
      if (state == GLUT_DOWN) {
        trackball_start_transl_z(tb, x, y);
      } else {
        trackball_stop_transl_z(tb);
      }
      break;
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) {
        trackball_start_transl_xy(tb, x, y);
      } else {
        trackball_stop_transl_xy(tb);
      }
      break;
  }
}

static void
motion(int x, int y) {
  trackball_move(tb, x, y);
  gl_transf_from_trackball(win->gl_transf, tb);
  glutPostRedisplay();
}

static void
draw_geometry(void) {
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT |
               GL_POINT_BIT | GL_LINE_BIT | GL_POLYGON_BIT | GL_LIGHTING_BIT);
  
  glEnable(GL_DEPTH_TEST);
  if (lighting) {
    glCallList(light_list);
  }
  glPointSize(size);
  glLineWidth(size);
  switch (style) {
    case POINT: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
    case LINE:  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  break;
    case FILL:  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  break;
  }
  switch (model) {
    case TRIANGLE:
      glBegin(GL_TRIANGLES);
      glNormal3f(0.0, 0.0, 1.0);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(0.0, 0.0);
      glColor3f(0.0, 1.0, 0.0);
      glVertex2f(1.0, 0.0);
      glColor3f(0.0, 0.0, 1.0);
      glVertex2f(0.0, 1.0);
      glEnd();
      break;
    case SPHERE:
      glColor3f(1.0, 0.0, 0.0);
      glutSolidSphere(1.0, 10, 10);
      break;
    case TORUS:
      glColor3f(1.0, 0.0, 0.0);
      glutSolidTorus(0.5, 1.0, 15, 15);
      break;
    case TEAPOT:
      glColor3f(1.0, 0.0, 0.0);
      glutSolidTeapot(1.0);
      break;
  }
  
  glPopAttrib();
}

/*
 * Main program
 */
int
main(int argc, char **argv) {
  init(argc, argv);
  glutMainLoop();
  return 0;
}
