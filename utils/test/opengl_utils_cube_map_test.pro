#
# opengl_utils_cube_map_test.pro
# tmake project file
#
TEMPLATE      = app
unix:CONFIG   = opengl warn_on debug
win32:CONFIG  = console opengl warn_on
DEFINES       = OPENGL_UTILS_CUBE_MAP_TEST
INCLUDEPATH   = ..
#
SOURCES       = ../sgilib.c \
                ../opengl_utils.c \
                ../opengl_buffer.c \
                ../opengl_buffer_io.c \
                opengl_utils_cube_map_test.c
TARGET        = opengl_utils_cube_map_test
