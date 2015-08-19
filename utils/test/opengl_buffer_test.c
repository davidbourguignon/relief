#include <opengl_buffer.h>
#include <glut_widget.h>
#include <glut_utils.h>

typedef enum {
  WIREFRAME,
  ITEMBUF_1D,
  ITEMBUF_2D,
  PROJECTION
} RenderType;

/*
 * Data
 */
static const GLfloat HELP_BG_COLOR[4] = {0.0, 0.0, 0.0, 0.1};
static const GLfloat TEXT_COLOR[4]    = {1.0, 0.0, 0.0, 1.0};
static const char *HELP_TEXT[]
  = {"H e l p",
     "b - toggle 1D/2D item Buffer",
     "c - toggle simple/conservative item Buffer",
     "p - toggle Projection with/without item buffer visibility",
     "v - toggle Verbose item buffer",
     "",
     "f - toggle Full screen",
     "h - toggle Help",
     "i - reInitialize trackball",
     "s - toggle display Speed",
     "q - Quit",
     ""};
static const int HELP_TEXT_SIZE = sizeof(HELP_TEXT)/sizeof(char *);
static const unsigned int N_1D[1] = {18};         /* 3 cubes * 6 faces */
static const unsigned int N_2D[4] = {3, 6, 6, 6}; /* 3 cubes * 6 faces */
#define SIZE (0.25)
static const GLfloat vertices[24] /* 8 vertices * 3 coordinates */
  = {-SIZE, -SIZE,  SIZE,
     -SIZE, -SIZE, -SIZE,
     -SIZE,  SIZE, -SIZE,
     -SIZE,  SIZE,  SIZE,
      SIZE, -SIZE,  SIZE,
      SIZE, -SIZE, -SIZE,
      SIZE,  SIZE, -SIZE,
      SIZE,  SIZE,  SIZE};
static const GLubyte indices[6][4]
  = {{4, 5, 6, 7}, {0, 3, 2, 1},  /* +x, -x */
     {2, 3, 7, 6}, {0, 1, 5, 4},  /* +y, -y */
     {0, 4, 7, 3}, {1, 2, 6, 5}}; /* +z, -z */
#if defined(IS_BIG_ENDIAN)
static const GLubyte colors_1D[3][6][4]
  = {{{0, 0, 0,  0}, {0, 0, 0,  1}, {0, 0, 0,  2},
      {0, 0, 0,  3}, {0, 0, 0,  4}, {0, 0, 0,  5}},
     {{0, 0, 0,  6}, {0, 0, 0,  7}, {0, 0, 0,  8},
      {0, 0, 0,  9}, {0, 0, 0, 10}, {0, 0, 0, 11}},
     {{0, 0, 0, 12}, {0, 0, 0, 13}, {0, 0, 0, 14},
      {0, 0, 0, 15}, {0, 0, 0, 16}, {0, 0, 0, 17}}};
static const GLubyte colors_2D[3][6][4]
  = {{{0, 0, 0,  0}, {0, 1, 0,  0}, {0, 2, 0,  0},
      {0, 3, 0,  0}, {0, 4, 0,  0}, {0, 5, 0,  0}},
     {{0, 0, 0,  1}, {0, 1, 0,  1}, {0, 2, 0,  1},
      {0, 3, 0,  1}, {0, 4, 0,  1}, {0, 5, 0,  1}},
     {{0, 0, 0,  2}, {0, 1, 0,  2}, {0, 2, 0,  2},
      {0, 3, 0,  2}, {0, 4, 0,  2}, {0, 5, 0,  2}}};
#elif defined(IS_LITTLE_ENDIAN)
static const GLubyte colors_1D[3][6][4]
  = {{{ 0, 0, 0, 0}, { 1, 0, 0, 0}, { 2, 0, 0, 0},
      { 3, 0, 0, 0}, { 4, 0, 0, 0}, { 5, 0, 0, 0}},
     {{ 6, 0, 0, 0}, { 7, 0, 0, 0}, { 8, 0, 0, 0},
      { 9, 0, 0, 0}, {10, 0, 0, 0}, {11, 0, 0, 0}},
     {{12, 0, 0, 0}, {13, 0, 0, 0}, {14, 0, 0, 0},
      {15, 0, 0, 0}, {16, 0, 0, 0}, {17, 0, 0, 0}}};
static const GLubyte colors_2D[3][6][4]
  = {{{ 0, 0, 0, 0}, { 0, 0, 1, 0}, { 0, 0, 2, 0},
      { 0, 0, 3, 0}, { 0, 0, 4, 0}, { 0, 0, 5, 0}},
     {{ 1, 0, 0, 0}, { 1, 0, 1, 0}, { 1, 0, 2, 0},
      { 1, 0, 3, 0}, { 1, 0, 4, 0}, { 1, 0, 5, 0}},
     {{ 2, 0, 0, 0}, { 2, 0, 1, 0}, { 2, 0, 2, 0},
      { 2, 0, 3, 0}, { 2, 0, 4, 0}, { 2, 0, 5, 0}}};
#endif
static GLfloat projected_vertices[16]; /* 8 vertices * 2 coordinates */
#if DEBUG
static GLerror *err     = NULL;
#endif
static GLUTwin *win     = NULL;
static GLUTfps *fps     = NULL;
static Trackball *tb    = NULL;
static GLitembuf *ib_1D = NULL, *ib_2D = NULL;
static GLuint help_list = 0;
static GLboolean display_help = GL_FALSE;
static GLboolean full_screen  = GL_FALSE;
static GLboolean report_speed = GL_FALSE;
static GLboolean itembuf_1D   = GL_TRUE;
static GLboolean conservative = GL_FALSE;
static GLboolean project      = GL_TRUE;
static GLboolean verbose      = GL_TRUE;

/*
 * Functions declarations
 */
static void init    (int argc, char **argv);
static void reshape (int w, int h);
static void display (void);
static void keyboard(unsigned char key, int x, int y);
static void mouse   (int button, int state, int x, int y);
static void motion  (int x, int y);

static void draw_cube       (const int cube_index,
                             const RenderType render_type);
static void draw_three_cubes(const RenderType render_type);

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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
  win->id = glutCreateWindow(argv[0]);
  
  if (!glut_utils_gl_itembuf_supported()) {
    exit(1);
  } else {
    ib_1D = gl_itembuf_new(GL_ITEMBUF_1D);
    ib_2D = gl_itembuf_new(GL_ITEMBUF_2D);
  }
  
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  
  help_list = glGenLists(1);
  glNewList(help_list, GL_COMPILE);
  glut_widget_menu(HELP_TEXT, HELP_TEXT_SIZE,
                   GLUT_BITMAP_HELVETICA_18,
                   GLUT_BITMAP_HELVETICA_12,
                   GLUT_BITMAP_HELVETICA_10,
                   HELP_BG_COLOR, TEXT_COLOR);
  glEndList();
  
  quatd_set_axis_angle(q1, vec3d_set(v, 1.0, 0.0, 0.0), -M_PI_4 / 4);
  quatd_set_axis_angle(q2, vec3d_set(v, 0.0, 1.0, 0.0), +M_PI_4 / 2);
  trackball_set_init_transf(tb,
                            quatd_mult(q, q1, q2),
                            vec3d_set(v, 0.0, 0.0, -5.0));
  trackball_init_transf(tb);
  gl_transf_from_trackball(win->gl_transf, tb);
  
  gl_itembuf_set_items(ib_1D, N_1D);
  gl_itembuf_set_items(ib_2D, N_2D);
  
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor3f(0.0, 0.0, 0.0);
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
  gl_itembuf_set_port(ib_1D, win->gl_transf->viewport);
  gl_itembuf_set_port(ib_2D, win->gl_transf->viewport);
}

static void
display(void) {
  if (report_speed) {
    glut_fps_start(fps);
  }
  
  glPushMatrix();
  glMultMatrixd(win->gl_transf->modelview_matrix);
  
  if (itembuf_1D) {
    gl_itembuf_render_begin(ib_1D, GL_TRUE);
    draw_three_cubes(ITEMBUF_1D);
    gl_itembuf_render_end(ib_1D);
    gl_itembuf_reset_items(ib_1D);
    if (conservative) {
      gl_itembuf_conservative_lookup(ib_1D);
    } else {
      gl_itembuf_simple_lookup(ib_1D);
    }
    if (verbose) {
      fprintf(stdout, "Results for 1D itembuffer:\n");
      gl_itembuf_print(ib_1D, stdout);
      fprintf(stdout, "\n");
    }
  } else {
    gl_itembuf_render_begin(ib_2D, GL_TRUE);
    draw_three_cubes(ITEMBUF_2D);
    gl_itembuf_render_end(ib_2D);
    gl_itembuf_reset_items(ib_2D);
    if (conservative) {
      gl_itembuf_conservative_lookup(ib_2D);
    } else {
      gl_itembuf_simple_lookup(ib_2D);
    }
    if (verbose) {
      fprintf(stdout, "Results for 2D itembuffer:\n");
      gl_itembuf_print(ib_2D, stdout);
      fprintf(stdout, "\n");
    }
  }
  
  glDisable(GL_DITHER);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DITHER);
  
  if (project) {
    draw_three_cubes(PROJECTION);
  } else {
    draw_three_cubes(WIREFRAME);
  }
  
  glPopMatrix();
  
  if (report_speed) {
    glut_fps_stop(fps);
    glut_widget_string(fps->report_string,
                       GLUT_BITMAP_HELVETICA_18, TEXT_COLOR, 0.025, 0.025);
  }
  if (display_help) {
    glCallList(help_list);
  }
  
#if DEBUG
  (void) gl_error_report(err);
#endif
  
  glutSwapBuffers();
}

static void
keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'b':
      itembuf_1D = (!itembuf_1D);
      break;
    case 'c':
      conservative = (!conservative);
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
      trackball_init_transf(tb);
      gl_transf_from_trackball(win->gl_transf, tb);
      break;
    case 'p':
      project = (!project);
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
  glutPostRedisplay();
}

static void
draw_cube(const int cube_index, const RenderType render_type) {
  int i;
  
  if (render_type == PROJECTION) {
    GLvecd object_coord, window_coord;
    GLboolean success = GL_FALSE;
    
    gl_transf_set_mv_mat(win->gl_transf);
    for (i = 0; i < 8; i++) {
      gl_vecd_set(object_coord,
                  vertices[3*i], vertices[3*i + 1], vertices[3*i + 2], 1.0);
      success = gl_transf_project(win->gl_transf, object_coord, window_coord);
      assert(success);
      projected_vertices[2*i]     = window_coord[0];
      projected_vertices[2*i + 1] = window_coord[1];
    }
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(win->gl_transf->viewport[0], win->gl_transf->viewport[2],
               win->gl_transf->viewport[1], win->gl_transf->viewport[3]);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  }
  for (i = 0; i < 6; i++) {
    switch (render_type) {
      case WIREFRAME:
        glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, indices[i]);
        break;
      case ITEMBUF_1D:
        glColor4ubv(colors_1D[cube_index][i]);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indices[i]);
        break;
      case ITEMBUF_2D:
        glColor4ubv(colors_2D[cube_index][i]);
        glDrawElements(GL_QUADS, 4, GL_UNSIGNED_BYTE, indices[i]);
        break;
      case PROJECTION:
        if (itembuf_1D) {
          if (ib_1D->items._1D[cube_index*6 + i] != 0) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, indices[i]);
          }
        } else {
          if (ib_2D->items._2D[cube_index][i] != 0) {
            glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_BYTE, indices[i]);
          }
        }
        break;
    }
  }
  if (render_type == PROJECTION) {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
}

static void
draw_three_cubes(const RenderType render_type) {
  glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
  glPushAttrib(GL_TRANSFORM_BIT);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  if (render_type == PROJECTION) {
    glVertexPointer(2, GL_FLOAT, 0, projected_vertices);
  } else {
    glVertexPointer(3, GL_FLOAT, 0, vertices);
  }
  glMatrixMode(GL_MODELVIEW);
  
  glPushMatrix();
  glTranslatef(1.0, 0.0, 0.0); /* x */
  draw_cube(0, render_type);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0, 1.0, 0.0); /* y */
  draw_cube(1, render_type);
  glPopMatrix();
  
  glPushMatrix();
  glTranslatef(0.0, 0.0, 1.0); /* z */
  draw_cube(2, render_type);
  glPopMatrix();
  
  if (render_type == PROJECTION) {
    gl_transf_set_mv_mat(win->gl_transf);
  }
  
  glPopAttrib();
  glPopClientAttrib();
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
