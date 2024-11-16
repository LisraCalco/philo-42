/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:23:52 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 14:44:11 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void	philosopher(t_philo *philosopher)
{
	if (philosopher->id % 2 == 0)
		usleep(100);
	while (philo_is_dead(philosopher) == 0)
	{
		philo_think(philosopher);
		philo_eat(philosopher);
		philo_sleep(philosopher);
	}
	return ;
}

int	check_all_philo_eat_time( t_program *program)
{
	int	i;

	i = 0;
	while (i < program->num_of_philos)
	{
		pthread_mutex_lock(program->philos[i].meal_lock);
		if (program->philos[i].num_times_to_eat == -1)
		{
			pthread_mutex_unlock(program->philos[i].meal_lock);
			i++;
			return (0);
		}
		if (program->philos[i].num_times_to_eat != -1
			&& program->philos[i].meals_eaten
			< program->philos[i].num_times_to_eat)
		{
			pthread_mutex_unlock(program->philos[i].meal_lock);
			return (0);
		}
		pthread_mutex_unlock(program->philos[i].meal_lock);
		i++;
	}
	return (1);
}

void	set_program_dead(t_program *program)
{
	pthread_mutex_lock(&program->dead_lock);
	program->dead_flag = 1;
	pthread_mutex_unlock(&program->dead_lock);
}

int	didnt_eat_recently(t_philo *philo)
{
	pthread_mutex_lock(philo->meal_lock);
	if (get_execution_time(philo->start_time)
		- philo->last_meal < philo->time_to_die)
	{
		pthread_mutex_unlock(philo->meal_lock);
		return (0);
	}
	pthread_mutex_unlock(philo->meal_lock);
	return (1);
}

void	monitor(t_program *program)
{
	int	i;

	while (1)
	{
		i = 0;
		if (check_all_philo_eat_time(program) == 1)
		{
			set_program_dead(program);
			return ;
		}
		while (i < program->num_of_philos)
		{
			if (didnt_eat_recently(&program->philos[i]) == 1)
			{
				print_msg(&program->philos[i], DEAD);
				set_program_dead(program);
				return ;
			}
			i++;
		}
		usleep(1000);
	}
}
