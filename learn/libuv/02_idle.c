#include "uv.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void idle_cb(uv_idle_t *handle)
{
  printf("idle_cb");
  static int64_t count = -1;
  count++;
  if ((count % 10000) == 0)
    printf(".");
  if (count >= 50000)
    ;
  // uv_idle_stop(handle);
}

void async_callback(uv_async_t *handle)
{
  printf("Async callback triggered, starting idle handle...\n");
}

void idle_cb2(uv_idle_t *handle)
{
  printf("idle_cb2");
  static int64_t count = -1;
  count++;
  if ((count % 10000) == 0)
    printf("2");
  if (count >= 50000)
    ;
}

int main()
{
  printf("debug 0");
  int err = 0;
  uv_idle_t idle_handle;
  // uv_idle_t idle_handle2;
  uv_async_t async_handle;

  printf("debug 1, %d, %p \n", err, &idle_handle);

  // /* 1. create the event loop */
  uv_loop_t *loop = malloc(sizeof(uv_loop_t));
  if (loop == NULL)
  {
    printf("Error: %s\n", strerror(errno));
  }

  // uv_default_loop()

  err = uv_loop_init(loop);
  printf("uv_loop_init err %d \n", err);
  printf("debug 3 \n");
  /* 2. initialize an idle handler for the loop */

  printf("Error: %s\n", strerror(errno));
  printf("err %d \n", err);

  // err = uv_idle_init(uv_default_loop(), &idle_handle);
  // err = uv_idle_init(uv_default_loop(), &idle_handle2);
  // // /* 3. start the idle handler with a function to call */
  // err = uv_idle_start(&idle_handle, idle_cb);
  // printf("Error: %s\n", strerror(errno));

  // err = uv_idle_start(&idle_handle2, idle_cb2);
  // printf("Error: %s\n", strerror(errno));

  uv_async_init(uv_default_loop(), &async_handle, async_callback);

  printf("debug 3 \n");
  // /* 4. start the event loop */
  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  // From another thread or part of the program, call this to add the idle task:
  uv_async_send(&async_handle);
  printf("end\n");
  return 0;
}
