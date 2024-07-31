#include "./9cc.h"

// Reports an error and exit.
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);

  fprintf(stderr, "loc is %p\n", loc);
  fprintf(stderr, "user_input is %p\n", g_user_input);

  int pos = loc - g_user_input;

  fprintf(stderr, "pos is %d\n", pos);

  fprintf(stderr, "%s\n", g_user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}