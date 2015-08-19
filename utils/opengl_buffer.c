#include "opengl_buffer.h"

const GLuint  GL_FRAMEBUF_NULL_INDEX    =  ~0;
const GLuint  GL_ITEMBUF_NULL_ID        =  ~0;
const GLvecub GL_ITEMBUF_NULL_COLOR     = {~0, ~0, ~0, ~0};
const GLuint  GL_SELECTBUF_DEFAULT_NAME =  ~0;

const GLsizei GL_2D_SIZE               = 2;
const GLsizei GL_3D_SIZE               = 3;
const GLsizei GL_3D_COLOR_SIZE         = 3 + 4; /* coords + RGBA color */
const GLsizei GL_3D_COLOR_TEXTURE_SIZE = 7 + 4;
const GLsizei GL_4D_COLOR_TEXTURE_SIZE = 8 + 4;

/*
 * GLframebuf definitions
 */
GLframebuf *
gl_framebuf_eq(GLframebuf *buf, const GLframebuf *buf_src,
               GLboolean copy_pixels) {
  GLveci port;
  gl_veci_set(port, buf_src->x, buf_src->y, buf_src->width, buf_src->height);
  gl_framebuf_set_format(buf, buf_src->format);
  buf->type = buf_src->type;
  buf->pixels = NULL;
  gl_framebuf_set_port(buf, port);
  if (copy_pixels) {
    switch (buf->type) {
      case GL_UNSIGNED_BYTE:
        memcpy(buf->pixels, buf_src->pixels,
               buf->width*buf->height*buf->components*sizeof(GLubyte));
        break;
      case GL_FLOAT:
        memcpy(buf->pixels, buf_src->pixels,
               buf->width*buf->height*buf->components*sizeof(GLfloat));
        break;
      default:
        fprintf(stderr, "Error: Unsupported type!\n");
        assert(buf->type == GL_UNSIGNED_BYTE || buf->type == GL_FLOAT);
        break;
    }
  }
  return buf;
}

GLframebuf *
gl_framebuf_set_port(GLframebuf *buf, const GLveci port) {
  assert(port[0] > -1 && port[1] > -1 && port[2] > 0 && port[3] > 0);
  buf->x = port[0];
  buf->y = port[1];
  if (buf->width != port[2] || buf->height != port[3]) {
    buf->width = port[2];
    buf->height = port[3];
    switch (buf->type) {
      case GL_UNSIGNED_BYTE:
        buf->pixels
          = (GLubyte *) realloc(buf->pixels,
                                buf->width*buf->height*buf->components*
                                sizeof(GLubyte));
        break;
      case GL_FLOAT:
        buf->pixels
          = (GLfloat *) realloc(buf->pixels,
                                buf->width*buf->height*buf->components*
                                sizeof(GLfloat));
        break;
      default:
        fprintf(stderr, "Error: Unsupported type!\n");
        assert(buf->type == GL_UNSIGNED_BYTE || buf->type == GL_FLOAT);
        break;
    }
    assert(buf->pixels != NULL);
  }
  return buf;
}

size_t
gl_framebuf_size(const GLframebuf *buf) {
  size_t size = buf->width*buf->height*buf->components;
  assert(size != 0);
  switch (buf->type) {
    case GL_UNSIGNED_BYTE:
      size *= sizeof(GLubyte);
      break;
    case GL_FLOAT:
      size *= sizeof(GLfloat);
      break;
    default:
      fprintf(stderr, "Error: Unsupported type!\n");
      assert(buf->type == GL_UNSIGNED_BYTE || buf->type == GL_FLOAT);
      break;
  }
  return size;
}

GLframebuf *
gl_framebuf_read(GLframebuf *buf, GLenum mode) {
  assert(buf->pixels != NULL);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPushAttrib(GL_PIXEL_MODE_BIT);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  if (buf->format == GL_LUMINANCE || buf->format == GL_LUMINANCE_ALPHA) {
#if OPENGL_FRAMEBUF_USE_NTSC_STD
    /* US National Television Standard Committee (NTSC) luminance */
    glPixelTransferf(GL_RED_SCALE,   0.299f);
    glPixelTransferf(GL_GREEN_SCALE, 0.587f);
    glPixelTransferf(GL_BLUE_SCALE,  0.114f);
#elif OPENGL_FRAMEBUF_USE_CIE_STD
    /*
     * Contemporary CRT phosphors are standardized in Rec. 709. The weights
     * to compute true CIE luminance from linear red, green and blue, for the
     * Rec. 709, are these:
     *   Y = 0.212671 * R + 0.715160 * G + 0.072169 * B
     */
    glPixelTransferf(GL_RED_SCALE,   0.212671f);
    glPixelTransferf(GL_GREEN_SCALE, 0.715160f);
    glPixelTransferf(GL_BLUE_SCALE,  0.072169f);
#else
    glPixelTransferf(GL_RED_SCALE,   0.3333f);
    glPixelTransferf(GL_GREEN_SCALE, 0.3334f);
    glPixelTransferf(GL_BLUE_SCALE,  0.3333f);
#endif
  }
  glReadBuffer(mode);
  glReadPixels(buf->x, buf->y, buf->width, buf->height,
               buf->format, buf->type, buf->pixels);
  glPopAttrib();
  glPopClientAttrib();
  return buf;
}

/*
 * GLitembuf definitions
 */
void
gl_itembuf_delete(GLitembuf *buf) {
  assert(buf != NULL);
  gl_framebuf_delete(buf->gl_framebuf);
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    free(buf->items._1D);
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    if (buf->nitems._2D != NULL && buf->items._2D != NULL) {
      GLuint i;
      for (i = 0; i < buf->nitems._2D[0]; i++) {
        free(buf->items._2D[i]);
      }
    }
    free(buf->nitems._2D);
    free(buf->items._2D);
  }
  free(buf);
#if DEBUG
  buf = NULL;
#endif
}

GLitembuf *
gl_itembuf_set_items(GLitembuf *buf, const GLuint *n) {
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    if (buf->nitems._1D != n[0]) {
      const unsigned long MAX = ~0;
      assert(n[0] < MAX);
      buf->nitems._1D = n[0];
      if (buf->items._1D != NULL) {
        free(buf->items._1D);
#if DEBUG
        buf->items._1D = NULL;
#endif
      }
      buf->items._1D = (GLuint *) calloc(n[0], sizeof(GLuint));
      assert(buf->items._1D != NULL);
    } else {
      gl_itembuf_reset_items(buf);
    }
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    /* We could be even more subtle by setting only the subarrays that have
       changed... */
    int nitems_2D_changed = 0;
    GLuint i;
    if (buf->nitems._2D == NULL) {
      nitems_2D_changed = 1; /* True */
    } else {
      if (buf->nitems._2D[0] != n[0]) {
        nitems_2D_changed = 1; /* True */
      } else {
        nitems_2D_changed = memcmp(buf->nitems._2D, n,
                                   (n[0] + 1)*sizeof(GLuint));
      }
    }
    if (nitems_2D_changed) {
      const unsigned short MAX = ~0;
      assert(n[0] < MAX);
#if DEBUG
      for (i = 0; i < n[0]; i++) {
        assert(n[i+1] < MAX);
      }
#endif
      buf->nitems._2D
        = (GLuint *) realloc(buf->nitems._2D, (n[0] + 1)*sizeof(GLuint));
      assert(buf->nitems._2D != NULL);
      memcpy(buf->nitems._2D, n, (n[0] + 1)*sizeof(GLuint));
      if (buf->items._2D != NULL) {
        for (i = 0; i < buf->nitems._2D[0]; i++) {
          if (buf->items._2D[i] != NULL) {
            free(buf->items._2D[i]);
#if DEBUG
            buf->items._2D[i] = NULL;
#endif
          }
        }
        free(buf->items._2D);
#if DEBUG
        buf->items._2D = NULL;
#endif
      }
      buf->items._2D = (GLuint **) calloc(n[0], sizeof(GLuint *));
      assert(buf->items._2D != NULL);
      for (i = 0; i < buf->nitems._2D[0]; i++) {
        buf->items._2D[i] = (GLuint *) calloc(n[i+1], sizeof(GLuint));
        assert(buf->items._2D[i] != NULL);
      }
    } else {
      gl_itembuf_reset_items(buf);
    }
  }
  return buf;
}

GLitembuf *
gl_itembuf_simple_lookup(GLitembuf *buf) {
  /*
   * We simply count the number of pixels corresponding to each color ID.
   * Fast but sometimes wrong due to rasterization errors.
   */
  GLsizei size = buf->gl_framebuf->width * buf->gl_framebuf->height;
  GLuint *pixels = (GLuint *) buf->gl_framebuf->pixels;
  int i;
  
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    for (i = 0; i < size; i++) {
      GLuint id = pixels[i];
      if (id != GL_ITEMBUF_NULL_ID) {
        assert(id < buf->nitems._1D);
        buf->items._1D[id]++;
      }
    }
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    for (i = 0; i < size; i++) {
      GLuint id = pixels[i];
      if (id != GL_ITEMBUF_NULL_ID) {
#if defined(IS_BIG_ENDIAN)
        GLuint id_0 = (id & 0xFFFF0000) >> 16; /* 32 bits uint */
        GLuint id_1 = (id & 0x0000FFFF);
#elif defined(IS_LITTLE_ENDIAN)
        GLuint id_0 = (id & 0x0000FFFF);
        GLuint id_1 = (id & 0xFFFF0000) >> 16; /* 32 bits uint */
#endif
        assert(id_0 < buf->nitems._2D[0] && id_1 < buf->nitems._2D[id_0 + 1]);
        buf->items._2D[id_0][id_1]++;
      }
    }
  }
  return buf;
}

GLitembuf *
gl_itembuf_conservative_lookup(GLitembuf *buf) {
  /*
   * We count the number of pixel cross patterns corresponding to each
   * color ID. Slower than the simple method but much more robust to
   * rasterization errors.
   */
  GLsizei width  = buf->gl_framebuf->width;
  GLsizei height = buf->gl_framebuf->height;
  GLuint *pixels = (GLuint *) buf->gl_framebuf->pixels;
  int i, j;
  
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    /* We take care of excluding image borders */
    for (i = 1; i < height - 1; i++) {
      for (j = 1; j < width - 1; j++) {
        GLuint id = pixels[i*width + j];
        if (id != GL_ITEMBUF_NULL_ID) {
          GLuint id_top    = pixels[(i+1)*width +  j   ];
          GLuint id_bottom = pixels[(i-1)*width +  j   ];
          GLuint id_right  = pixels[ i   *width + (j+1)];
          GLuint id_left   = pixels[ i   *width + (j-1)];
          if (id_top   == id && id_bottom == id &&
              id_right == id && id_left   == id) {
            assert(id < buf->nitems._1D);
            buf->items._1D[id]++;
          }
        }
      }
    }
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    /* We take care of excluding image borders */
    for (i = 1; i < height - 1; i++) {
      for (j = 1; j < width - 1; j++) {
        GLuint id = pixels[i*width + j];
        if (id != GL_ITEMBUF_NULL_ID) {
          GLuint id_top    = pixels[(i+1)*width +  j   ];
          GLuint id_bottom = pixels[(i-1)*width +  j   ];
          GLuint id_right  = pixels[ i   *width + (j+1)];
          GLuint id_left   = pixels[ i   *width + (j-1)];
          if (id_top   == id && id_bottom == id &&
              id_right == id && id_left   == id) {
#if defined(IS_BIG_ENDIAN)
            GLuint id_0 = (id & 0xFFFF0000) >> 16;
            GLuint id_1 = (id & 0x0000FFFF);
#elif defined(IS_LITTLE_ENDIAN)
            GLuint id_0 = (id & 0x0000FFFF);
            GLuint id_1 = (id & 0xFFFF0000) >> 16;
#endif
            assert(id_0 < buf->nitems._2D[0] &&
                   id_1 < buf->nitems._2D[id_0 + 1]);
            buf->items._2D[id_0][id_1]++;
          }
        }
      }
    }
  }
  return buf;
}

int
gl_itembuf_print(const GLitembuf *buf, FILE *stream) {
  int return_value = 0;
  
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    GLuint i;
    for (i = 0; i < buf->nitems._1D; i++) {
      return_value += fprintf(stream, "%u\t%u\n", i, buf->items._1D[i]);
    }
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    GLuint i, j;
    for (i = 0; i < buf->nitems._2D[0]; i++) {
      for (j = 0; j < buf->nitems._2D[i+1]; j++) {
        return_value
          += fprintf(stream, "%u\t%u\t%u\n", i, j, buf->items._2D[i][j]);
      }
    }
  }
  return return_value;
}

/*
 * GLselectbuf definitions
 */
GLselectbuf *
gl_selectbuf_depth_lookup(GLselectbuf *buf) {
  GLuint *data = buf->data;
  GLint nhits = buf->nhits;
  GLuint nnames;
  GLuint z_min, z_min_curr, z_max, z_max_curr;
  GLuint name_min, name_max, name_curr;
  int i;
  
  /* Initialize */
  nnames = *data;
  data++;
  z_min = *data;
  data++;
  z_max = *data;
  data += nnames;
  name_min = name_max = *data;
  data = buf->data;
  
  /* Lookup */
  for (i = 0; i < nhits; i++) {
    nnames = *data;
    data++;
    z_min_curr = *data;
    data++;
    z_max_curr = *data;
    data += nnames;
    name_curr = *data;
    if (z_max_curr > z_max) {
      z_max = z_max_curr;
      name_max = name_curr;
    }
    if (z_min_curr < z_min) {
      z_min = z_min_curr;
      name_min = name_curr;
    }
  }
  
  //buf->z_min = ; //voir gl_feedbuf_print()
  //buf->z_max = ; //voir gl_feedbuf_print()
  //buf->name_min = name_min;
  //buf->name_max = name_max;
  return buf;
}

int
gl_selectbuf_print(const GLselectbuf *buf, FILE *stream) {
  //const GLuint SCALE_FACTOR = 0x7FFFFFFF;
  const GLuint SCALE_FACTOR = 0xFFFFFFFE;
  GLuint *data = buf->data;
  GLint nhits = buf->nhits;
  GLuint nnames;
  int i;
  unsigned int j;
  int return_value = 0;
  
  return_value += fprintf(stream, "Number of hits %d\n", nhits);
  for (i = 0; i < nhits; i++) {
    nnames = *data;
    return_value += fprintf(stream, "Number of names for hit %d\n", nnames);
    data++;
    //return_value += fprintf(stream, "\tz min is %g ",
    //                        (float) *data/SCALE_FACTOR);
    return_value += fprintf(stream, "\tz min is %g ",
                            (float) *data / (float) SCALE_FACTOR);
    data++;
    //return_value += fprintf(stream, "z max is %g\n",
    //                        (float) *data/SCALE_FACTOR);
    return_value += fprintf(stream, "z max is %g\n",
                            (float) *data / (float) SCALE_FACTOR);
    data++;
    if (nnames > 1) {
      return_value += fprintf(stream, "\tThe names are ");
    } else {
      return_value += fprintf(stream, "\tThe name is ");
    }
    for (j = 0; j < nnames; j++) {
      return_value += fprintf(stream, "%d ", *data);
      data++;
    }
    return_value += fprintf(stream, "\n");
  }
  return return_value;
}

/*
 * GLfeedbuf definitions
 */
int
gl_feedbuf_print_3D_COLOR(GLsizei effective_size, GLsizei *count,
                          const GLfloat *data, FILE *stream) {
  int return_value = 0;
  int i;
  
  return_value += fprintf(stream, "  ");
  for (i = 0; i < GL_3D_COLOR_SIZE; i++) {
    return_value += fprintf(stream, "%4.2f ", data[effective_size - (*count)]);
    *count = *count - 1;
  }
  return_value += fprintf(stream, "\n");
  return return_value;
}

GLfeedbuf *
gl_feedbuf_set_vertices(GLfeedbuf *buf, GLsizei n, GLenum type,
                        GLsizei connectivity) {
  const GLsizei SECURE_SCALE = 4;
  GLsizei size = 0;
  
  /* RGBA mode only! */
  switch (type) {
    case GL_3D_COLOR:
      size = n*(GL_3D_COLOR_SIZE + 1)*connectivity*SECURE_SCALE;
      /* number of vertices * (values + token) * vertex connectivity */
      buf->print = gl_feedbuf_print_3D_COLOR;
      break;
    default:
      fprintf(stderr, "Error: Unsupported type!\n");
      assert(type == GL_3D_COLOR);
      break;
  }
  buf->type = type;
  if (buf->size != size) {
    buf->size = size;
    buf->data = (GLfloat *) realloc(buf->data, buf->size*sizeof(GLfloat));
  }
  assert(buf->data != NULL);
  glFeedbackBuffer(buf->size, buf->type, buf->data);
  return buf;
}

int
gl_feedbuf_print(const GLfeedbuf *buf, FILE *stream) {
  const GLsizei effective_size = buf->effective_size;
  GLsizei count                = buf->effective_size;
  const GLfloat *data          = buf->data;
  int (*print) (const GLsizei, GLsizei *, const GLfloat *, FILE *)
                               = buf->print;
  GLfloat token    = 0.0;
  int nvertices    = 0;
  int return_value = 0;
  
  while (count > 0) {
    token = data[effective_size - count];
    count--;
    if (token == GL_PASS_THROUGH_TOKEN) {
      return_value += fprintf(stream, "GL_PASS_THROUGH_TOKEN\n");
      return_value += fprintf(stream, "  %4.2f\n",
                              data[effective_size - count]);
      count--;
    } else if (token == GL_POINT_TOKEN) {
      return_value += fprintf(stream, "GL_POINT_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
    } else if (token == GL_LINE_TOKEN) {
      return_value += fprintf(stream, "GL_LINE_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
      return_value += print(effective_size, &count, data, stream);
    } else if (token == GL_LINE_RESET_TOKEN) {
      return_value += fprintf(stream, "GL_LINE_RESET_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
      return_value += print(effective_size, &count, data, stream);
    } else if (token == GL_POLYGON_TOKEN) {
      return_value += fprintf(stream, "GL_POLYGON_TOKEN\n");
      nvertices = (int) data[effective_size - count];
      count--;
      for (; nvertices > 0; nvertices--) {
        return_value += print(effective_size, &count, data, stream);
      }
    } else if (token == GL_BITMAP_TOKEN) {
      return_value += fprintf(stream, "GL_BITMAP_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
    } else if (token == GL_DRAW_PIXEL_TOKEN) {
      return_value += fprintf(stream, "GL_DRAW_PIXEL_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
    } else if (token == GL_COPY_PIXEL_TOKEN) {
      return_value += fprintf(stream, "GL_COPY_PIXEL_TOKEN\n");
      return_value += print(effective_size, &count, data, stream);
    }
  }
  return return_value;
}
