#include "./9cc.h"

// グローバル変数の初期化-----------------------------
//  入力プログラム
char *g_user_input = NULL;
// 現在着目しているトークン
Token *g_token = NULL;
// intの型
Type g_int_type = {TY_INT, 8};
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
  Function *prog = program();

  // Assign offsets to local variables.
  for (Function *fn = prog; fn; fn = fn->next)
  {
    int offset = 0;
    for (LVarList *vl = fn->locals; vl; vl = vl->next)
    {
      LVar *var = vl->var;
      offset += var->ty->size;
      var->offset = offset;
    }
    fn->stack_size = offset;
  }

  // 抽象構文木を下りながらコード生成
  codegen(prog);

  return 0;
}