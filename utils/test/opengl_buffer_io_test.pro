#
# opengl_buffer_io_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = opengl warn_on debug
win32:CONFIG    = console opengl warn_on
INCLUDEPATH     = ..
unix:LIBS       +=  -lglut
win32:LIBS      +=  glut32.lib
#
SOURCES         = ../sgilib.c \
                  ../opengl_utils.c \
                  ../opengl_buffer.c \
                  ../opengl_buffer_io.c \
                  opengl_buffer_io_test.c
TARGET          = opengl_buffer_io_test
