#
# vec2_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = warn_on debug
win32:CONFIG    = console warn_on
INCLUDEPATH     = ..
#
SOURCES         = ../vec2.c vec2_test.c
TARGET          = vec2_test
