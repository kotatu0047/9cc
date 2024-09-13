#include "./9cc.h"

// 解析中に作成されたすべてのローカル変数インスタンスは
// このリストに蓄積されます。
static LVarList *locals;

// Find a local variable by name.
static LVar *find_var(Token *tok)
{
  for (LVarList *vl = locals; vl; vl = vl->next)
  {
    LVar *var = vl->var;
    if (strlen(var->name) == tok->len && !strncmp(tok->str, var->name, tok->len))
      return var;
  }
  return NULL;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 読んだトークンを返す。それ以外の場合には偽を返す。
static Token *consume(char *op)
{
  if (g_token->kind != TK_RESERVED ||
      strlen(op) != g_token->len ||
      memcmp(g_token->str, op, g_token->len))
  {
    return NULL;
  }
  Token *t = g_token;
  g_token = g_token->next;
  return t;
}

// 次のトークンが変数の場合、トークンを1つ読み進める。
// それ以外の場合にはNULLを返す
static Token *consume_ident(void)
{
  if (g_token->kind != TK_IDENT)
    return NULL;
  Token *t = g_token;
  g_token = g_token->next;
  return t;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
static void expect(char *op)
{
  if (g_token->kind != TK_RESERVED ||
      strlen(op) != g_token->len ||
      strncmp(g_token->str, op, g_token->len))
  {
    error_tok(g_token, "'%s'ではありません", op);
  }
  g_token = g_token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
static long expect_number()
{
  if (g_token->kind != TK_NUM)
    error_tok(g_token, "数ではありません");
  long val = g_token->val;
  g_token = g_token->next;
  return val;
}

// 現在のトークンが TK_IDENT であることを確認します。
char *expect_ident()
{
  if (g_token->kind != TK_IDENT)
    error_tok(g_token, "識別子ではありません");
  char *name = strndup(g_token->str, g_token->len);
  g_token = g_token->next;
  return name;
}

static bool at_eof()
{
  return g_token->kind == TK_EOF;
}

static Node *new_node(NodeKind kind, Token *tok)
{
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->kind = kind;
  node->tok = tok;
  return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok)
{
  Node *node = new_node(kind, tok);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_unary(NodeKind kind, Node *expr, Token *tok)
{
  Node *node = new_node(kind, tok);
  node->lhs = expr;
  return node;
}

static Node *new_node_num(long val, Token *tok)
{
  Node *node = new_node(ND_NUM, tok);
  node->val = val;
  return node;
}

static Node *new_var_node(LVar *var, Token *tok)
{
  Node *node = new_node(ND_LVAR, tok);
  node->var = var;
  return node;
}

// static Node *new_node_ident(Token *tok)
// {
//   Node *node = (Node *)calloc(1, sizeof(Node));
//   node->kind = ND_LVAR;
//   node->offset = (tok->str[0] - 'a' + 1) * 8;
//   return node;
// }

static LVar *new_lvar(char *name)
{
  LVar *var = (LVar *)calloc(1, sizeof(LVar));
  var->name = name;

  LVarList *vl = (LVarList *)calloc(1, sizeof(LVarList));
  vl->var = var;
  vl->next = locals;
  locals = vl;
  return var;
}

static Function *function();
static Node *stmt();
static Node *expr();
static Node *assign();
static Node *equality();
static Node *relational();
static Node *add();
static Node *mul();
static Node *unary();
static Node *primary();

static Node *read_expr_stmt()
{
  Token *tok = g_token;
  return new_unary(ND_EXPR_STMT, expr(), tok);
}

// program = function*
Function *program()
{
  Function head = {};
  Function *cur = &head;

  while (!at_eof())
  {
    cur->next = function();
    cur = cur->next;
  }

  return head.next;
}

static LVarList *read_func_params()
{
  if (consume(")"))
    return NULL;

  LVarList *head = (LVarList *)calloc(1, sizeof(LVarList));
  head->var = new_lvar(expect_ident());
  LVarList *cur = head;

  while (!consume(")"))
  {
    expect(",");
    cur->next = (LVarList *)calloc(1, sizeof(LVarList));
    cur->next->var = new_lvar(expect_ident());
    cur = cur->next;
  }

  return head;
}

// function = ident "(" params? ")" "{" stmt* "}"
// params = ident ("," ident)*
static Function *function()
{
  locals = NULL;
  Function *fn = (Function *)calloc(1, sizeof(Function));
  fn->name = expect_ident();
  expect("(");
  fn->params = read_func_params();
  expect("{");

  Node head = {};
  Node *cur = &head;
  while (!consume("}"))
  {
    cur->next = stmt();
    cur = cur->next;
  }

  fn->node = head.next;
  fn->locals = locals;
  return fn;
}

// stmt = "return" expr ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "{" stmt* "}"
//      | expr ";"
static Node *stmt()
{
  Token *tok;

  if (tok = consume("return"))
  {
    Node *node = new_unary(ND_RETURN, expr(), tok);
    expect(";");
    return node;
  }

  if (tok = consume("if"))
  {
    Node *node = new_node(ND_IF, tok);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    node->els = NULL;
    if (consume("else"))
    {
      node->els = stmt();
    }

    return node;
  }

  if (tok = consume("while"))
  {
    Node *node = new_node(ND_WHILE, tok);
    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();
    node->els = NULL;
    return node;
  }

  if (tok = consume("for"))
  {
    Node *node = new_node(ND_FOR, tok);
    expect("(");
    if (!consume(";"))
    {
      node->init = read_expr_stmt();
      expect(";");
    }
    if (!consume(";"))
    {
      node->cond = expr();
      expect(";");
    }
    if (!consume(")"))
    {
      node->inc = read_expr_stmt();
      expect(")");
    }
    node->then = stmt();
    node->els = NULL;
    return node;
  }

  if (tok = consume("{"))
  {
    Node head = {};
    Node *cur = &head;
    while (!consume("}"))
    {
      cur->next = stmt();
      cur = cur->next;
    }

    Node *node = new_node(ND_BLOCK, tok);
    node->body = head.next;
    return node;
  }

  Node *node = read_expr_stmt();
  expect(";");
  return node;
}

static Node *expr()
{
  return assign();
}

static Node *assign()
{
  Node *node = equality();
  Token *tok;
  if (tok = consume("="))
  {
    node = new_binary(ND_ASSIGN, node, assign(), tok);
  }
  return node;
}

static Node *equality()
{
  Node *node = relational();
  Token *tok;

  for (;;)
  {
    if (tok = consume("=="))
      node = new_binary(ND_EQ, node, relational(), tok);
    else if (tok = consume("!="))
      node = new_binary(ND_NE, node, relational(), tok);
    else
      return node;
  }
}

static Node *relational()
{
  Node *node = add();
  Token *tok;

  for (;;)
  {
    if (tok = consume("<"))
      node = new_binary(ND_LT, node, add(), tok);
    else if (tok = consume("<="))
      node = new_binary(ND_LE, node, add(), tok);
    else if (tok = consume(">"))
      node = new_binary(ND_LT, add(), node, tok);
    else if (tok = consume(">="))
      node = new_binary(ND_LE, add(), node, tok);
    else
      return node;
  }
}

static Node *add()
{
  Node *node = mul();
  Token *tok;

  for (;;)
  {
    if (tok = consume("+"))
      node = new_binary(ND_ADD, node, mul(), tok);
    else if (tok = consume("-"))
      node = new_binary(ND_SUB, node, mul(), tok);
    else
      return node;
  }
}

static Node *mul()
{
  Node *node = unary();
  Token *tok;

  for (;;)
  {
    if (tok = consume("*"))
      node = new_binary(ND_MUL, node, unary(), tok);
    else if (tok = consume("/"))
      node = new_binary(ND_DIV, node, unary(), tok);
    else
      return node;
  }
}

static Node *unary()
{
  Token *tok;

  if (consume("+"))
    return unary();
  if (tok = consume("-"))
    return new_binary(ND_SUB, new_node_num(0, tok), unary(), tok);
  return primary();
}

// func-args = "(" (assign ("," assign)*)? ")"
static Node *func_args()
{
  if (consume(")"))
    return NULL;

  Node *head = assign();
  Node *cur = head;
  while (consume(","))
  {
    cur->next = assign();
    cur = cur->next;
  }

  expect(")");
  return head;
}

// primary = "(" expr ")" | ident func-args? | num
static Node *primary()
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値か変数か関数のはず
  Token *tok;
  if (tok = consume_ident())
  {
    // Function call
    if (consume("("))
    {
      Node *node = new_node(ND_FUNCALL, tok);
      node->funcname = strndup(tok->str, tok->len);
      node->args = func_args();
      return node;
    }

    // 変数
    LVar *var = find_var(tok);
    if (!var)
    {
      var = new_lvar(strndup(tok->str, tok->len));
    }
    return new_var_node(var, tok);
  }

  // 数値のはず
  tok = g_token;
  if (tok->kind != TK_NUM)
    error_tok(tok, "数値でも識別子でもないトークンです");

  return new_node_num(expect_number(), tok);
}
