#
# glututils.pro
# tmake project file
#
TEMPLATE        = lib
unix:VERSION    = 0.1
DESTDIR         = .
unix:CONFIG     = opengl warn_on debug dll # staticlib
win32:CONFIG    = opengl warn_on dll # staticlib
win32:DEFINES   = WIN32_BUILD_LIBRARY WIN32_DLL
INCLUDEPATH     = .
unix:LIBS       += -lglut
win32:LIBS      += glut32.lib
#
SOURCES         = glut_utils.c glut_widget.c
TARGET          = glututils
