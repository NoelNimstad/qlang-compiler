/*
    Name                    Representation  Priority
    ------------------------------------------------
    TYPE_INT                int
    IDENTIFIER(a)           a
    OPERATOR_WEAK_ASSIGN    ?=
    INT(21)                 21
    OPERATOR_PLUS           +
    INT(21)                 21
    SEMI_COLON              ;
*/

#include "lexer.h"

struct node *newNode()
{
    struct node *node = (struct node *)malloc(sizeof(struct node));
    node->previous = NULL;
    node->children = NULL;
    node->childCount = 0;
    node->next = NULL;
    node->value.string = NULL;

    return node;
}