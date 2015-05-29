#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

#define UNKNOWN     0
#define SYMBOL      1
#define KEYWORD     2
#define IDENTIFIER  3
#define NUMBER      4
#define STRING      5
#define CHARACTER   6
#define AT_LBRACK   7
#define AT_LBRACE   8
#define RW_SLOT     9
#define RO_SLOT     10
#define SEMICOLON   11
#define COLON       12
#define PIPE        13
#define LPAR        14
#define RPAR        15
#define LBRACK      16
#define RBRACK      17
#define LBRACE      18
#define RBRACE      19
#define OPERATOR    20

#define ARRAY        100
#define OBJECT       101
#define HASHMAP      102
#define HASHSET      103
#define PAIR         104
#define UNARY_MSG    105
#define BINARY_MSG   106
#define KEYWORD_MSG  107
#define KEYWORD_PAIR 108

/* Table of Contents
 *****************************************************************************/
// Types
typedef struct AST {
    int type;
    int num_children;
    struct AST* children[];
} AST;

// Globals
static FILE* File;
static int CurrChar = ' ';
static int CurrTok  = UNKNOWN;
static intptr_t Token_Buffer[1024];
static intptr_t* Token_Stack = Token_Buffer-1;

// Parsing Rules
static void expression(void);
static void keyword_send(void);
static void binary_send(void);
static void unary_send(void);
static void operand(void);
static void literal(void);
static void array(void);
static void hashmap(void);
static void hashset(void);
static void object(void);

// Parsing Helpers
static void error(const char* msg);
static bool accept(int expected);
static bool expect(int expected);
static bool optional(int expected);

// Lexical Rules
static int token(void);
static void whitespace(void);
static int symbol(void);
static int identifier(void);
static int number(void);
static int string(void);
static int character(void);
static int punctuation(void);
static int operator(void);

// Lexical Analysis Helpers
static int current(void);
static void append(void);
static void discard(void);
static void expect_ch(int ch);
static void lex_error(void);

// Tree Routines
static AST* Tree(int type, int num_children);
static void PrintTree(AST* tree, int depth);
void shift(int type);
void reduce(int count);
void shift_reduce(int type, int nchildren);
void push_reduce(int type, int nchildren);

/* Parsing Rules
 *****************************************************************************/
static void expression(void)
{
    keyword_send();
    optional(SEMICOLON);
}

static void keyword_send(void)
{
    int count  = 0;
    binary_send();
    while (accept(KEYWORD)) {
        shift_reduce(KEYWORD,0);
        binary_send();
        push_reduce(KEYWORD_PAIR, 2);
        count++;
    }
    if (count > 0) {
        push_reduce(KEYWORD_MSG, count);
    }
}

static void binary_send(void)
{
    unary_send();
    if (accept(OPERATOR)) {
        shift_reduce(OPERATOR,0);
        unary_send();
        push_reduce(BINARY_MSG, 3);
    }
}

static void unary_send(void)
{
    operand();
    while (accept(IDENTIFIER)) {
        shift_reduce(IDENTIFIER, 0);
        push_reduce(UNARY_MSG, 2);
    }
}

static void operand(void)
{
    if (accept(LPAR)) {
        expect(LPAR);
        expression();
        expect(RPAR);
    } else {
        literal();
    }
}

static void literal(void)
{
    switch (CurrTok) {
        case IDENTIFIER: shift_reduce(IDENTIFIER, 0u); push_reduce(UNARY_MSG, 1); break;
        case NUMBER:     shift_reduce(NUMBER, 0u);     break;
        case STRING:     shift_reduce(STRING, 0u);     break;
        case SYMBOL:     shift_reduce(SYMBOL, 0u);     break;
        case CHARACTER:  shift_reduce(CHARACTER, 0u);  break;
        case LBRACK:     array();                      break;
        case LBRACE:     object();                     break;
        case AT_LBRACE:  hashmap();                    break;
        case AT_LBRACK:  hashset();                    break;
        default:         error("Invalid literal");
    }
}

static void array(void)
{
    int count = 0;
    expect(LBRACK);
    while (!accept(RBRACK)) {
        expression();
        count++;
    }
    expect(RBRACK);
    push_reduce(ARRAY, count);
}

static void hashmap(void)
{
    int count = 0;
    expect(AT_LBRACE);
    while (!accept(RBRACE)) {
        shift_reduce(STRING, 0);
        expect(COLON);
        expression();
        push_reduce(PAIR, 2);
        count++;
    }
    expect(RBRACE);
    push_reduce(HASHMAP, count);
}

static void hashset(void)
{
    int count = 0;
    expect(AT_LBRACK);
    while (!accept(RBRACK)) {
        expression();
        count++;
    }
    expect(RBRACK);
    push_reduce(HASHSET, count);
}

static void object(void)
{
    expect(LBRACE);
    if (accept(PIPE)) {
        expect(PIPE);
        expect(PIPE);
    }
    while (!accept(RBRACE)) {
        expression();
    }
    expect(RBRACE);
    push_reduce(OBJECT, 0);
}

/* Parsing Helpers
 *****************************************************************************/
static void error(const char* msg)
{
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static bool accept(int expected)
{
    if (CurrTok == UNKNOWN) {
        CurrTok = token();
    }
    return (CurrTok == expected);
}

static bool expect(int expected)
{
    bool accepted = accept(expected);
    if (!accepted)
        error("unexpected symbol");
    else
        CurrTok = UNKNOWN;
    return accepted;
}

static bool optional(int expected)
{
    if (accept(expected))
        return expect(expected);
    else
        return false;
}

/* Lexical Rules
 *****************************************************************************/
static int token(void)
{
    // Skip any whitespace.
    whitespace();

    if (EOF == current()) {
        return EOF;
    } else if ('$' == current()) {
        return symbol();
    } else if (isalpha(current())) {
        return identifier();
    } else if (isdigit(current())) {
        return number();
    } else if ('"' == current()) {
        return string();
    } else if ('\'' == current()) {
        return character();
    } else {
        return punctuation();
    }
}

static void whitespace(void)
{
    while(isspace(current()) || ('#' == current())) {
        if ('#' == current()) {
            discard();
            while (('\n' != current()) && (EOF != current()))
                discard();
            discard();
        } else {
            discard();
        }
    }
}

static int symbol(void)
{
    expect_ch('$');
    while (isalnum(current()))
        append();
    return SYMBOL;
}

static int identifier(void)
{
    while (isalnum(current()))
        append();

    if (':' == current()) {
        expect_ch(':');
        return KEYWORD;
    } else {
        return IDENTIFIER;
    }
}

static int number(void)
{
    append();
    while (isdigit(current()))
        append();
    return NUMBER;
}

static int string(void)
{
    expect_ch('"');
    while('"' != current())
        append();
    expect_ch('"');
    return STRING;
}

static int character(void)
{
    expect_ch('\'');
    append();
    expect_ch('\'');
    return CHARACTER;
}

static int punctuation(void)
{
    if ('@' == current()) {
        expect_ch('@');
        if ('[' == current()) {
            expect_ch('[');
            return AT_LBRACK;
        } else if ('{' == current()) {
            expect_ch('{');
            return AT_LBRACE;
        } else {
            return operator();
        }
    } else if ('<' == current()) {
        expect_ch('<');
        if ('-' == current()) {
            expect_ch('-');
            return RW_SLOT;
        } else {
            return operator();
        }
    } else {
        switch(current()) {
            case '=': append(); return RO_SLOT;    break;
            case ';': append(); return SEMICOLON;  break;
            case ':': append(); return COLON;      break;
            case '|': append(); return PIPE;       break;
            case '(': append(); return LPAR;       break;
            case ')': append(); return RPAR;       break;
            case '[': append(); return LBRACK;     break;
            case ']': append(); return RBRACK;     break;
            case '{': append(); return LBRACE;     break;
            case '}': append(); return RBRACE;     break;
            default:            return operator(); break;
        }
    }
}

static int operator(void)
{
    lex_error();
    return UNKNOWN;
}

/* Lexical Analysis Helpers
 *****************************************************************************/
static void lex_error(void)
{
    fprintf(stderr, "Lexer error\n");
    exit(1);
}

static int current(void)
{
    return CurrChar;
}

static void append(void)
{
    CurrChar = fgetc(File);
}

static void discard(void)
{
    CurrChar = fgetc(File);
}

static void expect_ch(int ch)
{
    if (ch == current())
        append();
    else
        lex_error();
}

/* Tree Routines
 *****************************************************************************/
AST* Tree(int type, int num_children)
{
    AST* tree   = (AST*)calloc(1, sizeof(AST) * (num_children * sizeof(AST*)));
    tree->type  = type;
    tree->num_children = num_children;
    return tree;
}

void PrintTree(AST* tree, int depth)
{
    int indent = depth * 2;
    printf("%*s(", indent, "");
    printf("%d", tree->type);
    if (tree->num_children == 0) {
        printf(")\n");
    } else {
        printf("\n");
        for (int child = 0; child < tree->num_children; child++) {
            PrintTree(tree->children[child], depth+1);
        }

        printf("%*s)\n", indent, "");
    }
}

void shift(int type)
{
    int curr = CurrTok;
    if (expect(type)) {
        *(++Token_Stack) = curr;
    }
}

void reduce(int count)
{
    int type  = *(Token_Stack--);
    AST* tree = Tree(type, count);
    intptr_t* stack = Token_Stack - (count-1);
    for (int i = 0; i < count; i++) {
        tree->children[i] = (void*)stack[i];
    }
    Token_Stack -= count ;
    *(++Token_Stack) = (intptr_t)tree;
}

void shift_reduce(int type, int nchildren)
{
    shift(type);
    reduce(nchildren);
}

void push_reduce(int type, int nchildren)
{
    *(++Token_Stack) = type;
    reduce(nchildren);
}

/* Main
 *****************************************************************************/
int main(int argc, char** argv) {
    extern void world_init(void);
    File = stdin;
    world_init();
    printf(":> ");
    while(true) {
        expression();
        printf("stack: %du\n", (int)((Token_Stack - Token_Buffer) + 1));
        /* Print and clear */
        PrintTree((AST*)*Token_Stack, 0);
        Token_Stack = Token_Buffer-1;
    }
    (void)argc;
    (void)argv;
    return 0;
}

