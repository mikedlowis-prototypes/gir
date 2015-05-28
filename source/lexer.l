%{

#include "parser.h"

size_t Row = 1;
size_t Column = 1;

int yywrap(void){ return 1; }

#define TOKEN(type) \
    puts(#type); return type

%}

%%

[\r]?[\n]                   Row++; Column = 1; printf(":> ");
[ \t]                       Column++;
#.*[\r\n]+                  Row++; Column = 1;

"="                         TOKEN(ASSIGN);
"<-"                        TOKEN(ASSIGN);

"."                         TOKEN(END);
","                         TOKEN(COMMA);
":"                         TOKEN(COLON);
"|"                         TOKEN(PIPE);
"("                         TOKEN(LPAR);
")"                         TOKEN(RPAR);
"@["                        TOKEN(ALBRACK);
"["                         TOKEN(LBRACK);
"]"                         TOKEN(RBRACK);
"@{"                        TOKEN(ALBRACE);
"{"                         TOKEN(LBRACE);
"}"                         TOKEN(RBRACE);

"self"                      TOKEN(SELF);
"true"|"false"              TOKEN(BOOL);
[0-9]+                      TOKEN(NUM);
'[^\\]'                     TOKEN(CHAR);
\"[^"]*\"                   TOKEN(STRING);

[+-]+                       TOKEN(BINOP);
[_a-zA-Z][_a-zA-Z0-9]*      TOKEN(ID);
[_a-zA-Z][_a-zA-Z0-9]*:     TOKEN(KEYW);

.                           fprintf(stderr, "Unrecognized Token\n"); exit(1);

%%

