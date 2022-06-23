#pragma once

#include <unistd.h>

enum ast_type
{
    AST_MOM,
    AST_CMD,
    AST_EOF,
    AST_NL,
    AST_IF,
    AST_COND,
    AST_THEN,
    AST_ELSE,
    AST_FI,
    AST_WHILE,
    AST_FOR,
    AST_UNTIL,
    AST_DO,
    AST_DONE,
    AST_PIPE,
    AST_REDIR,
    AST_VAR_ASSIGN,
    AST_NOPIPE,
    AST_NORED,
    AST_RED,
    AST_FILE,
    AST_ION,
    AST_REDL,
    AST_REDR,
    AST_VAR,
    AST_AND,
    AST_OR,
    AST_FUNC,
    AST_CONT,
    AST_BRK,
    AST_EXPORT,
    AST_CD,
    AST_EXIT,
    AST_UNSET,
    AST_DOT,
    AST_BLOCK,
    
};

enum cmd_type
{
    ECHO,
    FD,
    VAR,
    FUNC,
    OTHER,
};

struct command
{
    enum cmd_type type;
    char **cmd_arr;
    size_t index;
    char *cmd; //this is the whole coammnd for "echo hello world" it will store "echo hello world"
    char *args; //this is the arg of the coammnd for "echo hello world" it will store "hello world"
    int empty; //if = 1 means the cmd is empty (nothing init)
};

/**
 * This very simple AST structure should be sufficient for such a simple AST.
 * It is however, NOT GOOD ENOUGH for more complicated projects, such as a
 * shell. Please read the project guide for some insights about other kinds of
 * ASTs.
 */
struct ast
{
    enum ast_type type; ///< The kind of node we're dealing with
    struct command cmd; ///< If the node is a cmd, it stores a linked list of commands
    int fd;
    int cond;
    int negation;
    int cont; //continue counter
    int brk; //break counter
    size_t nb_children; // number of children
    struct ast **children; // the children of ast. contians the commands the if ...
};

/**
 ** \brief Allocate a new ast with the given type
 */
struct ast *ast_new(enum ast_type type);

/**
 ** \brief Recursively free the given ast
 */
void ast_free(struct ast *ast);
