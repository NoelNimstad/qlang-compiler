#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "qlang.h"

enum type
{
    TYPE_INT,
    TYPE_UINT8,
    TYPE_ERROR
};

enum nodeType
{
    NODE_START_OF_PROGRAM,
    NODE_TYPE, // type
    NODE_VALUE_STRING, // string value
    NODE_VALUE_INTEGER, // integer literal
    NODE_DECLARATION,
    NODE_OPERATOR,
    NODE_IDENTIFIER,
    NODE_ASSIGNMENT,
    NODE_TYPED_IDENTIFIER, // type identifier
    NODE_ERROR
};

struct node
{
    enum nodeType type;
    struct node *previous;
    struct node *left;
    struct node *right;
    struct node *next;
    union
    {
        int integer;
        double decimal;
        char *string;
    } value;
};

struct node *newNode(enum nodeType type);
struct node *generateAST(struct token *tokens);

#endif