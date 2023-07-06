/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   execution.c                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: svan-has <svan-has@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2023/06/15 14:35:16 by svan-has      #+#    #+#                 */
/*   Updated: 2023/07/06 18:26:26 by svan-has      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <fcntl.h>

void	redirection(t_exec *data);
void	*prepare(void);
void	execute(t_exec *data, int fdin, int fdout, int i);
char	**copy_environment_list(char **env);
void	*testing(t_exec *data);
char	*path_cmd(char *command, char **env);
void	check_builtins(char **cmd_table, t_exec *data);

void	execution(void)
{
	t_exec	*data;
	int		i;

	data = prepare();
	data = testing(data);
	

	redirection(data);
	if (data->num_commands == 1)
	{
		check_builtins(data->test_cmd[0], data);
		exit (0);
	}
	create_pipes(data);
	i = 0;
	while (i < data->num_commands)
	{
		data->fork_pid[i] = fork();
		if (data->fork_pid[i] == -1)
			exit (1);
		if (i == 0 && data->fork_pid[i] == 0)
			execute(data, data->fdin, data->pipe_fd[i][1], i);
		else if (i == data->num_commands - 1 && data->fork_pid[i] == 0)
			execute(data, data->pipe_fd[i - 1][0], data->fdout, i);
		else if (data->fork_pid[i] == 0)
			execute(data, data->pipe_fd[i - 1][0], data->pipe_fd[i][1], i);
		i++;
	}
	close_pipes_files(data);
	waitpid_forks(data);
	exit(0);
}

void	*prepare(void)
{
	int			i;
	t_exec		*data;
	extern char	**environ;

	data = malloc (1 * sizeof(t_exec));
	if (!data)
		exit (1);
	data->num_commands = 3;
	data->infile = 0;
	data->outfile = 1;
	data->fork_pid = malloc(data->num_commands * sizeof(int));
	if (!data->fork_pid)
		exit (1);
	i = 0;
	data->pipe_fd = (int **) malloc ((data->num_commands - 1) * sizeof(int *));
	while (i < data->num_commands - 1)
	{
		data->pipe_fd[i] = (int *) malloc (2 * sizeof(int));
		if (!data->pipe_fd[i])
			exit (1);
		i++;
	}
	data->env = copy_environment_list(environ);
	return (data);
}

char	**copy_environment_list(char **env)
{
	int			i;
	char		**new_environ;

	i = 0;
	while (env[i])
		i++;
	new_environ = null_check(malloc ((i + 1) * sizeof (char *)));
	i = 0;
	while (env[i])
	{
		new_environ[i] = null_check(ft_strdup(env[i]));
		i++;
	}
	new_environ[i] = NULL;
	return (new_environ);
}

void	redirection(t_exec *data)
{
	if (data->infile)
	{
		data->fdin = open("test_in", O_RDONLY);
		if (!data->fdin)
			exit(1);
	}
	else
		data->fdin = STDIN_FILENO;
	if (data->outfile)
	{
		data->fdout = open("test", O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (!data->fdout)
			exit(1);
	}
	else
		data->fdout = STDOUT_FILENO;
}

void	execute(t_exec *data, int fdin, int fdout, int i)
{	
	if (dup2(fdin, STDIN_FILENO) < 0)
		exit (1);
	if (dup2(fdout, STDOUT_FILENO) < 0)
		exit (1);
	close_pipes_files(data);
	check_builtins(data->test_cmd[i], data);
	execve(path_cmd(data->test_cmd[i][0], data->env), data->test_cmd[i], data->env);
	errno = ENOENT;
	error_exit(data->test_cmd[i][0]);
	exit(0);
}

char	*path_cmd(char *command, char **env)
{
	int			i;
	char		**paths;
	char		*cmd_path;

	if (ft_strchr(command, '/'))
		return (command);
	i = find_env_var("PATH", env);
	if (i >= 0)
		paths = null_check(ft_split(env[i] + find_value(env[i]) + 1, ':'));
	else
		return (NULL);
	i = 0;
	while (paths[i])
	{
		cmd_path = null_check(ft_strjoin(paths[i], "/"));
		cmd_path = null_check(ft_strjoin_free(cmd_path, command));
		if (access(cmd_path, X_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	return (command);
}

void	check_builtins(char **cmd_table, t_exec *data)
{
	int	i;

	i = -1;
	while (cmd_table[0][++i])
		cmd_table[0][i] = ft_tolower(cmd_table[0][i]);
	if (strncmp(cmd_table[0], "cd", ft_strlen(cmd_table[0])) == 0)
		cd_builtin(cmd_table, &data->env);
	else if (strncmp(cmd_table[0], "echo", ft_strlen(cmd_table[0])) == 0)
		echo_builtin(cmd_table);
	else if (strncmp(cmd_table[0], "env", ft_strlen(cmd_table[0])) == 0)
		env_builtin(data->env);
	else if (strncmp(cmd_table[0], "exit", ft_strlen(cmd_table[0])) == 0)
		exit_builtin(666);
	else if (strncmp(cmd_table[0], "cd", ft_strlen(cmd_table[0])) == 0)
		export_builtin(cmd_table[1], &data->env);
	else if (strncmp(cmd_table[0], "pwd", ft_strlen(cmd_table[0])) == 0)
		pwd_builtin();
	else if (strncmp(cmd_table[0], "unset", ft_strlen(cmd_table[0])) == 0)
		unset_builtin(cmd_table[1], &data->env);
}
void	*testing(t_exec *data)
{
	data->test_cmd[0][0] = ft_strdup("cd");
	data->test_cmd[0][1] = "source";
	data->test_cmd[0][2] = NULL;
	data->test_cmd[0][3] = NULL;
	data->test_cmd[1][0] = ft_strdup("env");
	data->test_cmd[1][1] = NULL;
	data->test_cmd[1][2] = NULL;
	data->test_cmd[2][0] = ft_strdup("echo");
	data->test_cmd[2][1] = "yay";
	return (data);
}
