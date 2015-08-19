#include <opengl_utils.h>

int
main(void) {
  GLfloat alpha = 1.0;
  GLint level = 0;
  GLenum internal_format = GL_RGBA8;
  GLsizei size = 512;
  gl_utils_build_normalization_cube_map(alpha, level, internal_format, size);
  return 0;
}
