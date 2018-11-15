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


// Function definitions
void* relaxation_runner(void* arg);


// Global variables
bool DEBUG = true;				// print data to the command line
int dim = 5;					// square array dimensions
int num_thr = 4;				// number of threads to use
float precision = 0.1f;			// precision to perform relaxation at
float **square_array;			// global square array of floats
struct relaxation_data {		// struct representing input data for each thread
	int thr_number;				// thread number (in order of creation)
};
pthread_mutex_t **mutex_array;	// array of mutexes to lock square array values

/*
 * Program entry.
 */
int main() {
	// initialise values
	square_array = initialise_square_array(dim);
	mutex_array = initialise_mutex_array(dim);

	if (DEBUG) print_initial_data(dim, num_thr, precision, square_array);
	
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
	
	// print final array
	if (DEBUG) {
		printf("\n-------------------------------------- Final square array\n");
		print_array(dim, square_array);
	}

	// free allocated array space and successfully exit program
 	free(square_array);
   	return 0;
}


/*
 * Loops through the square array to replace a value with an average of its 4
 * neighbouring values (left, right, up and down). Loops until all the values 
 * changed differ by less than the precision specified at the start of the
 * program. Does not update boundary values.
 * Returns a new square array with the updated values.
 */
void* relaxation_runner(void* arg) {
	// retrieve data from arg
	struct relaxation_data *arg_struct = (struct relaxation_data*) arg;
	int thread_number = arg_struct->thr_number;

	// local variables
	bool is_above_precision = true;
	int precision_counter = 0;
	int iteration_counter = 0;
	float difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	int i, j;

	if (DEBUG) {
		printf("Thread %d (id #%lu) created.\n", thread_number, pthread_self());
	}
		
	precision_counter = 0;
	while (is_above_precision) {

		if (DEBUG) {
			printf(
				"\n---------------------------------------- Iteration #%d\n", 
				iteration_counter
			);
		}
		
		for (i = 0; i < dim; i++) {
			for (j = 0; j < dim; j++) {
				// filter out border values
				if ( (!((i==0) || (i==dim-1))) && (!((j==0) || (j==dim-1))) ) {
					// value to replace
					float old_value = square_array[i][j]; 

					// get 4 surrounding values needed to average
					float v_left = square_array[i][j-1];
					float v_right = square_array[i][j+1];
					float v_up = square_array[i-1][j];
					float v_down = square_array[i+1][j];

					// perform the calculation
					float new_value = (v_left + v_right + v_up + v_down) / 4;

					// check if difference is smaller than precision
					difference = (float)fabs(old_value - new_value);
					if (difference < precision) {
						precision_counter++;
					} else {
						precision_counter = 0;	
					}

					// if difference smaller than precision for all values, stop
					if (precision_counter == number_of_values_to_change) {
						is_above_precision = false;
					}

					// replace the old value with the new one
					square_array[i][j] = new_value;

					// print iteration data
					if (DEBUG) {
						print_relaxation_data(old_value, v_left, v_right, v_up, 
						v_down, new_value, precision_counter);
					}
				}
			}
		}

		iteration_counter++;
	}
	printf("Thread %d (id #%lu) exited.\n", thread_number, pthread_self());
	pthread_exit(0);
}