#
# complx_test_c.pro
# tmake project file
#
TEMPLATE        = app
unix:CONFIG     = warn_on debug
win32:CONFIG    = console warn_on
INCLUDEPATH     = ..
#
SOURCES         = ../complx.c complx_test.c
TARGET          = complx_test
