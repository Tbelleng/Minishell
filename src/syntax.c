/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciefer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/24 08:58:14 by luciefer          #+#    #+#             */
/*   Updated: 2023/05/01 17:03:56 by luciefer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

static int	check_nb_cmd(t_pars **pars)
{
	int		i;
	int		j;
	t_pars	*tmp;

	j = 0;
	i = 0;
	tmp = (*pars);
	while ((*pars) != NULL)
	{
		if ((*pars)->token == CMD)
			i++;
		if ((*pars)->token == PIPE)
			j++;
		*pars = (*pars)->next;
	}
	*pars = tmp;
	if (i != j + 1)
		return (0);
	return (1);
}

static int	replace_expand(t_pars **pars, char **env)
{
	t_pars	*tmp;

	(void) env;
	tmp = *pars;
	while ((*pars) != NULL)
	{
		if ((*pars)->token == TXT_S)
			del_quote(*pars);
		else if ((*pars)->token == EXPAND || (*pars)->token == TXT
			|| (*pars)->token == TXT_D)
			replace_dollar(*pars, env);
		*pars = (*pars)->next;
	}
	*pars = tmp;
	return (1);
}

static int	check_redirect(t_pars *pars)
{
	int		i;
	t_pars	*tmp;

	i = 0;
	tmp = pars;
	while (pars != 0)
	{
		if (is_redirect(pars->token))
		{
			pars = tmp;
			return (1);
		}
		else
			pars = pars->next;
	}
	pars = tmp;
	return (0);
}

static int	check_arg(t_pars *pars, char **env)
{
	t_pars	*tmp;

	tmp = pars;
	if (check_redirect(pars))
	{
		return (check_syntax_redirect(pars));
	}
	else
	{
		if (pars->token != CMD)
		{
			pars = tmp;
			return (0);
		}
		else
		{
			pars = pars->next;
			while (pars != NULL)
			{
				if (pars->token == PIPE && pars->next != NULL)
				{
					check_syntax(&pars->next, env);
					break ;
				}
				if (pars->token != ARG)
				{
					pars = tmp;
					return (0);
				}
				pars = pars->next;
			}
		}
		pars = tmp;
		return (1);
	}
}

int	check_syntax(t_pars **pars, char **env)
{
	int	i;

	(void) env;
	i = check_nb_cmd(pars);
	i = i + replace_expand(pars, env);
	i = i + check_arg(*pars, env);
	if (i != 3)
		return (0);
	return (i);
}