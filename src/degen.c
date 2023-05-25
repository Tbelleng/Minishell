#include "../includes/minishell.h"

extern int g_global;


/*

void create_node_and_list(t_pipe *file, char *limiter)
{
	t_node  *node;
	
	node = ft_calloc(sizeof(t_node), 1);
	//if (node == NULL)
		/*exit function*/
	node -> limiter = limiter;
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


/*static int    pipe_count(t_pars **pars)
{
	int     count;
	t_pars  *tmp;
	
	count = 0;
	tmp = *pars;
	while(*pars != NULL)
	{
		if ((*pars)->token == PIPE)
			count++;
		*pars = (*pars)->next;
	}
	*pars = tmp;
	count = count * 2;
	return (count);
}*/


static void	neww(int infile, int outfile)
{
	if (infile != STDIN_FILENO)
	{
		dup2(infile, STDIN_FILENO);
		close(infile);
	}
	if (outfile != STDOUT_FILENO)
	{
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}
}

static char **return_arg(t_pars **pars, t_pipe *file, char **arg, int count)
{
	arg[count] = file->cmd_to_exec[file->pidx];
	count++;
	while ((*pars)->token != CMD)
			(*pars) = (*pars)->next;
	while ((*pars) != NULL && ((*pars)->token != PIPE && (*pars)->token != R_OUTPUT && (*pars)->token != R_DOUTPUT))
	{
		if ((*pars)->token != CMD && (*pars)->token != PIPE)
		{
			if((*pars)->token != CMD && (*pars)->token != R_INPUT && (*pars)->token != PIPE)
			{
				arg[count] = (*pars)->str;
				count++;
			}
			(*pars) = (*pars)->next;
		}
		else
			(*pars) = (*pars)->next;
	}
	arg[count] = 0;
	return (arg);
}

static char **tema_larg2(t_pipe *file, t_pars **pars)
{
	t_pars  *tmp;
	int     count;
	char    **arg;
	
	count = 0;
	tmp = *pars;
	arg = malloc(sizeof(char *) * 10);
	if (!arg)
		return (NULL);
	while (count != file->pidx && (*pars) != NULL)
	{
		if ((*pars)->token == PIPE)
			count++;
		(*pars) = (*pars)->next;
	}
	count = 0;
	arg = return_arg(pars, file, arg, count);
	*pars = tmp;
	return (arg);
}

static t_pars* find_cmd_pars(t_pars *pars)
{
	while ((pars) != NULL && pars->token != PIPE)
	{
		if ((pars)->token == CMD || (pars)->token == BUILTIN)
			return pars;
		pars = pars->next;
	}
	return (NULL);
}

static int    in_firstpid(t_pars **pars, t_pipe *file, int *last, t_pars *cmd)
{
	int nb_rd;
	
	nb_rd = 0;
	cmd = find_cmd_pars(*pars);
	while ((*pars) != NULL && (*pars)->token != PIPE)
	{
		if ((*pars)->token == R_INPUT)
		{
			last = INFILE;
			if (file->infile > 0)
					close(file->infile);
			file->infile = open((*pars)->next->str, O_RDONLY);
			if (file->infile < 0)
				msg_error(ERR_INFILE, file);
			nb_rd++;
		}
		else if ((*pars)->token == R_DINPUT)
			*last = HEREDOC;
		(*pars) = (*pars)->next;
	}
	if (nb_rd == 0)
		file->infile = 0;
	if (*last == HEREDOC)
		file->infile = find_doc_fd(file->node, cmd->limiter);
	return (file->infile);
}

static int    pipe_pass(t_pars **pars, t_pipe *file)
{
	int count;
	
	count = 0;
	while (count != file->pidx && (*pars) != NULL)
		{
			if ((*pars)->token == PIPE)
				count++;
			(*pars) = (*pars)->next;
		}
	return (count);
}

static int    in_otherpid(t_pars **pars, t_pipe *file, int *last, t_pars *cmd)
{
	int nb_rd;
	
	nb_rd = 0;
	while ((*pars) != NULL && (*pars)->token != PIPE)
	{
		if ((*pars)->token == R_INPUT)
		{
			*last = INFILE;
			file->infile = open((*pars)->next->str, O_RDONLY);
			if (file->infile < 0)
				msg_error(ERR_INFILE, file);
			nb_rd++;
		}
		else if ((*pars)->token == R_DINPUT)
		{
			*last = HEREDOC;
			nb_rd++;
		}
		(*pars) = (*pars)->next;
	}
	if (nb_rd == 0)
		file->infile = file->prev_pipes;
	if (*last == HEREDOC)
		file->infile = find_doc_fd(file->node, cmd->limiter);
	return (file->infile);
}

static int    redirect_in(t_pipe *file, t_pars **pars)
{
	int     count;
	int     nb_rd;
	t_pars  *tmp;
	t_pars  *cmd;
	int     last;
	
	count = 0;
	nb_rd = 0;
	tmp = *pars;
	last = -1;
	cmd = NULL;
	if (file->pidx == 0)
	{
		file->infile = in_firstpid(pars, file, &last, cmd);
		*pars = tmp;
		return (file->infile);
	}
	else
	{
		count = pipe_pass(pars, file);
		cmd = find_cmd_pars(*pars);
		file->infile = in_otherpid(pars, file, &last, cmd);
		return (file->infile);
	}
}

static int     redirect_count(t_pars **pars)
{
	int     count;
	
	count = 0;
	while ((*pars)->token != PIPE)
	{
		if ((*pars)->token == R_OUTPUT || (*pars)->token == R_DOUTPUT)
			count++;
		(*pars) = (*pars)->next;
	}
	return (count);
}

static int    redirect_out(t_pipe *file, t_pars **pars)
{
	int     count;
	t_pars  *tmp;
	
	count = 0;
	tmp = *pars;
	
	if (file->pidx != 0)
		count = pipe_pass(pars, file);
	else if (file->pidx == 0)
	{
		redirect_count(pars);
		*pars = tmp;
	}
	if (file->pidx == file->cmd_nb - 1)
		file->outfile = STDOUT_FILENO;
	else
		file->outfile = file->fd[1];	
	if (count != 0)
	{
		while ((*pars) != NULL && (*pars)->token != PIPE)
		{
			if ((*pars)->token == R_OUTPUT)
			{
				file->outfile = open((*pars)->next->str, O_TRUNC | O_CREAT | O_RDWR, 0000644);
				if (file->outfile < 0)
					msg_error(ERR_OUTFILE, file);
			}
			if ((*pars)->token == R_DOUTPUT)
			{
				file->outfile = open((*pars)->next->str, O_APPEND | O_CREAT | O_RDWR, 0000644);
				if (file->outfile < 0)
					msg_error(ERR_OUTFILE, file);
			}
			(*pars) = (*pars)->next;
		}
	}
	*pars = tmp;
	return (file->outfile);
}

static int     is_built_ins(t_pars **pars, t_pipe *file)
{
	int     count;
	t_pars  *tmp;
	
	count = 0;
	tmp = *pars;
	if (file->pidx == 0)
	{
		while ((*pars) != NULL && (*pars)->token != PIPE && (*pars)->token != R_OUTPUT && (*pars)->token != R_DOUTPUT)
		{
			if ((*pars)->token == BUILTIN)
			{
				*pars = tmp;
				return (1);
			}
			(*pars) = (*pars)->next;
		} 
	}
	else
	{
		while (count != file->pidx)
		{
			if ((*pars)->token == PIPE)
				count++;
			(*pars) = (*pars)->next;
		}
		while ((*pars) != NULL && (*pars)->token != PIPE && (*pars)->token != R_OUTPUT && (*pars)->token != R_DOUTPUT)
		{
			if ((*pars)->token == BUILTIN)
			{
				*pars = tmp;
				return (1);
			}
			(*pars) = (*pars)->next;
		}
	}
	*pars = tmp;
	return (0);
}

static void	multiple_cmd(t_pipe *file, t_data *data, t_pars **pars)
{
	int     i;
	int     in;
	int     out;
	t_node *tmp1;
	
	i = 0;
	file->pid[file->pidx] = fork();
	if (!file->pid[file->pidx])
	{
		in = redirect_in(file, pars);
		//printf("cmd :%s and his infile : %d\n", file->cmd_to_exec[file->pidx], in);
		if (in < 0)
		{
			close(file->fd[0]);
			close(file->fd[1]);
			int i = -1;
			while (data->env[++i])
				free(data->env[i]);
			free(data->env);
			i = -1;
			while (file->cmd_paths[++i])
				free(file->cmd_paths[i]);
			free(file->cmd_paths);
			i = -1;
			while(file->cmd_to_exec[++i])
				free(file->cmd_to_exec[i]);
			free(file->cmd_to_exec);
			free(file->cmd);
			free(file->paths);
			free(file->pid);
			t_pars *tmp;
			while ((*pars) != NULL)
			{
				tmp = (*pars)->next;
				free((*pars)->ID);
				free((*pars)->str);
				free(*pars);
				*pars = tmp;
			}
			msg(ERR_INFILE);
			exit (126);
		}
		out = redirect_out(file, pars);
		neww(in, out);
		close_pipes(file);
		if (is_built_ins(pars, file))
		{
			builtin_exe_mult(pars, file, data);
			close(file->fd[0]);
			close(file->fd[1]);
			int i = -1;
			while (data->env[++i])
				free(data->env[i]);
			free(data->env);
			i = -1;
			while (file->cmd_to_exec[++i])
				free(file->cmd_to_exec[i]);
			free(file->cmd_to_exec);
			i = -1;
			while (file->cmd_paths[++i])
				free(file->cmd_paths[i]);
			free(file->cmd_paths);
			free(file->paths);
			free(file->pid);
			while(file->node)
			{
				tmp1 = file->node->next;
				free(file->node);
				file->node = tmp1;
			}
			t_pars *tmp;
			while ((*pars) != NULL)
			{
				tmp = (*pars)->next;
				free((*pars)->ID);
				free((*pars)->str);
				free(*pars);
				*pars = tmp;
			}
			exit (1);
		}
		file->cmd_args = tema_larg2(file, pars);
		file->cmd = get_cmd(file->cmd_paths, file->cmd_args[0]);
		fprintf(stderr, "ARG to b executed is %s\n", file->cmd_args[0]);
		fprintf(stderr, "CMD to b executed is %s\n", file->cmd);
		if (!file->cmd)
		{
			int i = -1;
			while (data->env[++i])
				free(data->env[i]);
			free(data->env);
			free(file->cmd_args);
			i = -1;
			while (file->cmd_paths[++i])
				free(file->cmd_paths[i]);
			free(file->cmd_paths);
			i = -1;
			while(file->cmd_to_exec[++i])
				free(file->cmd_to_exec[i]);
			free(file->cmd_to_exec);
			free(file->cmd);
			free(file->paths);
			free(file->pid);
			while(file->node)
			{
				tmp1 = file->node->next;
				free(file->node);
				file->node = tmp1;
			}
			t_pars *tmp;
			while ((*pars) != NULL)
			{
				tmp = (*pars)->next;
				free((*pars)->ID);
				free((*pars)->str);
				free(*pars);
				*pars = tmp;
			}
			msg(ERR_CMD);
			exit (127);
		}
		execve(file->cmd, file->cmd_args, data->env);
		printf("EXEC FAIL\n");
		error_free(file);
		free(file->cmd);
		exit(1);
	}
}

void    mult_cmd(t_pipe *file, t_pars **pars, t_data *data)
{
	int    i;
	int     status = 0;
	
	i = -1;
	file->infile = 0;
	file->outfile = 1;
	file->prev_pipes = -1;
	//file->pipe_nb = pipe_count(pars);
	file->pid = malloc(sizeof(pid_t) * (file->cmd_nb));
	if (!file->pid)
		pid_err(file);
	while (file->pidx < file->cmd_nb)
	{
		if (file->pidx != file->cmd_nb - 1 && pipe(file->fd) < 0)
			msg_error(ERR_PIPE, file);
		printf("Infile: %d Outfile: %d\n", file->fd[0], file->fd[1]);
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

*/