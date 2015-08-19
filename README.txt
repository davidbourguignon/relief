Relief: A Modeling by Drawing Tool.
David Bourguignon, Raphaëlle Chaine, Marie-Paule Cani and George Drettakis.
In Proceedings of the First Eurographics Workshop on Sketch-Based Interfaces
and Modeling, pp. 151-160, Eurographics Association, Aire-la-Ville, 2004.
----------------------------------------------------------------------------

License
-------
Copyright (c) 2004 INRIA. Please contact george.drettakis@sophia.inria.fr for
any inquiry concerning the use of this software for commercial purposes.

Requirements
------------
gtk+-2.0 (http://www.gtk.org/)
gtkglext-1.0 (http://gtkglext.sourceforge.net/)
cgal-3.0 (http://www.cgal.org/)
opengl-1.4 (http://www.opengl.org/)
linuxwacom-0.4 (http://linuxwacom.sourceforge.net/), under linux
tmake-2.12 (http://tmake.sourceforge.net/), for generating makefiles

Output
------
OFF format can be converted to IV (OpenInventor), VRML (1.0 or 2.0),
STL (Stereolithography), and OBJ (Maya) formats using off2* converters
located in CGAL-3.0/examples/Polyhedron_IO/

Compiling
---------
With gcc-3.3.1 under linux-2.4, setup the makefile and type 'make'.
Example values for the environment variables (using tcsh):
setenv CGAL_OS_COMPILER i686_Linux-2.4.18-24.8.0_g++-3.2.0
setenv CGAL_ROOT        /usr/local/CGAL
setenv CGAL_INC_DIR     ${CGAL_ROOT}/include
setenv CGAL_CNF_INC_DIR ${CGAL_INC_DIR}/CGAL/config/${CGAL_OS_COMPILER}
setenv CGAL_LIB_DIR     ${CGAL_ROOT}/lib/${CGAL_OS_COMPILER}
setenv CGAL_MAKEFILE    ${CGAL_ROOT}/make/makefile_${CGAL_OS_COMPILER}

With msvc-13.10.3052 under mswinxp, setup the makefile and type 'nmake'.

Manual
------
The r e l i e f system is a modeling by drawing tool, that is all operations
are achieved by drawing. The core idea is very simple: we assume that a surface
is defined by a combination of creases, bumps and flat areas. In the drawing
world, this is translated into shades (black), highlights (white) and neutral
colors (midgrey), resulting in an intuitive rule for traditional artists.

The system is composed of two elements: a toolbox, where the user will find
various 2D and 3D tools, and a viewer, where both 2D drawings (pixel-based) and
3D meshes (point-based) are created and manipulated. If the user does not load
an existing mesh, he starts with a blank canvas. Then, he selects a drawing
tool and draw with the LEFT mouse button pressed.

All actions obtained with the mouse can also be performed with a tablet stylus
and eraser. Before use, the tablet has to be setup by selecting:
"Preferences" > "Input devices" and choosing the appropriate mode:
"Device" = "stylus" and "Mode" = "Screen"
(The same settings are used for "eraser").
Note that the other mode (Window) is apparently doing nothing (this is a tablet
driver issue).

Toolbox contents:
1) Tools for drawing: "Pencil", "Quill", "Brush", "Smudge". (Self-explanatory.)
A tablet is absolutely necessary to achieve anything useful.
2) Tool for modifying the drawing: "Eraser".
3) Tool for masking: "Frisket". Useful for preventing actions to take place on
certain areas. It is not another eraser.
3) Tools for modifying the mesh: "Burnisher" (for smoothing the surface) and
"Scraper" (for removing parts of the surface).
4) Color switch (from black to white, and back).

The user can draw freely without worrying two much about the geometry and the
topology of his strokes. However, the current shape-from-strokes algorithm
cannot handle non-circular topologies (such as a disk with a hole in it). To
solve this problem, we provide the user with the possibility of marking areas
considered as holes with special "marking strokes" obtained by maintaining the
SHIFT key pressed on the keyboard while drawing. *** Currently, the system
handles only one marking stroke per hole; if this was found inconvenient, it is
possible to change this without too much trouble. ***

Once the drawing is finished, the user maintain the SPACE key pressed and
click with the LEFT mouse button. The system then reconstruct the 3D mesh
corresponding to the 2D drawing according to the shading convention described
in the introduction. The relation between the tones of the drawing and the
geometrical offset applied is not absolute. It is proportional to the distance
between the point of view and a point on the surface (the closer to the
surface, the smaller, and thus more precise, the offset), and inversely
proportional to the current size of the surface (a small surface undergoes
small deformations). *** Any other scheme could be readily implemented. ***

The 3D mesh can then be looked at from another viewpoint by using a trackball:
to this end, the user maintains one of the following keys pressed while the
mouse is moved with the LEFT button pressed.
1) Maintain the Z key pressed to obtain a translation in depth.
2) Maintain the X key pressed to obtain a translation in the image plane.
3) Maintain the C key pressed to obtain a rotation.
(Press H to print out a reminder of these keys on the shell console.)

By pressing the RIGHT mouse button, the user opens a popup menu.
1) In the "Edit" submenu, he can obtain various modeling effects that will be
described below.
2) In the "Style" submenu, he can choose among various 3D mesh rendering
styles. (Self-explanatory.)
3) In the "Display" submenu, he can select various display gadgets
(self-explanatory) or reinitialize the trackball position.

Once a new position has been found, the user can release the key and start
drawing again. Two options are possible concerning depth inference. They are
obtained by toggling "Draw at depth" in the "Edit" submenu of the popup menu.
The user switches from a full depth computation (using existing depth values
from the 3D mesh) to a drawing "at depth", that is, at the closest depth found
on the surface under the drawing. *** I am hesitating between this method
and a more common "at depth" selection by looking at the depth under the cursor
when the beginning of the first stroke is drawn. *** If the full depth
computation is chosen, the "Frisket" tool can be handy to first mask certain
depth areas,and then draw on top of them as if on empty space!

Finally, two operations are provided for ease of modeling in the "Edit" submenu
of the popup menu:
1) An "Undo" command to cancel the last modification of the mesh (to undo a
drawing action, simply use the "Eraser"!).
2) A "Blob it" command to automatically compute a shading corresponding to
a blobbing of the drawn shape. This shading can of course be edited afterwards
as a standard drawing.

Currently, the I/O of the system is very limited. Only .off (Object File
Format) mesh files can be read and written. However, this format is pretty
simple, can be opened by Geomview (http://www.geomview.org/) and converted
in many other formats (I recommend VRML 1.0 in the converters given above) and
reused in your "favorite" non-drawing-based modeling package... :-)

Enjoy! And please send me your complaints/comments/congrats. Thanks!

http://www.davidbourguignon.net/
