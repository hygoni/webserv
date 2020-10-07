/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 17:52:57 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/02/25 17:54:33 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*list;

	if ((list = (t_list*)malloc(sizeof(t_list))) == NULL)
		return (NULL);
	list->content = content;
	list->next = NULL;
	return (list);
}
