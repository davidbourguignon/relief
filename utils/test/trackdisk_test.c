#include <glib.h>
#include <glut_widget.h>
#include <glut_utils.h>

/*
 * Data
 */
static const GLfloat HELP_TEXT_LIGHT_BG_COLOR[4] = {1.0, 1.0, 1.0, 0.1};
static const GLfloat HELP_TEXT_DARK_BG_COLOR[4]  = {0.0, 0.0, 0.0, 0.1};
static const GLfloat HELP_TEXT_COLOR[4]          = {1.0, 0.0, 0.0, 1.0};
static const char *HELP_TEXT[]
  = {"H e l p",
     "Mouse alone is used for trackdisk.",
     "With CTRL pressed, you can draw!",
     "",
     "a - toggle Axes",
     "c - change background Color",
     "d - toggle Disk",
     "f - toggle Full screen",
     "h - toggle Help",
     "i - reInitialize trackdisk",
     "s - toggle display Speed",
     "q - Quit",
     ""};
static const int HELP_TEXT_SIZE = sizeof(HELP_TEXT)/sizeof(char *);
#if DEBUG
static GLerror *err   = NULL;
#endif
static GLUTwin *win   = NULL;
static GLUTfps *fps   = NULL;
static Trackdisk *td  = NULL;
static GArray *points = NULL;
static GLuint light_help_list = 0, dark_help_list = 0;
static GLuint axes_list = 0, circle_list = 0;
static GLboolean white_background = GL_TRUE;
static GLboolean display_help     = GL_FALSE;
static GLboolean full_screen      = GL_FALSE;
static GLboolean report_speed     = GL_FALSE;
static GLboolean display_disk     = GL_FALSE;
static GLboolean display_axes     = GL_TRUE;
static GLboolean is_drawing       = GL_FALSE;

/*
 * Functions declarations
 */
static void init    (int argc, char **argv);
static void reshape (int w, int h);
static void display (void);
static void keyboard(unsigned char key, int x, int y);
static void mouse   (int button, int state, int x, int y);
static void motion  (int x, int y);

static void CALLBACKD error_callback(void);

/*
 * Functions definitions
 */
static void
init(int argc, char **argv) {
  Complxd c;
  Vec2d v;
  GLUquadricObj *qobj;
  
#if DEBUG
  err = gl_error_new();
#endif
  win = glut_win_new();
  fps = glut_fps_new();
  td = trackdisk_new();
  points = g_array_new(FALSE, FALSE, sizeof(Vec2f));
  
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
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_10,
                   HELP_TEXT_LIGHT_BG_COLOR, HELP_TEXT_COLOR);
  glEndList();
  
  dark_help_list = glGenLists(1);
  glNewList(dark_help_list, GL_COMPILE);
  glut_widget_menu(HELP_TEXT, HELP_TEXT_SIZE,
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_10,
                   HELP_TEXT_DARK_BG_COLOR, HELP_TEXT_COLOR);
  glEndList();
  
  axes_list = glGenLists(1);
  glNewList(axes_list, GL_COMPILE);
  glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
  glPushMatrix();
  glScalef(100.0, 100.0, 1.0);
  glLineWidth(2.0);
  glBegin(GL_LINES);
  glColor3f(1.0, 0.0, 0.0);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 0.0);
  glColor3f(0.0, 1.0, 0.0);
  glVertex2f(0.0, 0.0);
  glVertex2f(0.0, 1.0);
  glEnd();
  glPopMatrix();
  glPopAttrib();
  glEndList();
  
  qobj = gluNewQuadric();
  assert(qobj != NULL);
  gluQuadricCallback(qobj, GLU_ERROR, error_callback);
  gluQuadricDrawStyle(qobj, GLU_LINE);
  circle_list = glGenLists(1);
  glNewList(circle_list, GL_COMPILE);
  gluDisk(qobj, 0.0, 100.0, 20, 4);
  glEndList();
  
  complxd_set_angle(c, -M_PI_4 / 4);
  trackdisk_set_init_transf(td, c, vec2d_set(v, 256.0, 256.0), 1.0);
  trackdisk_init_transf(td);
  gl_transf_from_trackdisk(win->gl_transf, td);
}

static void
reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  gl_transf_set_viewport(win->gl_transf);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
  gl_transf_set_proj_mat(win->gl_transf);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glut_win_set_size(win, w, h);
  trackdisk_reshape(td, w, h);
}

static void
display(void) {
  unsigned int i;
  
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
  if (white_background) {
    glColor3f(0.0, 0.0, 0.0);
  } else {
    glColor3f(1.0, 1.0, 1.0);
  }
  /* Draw */
  glBegin(GL_POINTS);
  for (i = 0; i < points->len; i++) {
    glVertex2fv(g_array_index(points, Vec2f, i));
  }
  glEnd();
  if (display_disk) {
    glCallList(circle_list);
  }
  if (display_axes) {
    glCallList(axes_list);
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
    case 'c':
      white_background = (!white_background);
      break;
    case 'd':
      display_disk = (!display_disk);
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
    case 'h':
      display_help = (!display_help);
      break;
    case 'i':
      trackdisk_init_transf(td);
      gl_transf_from_trackdisk(win->gl_transf, td);
      break;
    case 'q':
    case 27:
      exit(0);
      break;
    case 's':
      report_speed = (!report_speed);
      break;
    default:
      fprintf(stderr, "Error: unknown option !\n");
      break;
  }
  glutPostRedisplay();
}

static void
mouse(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
          is_drawing = GL_TRUE;
        } else {
          trackdisk_start_rotation(td, x, y);
        }
      } else {
        if (glutGetModifiers() == GLUT_ACTIVE_CTRL) {
          is_drawing = GL_FALSE;
        } else {
          trackdisk_stop_rotation(td);
        }
      }
      break;
    case GLUT_MIDDLE_BUTTON:
      if (state == GLUT_DOWN) {
        trackdisk_start_scale(td, x, y);
      } else {
        trackdisk_stop_scale(td);
      }
      break;
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) {
        trackdisk_start_transl(td, x, y);
      } else {
        trackdisk_stop_transl(td);
      }
      break;
  }
}

static void
motion(int x, int y) {
  if (is_drawing) {
    static GLvecd obj = {0.0, 0.0, 0.0, 0.0};
    static GLboolean success = GL_FALSE;
    static Vec2f p = {0.0, 0.0};
    
    success = glut_win_unproject(win, x, y, 0.5, obj);
    /*success = glut_win_unproject4(win, x, y, 0.5, 1.0, 0.0, 1.0, obj);*/
    assert(success);
    vec2f_set(p, (float) obj[0], (float) obj[1]);
    g_array_append_val(points, p);
  } else {
    trackdisk_move(td, x, y);
    gl_transf_from_trackdisk(win->gl_transf, td);
  }
  glutPostRedisplay();
}

static void CALLBACKD
error_callback(void) {
  const GLubyte *error_string;
  
  error_string = gluErrorString(GLU_ERROR);
  fprintf(stderr, "Quadric Error: %s\n", error_string);
  exit(1);
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
