#include "./9cc.h"

// 新しいトークンを作成してcurに繋げる
static Token *new_token(TokenKind kind, Token *cur, char *str, int len)
{
  Token *tok = (Token *)calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

/// @brief 文字列の先頭がqと同じ文字列で始まっているかを調べる
static bool start_str_with(char *p, char *q)
{
  return strncmp(p, q, strlen(q)) == 0;
}

static bool is_alpha(char c)
{
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

static bool is_alnum(char c)
{
  return is_alpha(c) || ('0' <= c && c <= '9');
}

// 入力文字列pが予約語であるかを調べる
// 予約語でない場合はNULLを返す
static char *starts_with_reserved(char *p)
{
  // Keywords
  static char *kw[] = {"return", "if", "else", "while"};
  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
  {
    int len = strlen(kw[i]);
    if (start_str_with(p, kw[i]) && !is_alnum(p[len]))
    {
      return kw[i];
    }
  }

  // 比較演算子
  static char *ops[] = {"==", "!=", "<=", ">="};
  for (int i = 0; i < sizeof(ops) / sizeof(*ops); i++)
  {
    int len = strlen(ops[i]);
    if (start_str_with(p, ops[i]))
    {
      return ops[i];
    }
  }

  return NULL;
}

// 入力文字列pをトークナイズしてそれを返す
Token *tokenize(char *p)
{
  Token head = {};
  // head.next = NULL;
  Token *cur = &head;

  while (*p)
  {
    // 空白スペースを読み飛ばす
    if (isspace(*p))
    {
      p++;
      continue;
    }

    char *kw = starts_with_reserved(p);
    if (kw)
    {
      int len = strlen(kw);
      cur = new_token(TK_RESERVED, cur, p, len);
      p += len;
      continue;
    }

    // Identifier
    if (is_alpha(*p))
    {
      char *q = p++;
      while (is_alnum(*p))
        p++;
      cur = new_token(TK_IDENT, cur, q, p - q);
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>' || *p == ';' || *p == '=')
    {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(p, "トークナイズできません");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}
