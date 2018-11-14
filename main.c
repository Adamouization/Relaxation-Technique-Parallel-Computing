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
void* relaxation_runner(void* prec);


// Global variables
int dim = 4;		// square array dimensions
int num_thr = 2;	// number of threads to use
double **square_array;


/*
 * Program entry.
 */
int main() {
	// initialise values
	double precision = 0.1; // precision to perform relaxation at
	square_array = initialise_square_array(dim);
	print_initial_data(dim, num_thr, precision, square_array);
	
	// initialise thread data
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	// perform relaxation in parallel
	pthread_create(&tid, &attr, relaxation_runner, &precision);
	pthread_join(tid, NULL); // wait until thread fiishes running
	
	// print final array
	printf("\n------------------------------------------ Final square array\n");
	print_array(dim, square_array);

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
	bool is_above_precision = true;
	int precision_counter = 0;
	int iteration_counter = 0;
	double difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	double precision = *(double *) arg;
	int i, j;

	while (is_above_precision) {
		precision_counter = 0;
		printf("\n------------------------------------------ Iteration #%d\n", 
			iteration_counter);
		for (i = 0; i < dim; i++) {
			for (j = 0; j < dim; j++) {
				// filter out border values
				if ( (!((i==0) || (i==dim-1))) && (!((j==0) || (j==dim-1))) ) {
					// value to replace
					double old_value = square_array[i][j]; 

					// get 4 surrounding values needed to average
					double v_left = square_array[i][j-1];
					double v_right = square_array[i][j+1];
					double v_up = square_array[i-1][j];
					double v_down = square_array[i+1][j];

					// perform the calculation
					double new_value = (v_left + v_right + v_up + v_down) / 4;

					// check if difference is smaller than precision
					difference = fabs(old_value - new_value);
					if (difference < precision) {
						precision_counter++;
					}

					// if difference smaller than precision for all values, stop
					if (precision_counter == number_of_values_to_change) {
						is_above_precision = false;
					}

					// replace the old value with the new one
					square_array[i][j] = new_value;

					// print data
					print_relaxation_data(old_value, v_left, v_right, v_up, 
						v_down, new_value, precision_counter);
				}
			}
		}
		iteration_counter++;
	}
	pthread_exit(0);
}