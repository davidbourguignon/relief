/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

/* Example showing how to use OpenGL's feedback mode to capture
   transformed vertices and output them as Encapsulated PostScript.
   Handles limited hidden surface removal by sorting and does
   smooth shading (albeit limited due to PostScript). */

#include <scalar.h>
#include "opengl_buffer.h"

typedef struct _GL_3D_COLOR_Vertex GL_3D_COLOR_Vertex;
typedef struct _Depth_Index        Depth_Index;

struct _GL_3D_COLOR_Vertex {
  GLfloat x, y, z;
  GLfloat red, green, blue, alpha; /* RGBA mode only! */
};

struct _Depth_Index {
  GLfloat *ptr;
  GLfloat depth;
};

static const double GOURAUD_THRESHOLD  = 0.1;  /* Lower for better (slower)
                                                  smooth shading. */
static const double SMOOTH_LINE_FACTOR = 0.06; /* Lower for better smooth
                                                  lines. */
static const char *GOURAUD_TRIANGLE_EPS[] = {
  "/bd{bind def}bind def /triangle { aload pop   setrgbcolor  aload pop 5 3",
  "roll 4 2 roll 3 2 roll exch moveto lineto lineto closepath fill } bd",
  "/computediff1 { 2 copy sub abs threshold ge {pop pop pop true} { exch 2",
  "index sub abs threshold ge { pop pop true} { sub abs threshold ge } ifelse",
  "} ifelse } bd /computediff3 { 3 copy 0 get 3 1 roll 0 get 3 1 roll 0 get",
  "computediff1 {true} { 3 copy 1 get 3 1 roll 1 get 3 1 roll 1 get",
  "computediff1 {true} { 3 copy 2 get 3 1 roll  2 get 3 1 roll 2 get",
  "computediff1 } ifelse } ifelse } bd /middlecolor { aload pop 4 -1 roll",
  "aload pop 4 -1 roll add 2 div 5 1 roll 3 -1 roll add 2 div 3 1 roll add 2",
  "div 3 1 roll exch 3 array astore } bd /gouraudtriangle { computediff3 { 4",
  "-1 roll aload 7 1 roll 6 -1 roll pop 3 -1 roll pop add 2 div 3 1 roll add",
  "2 div exch 3 -1 roll aload 7 1 roll exch pop 4 -1 roll pop add 2 div 3 1",
  "roll add 2 div exch 3 -1 roll aload 7 1 roll pop 3 -1 roll pop add 2 div 3",
  "1 roll add 2 div exch 7 3 roll 10 -3 roll dup 3 index middlecolor 4 1 roll",
  "2 copy middlecolor 4 1 roll 3 copy pop middlecolor 4 1 roll 13 -1 roll",
  "aload pop 17 index 6 index 15 index 19 index 6 index 17 index 6 array",
  "astore 10 index 10 index 14 index gouraudtriangle 17 index 5 index 17",
  "index 19 index 5 index 19 index 6 array astore 10 index 9 index 13 index",
  "gouraudtriangle 13 index 16 index 5 index 15 index 18 index 5 index 6",
  "array astore 12 index 12 index 9 index gouraudtriangle 17 index 16 index",
  "15 index 19 index 18 index 17 index 6 array astore 10 index 12 index 14",
  "index gouraudtriangle 18 {pop} repeat } { aload pop 5 3 roll aload pop 7 3",
  "roll aload pop 9 3 roll 4 index 6 index 4 index add add 3 div 10 1 roll 7",
  "index 5 index 3 index add add 3 div 10 1 roll 6 index 4 index 2 index add",
  "add 3 div 10 1 roll 9 {pop} repeat 3 array astore triangle } ifelse } bd",
  NULL
};

static GLfloat point_size = 0.0;

/*
 * Local functions declarations
 */
INLINED void     error                        (const GLfloat token);
static  GLfloat *gl_feedbuf_spew_primitive_eps(GLfloat *loc, FILE *stream);
static  int      compare                      (const void *a, const void *b);
static  void     gl_feedbuf_spew_sorted       (const GLfeedbuf *buf,
                                               FILE *stream);
INLINED void     gl_feedbuf_spew_unsorted     (const GLfeedbuf *buf,
                                               FILE *stream);

/*
 * Local functions definitions
 */
INLINED void
error(const GLfloat token) {
  fprintf(stderr, "Error: Incomplete implementation, unexpected token %f!\n",
          token);
  assert(token == GL_POINT_TOKEN ||
         token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN ||
         token == GL_POLYGON_TOKEN);
}

static GLfloat *
gl_feedbuf_spew_primitive_eps(GLfloat *loc, FILE *stream) {
  GLfloat token;
  GL_3D_COLOR_Vertex *vertex = NULL;
  GLfloat dx, dy, dr, dg, db, absR, absG, absB, distance, colormax;
  int steps;
  GLfloat xstep, ystep, rstep, gstep, bstep;
  GLfloat xnext, ynext, rnext, gnext, bnext;
  int i, nvertices;
  GLfloat red, green, blue;
  int smooth;
  
  token = *loc;
  loc++;
  
  if (token == GL_POINT_TOKEN) {
    vertex = (GL_3D_COLOR_Vertex *) loc;
    
    fprintf(stream, "%g %g %g setrgbcolor\n",
            vertex[0].red, vertex[0].green, vertex[0].blue);
    fprintf(stream, "%g %g %g 0 360 arc fill\n\n",
            vertex[0].x, vertex[0].y, point_size / 2.0);
    
    loc += GL_3D_COLOR_SIZE;
    
  } else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN) {
    vertex = (GL_3D_COLOR_Vertex *) loc;
    
    dr = vertex[1].red - vertex[0].red;
    dg = vertex[1].green - vertex[0].green;
    db = vertex[1].blue - vertex[0].blue;
    
    if (dr != 0 || dg != 0 || db != 0) {
      /* Smooth shaded line. */
      dx = vertex[1].x - vertex[0].x;
      dy = vertex[1].y - vertex[0].y;
      distance = sqrtf(dx * dx + dy * dy);
      
      absR = fabsf(dr);
      absG = fabsf(dg);
      absB = fabsf(db);
      
      colormax = scalf_max(absR, scalf_max(absG, absB));
      steps = (int) scalf_max(1.0, colormax * distance * SMOOTH_LINE_FACTOR);
      
      xstep = dx / steps;
      ystep = dy / steps;
      
      rstep = dr / steps;
      gstep = dg / steps;
      bstep = db / steps;
      
      xnext = vertex[0].x;
      ynext = vertex[0].y;
      rnext = vertex[0].red;
      gnext = vertex[0].green;
      bnext = vertex[0].blue;
      
      /* Back up half a step: we want the end points to be exactly
         their endpoint colors. */
      xnext -= xstep / 2.0;
      ynext -= ystep / 2.0;
      rnext -= rstep / 2.0;
      gnext -= gstep / 2.0;
      bnext -= bstep / 2.0;
    } else {
      /* Single color line. */
      steps = 0;
    }
    
    fprintf(stream, "%g %g %g setrgbcolor\n",
            vertex[0].red, vertex[0].green, vertex[0].blue);
    fprintf(stream, "%g %g moveto\n", vertex[0].x, vertex[0].y);
    
    for (i = 0; i < steps; i++) {
      xnext += xstep;
      ynext += ystep;
      rnext += rstep;
      gnext += gstep;
      bnext += bstep;
      fprintf(stream, "%g %g lineto stroke\n", xnext, ynext);
      fprintf(stream, "%g %g %g setrgbcolor\n", rnext, gnext, bnext);
      fprintf(stream, "%g %g moveto\n", xnext, ynext);
    }
    
    fprintf(stream, "%g %g lineto stroke\n", vertex[1].x, vertex[1].y);
    
    loc += 2 * GL_3D_COLOR_SIZE;
    
  } else if (token == GL_POLYGON_TOKEN) {
    nvertices = (int) (*loc);
    loc++;
    
    if (nvertices > 0) {
      vertex = (GL_3D_COLOR_Vertex *) loc;
      red = vertex[0].red;
      green = vertex[0].green;
      blue = vertex[0].blue;
      smooth = 0;
      
      for (i = 1; i < nvertices; i++) {
        if (red != vertex[i].red || green != vertex[i].green ||
            blue != vertex[i].blue) {
          smooth = 1;
          break;
        }
      }
      if (smooth) {
        /* Smooth shaded polygon; varying colors at vetices. */
        /* Break polygon into "nvertices - 2" triangle fans. */
        for (i = 0; i < nvertices - 2; i++) {
          fprintf(stream, "[%g %g %g %g %g %g]",
                  vertex[0].x, vertex[i + 1].x, vertex[i + 2].x,
                  vertex[0].y, vertex[i + 1].y, vertex[i + 2].y);
          fprintf(stream,
                  " [%g %g %g] [%g %g %g] [%g %g %g] gouraudtriangle\n",
                  vertex[0].red, vertex[0].green, vertex[0].blue,
                  vertex[i + 1].red, vertex[i + 1].green, vertex[i + 1].blue,
                  vertex[i + 2].red, vertex[i + 2].green, vertex[i + 2].blue);
        }
      } else {
        /* Flat shaded polygon; all vertex colors the same. */
        fprintf(stream, "newpath\n");
        fprintf(stream, "%g %g %g setrgbcolor\n", red, green, blue);
        
        /* Draw a filled triangle. */
        fprintf(stream, "%g %g moveto\n", vertex[0].x, vertex[0].y);
        for (i = 1; i < nvertices; i++) {
          fprintf(stream, "%g %g lineto\n", vertex[i].x, vertex[i].y);
        }
        fprintf(stream, "closepath fill\n\n");
      }
    }
    loc += nvertices * GL_3D_COLOR_SIZE;
    
  } else {
    error(token);
  }
  
  return loc;
}

static int
compare(const void *a, const void *b) {
  Depth_Index *p1 = (Depth_Index *) a;
  Depth_Index *p2 = (Depth_Index *) b;
  GLfloat diff = p2->depth - p1->depth; /* Sorting in descending order for */
                                        /* rendering from back to front.   */
  if (diff > 0.0) {
    return 1;
  } else if (diff < 0.0) {
    return -1;
  } else {
    return 0;
  }
}

static void
gl_feedbuf_spew_sorted(const GLfeedbuf *buf, FILE *stream) {
  GLfloat *loc = NULL, *end = NULL;
  GLfloat token;
  int nprimitives, nvertices, index, i;
  Depth_Index *primitives = NULL;
  GL_3D_COLOR_Vertex *vertex = NULL;
  GLfloat depth_sum;
  
  loc = buf->data;
  end = buf->data + buf->effective_size;
  
  /* Count how many primitives there are. */
  nprimitives = 0;
  while (loc < end) {
    token = *loc;
    loc++;
    if (token == GL_POINT_TOKEN) {
      loc += GL_3D_COLOR_SIZE;
      nprimitives++;
    } else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN) {
      loc += 2 * GL_3D_COLOR_SIZE;
      nprimitives++;
    } else if (token == GL_POLYGON_TOKEN) {
      nvertices = *loc;
      loc++;
      loc += (nvertices * GL_3D_COLOR_SIZE);
      nprimitives++;
    } else {
      error(token);
    }
  }
  
  /* Allocate an array of pointers that will point back at primitives in the
     feedback buffer. There will be one entry per primitive. This array is
     also where we keep the primitive's average depth. There is one entry per
     primitive in the feedback buffer. */
  primitives = (Depth_Index *) malloc(nprimitives * sizeof(Depth_Index));
  loc = buf->data;
  
  index = 0;
  while (loc < end) {
    primitives[index].ptr = loc; /* Save this primitive's location. */
    token = *loc;
    loc++;
    if (token == GL_POINT_TOKEN) {
      vertex = (GL_3D_COLOR_Vertex *) loc;
      primitives[index].depth = vertex[0].z;
      loc += GL_3D_COLOR_SIZE;
    } else if (token == GL_LINE_TOKEN || token == GL_LINE_RESET_TOKEN) {
      vertex = (GL_3D_COLOR_Vertex *) loc;
      depth_sum = vertex[0].z + vertex[1].z;
      primitives[index].depth = depth_sum / 2.0;
      loc += 2 * GL_3D_COLOR_SIZE;
    } else if (token == GL_POLYGON_TOKEN) {
      nvertices = *loc;
      loc++;
      vertex = (GL_3D_COLOR_Vertex *) loc;
      depth_sum = vertex[0].z;
      for (i = 1; i < nvertices; i++) {
        depth_sum += vertex[i].z;
      }
      primitives[index].depth = depth_sum / nvertices;
      loc += (nvertices * GL_3D_COLOR_SIZE);
    } else {
      error(token);
    }
    index++;
  }
  
  assert(index == nprimitives);
  
  /* Sort the primitives back to front. */
  qsort(primitives, nprimitives, sizeof(Depth_Index), compare);
  
  /* Understand that sorting by a primitives average depth doesn't allow us
     to disambiguate some cases like self intersecting polygons. Handling
     these cases would require breaking up the primitives. That's too
     involved for this example. Sorting by depth is good enough for lots of
     applications. */
  
  /* Emit the Encapsulated PostScript for the primitives in
     back to front order. */
  for (index = 0; index < nprimitives; index++) {
    (void) gl_feedbuf_spew_primitive_eps(primitives[index].ptr, stream);
  }
  
  free(primitives);
}

INLINED void
gl_feedbuf_spew_unsorted(const GLfeedbuf *buf, FILE *stream) {
  GLfloat *loc = NULL, *end = NULL;
  
  loc = buf->data;
  end = buf->data + buf->effective_size;
  while (loc < end) {
    loc = gl_feedbuf_spew_primitive_eps(loc, stream);
  }
}

/*
 * GLfeedbuf definitions
 */
GLboolean
gl_feedbuf_fwrite_eps(const GLfeedbuf *buf, FILE *stream,
                      const GLboolean sort) {
  GLvecf viewport, clear_color;
  GLfloat line_width;
  int i;
  
  assert(buf != NULL && stream != NULL);
  assert(buf->type == GL_3D_COLOR && buf->data != NULL);
  clearerr(stream);
  
  /* Read back a bunch of OpenGL state to help make the EPS consistent with
     the OpenGL viewport, clear color, line width and point size. */
  glGetFloatv(GL_VIEWPORT, viewport);
  glGetFloatv(GL_COLOR_CLEAR_VALUE, clear_color);
  glGetFloatv(GL_LINE_WIDTH, &line_width);
  glGetFloatv(GL_POINT_SIZE, &point_size);
  
  /* Emit EPS header. */
  fputs("%!PS-Adobe-2.0 EPSF-2.0\n", stream);
  /* Notice %% for a single % in the fprintf calls. */
  fprintf(stream, "%%%%Creator: gl_feedbuf_fwrite_eps "
                  "(using OpenGL feedback)\n");
  fprintf(stream, "%%%%BoundingBox: %g %g %g %g\n",
          viewport[0], viewport[1], viewport[2], viewport[3]);
  fputs("%%EndComments\n", stream);
  fputs("\n", stream);
  fputs("gsave\n", stream);
  fputs("\n", stream);
  
  /* Output Frederic Delhoume's "gouraudtriangle" PostScript fragment. */
  fputs("% the gouraudtriangle PostScript fragment below is free\n", stream);
  fputs("% written by Frederic Delhoume (delhoume@ilog.fr)\n", stream);
  fprintf(stream, "/threshold %g def\n", GOURAUD_THRESHOLD);
  for (i = 0; GOURAUD_TRIANGLE_EPS[i]; i++) {
    fprintf(stream, "%s\n", GOURAUD_TRIANGLE_EPS[i]);
  }
  
  fprintf(stream, "\n%g setlinewidth\n", line_width);
  
  /* Clear the background like OpenGL had it. */
  fprintf(stream, "%g %g %g setrgbcolor\n",
          clear_color[0], clear_color[1], clear_color[2]);
  fprintf(stream, "%g %g %g %g rectfill\n\n",
          viewport[0], viewport[1], viewport[2], viewport[3]);
  
  if (sort) {
    gl_feedbuf_spew_sorted(buf, stream);
  } else {
    gl_feedbuf_spew_unsorted(buf, stream);
  }
  
  /* Emit EPS trailer. */
  fputs("grestore\n\n", stream);
  fputs("%Add `showpage' to the end of this file to be able to print to a "
        "printer.\n", stream);
  
  return (!ferror(stream));
}
