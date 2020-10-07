/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 19:27:15 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/06 02:17:36 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*new_lst;
	t_list	*current;

	if ((new_lst = ft_lstnew(f(lst->content))) == NULL)
		return (NULL);
	lst = lst->next;
	current = new_lst;
	while (lst)
	{
		if ((current->next = ft_lstnew(f(lst->content))) == NULL)
		{
			ft_lstclear(&new_lst, del);
			return (NULL);
		}
		current = current->next;
		lst = lst->next;
	}
	return (new_lst);
}
