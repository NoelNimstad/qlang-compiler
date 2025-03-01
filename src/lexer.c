/*
    Name                    Representation  Priority
    ------------------------------------------------
    TYPE_INT                int             0
    IDENTIFIER(a)           a               0
    OPERATOR_WEAK_ASSIGN    ?=              3
    INT(21)                 21              0
    OPERATOR_PLUS           +               1
    INT(21)                 21              0
    SEMI_COLON              ;               0
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