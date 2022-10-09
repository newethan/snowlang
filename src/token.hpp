#pragma once

#include <iostream>
#include <regex>

#include "pos.hpp"

namespace snowlang
{
  enum TokenType
  {
    TT_NULL,   // null token
    TT_EOF,    // end of file
    TT_LBRACE, // brackets
    TT_RBRACE,
    TT_LBRACK,
    TT_RBRACK,
    TT_LPAREN,
    TT_RPAREN,
    TT_SEMICOLON, // punctuation and operations
    TT_COMMA,
    TT_PERIOD,
    TT_PLUS,
    TT_MINUS,
    TT_POW,
    TT_MULT,
    TT_DIV,
    TT_REM,
    TT_IMPORT, // keywords
    TT_MOD,
    TT_PRINT,
    TT_TICK,
    TT_HOLD,
    TT_LET,
    TT_CON,
    TT_ASSIGN,
    TT_IF,
    TT_ELIF,
    TT_ELSE,
    TT_FOR,
    TT_WHILE,
    TT_BREAK,
    TT_CONTINUE,
    TT_RETURN,
    TT_IN,
    TT_OR,
    TT_AND,
    TT_NOT,
    TT_GT,
    TT_GE,
    TT_EQ,
    TT_NEQ,
    TT_LE,
    TT_LT,
    TT_FLOAT, // tokens with variable value
    TT_INT,
    TT_IDEN,
    TT_STRLIT
  };

  struct Token
  {
    enum TokenType type = TT_NULL;
    std::string value;
    Pos pos;

    ////////////////////////
    //    constructors    //
    ////////////////////////

    // initializes null token
    Token() = default;

    // initializes token with given type, position and value (default empty)
    Token(enum TokenType t_type,
          const Pos &t_pos, const std::string &t_value = std::string())
        : type(t_type), value(t_value), pos(t_pos) {}
  };
}