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

#define LPAR    6
#define RPAR    7
#define LBRACK  8
#define RBRACK  9
#define LBRACE  10
#define RBRACE  11

#define COMMA   12
#define COLON   13
#define END     14
#define BINOP   15

#define RETURN  16
#define ASSIGN  17
#define ID      18
#define KEYW    19

#define ARRAY        100
#define MAP          101
#define PAIR         102
#define UNARY_MSG    103
#define BINARY_MSG   104
#define KEYWORD_MSG  105
#define KEYWORD_PAIR 106

#endif /* PARSER_H */

