#include <uv.h>
#include "common.h"

#define BUF_SIZE 100
static const char *filename = "sample.txt";

/* forward declarations */
void open_cb(uv_fs_t *);
void read_cb(uv_fs_t *);
void close_cb(uv_fs_t *);

typedef struct context_struct
{
  uv_fs_t *open_req;
  uv_fs_t *read_req;
  uv_buf_t iov;
} context_t;

void open_cb(uv_fs_t *open_req)
{
  int r = 0;
  if (open_req->result < 0)
    CHECK(open_req->result, "uv_fs_open callback");
  printf("open_cb hello---\n");
  context_t *context = open_req->data;

  /* 3. Create buffer and initialize it */
  char *buffer = malloc(BUF_SIZE + 1);
  buffer[BUF_SIZE] = '\0';
  
  context->iov = uv_buf_init(buffer, BUF_SIZE);
printf("buffer hello---\n");
  /* 4. Setup read request */
  uv_fs_t *read_req = malloc(sizeof(uv_fs_t)); /* = malloc ... */
  context->read_req = read_req;                /* = ? */
  read_req->data = context;                    /* = ? */

  /* 5. Read from the file into the buffer */
  r = uv_fs_read(open_req->loop, read_req, open_req->result, &(context->iov), 1, 0, read_cb);
  if (r < 0)
    CHECK(r, "uv_fs_read");
}

void read_cb(uv_fs_t *read_req)
{
  int r = 0;
  ;
  if (read_req->result < 0)
    CHECK(read_req->result, "uv_fs_read callback");

  context_t *context = (context_t *)read_req->data; /* = ? */

  /* 6. Report the contents of the buffer */
  printf("%s \n", context->iov.base);
  printf("%s \n", context->iov.base);

  free(context->iov.base);

  /* 7. Setup close request */
  uv_fs_t *close_req = malloc(sizeof(uv_fs_t));
  close_req->data = context;
  /* ? */

  /* 8. Close the file descriptor */
  r = uv_fs_close(read_req->loop, close_req, context->open_req->result, close_cb);
  if (r < 0)
    CHECK(r, "uv_fs_close");
}

void close_cb(uv_fs_t *close_req)
{
  if (close_req->result < 0)
    CHECK(close_req->result, "uv_fs_close callback");

  context_t *context = close_req->data;

  /* 9. Cleanup all requests and context */
  uv_fs_req_cleanup(context->open_req);
  uv_fs_req_cleanup(context->read_req);
  uv_fs_req_cleanup(close_req);
  free(context);
}

void init(uv_loop_t *loop)
{
  int r;
  printf("hello---\n");
  uv_fs_t *open_req = malloc(sizeof(uv_fs_t));

  /* 1. Setup open request */
  context_t *context = malloc(sizeof(context_t));
  context->open_req = open_req;
  open_req->data = context;
  printf("hello---\n");

  /* 2. Open file */
  r = uv_fs_open(loop, context->open_req, filename, O_RDONLY, 0400, open_cb);
  if (r < 0)
    CHECK(r, "uv_fs_open");
  printf("uv_fs_open--- %d \n", r);
}

int main()
{

uv_tcp_t;
uv_stream_t;
  char *test = NULL;
  char *test2 = "xxxxxxxxxxxx";
  char *test3 = "\0";

  printf("%s %s %s \n", test, test2, test3);
  printf("hello---\n");
  uv_loop_t *loop = uv_default_loop();

  init(loop);

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}