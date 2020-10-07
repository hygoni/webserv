/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 19:41:31 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/02/24 19:46:14 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *str, int val)
{
	int	i;

	i = ft_strlen(str);
	while (str[i] != (char)val)
	{
		if (i == 0)
			return (NULL);
		i--;
	}
	return ((char*)&str[i]);
}
