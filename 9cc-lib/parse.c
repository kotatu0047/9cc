#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "./9cc.h"

static bool at_eof()
{
  return g_token->kind == TK_EOF;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
static void expect(char *op)
{
  if (g_token->kind != TK_RESERVED ||
      strlen(op) != g_token->len ||
      memcmp(g_token->str, op, g_token->len))
  {
    error_at(g_token->str, "'%s'ではありません", op);
  }
  g_token = g_token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
static int expect_number()
{
  if (g_token->kind != TK_NUM)
    error_at(g_token->str, "数ではありません");
  int val = g_token->val;
  g_token = g_token->next;
  return val;
}

// 次のトークンが変数の場合、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
static Token *consume_ident()
{
  if (g_token->kind != TK_IDENT)
    error_at(g_token->str, "変数ではありません");
  Token *token = g_token;
  g_token = g_token->next;
  return token;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
static bool consume(char *op)
{
  if (g_token->kind != TK_RESERVED ||
      strlen(op) != g_token->len ||
      memcmp(g_token->str, op, g_token->len))
  {
    return false;
  }
  g_token = g_token->next;
  return true;
}

Node *new_node(NodeKind kind)
{
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->kind = kind;
  return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs)
{
  Node *node = new_node(kind);
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

static Node *new_node_num(int val)
{
  Node *node = new_node(ND_NUM);
  node->val = val;
  return node;
}

static Node *new_node_ident(Token *tok)
{
  Node *node = (Node *)calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (tok->str[0] - 'a' + 1) * 8;
  return node;
}

static Node *expr();

static Node *primary()
{
  // 次のトークンが"("なら、"(" expr ")"のはず
  if (consume("("))
  {
    Node *node = expr();
    expect(")");
    return node;
  }

  // そうでなければ数値か変数のはず
  if (g_token->kind == TK_NUM)
  {
    return new_node_num(expect_number());
  }
  else if (g_token->kind == TK_IDENT)
  {
    Token *tok = consume_ident();
    if (tok)
    {
      return new_node_ident(tok);
    }
  }

  // 数値か変数でなければエラー
  error_at(g_token->str, "数値か変数である必要があります");
}

static Node *unary()
{
  if (consume("+"))
    return unary();
  if (consume("-"))
    return new_binary(ND_SUB, new_node_num(0), unary());
  return primary();
}

static Node *mul()
{
  Node *node = unary();

  for (;;)
  {
    if (consume("*"))
      node = new_binary(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_binary(ND_DIV, node, unary());
    else
      return node;
  }
}

static Node *add()
{
  Node *node = mul();

  for (;;)
  {
    if (consume("+"))
      node = new_binary(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_binary(ND_SUB, node, mul());
    else
      return node;
  }
}

static Node *relational()
{
  Node *node = add();

  for (;;)
  {
    if (consume("<"))
      node = new_binary(ND_LT, node, add());
    else if (consume("<="))
      node = new_binary(ND_LE, node, add());
    else if (consume(">"))
      node = new_binary(ND_LT, add(), node);
    else if (consume(">="))
      node = new_binary(ND_LE, add(), node);
    else
      return node;
  }
}

static Node *equality()
{
  Node *node = relational();

  for (;;)
  {
    if (consume("=="))
      node = new_binary(ND_EQ, node, relational());
    else if (consume("!="))
      node = new_binary(ND_NE, node, relational());
    else
      return node;
  }
}

static Node *assign()
{
  Node *node = equality();
  if (consume("="))
  {
    node = new_binary(ND_ASSIGN, node, assign());
  }
  return node;
}

static Node *expr()
{
  return assign();
}

static Node *stmt()
{
  Node *node = expr();
  expect(";");
  return node;
}

Node *program()
{
  Node head = {};
  Node *cur = &head;

  while (!at_eof())
  {
    cur->next = stmt();
    cur = cur->next;
  }
  return head.next;
}
