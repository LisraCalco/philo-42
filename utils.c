/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:23:55 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 14:07:29 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

long	ft_atoi(const char *string)
{
	int		i;
	long	result;
	int		is_negative;

	i = 0;
	result = 0;
	is_negative = 1;
	while (string[i] == ' ' || string[i] == '\t' || string[i] == '\n'
		|| string[i] == '\v' || string[i] == '\f' || string[i] == '\r')
		i++;
	if (string[i] == '-')
		is_negative = -1;
	if (string[i] == '-' || string[i] == '+')
		i++;
	while (string[i] >= '0' && string[i] <= '9')
	{
		result = result * 10 + (string[i] - '0');
		i++;
	}
	return (result * is_negative);
}

int	is_all_digit(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	is_not_negative(char *str)
{
	if (ft_atoi(str) < 0 || ft_atoi(str) > 2147483647)
		return (0);
	return (1);
}

long	get_current_epoch(void)
{
	struct timeval	t;

	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

long	get_execution_time(long start_time)
{
	return (get_current_epoch() - start_time);
}
