/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   minishell.h                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: psadeghi <psadeghi@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/06/14 17:33:17 by psadeghi      #+#    #+#                 */
/*   Updated: 2023/07/06 16:56:03 by svan-has      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <editline/readline.h>
# include "libft.h"

typedef struct s_funcstruc
{
	char	*name;
	void	(*func)(void*);
}	t_func;

typedef struct exec_struc
{
	int		infile;
	int		outfile;
	int		fdin;
	int		fdout;
	int		num_commands;
	int		**pipe_fd;
	int		*fork_pid;
	t_func	*builtin_func[7];
	char	*test_cmd[3][4];
	char	**env;
}	t_exec;

void	execution(void);
void	close_pipes_files(t_exec *data);
void	waitpid_forks(t_exec *data);
void	create_pipes(t_exec *data);
void	*null_check(void *check);
// void	builtin_func(t_exec *data);
int		array_size(char **array);
int		error_exit(char *message);

/* Built-ins */
int		echo_builtin(char **cmd_table);
int		cd_builtin(char **cmd_table, char ***env);
int		pwd_builtin(void);
int		env_builtin(char **env);
int		unset_builtin(char *variable, char ***env);
int		export_builtin(char *string, char ***env);
int		exit_builtin(int status);

/* Built-in Tools*/
int		find_env_var(char *variable, char **env);
int		find_value(char *string);

#endif