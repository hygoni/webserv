/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/25 21:06:27 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 21:14:14 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

int		ft_atoi(const char *str)
{
	int		sign;
	long	num;

	while (ft_isspace(*str))
		str++;
	sign = 1;
	if (*str == '+' || *str == '-')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	num = 0;
	while ('0' <= *str && *str <= '9')
	{
		num = num * 10 + (*str - '0');
		str++;
		if (num > INT_MAX) {
			throw "[libft::ft_atoi] number is bigger than INT_MAX, unsupported";
		} else if (num < INT_MIN) {
			throw "[libft::ft_atoi] number is smaller than INT_MIN, unsupported";
		}
	}
	return ((int)num * sign);
}
