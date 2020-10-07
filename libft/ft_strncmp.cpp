/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 19:56:34 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:21:15 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

int		ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned char	*left;
	unsigned char	*right;
	unsigned int	i;

	i = 0;
	left = (unsigned char*)s1;
	right = (unsigned char*)s2;
	if (n == 0)
		return (0);
	while (*left != '\0' && *left == *right && i < n - 1)
	{
		left++;
		right++;
		i++;
	}
	return (*left - *right);
}
