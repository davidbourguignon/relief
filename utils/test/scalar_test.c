#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <scalar.h>

int
main(int argc, char **argv) {
  double ad = 1.0,  bd = -2.0,  td = 0.5,  tmp1d = 0.0,  tmp2d = 0.0;
  float  af = 1.0f, bf = -2.0f, tf = 0.5f, tmp1f = 0.0f, tmp2f = 0.0f;
  int    ai = 1,    bi = -2,               tmp1i = 0,    tmp2i = 0;
  char file_named[10] = "outd.bin";
  char file_namef[10] = "outf.bin";
  char file_namei[10] = "outi.bin";
  FILE *filed = NULL, *filef = NULL, *filei = NULL;
  
  printf("a = 1.0 b = -2.0 t = 0.5\n");
  printf("\nmax(a, b)\n");
  printf("%f\n", scald_max(ad, bd));
  printf("%f\n", scalf_max(af, bf));
  printf("%i\n", scali_max(ai, bi));
  printf("\nmin(a, b)\n");
  printf("%f\n", scald_min(ad, bd));
  printf("%f\n", scalf_min(af, bf));
  printf("%i\n", scali_min(ai, bi));
  printf("\nlerp(a, b, t)\n");
  printf("%f\n", scald_lerp(ad, bd, td));
  printf("%f\n", scalf_lerp(af, bf, tf));
  printf("\nlog2(2) = %f\n", scald_log2(2.0));
  printf("\n*_power_of_two(a)\n");
  printf("64 is power of two? %s\n",
         scali_is_power_of_two(64) ? "Yes" : "No");
  printf("65 is power of two? %s\n",
         scali_is_power_of_two(65) ? "Yes" : "No");
  printf("65 nearest power of two? %i\n",
         scali_nearest_power_of_two(65));
  assert(argc == 1);
  
  if ((filed = fopen(file_named, "wb+")) == NULL) {
    fprintf(stderr, "Error: Unable to open file!\n");
    exit(1);
  }
  if ((filef = fopen(file_namef, "wb+")) == NULL) {
    fprintf(stderr, "Error: Unable to open file!\n");
    exit(1);
  }
  if ((filei = fopen(file_namei, "wb+")) == NULL) {
    fprintf(stderr, "Error: Unable to open file!\n");
    exit(1);
  }
  printf("\nwrite(a, file) and write(b, file)\n");
  scald_write(ad, filed);
  scald_write(bd, filed);
  assert(!ferror(filed));
  scalf_write(af, filef);
  scalf_write(bf, filef);
  assert(!ferror(filef));
  scali_write(ai, filei);
  scali_write(bi, filei);
  assert(!ferror(filei));
  rewind(filed);
  rewind(filef);
  rewind(filei);
  printf("read(a, file) and read(b, file)\n");
  tmp1d = scald_read(filed);
  tmp2d = scald_read(filed);
  tmp1f = scalf_read(filef);
  tmp2f = scalf_read(filef);
  tmp1i = scali_read(filei);
  tmp2i = scali_read(filei);
  printf("%f %f\n", tmp1d, tmp2d);
  printf("%f %f\n", tmp1f, tmp2f);
  printf("%i %i\n", tmp1i, tmp2i);
  fclose(filed);
  fclose(filef);
  fclose(filei);
  
  return 0;
}
