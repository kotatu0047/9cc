// トークンの種類
typedef enum
{
  TK_RESERVED, // 記号
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
  ND_EQ,  // ==
  ND_NE,  // !=
  ND_LT,  // <
  ND_LE,  // <=
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // num
} NodeKind;

typedef struct Node Node;

struct Node
{
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...);
Token *tokenize(char *p);
Node *expr();
void codeGen(Node *node);

//グローバル変数-----------------------------
// 入力プログラム
extern char *g_user_input;
// 現在着目しているトークン
extern Token *token;
//----------------------------------------
