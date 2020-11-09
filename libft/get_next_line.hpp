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

typedef struct	s_list
{
	int				fd;
	char			*buf;
	struct s_list	*next;
}				t_list;

t_list			*ft_lstnew(int fd);
t_list			*ft_lstfind(t_list *lst, int fd);
void			ft_lstadd_front(t_list **lst, t_list *new_node);
char			*ft_strchr(const char *str, int c);
char			*ft_strjoin(char *original, char *to_append);
int				get_next_line(int fd, char **line);
void			*ft_memcpy(void *dest, const void *src, size_t n);
char			*ft_strdup(const char *str);
void			ft_lstremove(t_list **lst, int fd, t_list *prev, t_list *curr);

#endif
