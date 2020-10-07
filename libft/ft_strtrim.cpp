/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 15:45:55 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/06 02:45:03 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strtrim(char const *s1, char const *set)
{
	char	is_set[256];
	size_t	i;
	size_t	len;

	len = ft_strlen(s1);
	if (len == 0)
		return ((char*)s1);
	ft_memset(is_set, 0, 256);
	i = 0;
	while (set[i] != '\0')
	{
		is_set[(int)set[i]] = TRUE;
		i++;
	}
	i = 0;
	while (s1[i] != '\0' && is_set[(int)s1[i]] == TRUE)
		i++;
	while (!s1[len] || (i < len && is_set[(int)s1[len]] == TRUE))
		len--;
	return (ft_substr(s1, i, (is_set[(int)s1[len]]) ? 0 : len - i + 1));
}
