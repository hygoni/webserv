/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_bzero.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 19:47:54 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:28:19 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include "libft.h"

void	*ft_bzero(void *ptr, size_t n)
{
	char	*c_ptr;
	size_t	i;

	c_ptr = (char*)ptr;
	i = 0;
	while (i < n)
	{
		c_ptr[i] = '\0';
		i++;
	}
	return (ptr);
}
