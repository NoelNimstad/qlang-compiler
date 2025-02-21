#ifndef LEXER_H
#define LEXER_H

#include <stdlib.h>
#include "token.h"

enum nodeType
{
    NODE_TYPE_INT,
    NODE_VALUE_STRING,
};

struct node
{
    enum nodeType type;
    struct node *previous;
    struct node **children;
    unsigned int childCount;
    struct node *next;
    union
    {
        int integer;
        double decimal;
        char *string;
    } value;
};

struct node *newNode();

#endif