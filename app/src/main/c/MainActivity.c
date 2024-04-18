#include <android/log.h>
#include <assert.h>
#include <bare.h>
#include <jni.h>
#include <unistd.h>
#include <uv.h>

#include "MainActivity.bundle.h"

static int pfd[2];
static pthread_t thr;
static const char *tag = "myapp";

static void *thread_func(void*)
{
  ssize_t rdsz;
  char buf[128];
  while((rdsz = read(pfd[0], buf, sizeof buf - 1)) > 0) {
    if(buf[rdsz - 1] == '\n') --rdsz;
    buf[rdsz] = 0;  /* add null-terminator */
    __android_log_write(ANDROID_LOG_DEBUG, tag, buf);
  }
  return 0;
}

int start_logger(const char *app_name)
{
  tag = app_name;

  /* make stdout line-buffered and stderr unbuffered */
  setvbuf(stdout, 0, _IOLBF, 0);
  setvbuf(stderr, 0, _IONBF, 0);

  /* create the pipe and redirect stdout and stderr */
  pipe(pfd);
  dup2(pfd[1], 1);
  dup2(pfd[1], 2);

  /* spawn the logging thread */
  if(pthread_create(&thr, 0, thread_func, 0) == -1)
    return -1;
  pthread_detach(thr);
  return 0;
}

JNIEXPORT void JNICALL
Java_com_holepunch_bare_MainActivity_init (JNIEnv *env, jobject self) {
  int err;
  start_logger("my_bare_app");

  int argc = 0;
  char **argv = NULL;

  argv = uv_setup_args(argc, argv);

  js_platform_t *platform;
  err = js_create_platform(uv_default_loop(), NULL, &platform);
  assert(err == 0);

  bare_t *bare;
  err = bare_setup(uv_default_loop(), platform, NULL, argc, argv, NULL, &bare);
  assert(err == 0);

  uv_buf_t source = uv_buf_init((char *) bundle, bundle_len);

  bare_run(bare, "/main.bundle", &source);

  int exit_code;
  err = bare_teardown(bare, &exit_code);
  assert(err == 0);

  err = js_destroy_platform(platform);
  assert(err == 0);

  err = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
  assert(err == 0);

  err = uv_loop_close(uv_default_loop());
  assert(err == 0);
}
