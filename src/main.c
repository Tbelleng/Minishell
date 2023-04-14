/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luciefer <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/06 18:37:36 by luciefer          #+#    #+#             */
/*   Updated: 2023/04/14 13:20:44 by luciefer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	main(int ac, char **av, char **envp)
{
	char	*str;
	t_list	*pars;

	while(1)
	{
		str = readline("$> ");
		if (!ft_parcing(pars, str, envp))
			printf("error\n");
		exec_cmd(str);
		free(str);
	}
}
