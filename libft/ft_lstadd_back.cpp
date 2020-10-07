/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 18:00:23 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:41:15 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new_list)
{
	t_list	*last;

	if (*lst == NULL)
	{
		*lst = new_list;
	}
	else
	{
		last = ft_lstlast(*lst);
		last->next = new_list;
	}
}
