#include "parser.h"

#define ADVANCE(_n, _t) { (_n)->next = newNode((_t)); (_n)->next->previous = (_n); (_n) = (_n)->next; };

struct node *newNode(enum nodeType type)
{
    struct node *node = (struct node *)malloc(sizeof(struct node));
    ASSERT(node != NULL, "Failed to allocate memory for node.\n");

    node->previous = NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->value.string = NULL;
    node->type = type;

    return node;
}

enum type convertTokenTypeToNodeType(enum tokenType tokenType)
{
    switch(tokenType)
    {
        case TOKEN_TYPE_INT:
            return TYPE_INT;
        case TOKEN_TYPE_UINT8:
            return TYPE_UINT8;
        default:
            return TYPE_ERROR;
    }
}

unsigned char tokenIsType(struct token token)
{
    enum tokenType type = token.type;

    return type == TOKEN_TYPE_INT
        || type == TOKEN_TYPE_UINT8;
}

void movePreviousNodeToLeft(struct node *node)
{
    node->previous->previous->next = node; // set previous node's previous node's next node to this node
    node->left = node->previous;
    node->previous = node->left->previous; // make this node's previous node be the moved down node's previous node

    node->left->previous = NULL; // node which got moved down is no longer part of the linked list
    node->left->next = NULL;
}

struct node *parseFactor(struct token **currentToken)
{
    struct node *factor = NULL;

    if((*currentToken)->type == TOKEN_VALUE_INT)
    {
        factor = newNode(NODE_VALUE_INTEGER);
        factor->value.string = (*currentToken)->value.string;
    }
    else if((*currentToken)->type == TOKEN_IDENTIFIER)
    {
        factor = newNode(NODE_IDENTIFIER);
        factor->value.string = (*currentToken)->value.string;
    }
    else
    {
        ASSERT(0, "Unexpected token in expression on line %lu.\n", (*currentToken)->line);
    }

    *currentToken = (*currentToken) + 1; // move to the next token
    return factor;
}

struct node *parseTerm(struct token **currentToken)
{
    struct node *term = parseFactor(currentToken);

    while((*currentToken)->type == TOKEN_OPERATOR_MULTIPLY ||
          (*currentToken)->type == TOKEN_OPERATOR_DIVIDE)
    {
        struct node *operatorNode = newNode(NODE_OPERATOR);
        operatorNode->value.string = (*currentToken)->value.string;
        operatorNode->left = term;

        *currentToken = (*currentToken) + 1;
        operatorNode->right = parseFactor(currentToken);

        term = operatorNode;
    }

    return term;
}

struct node *parseExpression(struct token **currentToken)
{
    struct node *expression = parseTerm(currentToken);

    while((*currentToken)->type == TOKEN_OPERATOR_ADD ||
          (*currentToken)->type == TOKEN_OPERATOR_SUBTRACT)
    {
        struct node *operatorNode = newNode(NODE_OPERATOR);
        operatorNode->value.string = (*currentToken)->value.string; // store the operator as a string
        operatorNode->left = expression;

        *currentToken = (*currentToken) + 1; // move past operator
        operatorNode->right = parseTerm(currentToken);

        expression = operatorNode; // update expression to be the new root
    }

    return expression;
}

struct node *generateAST(struct token *tokens)
{
    struct token *currentToken = tokens;
    struct node *head = newNode(NODE_START_OF_PROGRAM);
    struct node *tail = head;
    
    while(currentToken->type != TOKEN_END_OF_FILE)
    {
        if(tokenIsType(*currentToken))
        {
            if(PEEK(currentToken).type == TOKEN_IDENTIFIER) // typed identifier
            {
                /*
                       TYPED_IDENTIFIER
                         /          \
                        /            \
                      TYPE        IDENTIFIER
                */

                ADVANCE(tail, NODE_TYPED_IDENTIFIER); // typed identifier node
               
                tail->left = newNode(NODE_TYPE); // type node
                tail->left->type = NODE_TYPE;
                tail->left->value.integer = convertTokenTypeToNodeType(currentToken->type);

                currentToken++; // identifier node
                tail->right = newNode(NODE_IDENTIFIER);
                tail->right->type = NODE_IDENTIFIER;
                tail->right->value.string = currentToken->value.string;

                currentToken++;
                continue;
            } else 
            {
                ASSERT(0, "Expected identifier after type on line %lu.\n", currentToken->line);
                currentToken++;
            }
        }

        switch(currentToken->type)
        {
            case TOKEN_VALUE_INT:
            {
                ADVANCE(tail, NODE_VALUE_INTEGER);
                tail->value.string = currentToken->value.string;
                break;
            }
            case TOKEN_OPERATOR_WEQUAL:
            {
                if(tail->type == NODE_TYPED_IDENTIFIER) // new declaration
                {
                    /*        
                                @mut DECLARATION
                                /              \
                               /                \
                         TYPED_IDENTIFIER    EXPRESSION
                    */
                    
                    ADVANCE(tail, NODE_DECLARATION);
                    movePreviousNodeToLeft(tail);

                    currentToken++; // move past ?=

                    tail->right = parseExpression(&currentToken);
                } else 
                {
                    SOFT_ASSERT(0, "?= used for a normal assignment on line %lu.\n", currentToken->line);

                    ADVANCE(tail, NODE_ASSIGNMENT);
                    movePreviousNodeToLeft(tail);
    
                    currentToken++; // move past ?=
    
                    tail->right = parseExpression(&currentToken);
                }
                break;
            }
            case TOKEN_IDENTIFIER:
            {
                ADVANCE(tail, NODE_IDENTIFIER);
                tail->value.string = currentToken->value.string;
                break;
            }
            case TOKEN_OPERATOR_ASSIGN:
            {
                ADVANCE(tail, NODE_ASSIGNMENT);
                movePreviousNodeToLeft(tail);

                currentToken++; // move past =

                tail->right = parseExpression(&currentToken);
                break;
            }
            default: 
            {
                break;
            }
        }

        currentToken++;
    }

    return head;
}