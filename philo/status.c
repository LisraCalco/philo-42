/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   status.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/16 11:26:17 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 14:07:02 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int	philo_is_dead(t_philo *philo)
{
	int	dead;

	pthread_mutex_lock(philo->dead_lock);
	dead = *philo->dead;
	pthread_mutex_unlock(philo->dead_lock);
	return (dead);
}

void	philo_think(t_philo *philo)
{
	usleep(1000);
	if (philo_is_dead(philo) == 0)
		print_msg(philo, THINKING);
}

void	philo_eat(t_philo *philo)
{
	pthread_mutex_lock(philo->l_fork);
	if (philo_is_dead(philo) == 0)
		print_msg(philo, FORK_TAKEN);
	if (philo->num_of_philos == 1)
	{
		pthread_mutex_unlock(philo->l_fork);
		usleep(philo->time_to_die * 1000);
		return ;
	}
	pthread_mutex_lock(philo->r_fork);
	if (philo_is_dead(philo) == 0)
		print_msg(philo, FORK_TAKEN);
	pthread_mutex_lock(philo->meal_lock);
	philo->last_meal = get_execution_time(philo->start_time);
	pthread_mutex_unlock(philo->meal_lock);
	if (philo_is_dead(philo) == 0)
		print_msg(philo, EATING);
	usleep(philo->time_to_eat * 1000);
	pthread_mutex_lock(philo->meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(philo->meal_lock);
	pthread_mutex_unlock(philo->r_fork);
	pthread_mutex_unlock(philo->l_fork);
}

void	philo_sleep(t_philo *philo)
{
	if (philo_is_dead(philo) == 0)
		print_msg(philo, SLEEPING);
	usleep(philo->time_to_sleep * 1000);
}

void	print_msg(t_philo *philo, char *msg)
{
	pthread_mutex_lock(philo->write_lock);
	if (philo_is_dead(philo) == 0)
		printf("%ld %d %s", get_execution_time(philo->start_time),
			philo->id, msg);
	pthread_mutex_unlock(philo->write_lock);
}
