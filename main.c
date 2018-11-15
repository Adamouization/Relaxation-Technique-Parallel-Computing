/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Relaxation technique for solving differential equations in a shared memory 
 * architecture using pthreads
 *
 * Local usage: 
 * 1) "gcc main.c array_helpers.c print_helpers.c -o main.exe -pthread -Wall 
 *     -Wextra -Wconversion"
 * 2) "./main.exe"
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include "array_helpers.h"
#include "print_helpers.h"


/* Global variables */
bool DEBUG = false;				// print data to the command line
int dim = 100;					// square array dimensions
int num_thr = 500;				// number of threads to use
float precision = 0.01f;		// precision to perform relaxation at
float **square_array;			// global square array of floats
pthread_mutex_t **mutex_array;	// array of mutexes to lock square array values
struct relaxation_data {		// struct representing input data for each thread
	int thr_number;				// thread number (in order of creation)
};


/*
 * Threaded function that loops loops through the square array of floats to 
 * replace each value (except boundary values) with an average of its 4
 * neighbouring values (left, right, up and down). Each thread loops until all 
 * the updated values differ by less than the precision specified at the start 
 * of the program. 
 * Each thread exits once it has iterated through the array once and each update
 * differs by less than the precision.
 * Uses an array of mutexes, symmetric to the array of floats, to lock
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
	float difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	int i, j;

	if (DEBUG) {
		printf("Thread %d (id #%lu) created.\n\n", thread_number, 
			pthread_self());
	}
		
	while (is_above_precision) {
		for (i = 0; i < dim; i++) {
			for (j = 0; j < dim; j++) {
				// filter out border values
				if ( (!((i==0) || (i==dim-1))) && (!((j==0) || (j==dim-1))) ) {
					
					// lock and retrieve current value to replace
					pthread_mutex_lock(&mutex_array[i][j]);
					float old_value = square_array[i][j];

					// retrieve the 4 surrounding values needed to average
					float v_left = square_array[i][j-1];
					float v_right = square_array[i][j+1];
					float v_up = square_array[i-1][j];
					float v_down = square_array[i+1][j];

					// calculate new value, replace the old value and unlock
					float new_value = (v_left + v_right + v_up + v_down) / 4;
					square_array[i][j] = new_value;
					pthread_mutex_unlock(&mutex_array[i][j]);
					updates_counter++;

					// check if difference is smaller than precision
					difference = (float)fabs(old_value - new_value);
					if (difference < precision) {
						precision_counter++;
					} else {
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
			}
			if (!(is_above_precision)) {
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
int main() {
	// initialise values
	square_array = initialise_square_array(dim);
	mutex_array = initialise_mutex_array(dim);

	if (DEBUG) print_parameters(dim, num_thr, precision, square_array);
	
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

	// print final results
	print_final_results(dim, num_thr, precision);
	
	// print final array
	if (DEBUG) {
		printf("\nFinal square array\n");
		print_array(dim, square_array);
	}

	// free allocated array space and successfully exit program
 	free(square_array);
   	return 0;
}