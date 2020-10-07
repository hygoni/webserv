/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 15:52:39 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 21:28:56 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdlib.h>
#include "libft.h"

static size_t	ft_count(const char *s, char c)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] != c && s[i] != '\0')
				i++;
			continue;
		}
		i++;
	}
	return (count);
}

static int		ft_split_sub(const char *s, char c, char **split, size_t len)
{
	size_t	size;

	size = ft_strlen_chr(s, c) + 1;
	if ((split[len] = (char*)malloc(sizeof(char) * size)) == NULL)
	{
		ft_free_null_terminated_array((void**)split);
		return (FALSE);
	}
	ft_strlcpy(split[len], s, size);
	return (TRUE);
}

char			**ft_split(const char *s, char c)
{
	size_t	i;
	size_t	len;
	char	**split;

	len = ft_count(s, c);
	if ((split = (char**)ft_calloc(len + 1, sizeof(char*))) == NULL)
		return (NULL);
	split[len] = NULL;
	i = 0;
	len = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
		{
			if (!ft_split_sub(&s[i], c, split, len++))
				return (NULL);
			while (s[i] != c && s[i] != '\0')
				i++;
			continue;
		}
		i++;
	}
	return (split);
}
