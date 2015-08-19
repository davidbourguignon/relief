TEMPLATE          = app
unix:CONFIG       = opengl # warn_on debug
win32:CONFIG      = console opengl # warn_on
win32:TMAKE_CXXFLAGS += -GR -GX
#
DEFINES           = CGAL_USE_GMP
unix:INCLUDEPATH  =  /usr/include/gtkglext-1.0 \
                     /usr/lib/gtkglext-1.0/include \
                     /usr/include/gtk-2.0 \
                     /usr/lib/gtk-2.0/include \
                     /usr/include/atk-1.0 \
                     /usr/include/pango-1.0 \
                     /usr/include/Xft2 /usr/include/freetype2 \
                     /usr/include/glib-2.0 \
                     /usr/lib/glib-2.0/include \
                     $(CGAL_CNF_INC_DIR) $(CGAL_INC_DIR) \
                     $(HOME)/src/utils
win32:INCLUDEPATH =  C:/GtkGLExt/1.0/include/gtkglext-1.0 \
                     C:/GtkGLExt/1.0/lib/gtkglext-1.0/include \
                     C:/Gtk/2.0/include/gtk-2.0 \
                     C:/Gtk/2.0/lib/gtk-2.0/include \
                     C:/Gtk/2.0/include/atk-1.0 \
                     C:/Gtk/2.0/include/pango-1.0 \
                     C:/Gtk/2.0/include/glib-2.0 \
                     C:/Gtk/2.0/lib/glib-2.0/include \
                     C:/CGAL-3.0.1/auxiliary/wingmp/gmp-4.1.2/msvc \
                     C:/CGAL-3.0.1/include/CGAL/config/msvc7 \
                     C:/CGAL-3.0.1/include \
                     D:/home/dbourgui/src/utils
unix:LIBS         += -lXt -lSM -lICE -ldl \
                     -L/usr/lib \
                     -lgtkglext-x11-1.0 -lgdkglext-x11-1.0 \
                     -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 \
                     -latk-1.0 \
                     -lpangoxft-1.0 -lpangox-1.0 -lpango-1.0 \
                     -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 \
                     -L$(CGAL_LIB_DIR) -lCGAL -lgmp \
                     -L$(HOME)/$(HOSTTYPE)/lib -lopenglutils
win32:LIBS        += /libpath:C:/GtkGLExt/1.0/lib \
                     gtkglext-win32-1.0.lib gdkglext-win32-1.0.lib \
                     /libpath:C:/Gtk/2.0/lib \
                     gtk-win32-2.0.lib gdk-win32-2.0.lib \
                     atk-1.0.lib gdk_pixbuf-2.0.lib \
                     pangowin32-1.0.lib pangoft2-1.0.lib \
                     pango-1.0.lib \
                     gobject-2.0.lib gmodule-2.0.lib glib-2.0.lib \
                     intl.lib iconv.lib \
                     /libpath:C:/CGAL-3.0.1/lib/msvc7 \
                     CGAL.lib \
                     /libpath:C:/CGAL-3.0.1/auxiliary/wingmp/gmp-4.1.2/msvc \
                     gmp.lib
#
SOURCES           =  main.cc \
                     application.cc \
                     toolbox.cc \
                     tool.cc \
                     pencil_tool.cc \
                     quill_tool.cc \
                     brush_tool.cc \
                     smudge_tool.cc \
                     eraser_tool.cc \
                     frisket_tool.cc \
                     burnisher_tool.cc \
                     scraper_tool.cc \
                     tool_texture.cc \
                     file.cc \
                     drawing.cc \
                     triangulation.cc \
                     triangulation_display.cc \
                     reconstruct_curve.cc \
                     meshing.cc \
                     tetrahedrization.cc \
                     tetrahedrization_iostream.cc \
                     tetrahedrization_display.cc \
                     reconstruct_surface.cc \
                     smooth_surface.cc \
                     tesselation.cc \
                     viewer.cc \
                     debug.cc \
                     images.c
TARGET            =  relief
