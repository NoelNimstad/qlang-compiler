#ifndef QLANG_H
#define QLANG_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "tokenize.h"
#include "parser.h"

// Qlang struct
struct qlangStruct
{
    unsigned char debugTokens;
    unsigned char debugNodes;
    char *fileContents;
    unsigned int line, column;
    struct token *tokens;
    struct node *head;
};
/*
    The qlang struct will contain everything
    which is needed for the qlang compiler to
    run through some file, including settings,
    line & column values as well as tokens
    and the AST.
*/

// ANSI terminal colors
#define COLOR_RED    "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_RESET  "\x1B[0m"

// Peeking
#define PEEK(_p) (*((_p)+1))

// Catch & warn for errors
#define ASSERT(_e, ...) if(!(_e)){ fprintf(stderr, COLOR_RED "Qlang error" COLOR_RESET ": "); fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define SOFT_ASSERT(_w, ...) if(!(_w)){ fprintf(stderr, COLOR_YELLOW "Qlang warning" COLOR_RESET ": "); fprintf(stderr, __VA_ARGS__); }

// Debug
void debugPrintTokens(struct qlangStruct *qlang);
void debugPrintNodes(struct qlangStruct *qlang);

// Cleanup
void freeNode(struct node *head);
void destroyQlangStruct(struct qlangStruct *qlang);

// Main process (to process some file)
void processFile(const char *path, unsigned char debugTokens, unsigned char debugNodes);

#endif // QLANG_H