/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 14:29:00 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 15:07:14 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int	ft_strlen(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	can_fit_in_int(char *str)
{
	int	i;

	i = 0;
	if (str[0] == '-' || str[0] == '+')
		i++;
	if (ft_strlen(str + i) > 10)
		return (0);
	return (1);
}

int	start_philo_routine(t_program *program)
{
	int	i;

	i = 0;
	while (i < program->num_of_philos)
	{
		pthread_create(&program->philos[i].thread, NULL,
			(void *)philosopher, &program->philos[i]);
		i++;
	}
	return (0);
}

int	check_all_args(int argc, char **argv)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (!is_all_digit(argv[i]) || !can_fit_in_int(argv[i])
			|| !is_not_negative(argv[i]))
		{
			printf("Error: only valid positive integers are allowed\n");
			return (1);
		}
		i++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_program	*program;

	if (argc < 5 || argc > 6)
	{
		printf(USAGE_ERROR);
		return (1);
	}
	if (check_all_args(argc, argv))
		return (1);
	program = malloc(sizeof(t_program));
	program->forks = malloc(sizeof(pthread_mutex_t) * ft_atoi(argv[1]));
	program->philos = malloc(sizeof(t_philo) * ft_atoi(argv[1]));
	program->num_of_philos = ft_atoi(argv[1]);
	program->dead_flag = 0;
	init_mutex(program);
	init_philos(program, argv, argc);
	init_monitor_thread(program);
	start_philo_routine(program);
	destroy_mutex(program);
	free(program->philos);
	free(program->forks);
	free(program);
	return (0);
}
