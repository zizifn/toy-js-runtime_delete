
#ifndef __LEARNUV_H__
#define __LEARNUV_H__
#include <errno.h>
#include <string.h>
#include <stdlib.h> 

#define CHECK(r, msg) if (r) {                                                       \
  printf("Error: %s , %s\n", msg, strerror(errno)); \
  exit(1);                                                                           \
}

#endif