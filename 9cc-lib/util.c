#include "./9cc.h"

// Reports an error and exit.
// void error(char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }

// エラー箇所を報告する
static void error_at(char *loc, char *fmt, va_list ap)
{
  int pos = loc - g_user_input;
  fprintf(stderr, "%s\n", g_user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  
  error_at(loc, fmt, ap);
}

// エラー箇所を報告する
void error_tok(Token *tok, char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  error_at(tok->str, fmt, ap);
}