#ifndef __OPENGL_BUFFER_H__
#define __OPENGL_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <platform_defs.h>
#include <opengl_utils.h>

typedef struct _GLframebuf  GLframebuf;
typedef struct _GLitembuf   GLitembuf;
typedef struct _GLselectbuf GLselectbuf;
typedef struct _GLfeedbuf   GLfeedbuf;

typedef enum {
  GL_FILE_SGI
} GLfileFormat;

typedef enum {
  GL_ITEMBUF_1D,
  GL_ITEMBUF_2D
} GLitembufType;

struct _GLframebuf {
  GLint x, y;
  GLsizei width, height, components;
  GLenum format, type;
  GLvoid *pixels;
};

struct _GLitembuf {
  /* Buffer */
  GLframebuf *gl_framebuf;
  
  /* Lookup */
  GLitembufType gl_itembuf_type;
  union {
    GLuint  _1D;
    GLuint *_2D;
  } nitems;
  union {
    GLuint  *_1D;
    GLuint **_2D;
  } items;
};

struct _GLselectbuf {
  /* Buffer */
  GLsizei size;
  GLuint *data;
  
  /* Lookup */
  GLint nhits;
  //GLclampf z_min, z_max;
  //GLuint name_min, name_max;
};

struct _GLfeedbuf {
  GLsizei size, effective_size;
  GLenum type;
  GLfloat *data;
  int (*print) (GLsizei, GLsizei *, const GLfloat *, FILE *);
};

EXTERND const GLuint  GL_FRAMEBUF_NULL_INDEX;
EXTERND const GLuint  GL_ITEMBUF_NULL_ID;
EXTERND const GLvecub GL_ITEMBUF_NULL_COLOR;
EXTERND const GLuint  GL_SELECTBUF_DEFAULT_NAME;

EXTERND const GLsizei GL_2D_SIZE;
EXTERND const GLsizei GL_3D_SIZE;
EXTERND const GLsizei GL_3D_COLOR_SIZE;
EXTERND const GLsizei GL_3D_COLOR_TEXTURE_SIZE;
EXTERND const GLsizei GL_4D_COLOR_TEXTURE_SIZE;

/*
 * GLframebuf declarations
 */
INLINED GLframebuf *gl_framebuf_new       (void);
INLINED void        gl_framebuf_delete    (GLframebuf *buf);
EXTERND GLframebuf *gl_framebuf_eq        (GLframebuf *buf,
                                           const GLframebuf *buf_src,
                                           GLboolean copy_pixels);
INLINED GLframebuf *gl_framebuf_set_format(GLframebuf *buf, GLenum format);
EXTERND GLframebuf *gl_framebuf_set_port  (GLframebuf *buf,
                                           const GLveci port);
INLINED GLuint      gl_framebuf_index     (const GLframebuf *buf,
                                           GLint x, GLint y);
EXTERND size_t      gl_framebuf_size      (const GLframebuf *buf);
EXTERND GLframebuf *gl_framebuf_read      (GLframebuf *buf, GLenum mode);
INLINED GLframebuf *gl_framebuf_draw      (GLframebuf *buf, GLenum mode);
INLINED GLframebuf *gl_framebuf_copy      (GLframebuf *buf, GLenum type,
                                           GLenum read_mode,
                                           GLenum draw_mode);
EXTERND GLboolean   gl_framebuf_sread     (GLframebuf *buf,
                                           const char *name,
                                           GLfileFormat file_format);
EXTERND GLboolean   gl_framebuf_swrite    (const GLframebuf *buf,
                                           const char *name,
                                           GLfileFormat file_format,
                                           GLboolean flag);
EXTERND GLboolean   gl_framebuf_fread     (GLframebuf *buf,
                                           FILE *stream,
                                           GLfileFormat file_format);
EXTERND GLboolean   gl_framebuf_fwrite    (const GLframebuf *buf,
                                           FILE *stream,
                                           GLfileFormat file_format,
                                           GLboolean flag);
EXTERND GLboolean   gl_framebuf_tread     (GLframebuf *buf,
                                           const GLtexture *tex,
                                           GLint level);
EXTERND GLframebuf *gl_framebuf_ht        (GLframebuf *buf);
EXTERND GLframebuf *gl_framebuf_edt       (GLframebuf *buf,
                                           double *euclidean_distance_max);

/*
 * GLitembuf declarations
 */
INLINED GLitembuf *gl_itembuf_new                (GLitembufType type);
EXTERND void       gl_itembuf_delete             (GLitembuf *buf);
INLINED GLitembuf *gl_itembuf_set_port           (GLitembuf *buf,
                                                  const GLveci port);
EXTERND GLitembuf *gl_itembuf_set_items          (GLitembuf *buf,
                                                  const GLuint *n);
INLINED GLitembuf *gl_itembuf_render_begin       (GLitembuf *buf,
                                                  GLboolean write_depth);
INLINED GLitembuf *gl_itembuf_render_end         (GLitembuf *buf);
INLINED GLitembuf *gl_itembuf_reset_items        (GLitembuf *buf);
EXTERND GLitembuf *gl_itembuf_simple_lookup      (GLitembuf *buf);
EXTERND GLitembuf *gl_itembuf_conservative_lookup(GLitembuf *buf);
EXTERND int        gl_itembuf_print              (const GLitembuf *buf,
                                                  FILE *stream);

/*
 * GLselectbuf declarations
 */
INLINED GLselectbuf *gl_selectbuf_new          (void);
INLINED void         gl_selectbuf_delete       (GLselectbuf *buf);
INLINED GLselectbuf *gl_selectbuf_set_hits     (GLselectbuf *buf, GLsizei n,
                                                GLsizei nnames_per_hit);
INLINED GLselectbuf *gl_selectbuf_picking_begin(GLselectbuf *buf,
                                                GLdouble x, GLdouble y,
                                                GLclampd sensitivity,
                                                GLtransf *trsf);
INLINED GLselectbuf *gl_selectbuf_picking_end  (GLselectbuf *buf);
INLINED GLselectbuf *gl_selectbuf_render_begin (GLselectbuf *buf);
INLINED GLselectbuf *gl_selectbuf_render_end   (GLselectbuf *buf);
EXTERND GLselectbuf *gl_selectbuf_depth_lookup (GLselectbuf *buf);
EXTERND int          gl_selectbuf_print        (const GLselectbuf *buf,
                                                FILE *stream);

/*
 * GLfeedbuf declarations
 */
INLINED GLfeedbuf *gl_feedbuf_new         (void);
INLINED void       gl_feedbuf_delete      (GLfeedbuf *buf);
EXTERND GLfeedbuf *gl_feedbuf_set_vertices(GLfeedbuf *buf,
                                           GLsizei n, GLenum type,
                                           GLsizei connectivity);
INLINED GLfeedbuf *gl_feedbuf_render_begin(GLfeedbuf *buf);
INLINED GLfeedbuf *gl_feedbuf_render_end  (GLfeedbuf *buf);
EXTERND int        gl_feedbuf_print       (const GLfeedbuf *buf,
                                           FILE *stream);
EXTERND GLboolean  gl_feedbuf_fwrite_eps  (const GLfeedbuf *buf,
                                           FILE *stream, GLboolean sort);
INLINED GLboolean  gl_feedbuf_write_eps   (const GLfeedbuf *buf,
                                           const char *name,
                                           GLboolean sort);

/*
 * GLframebuf definitions
 */
INLINED GLframebuf *
gl_framebuf_new(void) {
  GLframebuf *buf = (GLframebuf *) malloc(sizeof(GLframebuf));
  assert(buf != NULL);
  buf->x = buf->y = buf->width = buf->height = 0;
  buf->components = 3;
  buf->format = GL_RGB;
  buf->type = GL_UNSIGNED_BYTE;
  buf->pixels = NULL;
  return buf;
}

INLINED void
gl_framebuf_delete(GLframebuf *buf) {
  assert(buf != NULL);
  free(buf->pixels);
  free(buf);
#if DEBUG
  buf = NULL;
#endif
}

INLINED GLuint
gl_framebuf_index(const GLframebuf *buf, GLint x, GLint y) {
  GLint index_x = x - buf->x;
  GLint index_y = y - buf->y;
  
  if (index_x < 0                  ||
      index_y < 0                  ||
      x > buf->x + buf->width  - 1 ||
      y > buf->y + buf->height - 1) {
    return GL_FRAMEBUF_NULL_INDEX;
  } else {
    return (index_y * buf->width + index_x);
  }
}

INLINED GLframebuf *
gl_framebuf_set_format(GLframebuf *buf, GLenum format) {
  buf->format = format;
  switch (format) {
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_LUMINANCE:
    case GL_STENCIL_INDEX:
    case GL_DEPTH_COMPONENT:
      buf->components = 1;
      break;
    case GL_LUMINANCE_ALPHA:
      buf->components = 2;
      break;
    case GL_RGB:
      buf->components = 3;
      break;
    case GL_RGBA:
      buf->components = 4;
      break;
    default:
      fprintf(stderr, "Error: Unsupported format!\n");
      assert(format == GL_RED             ||
             format == GL_GREEN           ||
             format == GL_BLUE            ||
             format == GL_ALPHA           ||
             format == GL_LUMINANCE       ||
             format == GL_STENCIL_INDEX   ||
             format == GL_DEPTH_COMPONENT ||
             format == GL_LUMINANCE_ALPHA ||
             format == GL_RGB             ||
             format == GL_RGBA            );
      break;
  }
  return buf;
}

INLINED GLframebuf *
gl_framebuf_draw(GLframebuf *buf, GLenum mode) {
  assert(buf->pixels != NULL);
  glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
  glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glRasterPos2i(buf->x, buf->y);
  glDrawBuffer(mode);
  glDrawPixels(buf->width, buf->height, buf->format, buf->type, buf->pixels);
  glPopAttrib();
  glPopClientAttrib();
  return buf;
}

INLINED GLframebuf *
gl_framebuf_copy(GLframebuf *buf, GLenum type,
                 GLenum read_mode, GLenum draw_mode) {
  glPushAttrib(GL_PIXEL_MODE_BIT | GL_COLOR_BUFFER_BIT);
  glReadBuffer(read_mode);
  glDrawBuffer(draw_mode);
  glCopyPixels(buf->x, buf->y, buf->width, buf->height, type);
  glPopAttrib();
  return buf;
}

/*
 * GLitembuf definitions
 */
INLINED GLitembuf *
gl_itembuf_new(GLitembufType type) {
  GLitembuf *buf = (GLitembuf *) malloc(sizeof(GLitembuf));
  assert(buf != NULL);
  buf->gl_framebuf = gl_framebuf_new();
  gl_framebuf_set_format(buf->gl_framebuf, GL_RGBA);
  buf->gl_itembuf_type = type;
  if (type == GL_ITEMBUF_1D) {
    buf->nitems._1D = 0u;
    buf->items._1D = NULL;
  } else if (type == GL_ITEMBUF_2D) {
    buf->nitems._2D = NULL;
    buf->items._2D = NULL;
  } else {
    fprintf(stderr, "Error: Unsupported GLitembuf type!\n");
    assert(type == GL_ITEMBUF_1D || type == GL_ITEMBUF_2D);
  }
  return buf;
}

INLINED GLitembuf *
gl_itembuf_set_port(GLitembuf *buf, const GLveci port) {
  gl_framebuf_set_port(buf->gl_framebuf, port);
  return buf;
}

INLINED GLitembuf *
gl_itembuf_render_begin(GLitembuf *buf, GLboolean write_depth) {
  glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT |
               GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  if (!write_depth) {
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
  }
  glShadeModel(GL_FLAT);
  glClearColor(GL_CLAMPF_MAX, GL_CLAMPF_MAX, GL_CLAMPF_MAX, GL_CLAMPF_MAX);
  if (write_depth) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  glColor4ub(GL_UBYTE_MAX, GL_UBYTE_MAX, GL_UBYTE_MAX, GL_UBYTE_MAX);
  return buf;
}

INLINED GLitembuf *
gl_itembuf_render_end(GLitembuf *buf) {
  gl_framebuf_read(buf->gl_framebuf, GL_BACK); /* Needs double buffer */
  glPopAttrib();
  return buf;
}

INLINED GLitembuf *
gl_itembuf_reset_items(GLitembuf *buf) {
  if (buf->gl_itembuf_type == GL_ITEMBUF_1D) {
    memset(buf->items._1D, 0, buf->nitems._1D*sizeof(GLuint));
  } else if (buf->gl_itembuf_type == GL_ITEMBUF_2D) {
    GLuint i;
    
    for (i = 0; i < buf->nitems._2D[0]; i++) {
      memset(buf->items._2D[i], 0, buf->nitems._2D[i+1]*sizeof(GLuint));
    }
  }
  return buf;
}

/*
 * GLselectbuf definitions
 */
INLINED GLselectbuf *
gl_selectbuf_new(void) {
  GLselectbuf *buf = (GLselectbuf *) malloc(sizeof(GLselectbuf));
  assert(buf != NULL);
  buf->size = 0;
  buf->data = NULL;
  buf->nhits = 0;
  return buf;
}

INLINED void
gl_selectbuf_delete(GLselectbuf *buf) {
  assert(buf != NULL);
  free(buf->data);
  free(buf);
#if DEBUG
  buf = NULL;
#endif
}

INLINED GLselectbuf *
gl_selectbuf_set_hits(GLselectbuf *buf,
                      GLsizei n, GLsizei nnames_per_hit) {
  const GLsizei SECURE_SCALE = 2;
  GLsizei size = n*(3 + nnames_per_hit)*SECURE_SCALE;
  
  if (buf->size != size) {
    buf->size = size;
    buf->data = (GLuint *) realloc(buf->data, buf->size*sizeof(GLuint));
  }
  assert(buf->data != NULL);
  glSelectBuffer(buf->size, buf->data);
  return buf;
}

INLINED GLselectbuf *
gl_selectbuf_picking_begin(GLselectbuf *buf,
                           GLdouble x, GLdouble y,
                           GLclampd sensitivity, GLtransf *trsf) {
  const GLdouble SIZE = 4.0;
  
  glPushAttrib(GL_TRANSFORM_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluPickMatrix(x, y, sensitivity*SIZE, sensitivity*SIZE, trsf->viewport);
  glMultMatrixd(trsf->projection_matrix);
  return buf;
}

INLINED GLselectbuf *
gl_selectbuf_picking_end(GLselectbuf *buf) {
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
  return buf;
}

INLINED GLselectbuf *
gl_selectbuf_render_begin(GLselectbuf *buf) {
  (void) glRenderMode(GL_SELECT);
  glInitNames();
  glPushName(GL_SELECTBUF_DEFAULT_NAME);
  return buf;
}

INLINED GLselectbuf *
gl_selectbuf_render_end(GLselectbuf *buf) {
  GLint nhits = glRenderMode(GL_RENDER);
  if (nhits < 0) {
    fprintf(stderr, "Error: Selection array overflow!\n");
    assert(nhits >= 0);
  } else {
    buf->nhits = nhits;
  }
  return buf;
}

/*
 * GLfeedbuf definitions
 */
INLINED GLfeedbuf *
gl_feedbuf_new(void) {
  GLfeedbuf *buf = (GLfeedbuf *) malloc(sizeof(GLfeedbuf));
  assert(buf != NULL);
  buf->size = buf->effective_size = 0;
  buf->type = GL_3D_COLOR;
  buf->data = NULL;
  buf->print = NULL;
  return buf;
}

INLINED void
gl_feedbuf_delete(GLfeedbuf *buf) {
  assert(buf != NULL);
  free(buf->data);
  free(buf);
#if DEBUG
  buf = NULL;
#endif
}

INLINED GLfeedbuf *
gl_feedbuf_render_begin(GLfeedbuf *buf) {
  (void) glRenderMode(GL_FEEDBACK);
  return buf;
}

INLINED GLfeedbuf *
gl_feedbuf_render_end(GLfeedbuf *buf) {
  GLint effective_size = glRenderMode(GL_RENDER);
  if (effective_size < 0) {
    fprintf(stderr, "Error: Feedback array overflow!\n");
    assert(effective_size >= 0);
  } else {
    buf->effective_size = (GLsizei) effective_size;
  }
  return buf;
}

INLINED GLboolean
gl_feedbuf_write_eps(const GLfeedbuf *buf, const char *name, GLboolean sort) {
  FILE *file_out       = NULL;
  GLboolean is_written = GL_FALSE;
  
  if ((file_out = fopen(name, "w")) == NULL) {
    fprintf(stderr, "Error: Could not open file %s!\n", name);
    return GL_FALSE;
  }
  is_written = gl_feedbuf_fwrite_eps(buf, file_out, sort);
  fclose(file_out);
  return is_written;
}

#ifdef __cplusplus
}
#endif

#endif /* !__OPENGL_BUFFER_H__ */
