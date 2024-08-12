#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum
{
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

struct Token
{
  TokenKind kind;
  Token *next;
  long val;
  char *str;
  int len;
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar
{
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int offset; // RBPからのオフセット
};

typedef enum
{
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_WHILE,     // "while"
  ND_EXPR_STMT, // Expression statement
  ND_LVAR, // ローカル変数
  ND_NUM,  // 整数
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *next; // Next node
  Node *lhs;
  Node *rhs;
  
  // "if" or "while" statement
  Node *cond;
  Node *then;
  Node *els;

  LVar *var; // Used if kind == ND_VAR
  long val;
  // int offset;
};

typedef struct Function Function;
struct Function
{
  Node *node;
  LVar *locals;
  int stack_size;
};

// エラー箇所を報告する
void error(char *fmt, ...);
// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...);

// トークナイズ
Token *tokenize(char *p);
// パース
Function *program();
// アセンブリを生成
void codegen(Function *node);

// グローバル変数-----------------------------
//  入力プログラム
extern char *g_user_input;
// 現在着目しているトークン
extern Token *g_token;

// ローカル変数
extern LVar *g_locals;
//----------------------------------------
