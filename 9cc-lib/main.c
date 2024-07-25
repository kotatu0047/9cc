#include <stdio.h>
#include <string.h>
#include "./9cc.h"

// bool at_eof()
// {
//   return token->kind == TK_EOF;
// }

//グローバル変数の初期化-----------------------------
// 入力プログラム
char *g_user_input = NULL;
// 現在着目しているトークン
Token *token = NULL;
//----------------------------------------


int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズしてパースする
  g_user_input = argv[1];
  token = tokenize(g_user_input);
  Node *node = expr();

  // 抽象構文木を下りながらコード生成
  codeGen(node);

  return 0;
}