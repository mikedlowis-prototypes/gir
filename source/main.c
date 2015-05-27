#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser.h"

extern int yylex (void);
extern size_t Row;
extern size_t Column;

/*****************************************************************************/
typedef struct AST {
    int type;
    int num_children;
    struct AST* children[];
} AST;

AST* Tree(int type, int num_children) {
    AST* tree   = (AST*)calloc(1, sizeof(AST) * (num_children * sizeof(AST*)));
    tree->type  = type;
    tree->num_children = num_children;
    return tree;
}

void PrintTree(AST* tree, int depth) {
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

/*****************************************************************************/

int Current = UNKNOWN;

static void error(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static bool accept(int expected) {
    if (Current == UNKNOWN) {
        Current = yylex();
    }
    return (Current == expected);
}

static bool expect(int expected) {
    bool accepted = accept(expected);
    if (!accepted)
        error("unexpected symbol");
    else
        Current = UNKNOWN;
    return accepted;
}

static bool optional(int expected) {
    if (accept(expected))
        return expect(expected);
    else
        return false;
}

/*****************************************************************************/

intptr_t Token_Buffer[1024];
intptr_t* Token_Stack = Token_Buffer-1;

void shift(int type) {
    int curr = Current;
    if (expect(type)) {
        *(++Token_Stack) = curr;
    }
}

void reduce(int count) {
    int type  = *(Token_Stack--);
    AST* tree = Tree(type, count);
    intptr_t* stack = Token_Stack - (count-1);
    for (int i = 0; i < count; i++) {
        tree->children[i] = stack[i];
    }
    Token_Stack -= count ;
    *(++Token_Stack) = (intptr_t)tree;
}

void shift_reduce(int type, int nchildren) {
    shift(type);
    reduce(nchildren);
}

void push_reduce(int type, int nchildren) {
    *(++Token_Stack) = type;
    reduce(nchildren);
}

/*****************************************************************************/

static void expression(void);
static void keyword_send(void);
static void binary_send(void);
static void unary_send(void);
static void operand(void);
static void messages(void);
static void literal(void);
static void array(void);
static void hashmap(void);

static void expression(void) {
    if (accept(ID)) {
        shift_reduce(ID, 0);
        if (accept(ASSIGN)) {
            expect(ASSIGN);
            expression();
            push_reduce(ASSIGN, 2);
        }
    } else {
        keyword_send();
    }
    optional(END);
}

static void keyword_send(void) {
    int count  = 0;
    binary_send();
    while (accept(KEYW)) {
        shift_reduce(KEYW,0);
        binary_send();
        push_reduce(KEYWORD_PAIR, 2);
        count++;
    }
    if (count > 0) {
        push_reduce(KEYWORD_MSG, count);
    }
}

static void binary_send(void) {
    unary_send();
    if (accept(BINOP)) {
        shift_reduce(BINOP,0);
        unary_send();
        push_reduce(BINARY_MSG, 3);
    }
}

static void unary_send(void) {
    operand();
    while (accept(ID)) {
        shift_reduce(ID, 0);
        push_reduce(UNARY_MSG, 2);
    }
}

static void operand(void) {
    if (accept(LPAR)) {
        expect(LPAR);
        expression();
        expect(RPAR);
    } else {
        literal();
    }
}

static void literal(void) {
    switch (Current) {
        case ID:     shift_reduce(ID, 0u);     break;
        case NUM:    shift_reduce(NUM, 0u);    break;
        case SELF:   shift_reduce(SELF, 0u);   break;
        case STRING: shift_reduce(STRING, 0u); break;
        case BOOL:   shift_reduce(BOOL, 0u);   break;
        case CHAR:   shift_reduce(CHAR, 0u);   break;
        case LBRACK: array();                  break;
        case LBRACE: hashmap();                break;
        default:     error("Invalid literal");
    }
}

static void array(void) {
    int count = 0;
    expect(LBRACK);
    if (!accept(RBRACK)) {
        do {
            optional(COMMA);
            expression();
            count++;
        } while(accept(COMMA));
    }
    expect(RBRACK);
    push_reduce(ARRAY, count);
}

static void hashmap(void) {
    int count = 0;
    expect(LBRACE);
    if (!accept(RBRACE)) {
        do {
            optional(COMMA);
            shift_reduce(STRING, 0);
            expect(COLON);
            expression();
            push_reduce(PAIR, 2);
            count++;
        } while(accept(COMMA));
    }
    expect(RBRACE);
    push_reduce(MAP, count);
}

/*****************************************************************************/

int main(int argc, char** argv) {
    extern void world_init(void);
    world_init();
    printf(":> ");
    while(true) {
        expression();
        printf("stack: %du\n", (int)((Token_Stack - Token_Buffer) + 1));
        /* Print and clear */
        PrintTree((AST*)*Token_Stack, 0);
        Token_Stack = Token_Buffer-1;
    }
    return 0;
}

