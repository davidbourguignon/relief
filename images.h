#ifndef __IMAGES_H__
#define __IMAGES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/gl.h>

extern const char *BRUSH_XPM[];
extern const char *ERASER_XPM[];
extern const char *PENCIL_XPM[];
extern const char *QUILL_XPM[];
extern const char *SMUDGE_XPM[];

/*
 * Declarations
 */
extern GLubyte *color_image          (unsigned int size, GLubyte color);
extern GLubyte *circle_image         (unsigned int size,
                                      unsigned int circle_thickness,
                                      GLubyte color,
                                      GLubyte color_in, GLubyte color_out);
extern GLubyte *circle_gradient_image(unsigned int size,
                                      unsigned int circle_thickness,
                                      GLubyte color_begin, GLubyte color_end);

#ifdef __cplusplus
}
#endif

#endif /* __IMAGES_H__ */
