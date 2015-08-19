#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "images.h"

const char *BRUSH_XPM[] = {
  "24 24 5 1",
  ".    c None",
  "+    c #000000",
  "@    c #DBDBDB",
  "#    c #929292",
  "$    c #FFFFFF",
  "........................",
  "........................",
  "........................",
  "........................",
  "..............+@@+#.....",
  "..............+@@+#.....",
  ".............+@@+##.....",
  ".............+@@+#......",
  "............+@@+##......",
  "............+@@+#.......",
  "...........+@@+##.......",
  "...........+@@+#........",
  "..........++++##........",
  ".........+@@++#.........",
  "........+$$@++#.........",
  "........+$@#++#.........",
  ".......+$@#++##.........",
  ".......+@#++##..........",
  "......+$#++##...........",
  ".....++++###............",
  "......####..............",
  "........................",
  "........................",
  "........................"
};

const char *ERASER_XPM[] = {
  "24 24 5 1",
  ".    c None",
  "+    c #000000",
  "@    c #FFFFFF",
  "#    c #929292",
  "$    c #DBDBDB",
  "........................",
  "........................",
  "........................",
  "........................",
  "........................",
  "........................",
  "........................",
  "...........++++++++.....",
  "..........+@@@@@@++#....",
  ".........+@@@@@@+$+#....",
  "........+@@@@@@+$$+#....",
  ".......+@@@@@@+$$+##....",
  "......+@@@@@@+$$+##.....",
  ".....+@@@@@@+$$+##......",
  "....++++++++$$+##.......",
  "....+$$$$$$+$+##........",
  "....+$$$$$$++##.........",
  "....++++++++##..........",
  ".....########...........",
  "........................",
  "........................",
  "........................",
  "........................",
  "........................"
};

const char *PENCIL_XPM[] = {
  "24 24 5 1",
  ".    c None",
  "+    c #000000",
  "@    c #DBDBDB",
  "#    c #929292",
  "$    c #FFFFFF",
  "........................",
  "........................",
  "........................",
  "........................",
  "...........++++.........",
  "...........+@@@+........",
  "..........+@@@@+#.......",
  "..........++@@+##.......",
  ".........+$$+++#........",
  ".........+$@@+##........",
  "........+$$@@+#.........",
  "........+$@@+##.........",
  ".......+$$@@+#..........",
  ".......+$@@+##..........",
  ".......++@@+#...........",
  ".......++++##...........",
  ".......+++##............",
  ".......++##.............",
  ".......+##..............",
  "........#...............",
  "........................",
  "........................",
  "........................",
  "........................"
};

const char *QUILL_XPM[] = {
  "24 24 5 1",
  ".    c None",
  "+    c #000000",
  "@    c #929292",
  "#    c #DBDBDB",
  "$    c #FFFFFF",
  "........................",
  "........................",
  "........................",
  "................++......",
  "...............++@+.....",
  "..............++@+++....",
  ".............++@++++....",
  "............++@++++@....",
  "...........++@++++@.....",
  "..........++@++++@......",
  "........+++@++++@.......",
  "......+++++++++@........",
  ".....++#$$++++@.........",
  ".....+$+@+#++@..........",
  "....++$#+@#++@..........",
  "....+$$+@+@+@...........",
  "....+$+@##++@...........",
  "...+$+@#+++@............",
  "...++@+++@@.............",
  "..++++@@@...............",
  "..++@@..................",
  "...@@...................",
  "........................",
  "........................"
};

const char *SMUDGE_XPM[] = {
  "24 24 5 1",
  ".    c None",
  "+    c #000000",
  "@    c #FFFFFF",
  "#    c #929292",
  "$    c #DBDBDB",
  "........................",
  "........................",
  "........................",
  "........................",
  "...........+@@@.........",
  "..........+@@@@@@+......",
  ".........+@@@@@@@+#.....",
  "........+@@@@@@@@+#.....",
  ".......+@+@@@@@@@$+.....",
  "......+@+@+@@@@@$$+#....",
  "......++@+@+@@@$$$+#....",
  "......+@+@+@@$+$$#+#....",
  "......++@+@@$++$##+#....",
  ".......+$+@$+#+###+#....",
  ".......++@@+++###+##....",
  "........+@$+####+##.....",
  ".......+@@++++++##......",
  ".......+@$+######.......",
  "......+@$+##............",
  ".....+@$+##.............",
  ".....+++##..............",
  "......###...............",
  "........................",
  "........................"
};

/*
 * Definitions
 */
GLubyte *
color_image(unsigned int size, GLubyte color) {
  GLubyte *data = NULL;
  unsigned int i;
  
  data = (GLubyte *) malloc(size*size*sizeof(GLubyte));
  assert(data != NULL);
  for (i = 0; i < size*size; i++) {
    data[i] = color;
  }
  return data;
}

GLubyte *
circle_image(unsigned int size, unsigned int circle_thickness,
             GLubyte color, GLubyte color_in, GLubyte color_out) {
  double circle_thickness_mod = (circle_thickness > 0 ? circle_thickness : 1);
  double outside_radius = size/2;
  double inside_radius = outside_radius - circle_thickness_mod;
  GLubyte *data = NULL;
  GLubyte c;
  unsigned int i, j;
  
  data = (GLubyte *) malloc(size*size*sizeof(GLubyte));
  assert(data != NULL);
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      double dist_x = (double) i - outside_radius;
      double dist_y = (double) j - outside_radius;
      double dist = sqrt(dist_x*dist_x + dist_y*dist_y);
      if (dist <= inside_radius) {
        c = color_in;
      } else if (dist > inside_radius && dist < outside_radius) {
        c = color;
      } else {
        c = color_out;
      }
      data[i*size + j] = c;
    }
  }
  return data;
}

GLubyte *
circle_gradient_image(unsigned int size, unsigned int circle_thickness,
                      GLubyte color_begin, GLubyte color_end) {
  double circle_thickness_mod = (circle_thickness > 0 ? circle_thickness : 1);
  double thickness_inv = 1.0/circle_thickness_mod;
  double outside_radius = size/2 - 2; // to avoid clipping circle border
  double inside_radius = outside_radius - circle_thickness_mod;
  double color_first = (color_begin != 0 ? color_begin : color_begin + 1);
  double color_last = (color_end != 0 ? color_end : color_end + 1);
  GLubyte *data = NULL;
  GLubyte c;
  unsigned int i, j;
  
  data = (GLubyte *) malloc(size*size*sizeof(GLubyte));
  assert(data != NULL);
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      double dist_x = (double) i - (outside_radius + 2);
      double dist_y = (double) j - (outside_radius + 2);
      double dist = sqrt(dist_x*dist_x + dist_y*dist_y);
      if (dist <= inside_radius) {
        c = color_begin;
      } else if (dist > inside_radius && dist < outside_radius) {
        double alpha = (dist - inside_radius)*thickness_inv;
        c = (GLubyte) ((1.0 - alpha)*color_first + alpha*color_last);
      } else {
        c = color_end;
      }
      data[i*size + j] = c;
    }
  }
  return data;
}
