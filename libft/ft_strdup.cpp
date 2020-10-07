/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 15:20:42 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 22:30:12 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

char	*ft_strdup(const char *str)
{
	char	*copied;

	copied = (char*)malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (copied == NULL)
		return (NULL);
	ft_strcpy(copied, str);
	return (copied);
}
