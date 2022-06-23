#pragma once

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../func_list/func.h"

enum parser_status
{
    PARSER_OK,
    PARSER_UNEXPECTED_TOKEN,
    PARSER_CONTINUE,
};

/**
 * \brief Creates an AST by reading and interpreting tokens
 *   from the parser.
 */
enum parser_status parse_input(struct ast **ast, struct lexer *lexer,struct func **main_list);

