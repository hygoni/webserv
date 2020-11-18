/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 21:59:01 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/11/09 16:55:31 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H

# define BUFFER_SIZE 4096
# include <string.h>

typedef struct	s_buf
{
	int				fd;
	char			*buf;
	struct s_buf	*next;
}				t_buf;

t_buf			*ft_lstnew(int fd);
t_buf			*ft_lstfind(t_buf *lst, int fd);
int				get_next_line(int fd, char **line);
void			ft_lstremove(t_buf **lst, int fd, t_buf *prev, t_buf *curr);

#endif
