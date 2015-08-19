#
# openglutils.pro
# tmake project file
#
TEMPLATE        = lib
unix:VERSION    = 0.1
DESTDIR         = .
unix:CONFIG     = opengl warn_on debug dll # staticlib
win32:CONFIG    = opengl warn_on dll # staticlib
DEFINES         = OPENGL_FRAMEBUF_USE_NTSC_STD
win32:DEFINES   = WIN32_BUILD_LIBRARY WIN32_DLL
INCLUDEPATH     = .
#
SOURCES         = opengl_buffer.c \
                  opengl_buffer_edt.c opengl_buffer_eps.c \
                  opengl_buffer_ht.c opengl_buffer_io.c \
                  opengl_utils.c opengl_widget.c \
                  trackball.c trackdisk.c complx.c quat.c vec3.c vec2.c \
                  distmap.c sgilib.c
TARGET          = openglutils
