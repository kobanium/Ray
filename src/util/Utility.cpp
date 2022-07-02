#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "util/Utility.hpp"


////////////////////////////
//  テキスト入力 (float)  //
////////////////////////////
void
InputTxtFLT( const char *filename, float *ap, const int array_size )
{
  FILE *fp;
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
    exit(1);
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf_s(fp, "%f", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf(fp, "%f", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
      exit(1);
    }
  }
#endif
  fclose(fp);
}


/////////////////////////////
//  テキスト入力 (double)  //
/////////////////////////////
void
InputTxtDBL( const char *filename, double *ap, const int array_size )
{
  FILE *fp;
#if defined (_WIN32)
  errno_t err;

  err = fopen_s(&fp, filename, "r");
  if (err != 0) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf_s(fp, "%lf", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#else
  fp = fopen(filename, "r");
  if (fp == NULL) {
    std::cerr << "can not open -" << filename << "-" << std::endl;
  }
  for (int i = 0; i < array_size; i++) {
    if (fscanf(fp, "%lf", &ap[i]) == EOF) {
      std::cerr << "Read Error : " << filename << std::endl;
    }
  }
#endif
  fclose(fp);
}
