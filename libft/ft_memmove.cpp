/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 21:41:37 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/06 02:59:41 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	char	*c_src;
	char	*c_dest;

	if (dest == NULL || src == NULL)
		return (dest);
	c_src = (char*)src;
	c_dest = (char*)dest;
	if (c_dest < c_src)
		return (ft_memcpy(dest, src, n));
	while (n > 0)
	{
		c_dest[n - 1] = c_src[n - 1];
		n--;
	}
	return (dest);
}
