#ifndef PIPE_REDIR_H
#define PIPE_REDIR_H

#include "../ast/ast.h"
#include "env.h"

int eval_pipe(struct ast **pipe_node, struct env **env);
int eval_redirection(struct ast **redir_node, struct env **env);

#endif /* !PIPE_REDIR_H */