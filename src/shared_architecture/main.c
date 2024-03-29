/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Relaxation technique for solving differential equations in a shared memory 
 * architecture using pthreads
 *
 * Local usage: 
 * 1) "gcc main.c array_helpers.c print_helpers.c -o shared_relaxation -pthread -Wall 
 *     -Wextra -Wconversion"
 * 2) "./shared_relaxation <number_of_threads>"
 * 
 * Author: Adam Jaamour
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <sys/time.h>
#include "array_helpers.h"
#include "print_helpers.h"


/* Global variables */
bool DEBUG = false;				// print data to the command line
int dim = 100;					// square array dimensions
int num_thr;					// number of threads to use
double precision = 0.01f;		// precision to perform relaxation at
double **square_array;			// global square array of double
pthread_mutex_t **mutex_array;	// array of mutexes to lock square array values
struct timeval time1, time2;	// structure used to calculate program time
struct relaxation_data {		// struct representing input data for a thread
	int thr_number;				// thread number (in order of creation)
};


/*
 * Threaded function that loops loops through the square array of double to 
 * replace each value (except boundary values) with an average of its 4
 * neighbouring values (left, right, up and down). Each thread loops until all 
 * the updated values differ by less than the precision specified at the start 
 * of the program. 
 * Each thread exits once it has iterated through the array once and each update
 * differs by less than the precision.
 * Uses an array of mutexes, symmetric to the array of double, to lock
 * individual sections of the array so that only one thread updates a value at a
 * time.
 */
void* relaxation_runner(void* arg) {
	// retrieve data from arg
	struct relaxation_data *arg_struct = (struct relaxation_data*) arg;
	int thread_number = arg_struct->thr_number;

	// local variables
	bool is_above_precision = true;
	int precision_counter = 0;
	int updates_counter = 0;
	double difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	int i, j;

	if (DEBUG) {
		printf("Thread %d (id #%lu) created.\n\n", thread_number, 
			pthread_self());
	}
		
	while (is_above_precision) {
		for (i = 1; i < dim - 1; i++) {
			for (j = 1; j < dim - 1; j++) {
				// lock and retrieve current value to replace
				pthread_mutex_lock(&mutex_array[i][j]);
				double old_value = square_array[i][j];

				// retrieve the 4 surrounding values needed to average
				double v_left = square_array[i][j-1];
				double v_right = square_array[i][j+1];
				double v_up = square_array[i-1][j];
				double v_down = square_array[i+1][j];

				// calculate new value, replace the old value and unlock
				double new_value = (v_left + v_right + v_up + v_down) / 4;
				square_array[i][j] = new_value;
				pthread_mutex_unlock(&mutex_array[i][j]);
				updates_counter++;

				// check if difference is smaller than precision
				difference = (double)fabs(old_value - new_value);
				if (difference < precision) {
					precision_counter++;
				} else { // reset precision counter if diff smaller than prec
					precision_counter = 0;
				}

				// if difference smaller than precision for all values, stop
				if (precision_counter >= number_of_values_to_change) {
					is_above_precision = false;
					break;
				}

				pthread_mutex_lock(&mutex_array[i][j]);	
				// print current iteration data
				if (DEBUG) {
					print_relaxation_thread_data(thread_number, 
						updates_counter, i, j);
					print_relaxation_values_data(old_value, v_left, v_right, 
						v_up, v_down, new_value, precision_counter);
				}
				pthread_mutex_unlock(&mutex_array[i][j]);
			}
			if (!(is_above_precision)) { // check at end of current array iter
				break;
			}
		}
	}
	
	if (DEBUG) printf("Thread %d (id #%lu) exited.\n\n", thread_number, 
		pthread_self());
	
	pthread_exit(0);
}


/*
 * Program entry.
 */
int main(int argc, char *argv[]) {
	// initialise values
	square_array = initialise_square_array(dim);
	mutex_array = initialise_mutex_array(dim);

	// retrieve number of threads from command line argument
	if (argc == 2) {
		num_thr = atoi(argv[1]);
	} else if (argc > 2) {
		printf("Too many arguments provided.\n");
		exit(EXIT_FAILURE);
	} else {
		printf("One argument expected. Number of threads set to 10.\n\n");
		num_thr = 10;
	}

	if (DEBUG) print_parameters(dim, num_thr, precision, square_array);
	
	// start recording time
	gettimeofday(&time1, NULL);

	// initialise and create threads to perform relaxation in parallel
	pthread_t tids[num_thr];				// array of thread IDs
	struct relaxation_data args[num_thr];	// array of structs for thread input
	int i;
	for (i = 0; i < num_thr; i++) {
		args[i].thr_number = i + 1;	// 1-based thread numbers
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_create(&tids[i], &attr, relaxation_runner, &args[i]);
	}

	// wait until threads finish running
	for (i = 0; i < num_thr; i++) {
		pthread_join(tids[i], NULL);
	}

	// stop recording time
	gettimeofday(&time2, NULL);

	// print final results
	print_final_results(dim, num_thr, precision);
	printf ("Total time = %f seconds\n", 
		(double) (time2.tv_usec - time1.tv_usec) / 1000000 +
		(double) (time2.tv_sec - time1.tv_sec));
	
	// print final array
	if (DEBUG) {
		printf("\nFinal square array\n");
		print_array(dim, square_array);
	}

	// free allocated array space and successfully exit program
 	free(square_array);	
	free(mutex_array);
   	return 0;
}