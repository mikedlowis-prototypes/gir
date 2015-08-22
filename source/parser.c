/**
  @file parser.c
*/
#include "gir_internals.h"

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

#define ARRAY       100
#define OBJECT      101
#define HASHMAP     102
#define HASHSET     103
#define PAIR        104
#define UNARY_MSG   105
#define BINARY_MSG  106
#define KEYWORD_MSG 107
#define SELECTOR    108

#ifndef container_of
#define container_of(obj, type, member) \
    (type*)((uintptr_t)obj - offsetof(type, member))
#endif

/* Table of Contents
 *****************************************************************************/
// Types
typedef struct AST {
    slist_node_t link;
    slist_t children;
    char* text;
    int type;
} AST;

typedef struct strbuf_t {
    size_t index;
    size_t capacity;
    char*  string;
} strbuf_t;

// Globals
static FILE* File;
static int CurrChar = ' ';
static int Line = 1;
static int Column = 0;
static int CurrTok  = UNKNOWN;
static strbuf_t Token = {0,0,0};

// Parsing Rules
static AST* expression(void);
static AST* keyword_send(void);
static AST* binary_send(void);
static AST* unary_send(void);
static AST* operand(void);
static AST* literal(void);
static AST* array(void);
static AST* hashmap(void);
static AST* hashset(void);
static AST* object(void);

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
static void fetch(void);

// Tree Routines
static AST* ast_new(int type);
static AST* ast_tok(int type);
static void ast_add_child(AST* parent, AST* child);
static void ast_print(AST* tree, int depth);

// String Buffer
static void strbuf_init(strbuf_t* buf);
static void strbuf_putc(strbuf_t* buf, int ch);
static char* strbuf_string(strbuf_t* buf);

/*
 *****************************************************************************/
void exec_file(FILE* file, const char* prompt)
{
    File = file;
    while(true) {
        printf("%s", prompt);
        ast_print(expression(), 0);
    }
}

/* Parsing Rules
 *****************************************************************************/
static AST* expression(void)
{
    AST* expr = keyword_send();
    optional(SEMICOLON);
    return expr;
}

static AST* keyword_send(void)
{
    AST* expr = binary_send();
    AST* sel  = NULL;
    while (accept(KEYWORD)) {
        if (sel == NULL) {
            sel = ast_new(SELECTOR);
            ast_add_child(expr, sel);
        }
        ast_add_child(sel, ast_tok(KEYWORD));
        ast_add_child(expr, binary_send());
    }
    return expr;
}

static AST* binary_send(void)
{
    AST* expr = unary_send();
    if (accept(OPERATOR)) {
        AST* msg = ast_new(BINARY_MSG);
        ast_add_child(msg, expr);
        ast_add_child(msg, ast_new(OPERATOR));
        ast_add_child(msg, unary_send());
        expr = msg;
    }
    return expr;
}

static AST* unary_send(void)
{
    AST* expr = operand();
    while (accept(IDENTIFIER)) {
        AST* msg = ast_new(UNARY_MSG);
        ast_add_child(msg, expr);
        ast_add_child(msg, ast_tok(IDENTIFIER));
        expr = msg;
    }
    return expr;
}

static AST* operand(void)
{
    AST* obj = NULL;
    if (accept(LPAR)) {
        expect(LPAR);
        obj = expression();
        expect(RPAR);
    } else {
        obj = literal();
    }
    return obj;
}

static AST* literal(void)
{
    AST* obj = NULL;
    switch (CurrTok) {
        case IDENTIFIER: obj = ast_tok(IDENTIFIER); break;
        case NUMBER:     obj = ast_tok(NUMBER);     break;
        case STRING:     obj = ast_tok(STRING);     break;
        case SYMBOL:     obj = ast_tok(SYMBOL);     break;
        case CHARACTER:  obj = ast_tok(CHARACTER);  break;
        case LBRACK:     obj = array();             break;
        case LBRACE:     obj = object();            break;
        case AT_LBRACE:  obj = hashmap();           break;
        case AT_LBRACK:  obj = hashset();           break;
        default:         error("Invalid literal");
    }
    return obj;
}

static AST* array(void)
{
    AST* array_obj = ast_new(ARRAY);
    expect(LBRACK);
    while (!accept(RBRACK)) {
        ast_add_child(array_obj, expression());
    }
    expect(RBRACK);
    return array_obj;
}

static AST* hashmap(void)
{
    AST* hashmap = ast_new(HASHMAP);
    expect(AT_LBRACE);
    while (!accept(RBRACE)) {
        AST* pair = ast_new(PAIR);
        expect(STRING);
        expect(COLON);
        ast_add_child(pair, expression());
        ast_add_child(hashmap, pair);
    }
    expect(RBRACE);
    return hashmap;
}

static AST* hashset(void)
{
    AST* hashset = ast_new(HASHSET);
    expect(AT_LBRACK);
    while (!accept(RBRACK)) {
        ast_add_child(hashset, expression());
    }
    expect(RBRACK);
    return hashset;
}

static AST* object(void)
{
    AST* object = ast_new(OBJECT);
    expect(LBRACE);
    if (accept(PIPE)) {
        expect(PIPE);
        expect(PIPE);
    }
    while (!accept(RBRACE)) {
        ast_add_child(object, expression());
    }
    expect(RBRACE);
    return object;
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
    // Re-init the token
    strbuf_init(&Token);

    // Skip any whitespace.
    whitespace();

    // Get the next real token
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
static int current(void)
{
    return CurrChar;
}

static void append(void)
{
    strbuf_putc(&Token, CurrChar);
    fetch();
}

static void discard(void)
{
    fetch();
}

static void expect_ch(int ch)
{
    if (ch == current())
        append();
    else
        lex_error();
}

static void lex_error(void)
{
    fprintf(stderr, "Lexer error\n");
    exit(1);
}

static void fetch(void)
{
    CurrChar = fgetc(File);
    if (CurrChar == '\n') {
        Line++;
        Column = 0;
    } else {
        Column++;
    }
}

/* Tree Routines
 *****************************************************************************/
static AST* ast_new(int type)
{
    AST* tree  = (AST*)malloc(sizeof(AST));
    memset(tree, 0, sizeof(AST));
    tree->type = type;
    return tree;
}

static AST* ast_tok(int type)
{
    AST* ast  = ast_new(type);
    expect(type);
    ast->text = strbuf_string(&Token);
    return ast;
}

static void ast_add_child(AST* parent, AST* child)
{
    slist_push_back(&(parent->children), &(child->link));
}

static void ast_print(AST* tree, int depth)
{
    int indent = depth * 2;
    printf("%*s(", indent, "");
    printf("%d %s", tree->type, tree->text ? tree->text : "");
    if (slist_size(&tree->children) == 0) {
        printf(")\n");
    } else {
        printf("\n");
        slist_foreach(elem, &(tree->children)) {
            ast_print(container_of(elem, AST, link), depth+1);
        }
        printf("%*s)\n", indent, "");
    }
}

/* String Buffer
 *****************************************************************************/
static void strbuf_init(strbuf_t* buf)
{
    buf->index = 0;
    buf->capacity = 8;
    buf->string = (char*)malloc(buf->capacity);
    buf->string[buf->index] = 0;
}

static void strbuf_putc(strbuf_t* buf, int ch)
{
    if (buf->string == NULL) {
        strbuf_init(buf);
    } else if ((buf->index + 2) >= buf->capacity) {
        buf->capacity = buf->capacity << 1;
        buf->string = (char*)realloc(buf->string, buf->capacity);
    }
    buf->string[buf->index++] = ch;
    buf->string[buf->index] = '\0';
}

static char* strbuf_string(strbuf_t* buf)
{
    char* str = buf->string;
    strbuf_init(buf);
    return str;
}

