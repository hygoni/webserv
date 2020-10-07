/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memccpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 21:20:14 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:42:48 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memccpy(void *dest, const void *src, int val, size_t n)
{
	char		*c_dest;
	const char	*c_src;
	size_t		i;

	c_dest = (char*)dest;
	c_src = (char*)src;
	i = 0;
	while (i < n)
	{
		c_dest[i] = c_src[i];
		if (c_src[i] == (char)val)
			return ((char*)(&c_dest[i + 1]));
		i++;
	}
	return (NULL);
}
