#
# opengl_utils_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = opengl warn_on debug
win32:CONFIG    = console opengl warn_on
INCLUDEPATH     = ..
#
SOURCES         = ../opengl_utils.c opengl_utils_test.c
TARGET          = opengl_utils_test
