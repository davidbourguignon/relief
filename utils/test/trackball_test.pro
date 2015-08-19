#
# trackball_test.pro
# tmake project file
#
TEMPLATE      = app
unix:CONFIG   = opengl warn_on debug
win32:CONFIG  = console opengl warn_on
INCLUDEPATH   = ..
unix:LIBS     += -lglut
win32:LIBS    += glut32.lib
#
SOURCES       = ../vec3.c ../quat.c \
                ../opengl_utils.c ../opengl_widget.c \
                ../glut_utils.c ../glut_widget.c \
                ../trackball.c trackball_test.c
TARGET        = trackball_test
