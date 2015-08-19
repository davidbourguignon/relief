#include <opengl_buffer.h>
#include <GL/glut.h>

typedef enum {
  SGI
} Format;

GLframebuf *image1 = NULL;
GLframebuf *image2 = NULL;
Format format;

static void
init(int argc, char **argv) {
  GLboolean is_read = GL_FALSE, is_written = GL_FALSE;
  
  if (argc != 3 || !strcmp(argv[1], "") ||
                   !strcmp(argv[2], "") || strcmp(argv[2], "SGI")) {
    fprintf(stdout, "Usage: opengl_buffer_io_test <image file> <format>\n"
                    "Format: SGI\n");
    exit(EXIT_SUCCESS);
  }
  if (!strcmp(argv[2], "SGI")) format = SGI;
  else assert(GL_FALSE);
  image1 = gl_framebuf_new();
  switch (format) {
    case SGI: is_read = gl_framebuf_sread(image1, argv[1], GL_FILE_SGI); break;
    default:                                                             break;
  }
  assert(is_read);
  fprintf(stdout, "Reading %s %i x %i pixels... done.\n",
          argv[1], image1->width, image1->height);
  switch (format) {
    case SGI:
      is_written = gl_framebuf_swrite(image1, "out.sgi", GL_FILE_SGI, GL_TRUE);
      fprintf(stdout, "Writing out.sgi %i x %i pixels... done.\n",
              image1->width, image1->height);
      break;
    default:
      break;
  }
  assert(is_written);
  image2 = gl_framebuf_new();
  switch (format) {
    case SGI:
      is_read = gl_framebuf_sread(image2, argv[1], GL_FILE_SGI);
      fprintf(stdout, "Reading out.sgi %i x %i pixels... done.\n",
              image2->width, image2->height);
      break;
    default:
      break;
  }
  assert(is_read);
}

static void
reshape(int w, int h) {
  /*
   * Integer coordinates for exact rasterization
   * OpenGL 1.2 Programming Guide
   * Chapter 10, Fig. 10-1, and Appendix H, p. 677
   */
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, 0, h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

static void
display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  
  /*
   * Testing rasterization of various geometric primitives
   */
  glPushAttrib(GL_CURRENT_BIT);
  
  /* polygon vertices and pixel image positions are placed
     at integer coordinates */
  glBegin(GL_POLYGON);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex2i(0, 0);
  glVertex2i(1, 0);
  glVertex2i(1, 1);
  glVertex2i(0, 1);
  glEnd();
  
  /* point vertices, line vertices, and bitmap positions are placed
     at half-integer coordinates */
  glBegin(GL_POINTS);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(0.5f, 1.5f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(1.5f, 0.5f);
  glEnd();
  
  glBegin(GL_LINES);
  glColor3f(0.0f, 0.0f, 1.0f);
  glVertex2f(0.5f, 3.5f);
  glVertex2f(0.5f, 8.5f);
  glColor3f(1.0f, 0.0f, 0.0f);
  glVertex2f(3.5f, 0.5f);
  glVertex2f(8.5f, 0.5f);
  glEnd();
  
  /* rasterizing point vertices with intermediate positions:
     correspond roughly to a cast from float to int */
  glBegin(GL_POINTS);
  glColor3f(0.5f, 0.5f, 0.5f);
  glVertex2f(10.50f, 0.5f);
  glVertex2f(12.00f, 0.5f);
  glVertex2f(14.01f, 0.5f);
  glVertex2f(16.25f, 0.5f);
  glVertex2f(18.75f, 0.5f);
  glVertex2f(20.96f, 0.5f); /* right! */
  glVertex2f(22.97f, 0.5f); /* wrong! */
  glEnd();
  
  glPopAttrib();
  
  /*
   * Displaying images
   */
  image1->x = 1;
  image1->y = 1;
  gl_framebuf_draw(image1, GL_BACK);
  image2->x = image1->width + 2;
  image2->y = 1;
  gl_framebuf_draw(image2, GL_BACK);
  
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
  glutInitWindowSize(2 * image1->width + 3, image1->height + 2);
  glutInitWindowPosition(128, 128);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA);
  glutCreateWindow(argv[0]);
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
