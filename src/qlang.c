#include "qlang.h"

void debugPrintTokens(struct qlangStruct *qlang)
{
	printf(COLOR_GREEN "Qlang debug " COLOR_RESET "(tokens):\n");

	int i = 0;
	for(struct token *currentToken = qlang->tokens; currentToken->type != TOKEN_END_OF_FILE; currentToken++)
	{
		i++;
		printf("%lu\t", currentToken->line);
		switch(currentToken->type)
		{
			case TOKEN_TYPE_INT:
				printf("TYPE_INT");
				break;
			case TOKEN_TYPE_UINT8:
				printf("TYPE_UINT8");
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
				printf("INT(%s)", currentToken->value.string);
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
			case TOKEN_OPERATOR_DIVIDE:
				printf("OPERATOR_DIVIDE");
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
			case TOKEN_OPERATOR_PLUS_EQUALS:
				printf("OPERATOR_PLUS_EQUALS");
				break;
			default:
				break;
			}
		printf("\n");
    }

	printf(COLOR_GREEN "\tEND_OF_FILE (%d tokens)\n" COLOR_RESET, i + 1);
}

void debugPrintNode(struct node *node, int depth)
{
	if(node == NULL) return;

	for(int i = 0; i < depth; i++)
	{
		printf("|\t");
	}

	printf("| ");

	switch(node->type)
	{
		case NODE_START_OF_PROGRAM:
			printf("START_OF_PROGRAM\n");
			break;
		case NODE_TYPED_IDENTIFIER:
			printf("TYPED_IDENTIFIER\n");
			break;
		case NODE_TYPE:
			printf("TYPE(%d)\n", node->value.integer);
			break;
		case NODE_IDENTIFIER:
			printf("IDENTIFIER(%s)\n", node->value.string);
			break;
		case NODE_VALUE_INTEGER:
			printf("INT(%s)\n", node->value.string);
			break;
		case NODE_DECLARATION:
			printf("DECLARATION\n");
			break;
		case NODE_OPERATOR:
			printf("OPERATOR(%s)\n", node->value.string);
			break;
		case NODE_ASSIGNMENT:
			printf("ASSIGNMENT\n");
			break;
		default:
			printf("ERROR(%d)\n", node->type);
			break;
	}

	debugPrintNode(node->left, depth + 1);
	debugPrintNode(node->right, depth + 1);
	debugPrintNode(node->next, depth);
}

void debugPrintNodes(struct qlangStruct *qlang)
{
	printf(COLOR_GREEN "Qlang debug " COLOR_RESET "(nodes):\n");
	debugPrintNode(qlang->head, 0);
	printf("| " COLOR_GREEN "END_OF_FILE\n");
}

void freeNode(struct node *head)
{
    if(head == NULL) return;

    if(head->type == NODE_VALUE_STRING && head->value.string != NULL)
    {
        free(head->value.string);
    }

	free(head->left);
	free(head->right);
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

void processFile(const char *path, unsigned char debugTokens, unsigned char debugNodes)
{
    struct qlangStruct qlang;
	qlang.debugTokens = debugTokens;
	qlang.debugNodes = debugNodes;

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
	if(qlang.debugTokens) debugPrintTokens(&qlang);

	qlang.head = generateAST(qlang.tokens);
	if(qlang.debugNodes) debugPrintNodes(&qlang);

    destroyQlangStruct(&qlang);
}
