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

long get_current_epoch(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (t.tv_sec * 1000 + t.tv_usec / 1000);
}

long get_execution_time(long start_time)
{
	return (get_current_epoch() - start_time);
}

int init_philos(t_program *program, char **argv)
{
	int i;

	i = 0;
	while (i < program->num_of_philos)
	{
		program->philos[i].id = i + 1;
		program->philos[i].eating = 0;
		program->philos[i].meals_eaten = 0;
		program->philos[i].last_meal = 0;
		program->philos[i].time_to_die = ft_atoi(argv[2]);
		program->philos[i].time_to_eat = ft_atoi(argv[3]);
		program->philos[i].time_to_sleep = ft_atoi(argv[4]);
		program->philos[i].start_time = get_current_epoch();
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
	if (*philo->dead == 1)
		return ;
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is thinking\n", get_execution_time(philo->start_time), philo->id);
	pthread_mutex_unlock(philo->write_lock);
}

void philo_eat(t_philo *philo)
{
	if (*philo->dead == 1)
		return ;
	philo->last_meal = get_execution_time(philo->start_time);
	pthread_mutex_lock(philo->l_fork);
	pthread_mutex_lock(philo->write_lock);
	if (*philo->dead == 1)
	{
		pthread_mutex_unlock(philo->l_fork);
		pthread_mutex_unlock(philo->write_lock);
		return ;
	}
	printf("%ld %d has taken a fork\n", get_execution_time(philo->start_time), philo->id);
	pthread_mutex_unlock(philo->write_lock);
	if (philo->num_of_philos == 1)
	{
		pthread_mutex_unlock(philo->l_fork);
		*philo->dead = 1;
		return ;
	}
	pthread_mutex_lock(philo->r_fork);
	pthread_mutex_lock(philo->write_lock);
	if (*philo->dead == 1)
	{
		pthread_mutex_unlock(philo->r_fork);
		pthread_mutex_unlock(philo->write_lock);
		return ;
	}
	printf("%ld %d has taken a fork2\n", get_execution_time(philo->start_time), philo->id);
	pthread_mutex_unlock(philo->write_lock);
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is eating\n", get_execution_time(philo->start_time), philo->id);
	pthread_mutex_unlock(philo->write_lock);
	usleep(philo->time_to_eat * 1000);
	pthread_mutex_unlock(philo->r_fork);
	pthread_mutex_unlock(philo->l_fork);
}

void philo_sleep(t_philo *philo)
{
	if (*philo->dead == 1)
		return ;
	pthread_mutex_lock(philo->write_lock);
	printf("%ld %d is sleeping\n", get_execution_time(philo->start_time), philo->id);
	pthread_mutex_unlock(philo->write_lock);
	usleep(philo->time_to_sleep * 1000);
}

void philosopher(void *philosopher)
{
	t_philo *philo = (t_philo*)philosopher;
	//printf("time: %ld from philosopher %d\n", t.tv_sec * 1000 + t.tv_usec / 1000, philo->id);
	//eat with 2 forks, sleep, think
	if (philo->id % 2)
		usleep(5);
	while (1)
	{
		pthread_mutex_lock(philo->dead_lock);
        if (*philo->dead == 1)
        {
            pthread_mutex_unlock(philo->dead_lock);
            break;
        }
        pthread_mutex_unlock(philo->dead_lock);
		philo_think(philo);
		philo_eat(philo);
		philo_sleep(philo);
	}
	return ;
}

void monitor(t_program *program)
{
	while (1)
	{
		//printf("dead flag: %d\n", program->dead_flag);
		for (int i = 0; i < program->num_of_philos; i++)
		{
			if (get_execution_time(program->philos[i].start_time) - program->philos[i].last_meal >= program->philos[i].time_to_die)
			{
				pthread_mutex_lock(program->philos[i].dead_lock);
				*program->philos[i].dead = 1;
				pthread_mutex_unlock(program->philos[i].dead_lock);
				pthread_mutex_lock(program->philos[i].write_lock);
                printf("%ld %d died\n", get_execution_time(program->philos[i].start_time), program->philos[i].id);
                pthread_mutex_unlock(program->philos[i].write_lock);

                // Set dead_flag and exit monitor thread
                pthread_mutex_lock(&program->dead_lock);
                program->dead_flag = 1;
                pthread_mutex_unlock(&program->dead_lock);
				return ;
			}
		}
	}
}

void init_monitor_thread(t_program *program)
{
	pthread_create(&program->monitor_thread, NULL, (void *)monitor, program);
}

int	main(int argc, char **argv)
{
	if (argc < 5 || argc > 6)
	{
		ft_putstr("Usage: ./philosopher number_of_philosopher time_to_die time_to_eat time_to_sleep [number_of_time_each_philosopher_must_eat]\n");
		return (1);
	}
	t_program *program = malloc(sizeof(t_program));
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
	init_monitor_thread(program);
	for (int i = 0; i < ft_atoi(argv[1]); i++)
		pthread_join(program->philos[i].thread, NULL);
	pthread_join(program->monitor_thread, NULL);
	for (int i = 0; i < ft_atoi(argv[1]); i++)
		pthread_mutex_destroy(&program->forks[i]);
	pthread_mutex_destroy(&program->dead_lock);
	pthread_mutex_destroy(&program->meal_lock);
	pthread_mutex_destroy(&program->write_lock);

	free(program->philos);
	free(program->forks);
	free(program);
	return (0);
}
