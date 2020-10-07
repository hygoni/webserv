/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 20:07:19 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:20:54 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	little_len;

	little_len = ft_strlen(little);
	if (little_len > len)
		return (NULL);
	else if (len == 0)
		return ((char*)big);
	i = 0;
	while (i <= len - little_len)
	{
		if (ft_strncmp(&big[i], little, little_len) == 0)
			return ((char*)&big[i]);
		i++;
	}
	return (NULL);
}
