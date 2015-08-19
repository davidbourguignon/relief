#
# quat_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = warn_on debug
win32:CONFIG    = console warn_on
INCLUDEPATH     = ..
#
SOURCES         = ../quat.c quat_test.c
TARGET          = quat_test
