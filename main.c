/**
 * CM30225 Parallel Computing
 * Assessment Coursework 1
 * 
 * Relaxation technique for solving differential equations in a shared memory 
 * architecture using pthreads
 *
 * Local usage: 
 * 1) "gcc main.c -o main.exe -lpthread -Wall -Wextra -Wconversion"
 * 2) "main.exe"
 * 
 * Author: aj645
 * Date: 19-Nov-2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


// Function definitions
void initialise_square_array(void);
void relaxation(double precision);
void check_malloc(void);
double double_random(double low, double high);
void print_initial_data(double precision);
void print_array(void);
void print_relaxation_data(double old, double l, double r, double u, double d, double new, int precision);


// Global variables
#define dim 4		// square array dimensions
#define num_thr 2	// number of threads to use
double **square_array;


/*
 * Program entry.
 */
int main() {
	// initialise values
	double precision = 0.1; // precision to perform relaxation at
	initialise_square_array();
	print_initial_data(precision);

	// iterate averaging until precision reached
	relaxation(precision);
	printf("\n------------------------ Final square array\n");
	print_array();

	// free allocated array space and successfully exit program
 	free(square_array);
   	return 0;
}


/*
 * Initialises a square array by creating an initial array of pointers, each 
 * looking at 1D arrays of doubles.
 */
void initialise_square_array(void) {
	int i, j;
	
	// allocate space for a 1D array of double pointers.
	square_array = malloc(dim * sizeof(double*));
	check_malloc();

	// allocate space for multiple 1D arrays of doubles
	for (i = 0; i < dim; i++) {
		square_array[i] = malloc(dim * sizeof(double));
		check_malloc();
	}

	// populate the array with random doubles
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++) {
			square_array[i][j] = (i*dim) + (j*j*j*j) + 10;
			//square_array[i][j] = double_random(1.0, 10.0);
			//square_array[i][j] = rand() % 10;
		}
	}
}


/*
 * Loops through the square array to replace a value with an average of its 4
 * neighbouring values (left, right, up and down). Loops until all the values 
 * changed differ by less than the precision specified at the start of the
 * program. Does not update boundary values.
 * Returns a new square array with the updated values.
 */
void relaxation(double precision) {
	bool is_above_precision = true;
	int precision_counter = 0;
	int iteration_counter = 0;
	double difference = 0.0; // different between old and new value
	int number_of_values_to_change = ((dim-2) * (dim-2));
	int i, j;

	while (is_above_precision) {
		precision_counter = 0;
		printf("\n------------------------ Iteration #%d\n", iteration_counter);
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
}


/*
 * Check that malloc correctly allocated the amount of space required for the 
 * square array of doubles.
 * If not, exit the program with a failure.
 */
void check_malloc(void) {
	if (square_array == NULL) {
		fprintf(stderr, "Failed to allocate space for the array.\n");
		exit(EXIT_FAILURE);
	}
}


/*
 * Returns a random double ranging from 'low' to 'high'.
 */
double double_random (double low, double high) {
    return ((double)rand() * (high-low)) / (double)RAND_MAX + low;
}


/*
 * Prints the initial data values used to initiate the program.
 */
void print_initial_data(double precision) {
	printf("------------------------ Initial data:\n");
	printf("Array dimension: %d\n", dim);
	printf("Number of threads: %d\n", num_thr);
	printf("Precision: %f\n", precision);
	printf("Square array:\n");
	print_array();
}


/*
 * Prints an array to the command line.
 */
void print_array(void) {
	int i, j;
	for (i = 0; i < dim; i++) {
 		for (j = 0; j < dim; j++) {
 			printf("%f ", square_array[i][j]);
 		}
 		printf("\n");
 	}
}


/*
 * Prints the values used for each iteration of the relaxation technique.
 */
void print_relaxation_data(double old, double l, double r, double u, double d, 
	double new, int precision) {
	printf("Value to replace: %f\n", old);
	printf("l: %f, r: %f, u: %f, d: %f\n", l, r, u, d);
	printf("New value = %f\n", new);
	printf("precision_counter: %d\n", precision);
	printf("------\n");
}