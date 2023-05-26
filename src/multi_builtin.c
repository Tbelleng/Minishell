/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multi_builtin.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tbelleng <tbelleng@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:33:08 by tbelleng          #+#    #+#             */
/*   Updated: 2023/05/25 16:21:16 by tbelleng         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	built_in_first(t_pars **pars, t_pars *tmp)
{
	while ((*pars) != NULL && (*pars)->token != PIPE
		&& (*pars)->token != R_OUTPUT && (*pars)->token != R_DOUTPUT)
	{
		if ((*pars)->token == BUILTIN)
		{
			*pars = tmp;
			return (1);
		}
		(*pars) = (*pars)->next;
	}
	return (0);
}

int	built_in_next(t_pars **pars, t_pipe *file, t_pars *tmp)
{
	int	count;

	count = 0;
	while (count != file->pidx)
	{
		if ((*pars)->token == PIPE)
			count++;
		(*pars) = (*pars)->next;
	}
	while ((*pars) != NULL && (*pars)->token != PIPE
		&& (*pars)->token != R_OUTPUT && (*pars)->token != R_DOUTPUT)
	{
		if ((*pars)->token == BUILTIN)
		{
			*pars = tmp;
			return (1);
		}
		(*pars) = (*pars)->next;
	}
	return (0);
}

int	is_built_ins(t_pars **pars, t_pipe *file)
{
	int		count;
	int		built;
	t_pars	*tmp;

	count = 0;
	built = 0;
	tmp = *pars;
	if (file->pidx == 0)
		built = built_in_first(pars, tmp);
	else
		built = built_in_next(pars, file, tmp);
	*pars = tmp;
	return (built);
}