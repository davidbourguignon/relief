#include "opengl_utils.h"
#ifdef OPENGL_UTILS_CUBE_MAP_TEST
#  include <opengl_buffer.h>
#endif
#include <math.h>

const GLvecd  GL_VECD_NULL  = {0.0,  0.0,  0.0,  0.0 };
const GLvecf  GL_VECF_NULL  = {0.0f, 0.0f, 0.0f, 0.0f};
const GLveci  GL_VECI_NULL  = {0,    0,    0,    0   };
const GLvecub GL_VECUB_NULL = {0,    0,    0,    0   };

const GLvecd GL_VECD_ORIGIN = {0.0,  0.0,  0.0,  1.0 };
const GLvecf GL_VECF_ORIGIN = {0.0f, 0.0f, 0.0f, 1.0f};

const GLmatd  GL_MATD_NULL  = {0.0,  0.0,  0.0,  0.0,
                               0.0,  0.0,  0.0,  0.0,
                               0.0,  0.0,  0.0,  0.0,
                               0.0,  0.0,  0.0,  0.0 };
const GLmatd  GL_MATD_ID    = {1.0,  0.0,  0.0,  0.0,
                               0.0,  1.0,  0.0,  0.0,
                               0.0,  0.0,  1.0,  0.0,
                               0.0,  0.0,  0.0,  1.0 };

const GLubyte  GL_UBYTE_MIN  =  0;
const GLubyte  GL_UBYTE_MAX  = ~0;
const GLushort GL_USHORT_MIN =  0;
const GLushort GL_USHORT_MAX = ~0;
const GLuint   GL_UINT_MIN   =  0u;
const GLuint   GL_UINT_MAX   = ~0u;
const GLclampf GL_CLAMPF_MIN =  0.0f;
const GLclampf GL_CLAMPF_MAX =  1.0f;
const GLclampd GL_CLAMPD_MIN =  0.0;
const GLclampd GL_CLAMPD_MAX =  1.0;

const GLfloat GL_DEPTH_NEAR = 0.0f;
const GLfloat GL_DEPTH_FAR  = 1.0f;

const GLuint GL_TEXTURE_NULL_NAME = 0;
const GLuint GL_LIST_NULL_NAME    = 0;

const GLvecf GL_PURE_BLACK   = {0.0f, 0.0f, 0.0f, 1.0f};
const GLvecf GL_PURE_WHITE   = {1.0f, 1.0f, 1.0f, 1.0f};
const GLvecf GL_PURE_GRAY    = {0.5f, 0.5f, 0.5f, 1.0f};
const GLvecf GL_PURE_RED     = {1.0f, 0.0f, 0.0f, 1.0f};
const GLvecf GL_PURE_GREEN   = {0.0f, 1.0f, 0.0f, 1.0f};
const GLvecf GL_PURE_BLUE    = {0.0f, 0.0f, 1.0f, 1.0f};
const GLvecf GL_PURE_CYAN    = {0.0f, 1.0f, 1.0f, 1.0f};
const GLvecf GL_PURE_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};
const GLvecf GL_PURE_YELLOW  = {1.0f, 1.0f, 0.0f, 1.0f};

const GLvecf GL_PASTEL_RED     = {1.0f, 0.6f, 0.6f, 1.0f};
const GLvecf GL_PASTEL_GREEN   = {0.6f, 1.0f, 0.6f, 1.0f};
const GLvecf GL_PASTEL_BLUE    = {0.6f, 0.6f, 1.0f, 1.0f};
const GLvecf GL_PASTEL_CYAN    = {0.6f, 1.0f, 1.0f, 1.0f};
const GLvecf GL_PASTEL_MAGENTA = {1.0f, 0.6f, 1.0f, 1.0f};
const GLvecf GL_PASTEL_YELLOW  = {1.0f, 1.0f, 0.6f, 1.0f};

const GLvecf GL_TRNSP_BLACK   = {0.0f, 0.0f, 0.0f, 0.4f};
const GLvecf GL_TRNSP_WHITE   = {1.0f, 1.0f, 1.0f, 0.4f};
const GLvecf GL_TRNSP_GRAY    = {0.5f, 0.5f, 0.5f, 0.4f};
const GLvecf GL_TRNSP_RED     = {1.0f, 0.0f, 0.0f, 0.4f};
const GLvecf GL_TRNSP_GREEN   = {0.0f, 1.0f, 0.0f, 0.4f};
const GLvecf GL_TRNSP_BLUE    = {0.0f, 0.0f, 1.0f, 0.4f};
const GLvecf GL_TRNSP_CYAN    = {0.0f, 1.0f, 1.0f, 0.4f};
const GLvecf GL_TRNSP_MAGENTA = {1.0f, 0.0f, 1.0f, 0.4f};
const GLvecf GL_TRNSP_YELLOW  = {1.0f, 1.0f, 0.0f, 0.4f};

/*
 * Local functions declarations
 */
INLINED GLboolean _gl_texture_set       (GLtexture *tex,
                                         GLsizei w, GLsizei h, GLsizei d,
                                         GLenum format);
INLINED void      _build_cube_map_vector(GLvecf v, int i, int x, int y,
                                         GLsizei size);

/*
 * Local functions definitions
 */
INLINED GLboolean
_gl_texture_set(GLtexture *tex, GLsizei w, GLsizei h, GLsizei d,
                GLenum format) {
  if (w != 0 && h != 0 && d != 0) {
    assert(
      (tex->target == GL_TEXTURE_1D && w%2 == 0 && h   == 1 && d   == 1) ||
      (tex->target == GL_TEXTURE_2D && w%2 == 0 && h%2 == 0 && d   == 1) ||
      (tex->target == GL_TEXTURE_3D && w%2 == 0 && h%2 == 0 && d%2 == 0));
    tex->width = w;
    tex->height = h;
    tex->depth = d;
    tex->nlevels = (GLint) scald_log2(scald_max(scald_max(w, h), d)) + 1;
    assert(format == GL_ALPHA           ||
           format == GL_LUMINANCE       ||
         /*format == GL_INTENSITY       ||*/
           format == GL_LUMINANCE_ALPHA ||
           format == GL_RGB             ||
           format == GL_RGBA            );
    tex->format = format;
    return GL_TRUE;
  } else {
    return GL_FALSE;
  }
}

INLINED void
_build_cube_map_vector(GLvecf v, int i, int x, int y, GLsizei size) {
  const GLfloat HALF_UNIT = 0.5f;
  const GLvecf GL_VECF_HALF_UNIT = {0.5f, 0.5f, 0.5f, 1.0f};
  /* s, t in the range [0 ; 1] */
  float s  = ((float) x + 0.5f)/(float) size; /* Offset to pixel center */
  float t  = ((float) y + 0.5f)/(float) size;
  /* sc, tc in the range [-1 ; +1] */
  float sc = s * 2.0f - 1.0f;
  float tc = t * 2.0f - 1.0f;
  /*
   * The target column in the table below explains how the major axis
   * direction maps to the 2D image of a particular cube map target.
   *
   *  major axis
   *  direction     target                             sc     tc    ma
   *  ----------    -------------------------------    ---    ---   ---
   *   +rx          TEXTURE_CUBE_MAP_POSITIVE_X_ARB    -rz    -ry   rx
   *   -rx          TEXTURE_CUBE_MAP_NEGATIVE_X_ARB    +rz    -ry   rx
   *   +ry          TEXTURE_CUBE_MAP_POSITIVE_Y_ARB    +rx    +rz   ry
   *   -ry          TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB    +rx    -rz   ry
   *   +rz          TEXTURE_CUBE_MAP_POSITIVE_Z_ARB    +rx    -ry   rz
   *   -rz          TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB    -rx    -ry   rz
   */
  switch (i) {
    case 0: v[0]=  1.0f; v[1]= -tc  ; v[2]= -sc  ; v[3]= 1.0f; break;
    case 1: v[0]= -1.0f; v[1]= -tc  ; v[2]=  sc  ; v[3]= 1.0f; break;
    case 2: v[0]=  sc  ; v[1]=  1.0f; v[2]=  tc  ; v[3]= 1.0f; break;
    case 3: v[0]=  sc  ; v[1]= -1.0f; v[2]= -tc  ; v[3]= 1.0f; break;
    case 4: v[0]=  sc  ; v[1]= -tc  ; v[2]=  1.0f; v[3]= 1.0f; break;
    case 5: v[0]= -sc  ; v[1]= -tc  ; v[2]= -1.0f; v[3]= 1.0f; break;
  }
  gl_vecf_normalize(v);
  /* v in the range [0 ; 1] */
  gl_vecf_addeq(gl_vecf_multeq(v, HALF_UNIT), GL_VECF_HALF_UNIT);
}

/*
 * GLmatd definitions
 */
GLboolean
gl_matd_invert(const GLmatd src, GLmatd inv) {
  /*
  ** Adapted from sgi opengl sample implementation __gluInvertMatrixd()
  */
  int i, j, k, swap;
  GLdouble t;
  GLdouble tmp[4][4];
  
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      tmp[i][j] = src[i*4+j];
    }
  }
  gl_matd_eq(inv, GL_MATD_ID);
  
  for (i = 0; i < 4; i++) {
    /* Look for largest element in column */
    swap = i;
    for (j = i + 1; j < 4; j++) {
      if (fabs(tmp[j][i]) > fabs(tmp[i][i])) {
        swap = j;
      }
    }
    
    if (swap != i) {
      /* Swap rows */
      for (k = 0; k < 4; k++) {
        t = tmp[i][k];
        tmp[i][k] = tmp[swap][k];
        tmp[swap][k] = t;
        
        t = inv[i*4+k];
        inv[i*4+k] = inv[swap*4+k];
        inv[swap*4+k] = t;
      }
    }
    
    if (tmp[i][i] == 0) {
      /*
       * No non-zero pivot. The matrix is singular, which shouldn't happen.
       * This means the user gave us a bad matrix.
       */
#if DEBUG
      fprintf(stderr, "Error: singular matrix!\n");
#endif
      return GL_FALSE;
    }
    
    t = tmp[i][i];
    for (k = 0; k < 4; k++) {
      tmp[i][k] /= t;
      inv[i*4+k] /= t;
    }
    for (j = 0; j < 4; j++) {
      if (j != i) {
        t = tmp[j][i];
        for (k = 0; k < 4; k++) {
          tmp[j][k] -= tmp[i][k]*t;
          inv[j*4+k] -= inv[i*4+k]*t;
        }
      }
    }
  }
  return GL_TRUE;
}

GLmatd_t
gl_matd_from_complxd(GLmatd m, const Complxd c) {
  m[0]  = +c[0];
  m[1]  = +c[1];
  m[2]  = 0.0;
  m[3]  = 0.0;
  
  m[4]  = -c[1];
  m[5]  = +c[0];
  m[6]  = 0.0;
  m[7]  = 0.0;
  
  m[8]  = 0.0;
  m[9]  = 0.0;
  m[10] = 1.0;
  m[11] = 0.0;
  
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
  
  return m;
}

GLmatd_t
gl_matd_from_quatd(GLmatd m, const Quatd q) {
  m[0]  = 1.0 - 2.0*(q[2]*q[2] + q[3]*q[3]);
  m[1]  =       2.0*(q[1]*q[2] - q[0]*q[3]);
  m[2]  =       2.0*(q[1]*q[3] + q[0]*q[2]);
  m[3]  = 0.0;
  
  m[4]  =       2.0*(q[1]*q[2] + q[0]*q[3]);
  m[5]  = 1.0 - 2.0*(q[3]*q[3] + q[1]*q[1]);
  m[6]  =       2.0*(q[2]*q[3] - q[0]*q[1]);
  m[7]  = 0.0;
  
  m[8]  =       2.0*(q[1]*q[3] - q[0]*q[2]);
  m[9]  =       2.0*(q[2]*q[3] + q[0]*q[1]);
  m[10] = 1.0 - 2.0*(q[2]*q[2] + q[1]*q[1]);
  m[11] = 0.0;
  
  m[12] = 0.0;
  m[13] = 0.0;
  m[14] = 0.0;
  m[15] = 1.0;
  
  return m;
}

/*
 * GLtransf definitions
 */
GLboolean
gl_transf_invert(GLtransf *trsf) {
  if (trsf->changed) {
    if (trsf->projection_changed) {
      if (!gl_matd_invert(trsf->projection_matrix,
                          trsf->projection_matrix_inverse)) {
#if DEBUG
        fprintf(stderr, "Error: gl_matd_invert(projection_matrix) failure!\n");
#endif
        return GL_FALSE;
      }
      trsf->projection_changed = GL_FALSE;
    }
    if (trsf->modelview_changed) {
      if (!gl_matd_invert(trsf->modelview_matrix,
                          trsf->modelview_matrix_inverse)) {
#if DEBUG
        fprintf(stderr, "Error: gl_matd_invert(modelview_matrix) failure!\n");
#endif
        return GL_FALSE;
      }
      trsf->modelview_changed = GL_FALSE;
    }
    gl_matd_multm(trsf->final_matrix, trsf->projection_matrix,
                                      trsf->modelview_matrix);
    if (!gl_matd_invert(trsf->final_matrix,
                        trsf->final_matrix_inverse)) {
#if DEBUG
      fprintf(stderr, "Error: gl_matd_invert(final_matrix) failure!\n");
#endif
      return GL_FALSE;
    }
    trsf->changed = GL_FALSE;
  }
  return GL_TRUE;
}

GLboolean
gl_transf_unproject(GLtransf *trsf, const GLvecd win, GLvecd obj) {
  /*
  ** Adapted from sgi opengl sample implementation gluUnProject()
  */
  GLvecd in, out;
  
  if (!gl_transf_invert(trsf)) {
#if DEBUG
    fprintf(stderr, "Error: gl_transf_invert() failure!\n");
#endif
    return GL_FALSE;
  }
  gl_vecd_set(in, win[0], win[1], win[2], 1.0);
  /* Map x and y from window coordinates */
  in[0] = (in[0] - trsf->viewport[0]) / trsf->viewport[2];
  in[1] = (in[1] - trsf->viewport[1]) / trsf->viewport[3];
  /* Map to range [-1, 1] */
  in[0] = in[0] * 2 - 1;
  in[1] = in[1] * 2 - 1;
  in[2] = in[2] * 2 - 1;
  gl_matd_multv(out, trsf->final_matrix_inverse, in);
  if (out[3] == 0.0) {
#if DEBUG
    fprintf(stderr, "Error: invalid w coordinate value!\n");
#endif
    return GL_FALSE;
  }
  out[0] /= out[3];
  out[1] /= out[3];
  out[2] /= out[3];
  gl_vecd_set(obj, out[0], out[1], out[2], 1.0);
  return GL_TRUE;
}

GLboolean
gl_transf_unproject4(GLtransf *trsf, const GLvecd win,
                     GLclampd z_near, GLclampd z_far, GLvecd obj) {
  /*
  ** Adapted from sgi opengl sample implementation gluUnProject4()
  */
  GLvecd in, out;
  
  if (!gl_transf_invert(trsf)) {
#if DEBUG
    fprintf(stderr, "Error: gl_transf_invert() failure!\n");
#endif
    return GL_FALSE;
  }
  gl_vecd_eq(in, win);
  /* Map x and y from window coordinates */
  in[0] = (in[0] - trsf->viewport[0]) / trsf->viewport[2];
  in[1] = (in[1] - trsf->viewport[1]) / trsf->viewport[3];
  in[2] = (in[2] - z_near) / (z_far - z_near);
  /* Map to range [-1, 1] */
  in[0] = in[0] * 2 - 1;
  in[1] = in[1] * 2 - 1;
  in[2] = in[2] * 2 - 1;
  gl_matd_multv(out, trsf->final_matrix_inverse, in);
  if (out[3] == 0.0) {
#if DEBUG
    fprintf(stderr, "Error: invalid w coordinate value!\n");
#endif
    return GL_FALSE;
  }
  gl_vecd_eq(obj, out);
  return GL_TRUE;
}

GLboolean
gl_transf_project(GLtransf *trsf, const GLvecd obj, GLvecd win) {
  /*
  ** Adapted from sgi opengl sample implementation gluProject()
  */
  GLvecd in, out;
  
  gl_vecd_set(in, obj[0], obj[1], obj[2], 1.0);
  /* Inversion is not needed here: small overhead! */
  if (!gl_transf_invert(trsf)) {
#if DEBUG
    fprintf(stderr, "Error: gl_transf_invert() failure!\n");
#endif
    return GL_FALSE;
  }
  gl_matd_multv(out, trsf->final_matrix, in);
  if (out[3] == 0.0) {
#if DEBUG
    fprintf(stderr, "Error: invalid w coordinate value!\n");
#endif
    return GL_FALSE;
  }
  out[0] /= out[3];
  out[1] /= out[3];
  out[2] /= out[3];
  /* Map x, y and z to range [0, 1] */
  out[0] = out[0] * 0.5 + 0.5;
  out[1] = out[1] * 0.5 + 0.5;
  out[2] = out[2] * 0.5 + 0.5;
  /* Map x, y to viewport */
  out[0] = out[0] * trsf->viewport[2] + trsf->viewport[0];
  out[1] = out[1] * trsf->viewport[3] + trsf->viewport[1];
  gl_vecd_set(win, out[0], out[1], out[2], 1.0);
  return GL_TRUE;
}

GLtransf *
gl_transf_get_clip_dist(GLtransf *trsf, const GLvecd min, const GLvecd max,
                        GLdouble *z_near, GLdouble *z_far) {
  const GLdouble EPSILON = 0.1;
  GLdouble radius, secure_radius, z_center, z_near_val, z_far_val;
  GLvecd diff, center_w, center_c;
  
  /* Bounding box radius */
  radius = 0.5 * gl_vecd_norm(gl_vecd_sub(diff, max, min));
  /* Bounding box center */
  gl_vecd_diveq(gl_vecd_add(center_w, max, min), 2.0);
  /* From world frame to camera frame */
  gl_matd_multv(center_c, trsf->modelview_matrix, center_w);
  /* distance = center +- radius (with security margin) */
  z_center = - center_c[2] / center_c[3];
  secure_radius = radius * (1.0 + EPSILON);
  z_near_val = z_center - secure_radius;
  z_far_val  = z_center + secure_radius;
  if (z_near_val > 0.0) {
    *z_near = z_near_val;
  } else {
    *z_near = EPSILON;
  }
  if (z_far_val > 0.0) {
    *z_far = z_far_val;
  } else {
    *z_far = 2.0 * EPSILON;
  }
  return trsf;
}

int
gl_transf_print(const GLtransf *trsf, FILE *stream) {
  int result_v = 0, result_p = 0, result_m = 0;
  
  fprintf(stream, "Viewport\n");
  result_v = gl_veci_print(trsf->viewport, stream);
  fprintf(stream, "\nProjection matrix\n");
  result_p = gl_matd_print(trsf->projection_matrix, stream);
  fprintf(stream, "\nModelview matrix\n");
  result_m = gl_matd_print(trsf->modelview_matrix, stream);
  if (result_v < 0) {
    return result_v;
  } else if (result_p < 0) {
    return result_p;
  } else if (result_m < 0) {
    return result_m;
  } else {
    return (result_v + result_p + result_m);
  }
}

/*
 * GLtexture definitions
 */
GLboolean
gl_texture_set(GLtexture *tex, GLenum target,
               GLboolean (*func)(void *, GLboolean), void *data) {
  GLint width = 0, height = 0, depth = 0, format = 0;
  
  gl_texture_clear(tex);
  assert(target == GL_TEXTURE_1D ||
         target == GL_TEXTURE_2D ||
         target == GL_TEXTURE_3D);
  tex->target = target;
  assert(func != NULL);
  tex->setup = func;
  tex->setup_data = data;
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPushAttrib(GL_TEXTURE_BIT);
  
  if (!tex->setup(tex->setup_data, GL_TRUE)) return GL_FALSE;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &(tex->name));
  if (tex->name != GL_TEXTURE_NULL_NAME) {
    glBindTexture(tex->target, tex->name);
  } else {
    return GL_FALSE;
  }
  tex->setup(tex->setup_data, GL_FALSE);
  if (!gl_texture_release(tex)) return GL_FALSE;
  if (!gl_texture_bind(tex)) return GL_FALSE;
  glGetTexLevelParameteriv(tex->target, 0, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(tex->target, 0, GL_TEXTURE_HEIGHT, &height);
  glGetTexLevelParameteriv(tex->target, 0, GL_TEXTURE_DEPTH, &depth);
  glGetTexLevelParameteriv(tex->target, 0, GL_TEXTURE_INTERNAL_FORMAT,
                           &format);
  if (!_gl_texture_set(tex, width, height, depth, format)) return GL_FALSE;
  
  glPopAttrib();
  glPopClientAttrib();
  return GL_TRUE;
}

/*
 * OpenGL utils definitions
 */
GLboolean
gl_utils_is_extension_supported(const char *name) {
  /*
  ** Adapted from "All About OpenGL Extensions" by Mark Kilgard
  ** (http://www.opengl.org)
  */
  const GLubyte *extensions = NULL;
  const GLubyte *start = NULL;
  GLubyte *where = NULL;
  GLubyte *terminator = NULL;
  
  assert(!strcmp(name, "") && strchr(name, ' ') == NULL); /* fool-proof */
  extensions = glGetString(GL_EXTENSIONS);
  start = extensions;
  for (;;) {
    where = (GLubyte *) strstr((const char *) start, name);
    if (where == NULL) {
      break;
    }
    terminator = where + strlen(name);
    if (where == start || *(where - 1) == ' ') {
      if (*terminator == ' ' || *terminator == '\0') {
        return GL_TRUE;
      }
    }
    start = terminator;
  }
  return GL_FALSE;
}

void
gl_utils_build_normalization_cube_map(GLfloat alpha, GLint level,
                                      GLenum internal_format, GLsizei size) {
#if defined(GL_ARB_texture_cube_map) || defined(GL_EXT_texture_cube_map)
  const GLenum TEXTURE_CUBE_MAP_ID[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,
  };
  const int TEXTURE_CUBE_MAP_ID_SIZE
    = sizeof(TEXTURE_CUBE_MAP_ID)/sizeof(GLenum);
  const GLubyte GL_UBYTE_MAX = ~0;
  GLint components = 4;
  GLenum format    = GL_RGBA;
  GLenum type      = GL_UNSIGNED_BYTE;
  GLubyte *pixels  = (GLubyte *)malloc(size*size*components*sizeof(GLubyte));
  GLvecf v;
  int i, x, y;
#if OPENGL_UTILS_CUBE_MAP_TEST
  GLframebuf *image = gl_framebuf_new();
  char name[80];
#endif
  
  assert(pixels != NULL);
#if OPENGL_UTILS_CUBE_MAP_TEST
  image->width = image->height = size;
  image->components = components;
  image->format = format;
  image->type = type;
  image->pixels = pixels;
#endif
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (i = 0; i < TEXTURE_CUBE_MAP_ID_SIZE; i++) {
    for (y = 0; y < size; y++) {
      for (x = 0; x < size; x++) {
        _build_cube_map_vector(v, i, x, y, size);
        /* pixels in the range [0 ; GL_UBYTE_MAX] */
        pixels[components*(y*size + x) + 0] = (GLubyte) (GL_UBYTE_MAX*v[0]);
        pixels[components*(y*size + x) + 1] = (GLubyte) (GL_UBYTE_MAX*v[1]);
        pixels[components*(y*size + x) + 2] = (GLubyte) (GL_UBYTE_MAX*v[2]);
        pixels[components*(y*size + x) + 3] = (GLubyte) (GL_UBYTE_MAX*alpha);
      }
    }
#if OPENGL_UTILS_CUBE_MAP_TEST
    sprintf(name, "out%d.rgb", i);
    gl_framebuf_swrite(image, name, GL_FILE_SGI, GL_TRUE);
#endif
    glTexImage2D(TEXTURE_CUBE_MAP_ID[i],
                 level, internal_format, size, size, 0, format, type, pixels);
  }
  free(pixels);
#endif /* GL_ARB_texture_cube_map || GL_EXT_texture_cube_map */
}
