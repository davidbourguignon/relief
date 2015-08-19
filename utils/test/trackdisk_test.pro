#
# trackdisk_test.pro
# tmake project file
#
TEMPLATE          = app
unix:CONFIG       = opengl warn_on debug
win32:CONFIG      = console opengl warn_on
unix:INCLUDEPATH  = .. /usr/include/glib-2.0 /usr/lib/glib-2.0/include
win32:INCLUDEPATH = .. \
                    C:/gtk-2.0/include/glib-2.0 \
                    C:/gtk-2.0/lib/glib-2.0/include
unix:LIBS         += -lglut -lglib-2.0
win32:LIBS        += glut32.lib /libpath:C:/gtk-2.0/lib glib-2.0.lib
#
SOURCES           = ../vec2.c ../complx.c \
                    ../opengl_utils.c \
                    ../glut_utils.c ../glut_widget.c \
                    ../trackdisk.c trackdisk_test.c
TARGET            = trackdisk_test
