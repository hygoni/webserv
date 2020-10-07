/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 15:38:15 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/02/25 15:42:29 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

char	*ft_strjoin(const char *s1, const char *s2)
{
	char	*dest;
	size_t	size;

	size = ft_strlen(s1) + ft_strlen(s2) + 1;
	if ((dest = (char*)malloc(sizeof(char) * size)) == NULL)
		return (NULL);
	*dest = '\0';
	ft_strlcat(dest, s1, size);
	ft_strlcat(dest, s2, size);
	return (dest);
}
