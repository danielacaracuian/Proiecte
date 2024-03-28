// SPDX-License-Identifier: BSD-3-Clause

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmd.h"
#include "utils.h"

#define READ		0
#define WRITE		1

/**
 * Internal change-directory command.
 */
static bool shell_cd(word_t *dir)
{
	/* Execute cd. */
	if (dir == NULL)
		return false;

	return chdir(dir->string) != 0;
}

/**
 * Internal exit/quit command.
 */
static int shell_exit(void)
{
	/* Execute exit/quit. */

	return SHELL_EXIT;
}

static void redirect(simple_command_t *s)
{
	// Redirect input
	char *in_exp = get_word(s->in);
	char *out_exp = get_word(s->out);
	char *err_exp = get_word(s->err);

	if (s->in != NULL) {
		int fd = open(in_exp, O_RDONLY);

		DIE(fd < 0, "open");

		DIE(dup2(fd, STDIN_FILENO) < 0, "dup2");
		DIE(close(fd) < 0, "close");
	}

	// Redirect output
	if (s->out != NULL) {
		int flags;

		if (s->io_flags == IO_OUT_APPEND)
			flags = O_WRONLY | O_CREAT | O_APPEND;
		else
			flags = O_WRONLY | O_CREAT | O_TRUNC;

		int fd = open(out_exp, flags, 0644);

		DIE(fd < 0, "open");

		DIE(dup2(fd, STDOUT_FILENO) < 0, "dup2");
		if (s->err != NULL && strcmp(out_exp, err_exp) == 0)
			DIE(dup2(fd, STDERR_FILENO) < 0, "dup2");

		DIE(close(fd) < 0, "close");
	}

	// Redirect error
	if (s->err != NULL && ((s->out != NULL && strcmp(out_exp, err_exp) != 0) || s->out == NULL)) {
		int flags;

		if (s->io_flags == IO_ERR_APPEND)
			flags = O_WRONLY | O_CREAT | O_APPEND;
		else
			flags = O_WRONLY | O_CREAT | O_TRUNC;

		int fd = open(err_exp, flags, 0644);

		DIE(fd < 0, "open");

		DIE(dup2(fd, STDERR_FILENO) < 0, "dup2");
		DIE(close(fd) < 0, "close");
	}

	free(in_exp);
	free(out_exp);
	free(err_exp);
}

/**
 * Parse a simple command (internal, environment variable assignment,
 * external command).
 */
static int parse_simple(simple_command_t *s, int level, command_t *father)
{
	/* Sanity checks. */
	if (s == NULL)
		return 0;

	if (s->verb == NULL)
		return 0;

	int error_code;

	/* If builtin command, execute the command. */
	if (strcmp(s->verb->string, "cd") == 0) {
		int op_d, err_d;

		if (s->out != NULL) {
			char *out_exp = get_word(s->out);

			op_d = open(out_exp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			DIE(op_d < 0, "open");
			free(out_exp);
			close(op_d);
		}

		if (s->err != NULL) {
			char *err_exp = get_word(s->err);

			err_d = open(err_exp, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			DIE(err_d < 0, "open");
			free(err_exp);
		}

		error_code = shell_cd(s->params);

		if (error_code == 1) {
			if (s->err != NULL)
				dprintf(err_d, "no such file or directory\n");
			else
				fprintf(stderr, "no such file or directory\n");
		}

		if (s->err != NULL)
			DIE(close(err_d) < 0, "close");

		return error_code;
	}

	if (strcmp(s->verb->string, "exit") == 0 ||
		strcmp(s->verb->string, "quit") == 0) {
		return shell_exit();
	}

	/* If variable assignment, execute the assignment and return
	 * the exit status.
	 */
	if (s->verb->next_part != NULL && s->verb->next_part->next_part != NULL &&
		strcmp(s->verb->next_part->string, "=") == 0) {
		char *exp = get_word(s->verb->next_part->next_part);

		error_code = setenv(s->verb->string, exp, 1);

		free(exp);
		return error_code;
	}

	/*   If external command:
	 *   1. Fork new process
	 *     2c. Perform redirections in child
	 *     3c. Load executable in child
	 *   2. Wait for child
	 *   3. Return exit status
	 */
	pid_t pid = fork();
	pid_t ret_pid;

	DIE(pid == -1, "fork");

	if (pid == 0) {
		int param_count = 0;
		char **params = get_argv(s, &param_count);

		redirect(s);

		error_code = execvp(s->verb->string, params);
		free(params);

		if (error_code == -1) {
			printf("Execution failed for '%s'\n", s->verb->string);
			exit(error_code);
		}
	} else {
		ret_pid = waitpid(pid, &error_code, 0);
		DIE(ret_pid < 0, "waitpid failed");
	}

	return WEXITSTATUS(error_code);
}

/**
 * Process two commands in parallel, by creating two children.
 */
static bool run_in_parallel(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* Execute cmd1 and cmd2 simultaneously. */
	pid_t pid1, pid2;
	int exit_status, exit_status1;

	pid1 = fork();

	DIE(pid1 < 0, "fork");

	switch (pid1) {
	case 0:
		exit_status = parse_command(cmd1, level, father);
		exit(exit_status);

	default:
		pid2 = fork();

		DIE(pid2 < 0, "fork");

		if (pid2 == 0) {
			exit_status = parse_command(cmd2, level, father);
			exit(exit_status);
		}

		waitpid(pid1, &exit_status, 0);
		waitpid(pid2, &exit_status1, 0);

		exit_status += exit_status1;

		break;
	}

	return WEXITSTATUS(exit_status);
}

/**
 * Run commands by creating an anonymous pipe (cmd1 | cmd2).
 */
static bool run_on_pipe(command_t *cmd1, command_t *cmd2, int level,
		command_t *father)
{
	/* Redirect the output of cmd1 to the input of cmd2. */
	int pipefd[2];
	pid_t pid1, pid2;
	int exit_status = 0;

	DIE(pipe(pipefd) < 0, "pipe");

	pid1 = fork();
	DIE(pid1 < 0, "fork");

	switch (pid1) {
	case 0:
		close(pipefd[READ]);
		DIE(dup2(pipefd[WRITE], STDOUT_FILENO) < 0, "dup2");
		close(pipefd[WRITE]);

		exit_status = parse_command(cmd1, level + 1, father);
		exit(exit_status);

	default:
		pid2 = fork();
		DIE(pid2 < 0, "fork");
		switch (pid2) {
		case 0:
			close(pipefd[WRITE]);
			DIE(dup2(pipefd[READ], STDIN_FILENO) < 0, "dup2");
			close(pipefd[READ]);

			exit_status = parse_command(cmd2, level + 1, father);
			exit(exit_status);

		default:
			close(pipefd[READ]);
			close(pipefd[WRITE]);

			waitpid(pid1, NULL, 0);
			waitpid(pid2, &exit_status, 0);
			break;
		}

		close(pipefd[READ]);
		close(pipefd[WRITE]);

		break;
	}

	return exit_status; /* Replace with actual exit status. */
}

/**
 * Parse and execute a command.
 */
int parse_command(command_t *c, int level, command_t *father)
{
	/* sanity checks */
	if (c == NULL)
		return 0;

	int exit_code = 0;

	if (c->op == OP_NONE) {
		/* Execute a simple command. */
		exit_code = parse_simple(c->scmd, level, father);

		return exit_code;
	}

	switch (c->op) {
	case OP_SEQUENTIAL:
		/* Execute the commands one after the other. */
		exit_code = parse_command(c->cmd1, level + 1, c);
		exit_code = parse_command(c->cmd2, level + 1, c);

		break;

	case OP_PARALLEL:
		/* Execute the commands simultaneously. */
		exit_code = run_in_parallel(c->cmd1, c->cmd2, level, father);

		break;

	case OP_CONDITIONAL_NZERO:
		/* Execute the second command only if the first one
		 * returns non zero.
		 */
		exit_code = parse_command(c->cmd1, level + 1, c);
		if (exit_code != 0)
			exit_code = parse_command(c->cmd2, level + 1, c);

		break;

	case OP_CONDITIONAL_ZERO:
		/* Execute the second command only if the first one
		 * returns zero.
		 */
		exit_code = parse_command(c->cmd1, level + 1, c);

		if (exit_code == 0)
			exit_code = parse_command(c->cmd2, level + 1, c);

		break;

	case OP_PIPE:
		/* Redirect the output of the first command to the
		 * input of the second.
		 */
		exit_code = run_on_pipe(c->cmd1, c->cmd2, level, c);

		break;

	default:
		return SHELL_EXIT;
	}

	return exit_code;
}
