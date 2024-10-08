#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Type Type;

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
  char *name; // 変数の名前
  Type *ty;   // Type
  int offset; // RBPからのオフセット
};
typedef struct LVarList LVarList;
struct LVarList
{
  LVarList *next;
  LVar *var;
};

typedef enum
{
  ND_ADD,       // num + num
  ND_PTR_ADD,   // ptr + num or num + ptr
  ND_SUB,       // num - num
  ND_PTR_SUB,   // ptr - num
  ND_PTR_DIFF,  // ptr - ptr
  ND_MUL,       // *
  ND_DIV,       // /
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_ADDR,      // unary &
  ND_DEREF,     // unary *
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_WHILE,     // "while"
  ND_FOR,       // "for"
  ND_BLOCK,     // { ... }
  ND_FUNCALL,   // Function call
  ND_EXPR_STMT, // Expression statement
  ND_LVAR,      // ローカル変数
  ND_NUM,       // 整数
  ND_NULL       // 空の文
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *next; // Next node
  Type *ty;   // Type, e.g. int or pointer to int
  Token *tok; // Representative token
  Node *lhs;
  Node *rhs;

  // "if, "while" or "for" statement
  Node *cond;
  Node *then;
  Node *els;
  Node *init;
  Node *inc;

  // Block
  Node *body;

  // Function call
  char *funcname;
  Node *args;

  LVar *var; // Used if kind == ND_VAR
  long val;
  // int offset;
};

typedef struct Function Function;
struct Function
{
  Function *next;
  char *name;
  LVarList *params;
  Node *node;
  LVarList *locals;
  int stack_size;
};

//----------------------------------------
// util.c
//

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...);
void error_tok(Token *tok, char *fmt, ...);
//----------------------------------------

//----------------------------------------
// typing.c
//

typedef enum
{
  TY_INT,
  TY_PTR,
  TY_ARRAY,
} TypeKind;

struct Type
{
  TypeKind kind;
  int size; // sizeof() value
  Type *base;
  int array_len; // 配列の長さ
};

//tyが整数型かどうかを返す
bool is_integer(Type *ty);
// ポインター型に変換する
Type *pointer_to(Type *base);
// 配列型に変換する
Type *array_of(Type *base, int size);
// nodeとnode以下の全子要素に型を付与する
void add_type(Node *node);
//----------------------------------------

//----------------------------------------
// tokenize.c
//

// トークナイズ
Token *tokenize(char *p);
//----------------------------------------

//----------------------------------------
// parse.c
//

// パース
Function *program();
//----------------------------------------

//----------------------------------------
// codegen.c
//

// アセンブリを生成
void codegen(Function *node);
//----------------------------------------

// グローバル変数-----------------------------
//  入力プログラム
extern char *g_user_input;
// 現在着目しているトークン
extern Token *g_token;
// intの型
extern Type g_int_type;
//----------------------------------------
