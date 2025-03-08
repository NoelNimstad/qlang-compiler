#include "tokenize.h"

#define ADVANCE(_to, _ty) { (_to)->type = (_ty); (_to)++; };

struct keywordEntry
{
    char *string;
    enum tokenType value;
};

struct keywordEntry keywordTable[] = 
{
    "int", TOKEN_TYPE_INT
};

int compare(const void *s1, const void *s2)
{
    const struct keywordEntry *e1 = s1;
    const struct keywordEntry *e2 = s2;

    return strcmp(e1->string, e2->string);
}

struct token *generateTokens(char *source)
{
    SOFT_ASSERT(strlen(source) != 0, "Input string to tokenizer is empty.\n");

    struct token *tokens = (struct token *)malloc(0xff * sizeof(struct token)); // Default to 255 tokens
    struct token *currentToken = tokens;
    char *currentCharacter = source;

    while(*currentCharacter != '\0')
    {
        if(isspace(*currentCharacter)) // Ignore spaces
        {
            currentCharacter++;
            continue;
        }

        if(isalpha(*currentCharacter) || *currentCharacter == '_') // Identifier or keyword
        {
            unsigned char bufferLength = 32; 
            char *buffer = (char *)malloc(bufferLength);
            unsigned int i = 0;

            while(isalnum(*currentCharacter) || *currentCharacter == '_')
            {
                if(i >= bufferLength)
                {
                    bufferLength *= 2;
                    buffer = (char *)realloc(buffer, bufferLength);
                }

                buffer[i++] = *currentCharacter;
                currentCharacter++;
            }
            buffer[i] = '\0'; // Null terminate the string to avoid problems

            struct keywordEntry *result, key = { buffer };
            result = bsearch(&key, keywordTable, sizeof(keywordTable) / sizeof(keywordTable[0]), sizeof(keywordTable[0]), compare);
            if(result) // Is keyword
            {
                currentToken->type = result->value;
            } else 
            {
                currentToken->value.string = strdup(buffer);
                ADVANCE(currentToken, TOKEN_IDENTIFIER);
            }
            free(buffer);
            continue;
        }

        if(isnumber(*currentCharacter)) // Numbers
        {
            unsigned char bufferLength = 32; 
            char *buffer = (char *)malloc(bufferLength);
            unsigned int i = 0;

            while(isnumber(*currentCharacter))
            {
                if(i >= bufferLength)
                {
                    bufferLength *= 2;
                    buffer = (char *)realloc(buffer, bufferLength);
                }

                buffer[i++] = *currentCharacter;
                currentCharacter++;
            }
            buffer[i] = '\0'; // Null terminate the string to avoid problems

            int value = atoi(buffer); // convert string buffer to integer
            currentToken->value.integer = value;
            
            ADVANCE(currentToken, TOKEN_VALUE_INT);
            free(buffer);
            continue;
        }

        switch(*currentCharacter)
        {
            case ';':
                ADVANCE(currentToken, TOKEN_SEMI_COLON);
                break;
            case '?':
                if(PEEK(currentCharacter) == '=')
                {
                    currentCharacter++;
                    ADVANCE(currentToken, TOKEN_OPERATOR_WEQUAL);
                }
                break;
            case '=':
                ADVANCE(currentToken, TOKEN_OPERATOR_ASSIGN);
                break;
            case '.':
                if(PEEK(currentCharacter) == '.')
                {
                    currentCharacter++;
                    ADVANCE(currentToken, TOKEN_OPERATOR_SPREAD);
                }
                break;
            case '[':
                ADVANCE(currentToken, TOKEN_LEFT_SQUARE_BRACKET);
                break;
            case ']':
                ADVANCE(currentToken, TOKEN_RIGHT_SQUARE_BRACKET);
                break;
            case '+':
                ADVANCE(currentToken, TOKEN_OPERATOR_ADD);
                break;
            default:
                ADVANCE(currentToken, TOKEN_ERROR);
                break;
        }

        currentCharacter++;
    }

    ADVANCE(currentToken, TOKEN_END_OF_FILE);

    return tokens;
}