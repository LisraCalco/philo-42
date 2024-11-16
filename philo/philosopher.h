/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlegendr <tlegendr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/04 20:06:06 by tlegendr          #+#    #+#             */
/*   Updated: 2024/11/16 15:24:29 by tlegendr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <pthread.h>
# include <sys/time.h>

# define USAGE_ERROR "Usage: ./philo number_of_philosopher time_to_die time_to_eat \
time_to_sleep [number_of_time_each_philosopher_must_eat]\n"
# define FORK_TAKEN "has taken a fork\n"
# define EATING "is eating\n"
# define SLEEPING "is sleeping\n"
# define THINKING "is thinking\n"
# define DEAD "died\n"

typedef struct s_philo
{
	pthread_t		thread;
	int				id;
	int				eating;
	int				meals_eaten;
	long			last_meal;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	long			start_time;
	int				num_of_philos;
	int				num_times_to_eat;
	int				*dead;
	pthread_mutex_t	*r_fork;
	pthread_mutex_t	*l_fork;
	pthread_mutex_t	*write_lock;
	pthread_mutex_t	*dead_lock;
	pthread_mutex_t	*meal_lock;
}					t_philo;

typedef struct s_program
{
	int				num_of_philos;
	int				dead_flag;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	write_lock;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_t		monitor_thread;
}					t_program;

void	init_philo_values(t_program *program, char **argv, int argc, int i);
int		init_philos(t_program *program, char **argv, int argc);
void	init_monitor_thread(t_program *program);
void	init_mutex(t_program *program);
void	destroy_mutex(t_program *program);

int		check_all_args(int argc, char **argv);

void	philosopher(t_philo *philosopher);
int		check_all_philo_eat_time( t_program *program);
void	set_program_dead(t_program *program);
int		didnt_eat_recently(t_philo *philo);
void	monitor(t_program *program);

int		philo_is_dead(t_philo *philo);
void	philo_think(t_philo *philo);
void	philo_eat(t_philo *philo);
void	philo_sleep(t_philo *philo);
void	print_msg(t_philo *philo, char *msg);

long	ft_atoi(const char *string);
int		is_all_digit(char *str);
int		is_not_negative(char *str);
long	get_current_epoch(void);
long	get_execution_time(long start_time);

#endif