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
#include "get_next_line.hpp"
#include <string.h>

char	*ft_strchr(const char *str, int c)
{
	while (*str != '\0')
	{
		if (*str == c)
			return ((char*)str);
		str++;
	}
	return (NULL);
}

char	*ft_strdup(const char *str)
{
	char	*copied;
	size_t	len;

	len = 0;
	while (str[len] != '\0')
	{
		len++;
	}
	copied = (char*)malloc(sizeof(char) * (len + 1));
	ft_memcpy(copied, str, len);
	copied[len] = '\0';
	return (copied);
}

void	ft_lstremove(t_list **lst, int fd, t_list *prev, t_list *current)
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

t_list	*ft_lstnew(int fd)
{
	t_list	*list;

	if ((list = (t_list*)malloc(sizeof(t_list))) == NULL)
		return (NULL);
	if ((list->buf = ft_strdup("")) == NULL)
		return (NULL);
	list->fd = fd;
	list->next = NULL;
	return (list);
}

t_list	*ft_lstfind(t_list *lst, int fd)
{
	while (lst != NULL)
	{
		if (lst->fd == fd)
			return (lst);
		lst = lst->next;
	}
	return (NULL);
}
