/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 15:11:58 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/02/24 15:40:02 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*ft_memset(void *ptr, int val, size_t n)
{
	char	*c_ptr;
	size_t	i;

	c_ptr = (char*)ptr;
	i = 0;
	while (i < n)
	{
		c_ptr[i] = (char)val;
		i++;
	}
	return (ptr);
}
