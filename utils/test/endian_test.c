#include <stdlib.h>
#include <stdio.h>

static const int UCHAR_ARRAY_SIZE = 16;
static const int UINT_ARRAY_SIZE = 4;

int
main(void) {
  int i = 0;
  unsigned char *puchar = NULL;
  unsigned int *puint = NULL;
  
  puchar = (unsigned char *) malloc(UCHAR_ARRAY_SIZE*sizeof(unsigned char));
  if (puchar == NULL) exit(1);
  for (i = 0; i < UCHAR_ARRAY_SIZE; i++) {
    puchar[i] = i;
  }
  printf("uchar_array: ");
  for (i = 0; i < UCHAR_ARRAY_SIZE; i++) {
    printf("%d ", puchar[i]);
  }
  printf("\n");
  puint = (unsigned int *) puchar;
  for (i = 0; i < UINT_ARRAY_SIZE; i++) {
    puint[i] = i;
  }
  printf("uint_array: ");
  for (i = 0; i < UINT_ARRAY_SIZE; i++) {
    printf("%d ", puint[i]);
  }
  printf("\n");
  printf("uchar_array: ");
  for (i = 0; i < UCHAR_ARRAY_SIZE; i++) {
    printf("%d ", puchar[i]);
  }
  printf("\n");
  return 0;
}

/* SUR SGI MIPS PROCESSOR : BIG ENDIAN
uchar_array: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
uint_array: 0 1 2 3
uchar_array: 0 0 0 0 0 0 0 1 0 0 0 2 0 0 0 3
*/

/* SUR INTEL x86 PROCESSOR : LITTLE ENDIAN
uchar_array: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
uint_array: 0 1 2 3
uchar_array: 0 0 0 0 1 0 0 0 2 0 0 0 3 0 0 0
*/

/*
     Endianness issues arise in many other cases, such as when defining RGB
     color values in a single 32-bit variable. For example, the following code
     from IRIX defines an array of eight RGB color values:

     static const uint colors[MAX_SIZE] = {
     0xff080000,
     0xff190100,
     0xff2a0100,
     0xff3b0100,
     0xff4c0200,
     0xff5d0240,
     0xff6e0280,
     0xff7f02c0
     }

     The values must be byte-wise reversed in Linux (preferably also using a
     #ifdef __linux__ / #endif preprocessor directive so that the code could
     still be shared between the two platforms) to the following: 

     static const uint colors[MAX_SIZE] = {
     0x000008ff,
     0x000119ff,
     0x00012aff,
     0x00013bff,
     0x00024cff,
     0x40025dff,
     0x80026eff,
     0xc0027fff,
     };
*/
