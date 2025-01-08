#include <uv.h>

int main()
{
  int err;

  double uptime;
  err = uv_uptime(&uptime);
  printf("error is %d \n", err);
  printf("Uptime: %f\n", uptime);

  size_t resident_set_memory;
  err = uv_resident_set_memory(&resident_set_memory);
  printf("error is %d \n", err);
  printf("resident_set_memory: %ld\n", resident_set_memory);

  return 0;
}
