/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 21:57:45 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/11/09 16:54:32 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include "get_next_line.hpp"
#include "libft.h"

static int	parse_next_line(char **line, t_buf *list)
{
	char	*new_line;
	char	*buf;

	new_line = ft_strchr(list->buf, '\n');
	if (new_line == NULL)
	{
		*line = ft_strdup(list->buf);
		return (0);
	}
	else
	{
		*new_line = '\0';
		buf = list->buf;
		*line = ft_strdup(list->buf);
		if ((list->buf = ft_strdup(new_line + 1)) == NULL)
			return (-1);
		free(buf);
		return (1);
	}
}

static int	read_next_line(int fd, char **line, t_buf *list)
{
	char	*new_line;
	int		n_read;
	char	*tmp;
	char	*to_free;

	if ((tmp = (char*)malloc(BUFFER_SIZE + 1)) == NULL)
		return (-1);
	while (((new_line = ft_strchr(list->buf, '\n')) == NULL)
			&& (n_read = read(fd, tmp, BUFFER_SIZE)) > 0)
	{
		to_free = list->buf;
		tmp[n_read] = '\0';
		list->buf = ft_strjoin(list->buf, tmp);
		free(to_free);
	}
	free(tmp);
	if (n_read < 0)
		return (-1);
	return (parse_next_line(line, list));
}

int			get_next_line(int fd, char **line)
{
	t_buf			*new_node;
	int				ret;
	static t_buf	*buf_list = NULL;

	if (BUFFER_SIZE <= 0 || !line || fd < 0)
		return (-1);
	new_node = buf_list;
	while (new_node != NULL)
	{
		if (new_node->fd == fd)
			break ;
		new_node = new_node->next;
	}
	if (new_node == NULL)
	{
		if ((new_node = ft_lstnew(fd)) == NULL)
			return (-1);
		new_node->next = buf_list;
		buf_list = new_node;
	}
	ret = (read_next_line(fd, line, ft_lstfind(buf_list, fd)));
	if (ret == 0)
		ft_lstremove(&buf_list, fd, NULL, buf_list);
	return (ret);
}
