/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 22:08:35 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/07 13:29:31 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *ptr, int val, size_t n)
{
	unsigned char	*c_ptr;

	c_ptr = (unsigned char*)ptr;
	while (n > 0)
	{
		if (*c_ptr == (unsigned char)val)
			return (c_ptr);
		c_ptr++;
		n--;
	}
	return (NULL);
}
