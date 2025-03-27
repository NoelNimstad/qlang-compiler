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
                } else 
                {
                    SOFT_ASSERT(0, "?= used for a normal assignment on line %lu.\n", currentToken->line);
                }
                break;
            }
            case TOKEN_IDENTIFIER:
            {
                ADVANCE(tail, NODE_IDENTIFIER);
                tail->value.string = currentToken->value.string;
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