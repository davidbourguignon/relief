#
# scalar_test.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = warn_on debug
win32:CONFIG    = console warn_on
INCLUDEPATH     = ..
#
SOURCES         = scalar_test.c
TARGET          = scalar_test
