#include "qlang.h"

void debugPrintTokens(struct qlangStruct *qlang)
{
	printf(COLOR_GREEN "Qlang debug " COLOR_RESET "(tokens):\n");

	int i = 0;
	for(struct token *currentToken = qlang->tokens; currentToken->type != TOKEN_END_OF_FILE; currentToken++)
	{
		i++;
		printf("\t");
		switch(currentToken->type)
		{
			case TOKEN_TYPE_INT:
				printf("TYPE_INT");
				break;
			case TOKEN_IDENTIFIER:
				printf("IDENTIFIER(%s)", currentToken->value.string);
				break;
			case TOKEN_OPERATOR_ASSIGN:
				printf("OPERATOR_ASSIGN");
				break;
			case TOKEN_OPERATOR_WEQUAL:
				printf("OPERATOR_WEAK_ASSIGN");
				break;
			case TOKEN_VALUE_INT:
				printf("INT(%d)", currentToken->value.integer);
				break;
			case TOKEN_SEMI_COLON:
				printf("SEMI_COLON");
				break;
			case TOKEN_ERROR:
				printf("(ERROR)");
				break;
			case TOKEN_OPERATOR_SPREAD:
				printf("OPERATOR_SPREAD");
				break;
			case TOKEN_LEFT_SQUARE_BRACKET:
				printf("LEFT_SQUARE_BRACKET");
				break;
			case TOKEN_RIGHT_SQUARE_BRACKET:
				printf("RIGHT_SQUARE_BRACKET");
				break;
			case TOKEN_OPERATOR_ADD:
				printf("OPERATOR_PLUS");
				break;
			default:
				break;
			}
		printf("\n");
    }

	printf(COLOR_GREEN "\tEND_OF_FILE (%d tokens)\n" COLOR_RESET, i + 1);
}

void freeNode(struct node *head)
{
    if(head == NULL) return;

    if(head->children != NULL)
    {
        for(unsigned int i = 0; i < head->childCount; i++)
        {
            freeNode(head->children[i]);
        }
        free(head->children);
    }

    if(head->type == NODE_VALUE_STRING && head->value.string != NULL)
    {
        free(head->value.string);
    }

    freeNode(head->next);
    free(head);
}

void destroyQlangStruct(struct qlangStruct *qlang)
{
    free(qlang->fileContents);

	for(struct token *currentToken = qlang->tokens; currentToken->type != TOKEN_END_OF_FILE; currentToken++)
	{
        if(currentToken->type == TOKEN_IDENTIFIER && currentToken->value.string != NULL)
		{
            free(currentToken->value.string); // Free used strdup
        }
    }
	free(qlang->tokens);
	
	free(qlang->head);
}

void processFile(const char *path)
{
    struct qlangStruct qlang;

    { // Read file
		FILE *filePointer = fopen(path, "r");
		ASSERT(filePointer != NULL, "Input file \"%s\" does not exist.\n", path);

		fseek(filePointer, 0L, SEEK_END);
		size_t fileSize = ftell(filePointer);
		rewind(filePointer);

		SOFT_ASSERT(fileSize != 0, "Input file \"%s\" is empty.\n", path);

		qlang.fileContents = (char *)malloc(fileSize);
		ASSERT(qlang.fileContents != NULL, "Failed to allocate memory (%zu bytes) for file contents.\n", fileSize);

		fread(qlang.fileContents, 1, fileSize, filePointer);

		fclose(filePointer);
    }

	qlang.tokens = generateTokens(qlang.fileContents);
	debugPrintTokens(&qlang);

    destroyQlangStruct(&qlang);
}
