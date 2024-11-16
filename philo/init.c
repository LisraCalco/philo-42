/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:23:45 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 15:59:38 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	init_philo_values(t_program *program, char **argv, int argc, int i)
{
	program->philos[i].id = i + 1;
	program->philos[i].meals_eaten = 0;
	program->philos[i].last_meal = 0;
	program->philos[i].time_to_die = ft_atoi(argv[2]);
	program->philos[i].time_to_eat = ft_atoi(argv[3]);
	program->philos[i].time_to_sleep = ft_atoi(argv[4]);
	program->philos[i].start_time = get_current_epoch();
	program->philos[i].num_of_philos = program->num_of_philos;
	if (argc == 6)
		program->philos[i].num_times_to_eat = ft_atoi(argv[5]);
	else
		program->philos[i].num_times_to_eat = -1;
	program->philos[i].dead = &program->dead_flag;
}

int	init_philos(t_program *program, char **argv, int argc)
{
	int	i;

	i = 0;
	while (i < program->num_of_philos)
	{
		init_philo_values(program, argv, argc, i);
		if (i % 2)
		{
			program->philos[i].r_fork = &program->forks[i];
			program->philos[i].l_fork = &program->forks[(i + 1)
				% program->num_of_philos];
		}
		else
		{
			program->philos[i].r_fork = &program->forks[(i + 1)
				% program->num_of_philos];
			program->philos[i].l_fork = &program->forks[i];
		}
		program->philos[i].write_lock = &program->write_lock;
		program->philos[i].dead_lock = &program->dead_lock;
		program->philos[i].meal_lock = &program->meal_lock;
		i++;
	}
	return (0);
}

void	init_monitor_thread(t_program *program)
{
	pthread_create(&program->monitor_thread, NULL, (void *)monitor, program);
}

void	init_mutex(t_program *program)
{
	int	i;

	i = 0;
	while (i < program->num_of_philos)
	{
		pthread_mutex_init(&program->forks[i], NULL);
		i++;
	}
	pthread_mutex_init(&program->dead_lock, NULL);
	pthread_mutex_init(&program->meal_lock, NULL);
	pthread_mutex_init(&program->write_lock, NULL);
}

void	destroy_mutex(t_program *program)
{
	int	i;

	i = 0;
	while (i < program->num_of_philos)
	{
		pthread_join(program->philos[i].thread, NULL);
		i++;
	}
	pthread_join(program->monitor_thread, NULL);
	i = 0;
	while (i < program->num_of_philos)
	{
		pthread_mutex_destroy(&program->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&program->dead_lock);
	pthread_mutex_destroy(&program->meal_lock);
	pthread_mutex_destroy(&program->write_lock);
}
