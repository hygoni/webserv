/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 14:57:41 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:25:01 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t				i;
	const unsigned char	*left;
	const unsigned char	*right;

	if (n == 0)
		return (0);
	left = (const unsigned char*)s1;
	right = (const unsigned char*)s2;
	i = 0;
	while (*left == *right && i + 1 < n)
	{
		left++;
		right++;
		i++;
	}
	return (*left - *right);
}
