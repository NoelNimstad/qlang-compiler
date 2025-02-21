#include "tokenize.h"

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
                currentToken->type = TOKEN_IDENTIFIER;
                currentToken->value.string = strdup(buffer);
            }
            currentToken++;
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
            currentToken->type = TOKEN_VALUE_INT;
            currentToken->value.integer = value;

            currentToken++;
            free(buffer);
            continue;
        }

        switch(*currentCharacter)
        {
            case ';':
                currentToken->type = TOKEN_SEMI_COLON;
                currentToken++;
                break;
            case '?':
                if(PEEK(currentCharacter) == '=')
                {
                    currentCharacter++;
                    currentToken->type = TOKEN_OPERATOR_WEQUAL;
                    currentToken++;
                }
                break;
            case '=':
                currentToken->type = TOKEN_OPERATOR_ASSIGN;
                currentToken++;
                break;
            case '.':
                if(PEEK(currentCharacter) == '.')
                {
                    currentCharacter++;
                    currentToken->type = TOKEN_OPERATOR_SPREAD;
                    currentToken++;
                }
                break;
            case '[':
                currentToken->type = TOKEN_LEFT_SQUARE_BRACKET;
                currentToken++;
                break;
            case ']':
                currentToken->type = TOKEN_RIGHT_SQUARE_BRACKET;
                currentToken++;
                break;
            default:
                currentToken->type = TOKEN_ERROR;
                currentToken++;
                break;
        }

        currentCharacter++;
    }

    currentToken->type = TOKEN_END_OF_FILE;
    currentToken++;

    return tokens;
}