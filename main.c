/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/05 14:29:00 by tlegendr          #+#    #+#             */
/*   Updated: 2024/10/14 14:08:55 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <sys/time.h>


void	ft_putstr(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(1, &str[i], 1);
		i++;
	}
}

int ft_atoi(const char *str)
{
	int i;
	int sign;
	int res;

	i = 0;
	sign = 1;
	res = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\v' || str[i] == '\f' || str[i] == '\r')
		i++;
	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - '0';
		i++;
	}
	return (res * sign);
}

int init_philos(t_program *program, char **argv)
{
	int i;

	i = 0;
	while (i < program->num_of_philos)
	{
		struct timeval t;
		gettimeofday(&t, NULL);
		program->philos[i].id = i;
		program->philos[i].eating = 0;
		program->philos[i].meals_eaten = 0;
		program->philos[i].last_meal = 0;
		program->philos[i].time_to_die = ft_atoi(argv[2]);
		program->philos[i].time_to_eat = ft_atoi(argv[3]);
		program->philos[i].time_to_sleep = ft_atoi(argv[4]);
		program->philos[i].start_time = t.tv_sec * 1000 + t.tv_usec / 1000;
		program->philos[i].num_of_philos = program->num_of_philos;
		program->philos[i].num_times_to_eat = -1;
		program->philos[i].dead = &program->dead_flag;
		program->philos[i].r_fork = &program->forks[i];
		program->philos[i].l_fork = &program->forks[(i + 1) % program->num_of_philos];
		program->philos[i].write_lock = &program->write_lock;
		program->philos[i].dead_lock = &program->dead_lock;
		program->philos[i].meal_lock = &program->meal_lock;
		i++;
	}
	return (0);
}

void philo_think(t_philo *philo)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is thinking\n", (t.tv_sec * 1000 + t.tv_usec / 1000) - philo->start_time, philo->id);
	pthread_mutex_unlock(philo->write_lock);
}

void philo_eat(t_philo *philo)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	philo->last_meal = t.tv_sec * 1000 + t.tv_usec / 1000;
	pthread_mutex_lock(philo->l_fork);
	pthread_mutex_lock(philo->r_fork);
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d has taken a fork\n", t.tv_sec * 1000 + t.tv_usec / 1000, philo->id);
	pthread_mutex_unlock(philo->write_lock);
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is eating\n", t.tv_sec * 1000 + t.tv_usec / 1000, philo->id);
	pthread_mutex_unlock(philo->write_lock);
	usleep(philo->time_to_eat * 1000);
	pthread_mutex_unlock(philo->r_fork);
	pthread_mutex_unlock(philo->l_fork);
}

void philo_sleep(t_philo *philo)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is sleeping\n", t.tv_sec * 1000 + t.tv_usec / 1000, philo->id);
	pthread_mutex_unlock(philo->write_lock);
	usleep(philo->time_to_sleep * 1000);
}

void philosopher(void *philosopher)
{
	t_philo *philo = (t_philo*)philosopher;
	struct timeval t;
	gettimeofday(&t, NULL);
	//printf("time: %ld from philosopher %d\n", t.tv_sec * 1000 + t.tv_usec / 1000, philo->id);
	//eat with 2 forks, sleep, think
	while (philo && !*philo->dead)
	{
		philo_think(philo);
		philo_eat(philo);
		philo_sleep(philo);
	}
}

void monitor(t_program *program)
{
	struct timeval t;
	gettimeofday(&t, NULL);

	printf("dead flag: %d\n", program->dead_flag);
	while (1)
	{
		for (int i = 0; i < program->num_of_philos; i++)
		{
			gettimeofday(&t, NULL);
			if (t.tv_sec * 1000 + t.tv_usec / 1000 - program->philos[i].last_meal > program->philos[i].time_to_die)
			{
				pthread_mutex_lock(program->philos[i].dead_lock);
				*program->philos[i].dead = 1;
				pthread_mutex_lock(program->philos[i].write_lock);
				printf("%ld %d died\n", t.tv_sec * 1000 + t.tv_usec / 1000, program->philos[i].id);
				pthread_mutex_unlock(program->philos[i].write_lock);
				pthread_mutex_unlock(program->philos[i].dead_lock);
				return ;
			}
		}
	}
}

void init_monitor_thread(t_program *program)
{
	pthread_t monitor_thread;
	pthread_create(&monitor_thread, NULL, (void *)monitor, program);
	//pthread_join(monitor_thread, NULL);
}

int	main(int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		ft_putstr("Usage: ./philosopher number_of_philosopher time_to_die time_to_eat time_to_sleep [number_of_time_each_philosopher_must_eat]\n");
		return (1);
	}
	t_program *program = malloc(sizeof(t_program));
	struct timeval t;
	gettimeofday(&t, NULL);
	program->forks = malloc(sizeof(pthread_mutex_t) * ft_atoi(argv[1]));
	for (int i = 0; i < ft_atoi(argv[1]); i++)
		pthread_mutex_init(&program->forks[i], NULL);
	pthread_mutex_init(&program->dead_lock, NULL);
	pthread_mutex_init(&program->meal_lock, NULL);
	pthread_mutex_init(&program->write_lock, NULL);
	program->philos = malloc(sizeof(t_philo) * ft_atoi(argv[1]));
	program->num_of_philos = ft_atoi(argv[1]);
	program->dead_flag = 0;
	init_philos(program, argv);
	for (int i = 0; i < ft_atoi(argv[1]); i++)
		pthread_create(&program->philos[i].thread, NULL, (void *)philosopher, &program->philos[i]);
	//for (int i = 0; i < ft_atoi(argv[1]); i++)
	//	pthread_join(program->philos[i].thread, NULL);
	init_monitor_thread(program);
	free(program->philos);
	free(program->forks);
	return (0);
}
