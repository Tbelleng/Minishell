/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multi_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbelleng <tbelleng@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/01 16:56:53 by tbelleng          #+#    #+#             */
/*   Updated: 2023/05/25 19:32:41 by tbelleng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

extern int	g_global;

void	create_node_and_list(t_pipe *file, char *limiter)
{
	t_node	*node;

	node = ft_calloc(sizeof(t_node), 1);
	node->limiter = limiter;
	if (file->node == NULL)
	{
		file->node = node;
		file->last = node;
	}
	else
	{
		file->last->next = node;
		node->prev = file->last;
		file->last = node;
	}
}

static void	end_multi(t_pipe *file, t_data *data, t_pars **pars)
{
	if (is_built_ins(pars, file))
	{
		builtin_exe_mult(pars, file, data);
		close(file->fd[0]);
		close(file->fd[1]);
		free_builtin(pars, file, data);
		exit(1);
	}
	file->cmd_args = tema_larg2(file, pars);
	file->cmd = get_cmd(file->cmd_paths, file->cmd_args[0]);
	if (!file->cmd)
	{
		free_no_cmd(pars, file, data);
		msg(ERR_CMD);
		exit(127);
	}
	execve(file->cmd, file->cmd_args, data->env);
	error_free(file);
	free(file->cmd);
	exit(1);
}

static void	multiple_cmd(t_pipe *file, t_data *data, t_pars **pars)
{
	int	i;
	int	in;
	int	out;

	i = 0;
	file->pid[file->pidx] = fork();
	if (!file->pid[file->pidx])
	{
		in = redirect_in(file, pars);
		if (in < 0)
		{
			close(file->fd[0]);
			close(file->fd[1]);
			free_in(pars, file, data);
			msg(ERR_INFILE);
			exit(126);
		}
		out = redirect_out(file, pars);
		neww(in, out);
		close_pipes(file);
		end_multi(file, data, pars);
	}
}

static void	init_structt(t_pipe *file, t_pars **pars, t_data *data)
{
	file->infile = 0;
	file->outfile = 1;
	file->prev_pipes = -1;
	file->pid = malloc(sizeof(pid_t) * (file->cmd_nb));
	if (!file->pid)
	{
		free_in(pars, file, data);
		exit(1);
	}
}

void	mult_cmd(t_pipe *file, t_pars **pars, t_data *data)
{
	int	i;
	int	status;

	status = 0;
	init_structt(file, pars, data);
	while (file->pidx < file->cmd_nb)
	{
		if (file->pidx != file->cmd_nb - 1 && pipe(file->fd) < 0)
			msg_error(ERR_PIPE, file);
		multiple_cmd(file, data, pars);
		close(file->fd[1]);
		if (file->prev_pipes != -1)
			close(file->prev_pipes);
		file->prev_pipes = file->fd[0];
		file->pidx++;
	}
	close_here_doc_pipe(file->node, 1, 0);
	i = -1;
	while (++i < file->cmd_nb)
		waitpid(file->pid[i], &status, 0);
	if (WIFEXITED(status))
		g_global = WEXITSTATUS(status);
	parent_free(file);
	return ;
}
