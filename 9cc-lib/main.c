#include <stdio.h>
#include <string.h>
#include "./9cc.h"

// グローバル変数の初期化-----------------------------
//  入力プログラム
char *g_user_input = NULL;
// 現在着目しているトークン
Token *g_token = NULL;

// Node *g_code[100];
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
  g_token = tokenize(g_user_input);

  // 抽象構文木を作成
  Node *node = program();
  // 抽象構文木を下りながらコード生成
  codegen(node);

  return 0;
}