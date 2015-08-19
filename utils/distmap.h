#ifndef __DISTMAP_H__
#define __DISTMAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
** Adapted from:
** Olivier Cuisenaire - Euclidean Distance Transforms
** http://ltswww.epfl.ch/~cuisenai/DT/
*/

#include <platform_defs.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

EXTERND int distmap_4ssedp(int *map, int n[2]);

#ifdef __cplusplus
}
#endif

#endif /* !__DISTMAP_H__ */
