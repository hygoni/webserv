/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hyeyoo <9hrack@gmail.com>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/24 20:00:18 by hyeyoo            #+#    #+#             */
/*   Updated: 2020/04/03 19:23:33 by hyeyoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int	ft_strcmp(const char *s1, const char *s2)
{
	unsigned char *left;
	unsigned char *right;

	left = (unsigned char *)s1;
	right = (unsigned char *)s2;
	while (*left == *right && *left != '\0')
	{
		left++;
		right++;
	}
	return (*left - *right);
}
