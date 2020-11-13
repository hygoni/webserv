/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 23:11:25 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/11/09 16:52:03 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include "get_next_line.hpp"
#include "libft.h"


void ft_lstremove(t_buf **lst, int fd, t_buf *prev, t_buf *current)
{
	while (current)
	{
		if (current->fd == fd)
		{
			if (prev == NULL)
			{
				*lst = current->next;
				free(current->buf);
				free(current);
				current = *lst;
				continue;
			}
			else
			{
				prev->next = current->next;
				free(current->buf);
				free(current);
				current = prev->next;
			}
			continue;
		}
		prev = current;
		current = current->next;
	}
}

t_buf	*ft_lstnew(int fd)
{
	t_buf	*list;

	if ((list = (t_buf*)malloc(sizeof(t_buf))) == NULL)
		return (NULL);
	if ((list->buf = ft_strdup("")) == NULL)
		return (NULL);
	list->fd = fd;
	list->next = NULL;
	return (list);
}

t_buf	*ft_lstfind(t_buf *lst, int fd)
{
	while (lst != NULL)
	{
		if (lst->fd == fd)
			return (lst);
		lst = lst->next;
	}
	return (NULL);
}
