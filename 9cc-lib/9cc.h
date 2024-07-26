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
  int val;
  char *str;
  int len;
};

typedef enum
{
  ND_EQ,     // ==
  ND_NE,     // !=
  ND_LT,     // <
  ND_LE,     // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_NUM,    // 整数
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *next; // Next node
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

void error(char *fmt, ...);
// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);

Node *program(void);

void codegen(Node *node);

// グローバル変数-----------------------------
//  入力プログラム
extern char *g_user_input;
// 現在着目しているトークン
extern Token *g_token;

// extern Node *g_code[100];
//----------------------------------------
