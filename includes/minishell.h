/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciefer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/15 11:45:59 by luciefer          #+#    #+#             */
/*   Updated: 2023/05/05 16:35:55 by luciefer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <string.h>
# include <sys/wait.h>
# include <limits.h> 
# include <unistd.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <stdlib.h>
# include "../libft/libft.h"

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# define ERR_INFILE "Infile error\n"
# define ERR_OUTFILE "Outfile error\n"
# define ERR_INPUT "Invalid number of arguments\n"
# define ERR_PIPE "Pipe error \n"
# define ERR_CMD "Command not found\n"
# define ERR_HEREDOC "Here_doc error\n"
# define ERR_UNLINK "Unlink error\n"
# define NO_PATH "Path not found\n"
# define INVALID_ID "not a valid identifier\n"

# include <signal.h>
# include "../libft/libft.h"

enum	e_token
{
	ALPHA_NUM,
	IFS,
	S_QUOTE,
	D_QUOTE,
	DOLLAR,
	PIPE_C,
	REDIRECT,
	FINISH
};

enum	e_pars
{
	N_SORTED,
	CMD,
	PIPE,
	ARG,
	EXPAND,
	R_INPUT,
	R_OUTPUT,
	R_DINPUT,
	R_DOUTPUT,
	REDIRECT_TMP,
	TXT,
	TXT_S,
	TXT_D
};

typedef struct	s_pars
{
	struct s_pars		*prev;
	char				*str;
	enum e_token		*ID;
	enum e_pars			token;
	struct s_pars		*next;
}	t_pars;


typedef struct s_pipex
{
	pid_t	*pid;
	int		pidx;
	int		pid_numb;
	int		pipe_nb;
	int		cmd_nb;
	int     pipe_count;
	int		doc;
	int		*pipe;
	int		infile;
	char    *in_str;
	//char    *out_str;
	int     out_nb;
	int		outfile;
	char	*paths;
	char    *limit;
	char    **cmd_to_exec;
	char	**cmd_paths;
	char	**cmd_args;
	char	*cmd;

}			t_pipe;




/********************* PARCING *********************/

// token.c

void		put_token(t_pars **pars, char **env);

// token2.c

enum e_pars	check_quoted(char *str, enum e_token *ID);

// pars.c

t_pars	*ft_lstlast_(t_pars *lst);
void	_lst_back(t_pars *tmp, t_pars **pars);

// parcing.c

int	ft_parcing(t_pars **pars, char *str, char **env);
enum e_token	*char_ID(char *str);

void    is_cmd(t_pars **pars, t_pipe *file, char **envp);

/***************************************************/

void	ft_free_all();
int	    msg(char *err);
void	msg_error(char *err, t_pipe *data);
void	close_pipes(t_pipe *file);
void	parent_free(t_pipe *file);
void	pid_err(t_pipe *file);
void	infile_error(char *err, t_pipe *file);
void	error_free(t_pipe *file);
void	close_all1(t_pipe *file);
void	parent_free1(t_pipe *file);
void	child_free1(t_pipe *file);

/*************************TRIM-CMD*************************/

void    one_cmd(t_pipe *file, t_pars **pars, char **envp);


/************************EXECUTION*************************/

int    trimm_exec(t_pars **pars, char **envp);
int	execution(t_pars **pars, char **envp);
void	is_heredoc(t_pipe *file, t_pars **pars);
void    infile_read(t_pipe *file, t_pars **pars);
void	out_read(t_pipe *file, t_pars **pars);
void	out_read_v2(t_pipe *file, t_pars **pars);
void    mult_cmd(t_pipe *file, t_pars **pars, char **envp);
//void    mult_cmd(t_pipe *file, t_pars **pars, char **envp);
//void	multiple_cmd(t_pipe file, char **envp);
void    redirect_hdoc(t_pars **pars, t_pipe *file);
//char	*get_cmd(char **paths, char *cmd);

/************************GNL******************************/

char		*get_next_line(int fd);
char		*ft_strjoin1(char *readed, char *buff);
int			ft_strlen1(char *str);
char		*getting_line(char *rest);
int			check(char *str);
char		*trimmed_buff(char *rest);
int			ft_strlen_classic(char *str);
int				cmd_comp(char *cmd);

char	*get_cmd(char **paths, char *cmd);
char	*find_path(char **envp);

// token2.c

enum e_pars		check_quoted(char *str, enum e_token *ID);

// pars.c

int				create_pars(t_pars **pars, char *str, enum e_token *ID);

// parcing.c

int				ft_parcing(t_pars **pars, char *str, char **env);
void	put_id(char *str, enum e_token *ID);

// syntax.c

int				check_syntax(t_pars **pars, char **env);

// syntax_utils.c
void			del_quote(t_pars *pars);
void			replace_dollar(t_pars *pars, char **env, char *tmp);

// syntax_utils2.c
int				check_syntax_redirect(t_pars *pars);
int	is_redirect(enum e_pars pars);
void	ft_strcpy_dollar(char *tmp, char *str);
t_pars	*new_id(t_pars *pars);
char	*is_expand(t_pars *pars, char *tmp, int i);

// parsing_utils.c
t_pars	*ft_lstlast_(t_pars *lst);

// syntax_utils3.c
int	check_next(t_pars *pars);
int	check_binary(t_pars *pars);
/***************************************************/

// signal.c
void	siginthandler(int signal);

void			ft_free_all(void);
void			ft_free(t_pars *pars);

// free.c
int	malloc_sec(t_pars *pars, t_pars *new);
int	malloc_sec2(t_pars *pars, char *tmp);

// exec.c
/*void	ft_exec(t_pars *pars, char *str);
void	ft_exit(t_pars *pars, char *str);
*/
#endif
