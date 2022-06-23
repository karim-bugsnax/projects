#pragma once

#include "lexer.h"
#include "../token/token.h"

char *cut_out_segment(const char *in, int start, int end);
struct token *build_next_token(struct lexer *lexer, size_t *index);