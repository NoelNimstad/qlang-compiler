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

struct node *parseStatement(struct token **currentToken);
struct node *parseDeclaration(struct token **currentToken);
struct node *parseAssignment(struct token **currentToken);
struct node *parseExpression(struct token **currentToken);
struct node *parseTerm(struct token **currentToken);
struct node *parseFactor(struct token **currentToken);

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
    else if((*currentToken)->type != TOKEN_SEMI_COLON) // if we reach the bottom and still don't know what the token is, then throw an error
    {
        ASSERT(0, "Unexpected token in expression on line %lu.\n", (*currentToken)->line);
    }

    (*currentToken)++; // move to the next token
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

        (*currentToken)++;
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

        (*currentToken)++; // move past operator
        operatorNode->right = parseTerm(currentToken);

        expression = operatorNode; // update expression to be the new root
    }

    return expression;
}

struct node *parseAssignment(struct token **currentToken)
{
    struct node *assignment = parseExpression(currentToken);

    while((*currentToken)->type == TOKEN_OPERATOR_ASSIGN)
    {
        struct node *assignmentNode = newNode(NODE_ASSIGNMENT);
        assignmentNode->left = assignment;

        (*currentToken)++; // move past =
        assignmentNode->right = parseExpression(currentToken);
        
        assignment = assignmentNode;
    }

    return assignment;
}

struct node *parseDeclaration(struct token **currentToken)
{
    ASSERT(tokenIsType(**currentToken), "Expected type in declaration on line %lu.\n", (*currentToken)->line);

    struct node *typedIdentifier = newNode(NODE_TYPED_IDENTIFIER);
    
    typedIdentifier->left = newNode(NODE_TYPE);
    typedIdentifier->left->type = NODE_TYPE;
    typedIdentifier->left->value.integer = convertTokenTypeToNodeType((*currentToken)->type);
    
    (*currentToken)++;
    ASSERT((*currentToken)->type == TOKEN_IDENTIFIER, "Expected identifier after type on line %lu.\n", (*currentToken)->line);

    typedIdentifier->right = newNode(NODE_IDENTIFIER);
    typedIdentifier->right->type = NODE_IDENTIFIER;
    typedIdentifier->right->value.string = (*currentToken)->value.string;

    (*currentToken)++;

    struct node *declaration = newNode(NODE_DECLARATION);
    declaration->left = typedIdentifier;

    if((*currentToken)->type != TOKEN_SEMI_COLON) // if we aren't just stating that some variable exists
    {
        ASSERT((*currentToken)->type == TOKEN_OPERATOR_WEQUAL
            || (*currentToken)->type == TOKEN_OPERATOR_ASSIGN, "Expected = or ?= in declaration on line %lu.\n", (*currentToken)->line);

        (*currentToken)++;
        declaration->right = parseExpression(currentToken);
    }

    return declaration;
}

struct node *parseStatement(struct token **currentToken)
{
    if(tokenIsType(**currentToken))
    {
        return parseDeclaration(currentToken); // function and variable declarations
    } else
    {
        return parseAssignment(currentToken); // assignments (the highest precedence operator)
    }
}

struct node *generateAST(struct token *tokens)
{
    struct token *currentToken = tokens;
    struct node *head = newNode(NODE_START_OF_PROGRAM);
    struct node *tail = head;
    
    while(currentToken->type != TOKEN_END_OF_FILE)
    {
        tail->next = parseStatement(&currentToken);
        tail = tail->next;

        SOFT_ASSERT(currentToken->type == TOKEN_SEMI_COLON, "Expected semicolon on line %lu.\n", (currentToken - 1)->line) else
        {
            currentToken++;
        }
    }

    return head;
}