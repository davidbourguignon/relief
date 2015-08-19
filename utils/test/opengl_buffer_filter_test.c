#include <opengl_buffer.h>
#include <GL/glut.h>

typedef enum {
  EDT,
  HT
} Filter;

GLframebuf *image_orig = NULL;
GLframebuf *image_dest = NULL;
Filter filter;

static void
init(int argc, char **argv) {
  GLboolean is_read = GL_FALSE, is_written = GL_FALSE;
  double euclidean_distance_max = 0.0;
  
  if (argc != 3 || !strcmp(argv[1], "") ||
                   !strcmp(argv[2], "") || (strcmp(argv[2], "EDT") &&
                                            strcmp(argv[2], "HT"))) {
    fprintf(stdout, "Usage: opengl_buffer_filter_test <sgi image file> <filter>\n"
                    "Filter: EDT (euclidean distance transform)\n"
                    "        HT  (halftoning)\n");
    exit(EXIT_SUCCESS);
  }
  if (!strcmp(argv[2], "EDT")) filter = EDT;
  else if (!strcmp(argv[2], "HT")) filter = HT;
  else assert(GL_FALSE);
  image_orig = gl_framebuf_new();
  is_read = gl_framebuf_sread(image_orig, argv[1], GL_FILE_SGI);
  assert(is_read);
  switch (filter) {
    case EDT:
    case HT:
      if (image_orig->components != 1) {
        fprintf(stderr, "Error: not a grayscale image!\n");
        exit(EXIT_FAILURE);
      }
      break;
    default:
      break;
  }
  fprintf(stdout, "Reading %s %i x %i pixels... done.\n",
          argv[1], image_orig->width, image_orig->height);
  image_dest = gl_framebuf_new();
  is_read = gl_framebuf_sread(image_dest, argv[1], GL_FILE_SGI);
  assert(is_read);
  switch (filter) {
    case EDT:
      gl_framebuf_edt(image_dest, &euclidean_distance_max);
      fprintf(stdout, "Euclidean distance max %lf\n", euclidean_distance_max);
      break;
    case HT:
      gl_framebuf_ht(image_dest);
      break;
    default:
      break;
  }
  is_written = gl_framebuf_swrite(image_dest, "out.sgi", GL_FILE_SGI, GL_TRUE);
  assert(is_written);
}

static void
reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /* Integer coordinates for exact rasterization
     OpenGL 1.2 Programming Guide, p. 677 */
  gluOrtho2D(0, w, 0, h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

static void
display(void) {
  /* Testing origin */
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glRecti(0, 0, 1, 1);
  glColor3f(0.0, 0.0, 0.0);
  
  image_orig->x = 1;
  image_orig->y = 1;
  gl_framebuf_draw(image_orig, GL_BACK);
  image_dest->x = image_orig->width + 2;
  image_dest->y = 1;
  gl_framebuf_draw(image_dest, GL_BACK);
  glFlush();
}

static void
keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q': exit(EXIT_SUCCESS); break;
    default:                      break;
  }
}

int
main(int argc, char **argv) {
  glutInit(&argc, argv);
  init(argc, argv);
  glutInitWindowSize(2 * image_orig->width + 3, image_orig->height + 2);
  glutInitWindowPosition(128, 128);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA);
  glutCreateWindow(argv[0]);
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
