#
# vec3_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = warn_on debug
win32:CONFIG    = console warn_on
INCLUDEPATH     = ..
#
SOURCES         = ../vec3.c vec3_test.c
TARGET          = vec3_test
