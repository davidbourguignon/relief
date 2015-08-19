#include "opengl_buffer.h"
#include <distmap.h>
#include <opengl_utils.h>

/*
 * GLframebuf definitions
 */
GLframebuf *
gl_framebuf_edt(GLframebuf *buf, double *euclidean_distance_max) {
  GLsizei width = 0, height = 0;
  GLubyte *buf_pixels = NULL;
  int size[2] = {0, 0};
  int inval = 0;
  int *pixels = NULL;
  int sqdistmax = 0;
  double distmax = 0.0;
  int i = 0, j = 0;
  
  assert(buf != NULL && buf->components == 1 && buf->type == GL_UNSIGNED_BYTE);
  width = buf->width;
  height = buf->height;
  buf_pixels = buf->pixels;
  size[0] = size[1] = scali_max(width, height);
  // inval is the maximum squared distance + 1
  inval = size[0]*size[0] + size[1]*size[1] + 1;
  // pixels is a square image initialized to 0 = outval
  pixels = (int *) calloc(size[0]*size[1], sizeof(int));
  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      if (buf_pixels[i + j*width] == GL_UBYTE_MAX) {
        pixels[i + j*size[0]] = inval;
      }
    }
  }
  
  { // IMPORTED CODE BEGIN
    distmap_4ssedp(pixels, size);
  } // IMPORTED CODE END
  
  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      int sqdist = pixels[i + j*width];
      if (sqdist > sqdistmax) {
        sqdistmax = sqdist;
      }
    }
  }
  distmax = sqrt(sqdistmax);
  for (j = 0; j < height; j++) {
    for (i = 0; i < width; i++) {
      buf_pixels[i + j*width]
        = (GLubyte) ((sqrt(pixels[i + j*size[0]]) / distmax) * GL_UBYTE_MAX);
    }
  }
  free(pixels);
  
  *euclidean_distance_max = distmax;
  return buf;
}
