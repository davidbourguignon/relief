#include <opengl_widget.h>
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
     "a - toggle Axes",
     "b - toggle Ball",
     "c - change background Color",
     "d - adapt Distance to depth clipping planes",
     "f - toggle Full screen",
     "g - toggle Grid",
     "h - toggle Help",
     "i - reInitialize trackball",
     "s - toggle display Speed",
     "q - Quit",
     "t - toggle Teapot",
     "x - toggle bounding boX",
     ""};
static const int HELP_TEXT_SIZE = sizeof(HELP_TEXT)/sizeof(char *);
#define Z_NEAR_DEFAULT 1.0
#define Z_FAR_DEFAULT 10.0
static const GLdouble FOVY = 45.0;
#if DEBUG
static GLerror *err  = NULL;
#endif
static GLUTwin *win  = NULL;
static GLUTfps *fps  = NULL;
static Trackball *tb = NULL;
static GLuint light_help_list = 0, dark_help_list = 0;
static GLuint axes_list = 0, sphere_list = 0;
static GLuint box_list = 0, grid_list = 0, teapot_list = 0;
static GLvecd min, max;
static GLdouble z_near = Z_NEAR_DEFAULT, z_far = Z_FAR_DEFAULT;
static GLboolean white_background = GL_TRUE;
static GLboolean display_help     = GL_FALSE;
static GLboolean full_screen      = GL_FALSE;
static GLboolean report_speed     = GL_FALSE;
static GLboolean display_axes     = GL_TRUE;
static GLboolean display_ball     = GL_FALSE;
static GLboolean display_box      = GL_FALSE;
static GLboolean display_grid     = GL_FALSE;
static GLboolean display_teapot   = GL_FALSE;
static GLboolean adapt_clip_dist  = GL_FALSE;

/*
 * Functions declarations
 */
static void init    (int argc, char **argv);
static void reshape (int w, int h);
static void display (void);
static void keyboard(unsigned char key, int x, int y);
static void mouse   (int button, int state, int x, int y);
static void motion  (int x, int y);

static void set_projection_matrix(void);

/*
 * Functions definitions
 */
static void
init(int argc, char **argv) {
  Quatd q, q1, q2;
  Vec3d v;
  
#if DEBUG
  err = gl_error_new();
#endif
  win = glut_win_new();
  fps = glut_fps_new();
  tb = trackball_new();
  
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
  glut_widget_axes();
  glEndList();
  
  sphere_list = glGenLists(1);
  glNewList(sphere_list, GL_COMPILE);
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_DEPTH_TEST);
  glutWireSphere(1.0, 10, 10);
  glPopAttrib();
  glEndList();
  
  box_list = glGenLists(1);
  glNewList(box_list, GL_COMPILE);
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POLYGON_BIT);
  glEnable(GL_DEPTH_TEST);
  glColor4fv(GL_PURE_RED);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  gl_widget_box(6.0, 3.0, 6.0);
  glPopAttrib();
  glEndList();
  
  grid_list = glGenLists(1);
  glNewList(grid_list, GL_COMPILE);
  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_DEPTH_TEST);
  gl_widget_grid(5.0, 5.0, 10, 10);
  glPopAttrib();
  glEndList();
  
  teapot_list = glGenLists(1);
  glNewList(teapot_list, GL_COMPILE);
  glut_widget_teapot(2.0);
  glEndList();
  
  quatd_set_axis_angle(q1, vec3d_set(v, 1.0, 0.0, 0.0), -M_PI_4 / 4);
  quatd_set_axis_angle(q2, vec3d_set(v, 0.0, 1.0, 0.0), +M_PI_4 / 2);
  trackball_set_init_transf(tb, quatd_mult(q, q1, q2),
                                vec3d_set(v, 0.0, 0.0, -5.0));
  trackball_init_transf(tb);
  gl_transf_from_trackball(win->gl_transf, tb);
  
  gl_vecd_set(min, -5.0, -5.0, -5.0, 1.0);
  gl_vecd_set(max,  5.0,  5.0,  5.0, 1.0);
}

static void
reshape(int w, int h) {
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  gl_transf_set_viewport(win->gl_transf);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOVY, (GLdouble) w / (GLdouble) h, z_near, z_far);
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
  if (white_background) {
    glColor3f(0.0, 0.0, 0.0);
  } else {
    glColor3f(1.0, 1.0, 1.0);
  }
  if (display_axes) {
    glCallList(axes_list);
  }
  if (display_ball) {
    glCallList(sphere_list);
  }
  if (display_box) {
    glCallList(box_list);
  }
  if (display_grid) {
    glCallList(grid_list);
  }
  if (display_teapot) {
    glCallList(teapot_list);
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
      display_ball = (!display_ball);
      break;
    case 'c':
      white_background = (!white_background);
      break;
    case 'd':
      if (adapt_clip_dist) {
        adapt_clip_dist = GL_FALSE;
        z_near = Z_NEAR_DEFAULT;
        z_far = Z_FAR_DEFAULT;
        set_projection_matrix();
      } else {
        adapt_clip_dist = GL_TRUE;
      }
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
      display_grid = (!display_grid);
      break;
    case 'h':
      display_help = (!display_help);
      break;
    case 'i':
      trackball_init_transf(tb);
      gl_transf_from_trackball(win->gl_transf, tb);
      break;
    case 'q':
    case 27:
      exit(0);
      break;
    case 's':
      report_speed = (!report_speed);
      break;
    case 't':
      display_teapot = (!display_teapot);
      break;
    case 'x':
      display_box = (!display_box);
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
  if (adapt_clip_dist) {
    gl_transf_get_clip_dist(win->gl_transf, min, max, &z_near, &z_far);
    set_projection_matrix();
  }
  glutPostRedisplay();
}

static void
set_projection_matrix(void) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(FOVY,
                 (GLdouble) win->gl_transf->viewport[2] /
                 (GLdouble) win->gl_transf->viewport[3],
                 z_near, z_far);
  gl_transf_set_proj_mat(win->gl_transf);
  glMatrixMode(GL_MODELVIEW);
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
