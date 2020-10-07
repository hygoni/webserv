/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_free_array.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 16:25:13 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/02/25 16:28:50 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>

void	ft_free_array(void **ptr, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		free(ptr[i]);
		i++;
	}
	free(ptr);
}
