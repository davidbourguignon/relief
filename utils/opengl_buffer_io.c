#include "opengl_buffer.h"
#include <sgi.h>

/*
 * Local functions declarations
 */
INLINED GLframebuf *_gl_framebuf_set_components(GLframebuf *buf, GLsizei n);
static  GLboolean   _gl_framebuf_sread_sgi     (GLframebuf *buf,
                                                const char *name);
static  GLboolean   _gl_framebuf_swrite_sgi    (const GLframebuf *buf,
                                                const char *name,
                                                GLboolean rle_storage);
/* static  GLboolean   _gl_framebuf_fread_sgi     (GLframebuf *buf, */
/*                                                 FILE *stream); */
/* static  GLboolean   _gl_framebuf_fwrite_sgi    (const GLframebuf *buf, */
/*                                                 FILE *stream, */
/*                                                 GLboolean rle_storage); */

/*
 * Local functions definitions
 */
INLINED GLframebuf *
_gl_framebuf_set_components(GLframebuf *buf, GLsizei n) {
  buf->components = n;
  /* We set the most useful formats, but there are other formats available. */
  switch (n) {
    case 1: buf->format = GL_LUMINANCE;       break;
    case 2: buf->format = GL_LUMINANCE_ALPHA; break;
    case 3: buf->format = GL_RGB;             break;
    case 4: buf->format = GL_RGBA;            break;
    default:
      fprintf(stderr, "Error: Unsupported components number!\n");
      assert(n > 0 && n < 5);
      break;
  }
  return buf;
}

static GLboolean
_gl_framebuf_sread_sgi(GLframebuf *buf, const char *name) {
/* static GLboolean */
/* _gl_framebuf_fread_sgi(GLframebuf *buf, FILE *stream) { */
  sgi_t *sgip           = NULL;
  unsigned char *pixels = NULL;
  unsigned char *pptr   = NULL;
  unsigned short **rows = NULL;
  int x, y, z;
  
  assert(buf != NULL);
  if ((sgip = sgiOpen(name, SGI_READ, 0, 0, 0, 0, 0)) == NULL) {
    return GL_FALSE;
  }
  /* assert(buf != NULL && stream != NULL); */
  /* if ((sgip = sgiOpenFile(stream, SGI_READ, 0, 0, 0, 0, 0)) == NULL) { */
  /*   return GL_FALSE; */
  /* } */
  pixels = (unsigned char *) malloc(sgip->xsize * sgip->ysize * sgip->zsize *
                                    sizeof(unsigned char));
  pptr = pixels;
  rows = (unsigned short **) malloc(sgip->zsize * sizeof(unsigned short *));
  rows[0] = (unsigned short *) malloc(sgip->xsize * sgip->zsize *
                                      sizeof(unsigned short));
  for (z = 1; z < sgip->zsize; z++) {
    rows[z] = rows[0] + z * sgip->xsize;
  }
  for (y = 0; y < sgip->ysize; y++) {
    /* get rows from file */
    for (z = 0; z < sgip->zsize; z++) {
      if (sgiGetRow(sgip, rows[z], y, z) < 0) {
        return GL_FALSE;
      }
    }
    /* put rows into buffer */
    if (sgip->bpp == 1) {
      /* 8-bit (unsigned) pixels */
      for (x = 0; x < sgip->xsize; x++) {
        for (z = 0; z < sgip->zsize; z++, pptr++) {
          *pptr = (unsigned char) rows[z][x];
        }
      }
    } else {
      /* 16-bit (unsigned) pixels */
      for (x = 0; x < sgip->xsize; x++) {
        for (z = 0; z < sgip->zsize; z++, pptr++) {
          *pptr = rows[z][x] >> 8; /*FIXME*/
        }
      }
    }
  }
  if (sgiClose(sgip) < 0) {
    return GL_FALSE;
  }
  free(rows[0]);
  free(rows);
  buf->width  = sgip->xsize;
  buf->height = sgip->ysize;
  _gl_framebuf_set_components(buf, sgip->zsize);
  /* buf->type has default values */
  buf->pixels = pixels;
  return GL_TRUE;
}

static GLboolean
_gl_framebuf_swrite_sgi(const GLframebuf *buf, const char *name,
                        GLboolean rle_storage) {
/* static GLboolean */
/* _gl_framebuf_fwrite_sgi(const GLframebuf *buf, FILE *stream, */
/*                         GLboolean rle_storage) { */
  sgi_t *sgip           = NULL;
  unsigned char *pptr   = NULL;
  unsigned short **rows = NULL;
  int x, y, z;
  
  assert(buf != NULL);
  if ((sgip = sgiOpen(name, SGI_WRITE, (int) rle_storage, 1,
                      (int) buf->width, (int) buf->height,
                      (int) buf->components)) == NULL) {
    return GL_FALSE;
  }
  /* assert(buf != NULL && stream != NULL); */
  /* if ((sgip = sgiOpenFile(stream, SGI_WRITE, (int) rle_storage, 1, */
  /*                         (int) buf->width, (int) buf->height, */
  /*                         (int) buf->components)) == NULL) { */
  /*   return GL_FALSE; */
  /* } */
  pptr = (unsigned char *) buf->pixels;
  rows = (unsigned short **) malloc(sgip->zsize * sizeof(unsigned short *));
  rows[0] = (unsigned short *) malloc(sgip->xsize * sgip->zsize *
                                      sizeof(unsigned short));
  for (z = 1; z < sgip->zsize; z++) {
    rows[z] = rows[0] + z * sgip->xsize;
  }
  for (y = 0; y < sgip->ysize; y++) {
    /* get rows from buffer */
    for (x = 0; x < sgip->xsize; x++) {
      for (z = 0; z < sgip->zsize; z++, pptr++) {
        rows[z][x] = *pptr; /*FIXME*/
      }
    }
    /* put rows to file */
    for (z = 0; z < sgip->zsize; z++) {
      if (sgiPutRow(sgip, rows[z], y, z) < 0) {
        return GL_FALSE;
      }
    }
  }
  if (sgiClose(sgip) < 0) {
    return GL_FALSE;
  }
  free(rows[0]);
  free(rows);
  return GL_TRUE;
}

/*
 * GLframebuf definitions
 */
GLboolean
gl_framebuf_sread(GLframebuf *buf, const char *name,
                  GLfileFormat file_format) {
  FILE *file_in     = NULL;
  GLboolean is_read = GL_FALSE;
  
  if ((file_in = fopen(name, "rb+")) == NULL) {
    return GL_FALSE;
  }
  if (file_format == GL_FILE_SGI) { /* HACK */
    is_read = _gl_framebuf_sread_sgi(buf, name);
  } else {
    is_read = gl_framebuf_fread(buf, file_in, file_format);
  }
  /* is_read = gl_framebuf_fread(buf, file_in, file_format); */
  fclose(file_in);
  return is_read;
}

GLboolean
gl_framebuf_swrite(const GLframebuf *buf, const char *name,
                   GLfileFormat file_format, GLboolean flag) {
  FILE *file_out       = NULL;
  GLboolean is_written = GL_FALSE;
  
  if ((file_out = fopen(name, "wb+")) == NULL) {
    return GL_FALSE;
  }
  if (file_format == GL_FILE_SGI) { /* HACK */
    is_written = _gl_framebuf_swrite_sgi(buf, name, flag);
  } else {
    is_written = gl_framebuf_fwrite(buf, file_out, file_format, flag);
  }
  /* is_written = gl_framebuf_fwrite(buf, file_out, file_format, flag); */
  fclose(file_out);
  return is_written;
}

GLboolean
gl_framebuf_fread(GLframebuf *buf, FILE *stream, GLfileFormat file_format) {
  GLboolean is_read = GL_FALSE;
  
  assert(buf != NULL);
  free(buf->pixels);
  switch (file_format) {
    case GL_FILE_SGI:
      assert(GL_FALSE);
      /* is_read = _gl_framebuf_fread_sgi(buf, stream); */
      break;
    default:
      fprintf(stderr, "Error: Unsupported file format!\n");
      assert(file_format == GL_FILE_SGI);
    break;
  }
  return is_read;
}

GLboolean
gl_framebuf_fwrite(const GLframebuf *buf, FILE *stream,
                   GLfileFormat file_format, GLboolean flag) {
  GLboolean is_written = GL_FALSE;
  
  switch (file_format) {
    case GL_FILE_SGI:
      assert(GL_FALSE);
      /* is_written = _gl_framebuf_fwrite_sgi(buf, stream, flag); */
      break;
    default:
      fprintf(stderr, "Error: Unsupported file format!\n");
      assert(file_format == GL_FILE_SGI);
    break;
  }
  return is_written;
}

GLboolean
gl_framebuf_tread(GLframebuf *buf, const GLtexture *tex, GLint level) {
  GLveci port;
  
  assert(buf != NULL && buf->type == GL_UNSIGNED_BYTE);
  assert(tex->target == GL_TEXTURE_1D || tex->target == GL_TEXTURE_2D);
  assert(level >= 0 && level <= tex->nlevels - 1);
  glPushAttrib(GL_TEXTURE_BIT);
  
  gl_texture_bind(tex);
  port[0] = port[1] = 0;
  glGetTexLevelParameteriv(tex->target, level, GL_TEXTURE_WIDTH, &port[2]);
  glGetTexLevelParameteriv(tex->target, level, GL_TEXTURE_HEIGHT, &port[3]);
  gl_framebuf_set_format(buf, tex->format);
  gl_framebuf_set_port(buf, port);
  glGetTexImage(tex->target, level, buf->format, buf->type, buf->pixels);
  
  glPopAttrib();
  
  return GL_TRUE;
}
