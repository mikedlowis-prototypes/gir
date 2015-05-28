/**
  @file parser.h
  @brief TODO: Describe this file
  */
#ifndef PARSER_H
#define PARSER_H

#define UNKNOWN 0

#define NUM     1
#define SELF    2
#define STRING  3
#define BOOL    4
#define CHAR    5

#define LPAR    10
#define RPAR    11
#define ALBRACK 12
#define LBRACK  13
#define RBRACK  14
#define ALBRACE 15
#define LBRACE  16
#define RBRACE  17

#define COMMA   20
#define COLON   21
#define END     22
#define BINOP   23
#define PIPE    24

#define RETURN  30
#define ASSIGN  31
#define ID      32
#define KEYW    33

#define ARRAY        100
#define OBJECT       101
#define HASHMAP      102
#define HASHSET      103

#define PAIR         104
#define UNARY_MSG    105
#define BINARY_MSG   106
#define KEYWORD_MSG  107
#define KEYWORD_PAIR 108

#endif /* PARSER_H */

